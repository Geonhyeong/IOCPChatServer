#include "pch.h"
#include "Session.h"

Session::Session() : _recvBuffer(BUFFER_SIZE)
{
	_socket = INVALID_SOCKET;
	_acceptSocket = INVALID_SOCKET;
	_netAddress = {};
	_state = SessionState::DISCONNECT;
	_latestDisconnectedTimeSec = 0;
	_sendRegistered = false;
}

Session::~Session()
{
	SocketUtils::CloseSocket(_socket);
}

void Session::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
{
	switch (iocpEvent->eventType)
	{
	case EventType::ACCEPT:
		ProcessAccept();
		break;
	case EventType::RECV:
		ProcessRecv(numOfBytes);
		break;
	case EventType::SEND:
		ProcessSend(numOfBytes);
		break;
	case EventType::DISCONNECT:
		ProcessDisconnet();
		break;
	default:
		break;
	}
}

void Session::Accept(SOCKET acceptSocket)
{
	if (_state != SessionState::DISCONNECT)
	{
		SLog(L"Session::Accept(), but session is connected.");
		return;
	}

	_socket = SocketUtils::CreateSocket();
	_acceptSocket = acceptSocket;

	_acceptEvent.Init();
	_acceptEvent.ownerSession = shared_from_this();

	RegisterAccept();
}

void Session::Send(SendBufferRef sendBuffer)
{
	// TODO : Send할 때 버퍼 관리를 어떻게 할 것인가
	// SendQueue를 활용해서 1-Send를 구현할 것
	// 큐에 쌓인 버퍼들을 한번에 모아서 보내는 Scatter-Gather 기법을 사용할 것
	// SendEvent를 재사용할 것 (메모리 leak을 고려)
	
	// 1) 미리 Packet 클래스를 정의해서 Packet 객체를 받는다
	//		- 장점 : 다소 간단하게 구현이 가능하다
	//		- 단점 : 미리 만들어 놓은 Packet 클래스를 사용해야 하므로 확장성이 떨어진다
	// 2) SendBuffer를 정의해서 SendBuffer 객체를 받는다
	//		- 장점 : 확장성이 좋다. 컨텐츠단에서 패킷을 어떻게 정의하고 처리하는가에 관계없이 SendBuffer에 담아서 넘겨주기만 하면 된다.
	//				 또한, SendBuffer의 스마트 포인터를 넘겨주어서 복사비용을 최소화하고 RefCount가 0이 되면 자동으로 메모리 풀로 넘겨줄 수 있다.
	//		- 단점 : SendBuffer는 자주 할당/해제가 일어나서 메모리 풀 기법을 사용해야 한다. 구현이 복잡해짐.
	// 

	if (_state != SessionState::CONNECT)
	{
		SLog(L"Session::Send(), but session is disconnected.");
		return;
	}

	bool registerSend = false;
	{
		WRITE_LOCK;
		
		_sendQueue.push(sendBuffer);

		if (_sendRegistered.exchange(true) == false)
			registerSend = true;
	}

	if (registerSend)
		RegisterSend();
}

void Session::Disconnect()
{
	RegisterDisconnet();
}


#pragma region 네트워크 함수
void Session::RegisterAccept()
{
	if (_state != SessionState::DISCONNECT)
	{
		SLog(L"Session::RegisterAccept(), but session is connected.");
		return;
	}

	DWORD bytesReceived = 0;
	if (false == SocketUtils::AcceptEx(_acceptSocket, _socket, _recvBuffer.WritePos(), 0,
		sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &bytesReceived, &_acceptEvent))
	{
		const int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			_acceptEvent.ownerSession = nullptr;
			SLog(L"AcceptEx is failed : %d", errorCode);
			return;
		}
	}

	_state = SessionState::ACCEPT_IN_PROGRESS;
}

void Session::RegisterRecv()
{
	if (_state != SessionState::CONNECT)
	{
		SLog(L"Session::RegisterRecv(), but session is disconnected.");
		return;
	}

	_recvEvent.Init();
	_recvEvent.ownerSession = shared_from_this();

	WSABUF wsaBuf;
	wsaBuf.buf = reinterpret_cast<char*>(_recvBuffer.WritePos());
	wsaBuf.len = _recvBuffer.FreeSize();

	DWORD numOfBytes = 0;
	DWORD flags = 0;
	if (::WSARecv(_socket, &wsaBuf, 1, &numOfBytes, &flags, &_recvEvent, nullptr) == SOCKET_ERROR)
	{
		const int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			_recvEvent.ownerSession = nullptr;
			SLog(L"WSARecv is failed : %d", errorCode);
		}
	}
}

void Session::RegisterSend()
{
	if (_state != SessionState::CONNECT)
	{
		SLog(L"Session::RegisterSend(), but session is disconnected.");
		return;
	}

	_sendEvent.Init();
	_sendEvent.ownerSession = shared_from_this();

	{
		WRITE_LOCK;

		while (_sendQueue.empty() == false)
		{
			SendBufferRef sendBuffer = _sendQueue.front();
			_sendQueue.pop();

			_sendEvent.sendBuffers.push_back(sendBuffer);
		}
	}

	// Scatter-Gather (흩어져 있는 데이터들을 모아서 한 방에 보낸다)
	vector<WSABUF> wsaBufs;
	wsaBufs.reserve(_sendEvent.sendBuffers.size());
	for (SendBufferRef sendBuffer : _sendEvent.sendBuffers)
	{
		WSABUF wsaBuf;
		wsaBuf.buf = reinterpret_cast<char*>(sendBuffer->Buffer());
		wsaBuf.len = static_cast<LONG>(sendBuffer->WriteSize());
		wsaBufs.push_back(wsaBuf);
	}

	DWORD numOfBytes = 0;
	if (SOCKET_ERROR == ::WSASend(_socket, wsaBufs.data(), static_cast<DWORD>(wsaBufs.size()), &numOfBytes, 0, &_sendEvent, nullptr))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			_sendEvent.ownerSession = nullptr;
			_sendEvent.sendBuffers.clear(); // RELEASE_REF
			_sendRegistered.store(false);
			SLog(L"WSASend is failed : %d", errorCode);
		}
	}
}

void Session::RegisterDisconnet()
{
	if (_state != SessionState::CONNECT)
	{
		SLog(L"Session::RegisterDisconnect(), but session is disconnected.");
		return;
	}

	_disconnectEvent.Init();
	_disconnectEvent.ownerSession = shared_from_this();

	if (SocketUtils::DisconnectEx(_socket, &_disconnectEvent, TF_REUSE_SOCKET, 0))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			_disconnectEvent.ownerSession = nullptr;
			SLog(L"DisconnectEx is failed : %d", errorCode);
		}
	}
}

void Session::ProcessAccept()
{
	// SO_KEEPALIVE 옵션은 넣지 않기로 함. 왜냐하면 더욱 정밀하게 자원을 관리하고 추후에 응용(ex. 잠수유저 추방)하기 위해서 직접 구현하기로 함.
	if (SocketUtils::SetUpdateAcceptSocket(_socket, _acceptSocket) == false)
	{
		RegisterAccept();
		return;
	}

	// NetAddress 받아오기
	SOCKADDR_IN sockAddress;
	int32 sizeOfSockAddr = sizeof(sockAddress);
	if (SOCKET_ERROR == ::getpeername(_socket, reinterpret_cast<SOCKADDR*>(&sockAddress), &sizeOfSockAddr))
	{
		RegisterAccept();
		return;
	}
	_netAddress = NetAddress(sockAddress);

	_acceptEvent.ownerSession = nullptr;
	_state = SessionState::CONNECT;

	// 새로운 소켓을 IOCP에 등록
	IocpCore::GetInstance().Register(_socket);

	// 컨텐츠 코드 호출
	OnConnected();

	// 수신 등록
	RegisterRecv();
}

void Session::ProcessRecv(int32 numOfBytes)
{
	_recvEvent.ownerSession = nullptr;

	if (numOfBytes == 0)
	{
		SLog(L"Session::ProcessRecv() => Disconnect signal is received.");
		Disconnect();
		return;
	}

	if (_recvBuffer.OnWrite(numOfBytes) == false)
	{
		SLog(L"Session::ProcessRecv()::OnWrite() => RecvBuffer is overflowed.");
		Disconnect();
		return;
	}

	int32 dataSize = _recvBuffer.DataSize();
	int32 processLen = ParsingPacket(_recvBuffer.ReadPos(), dataSize);
	if (processLen < 0 || dataSize < processLen || _recvBuffer.OnRead(processLen) == false)
	{
		SLog(L"Session::ProcessRecv()::OnRead() => RecvBuffer is overflowed. dataSize : %d, processLen : %d", dataSize, processLen);
		Disconnect();
		return;
	}

	_recvBuffer.Clean();

	RegisterRecv();
}

void Session::ProcessSend(int32 numOfBytes)
{
	_sendEvent.ownerSession = nullptr;
	_sendEvent.sendBuffers.clear(); // RELEASE_REF

	if (numOfBytes == 0)
	{
		SLog(L"Session::ProcessSend() => Disconnect signal is received.");
		Disconnect();
		return;
	}

	OnSend(numOfBytes);

	WRITE_LOCK;
	if (_sendQueue.empty())
		_sendRegistered.store(false);
	else
		RegisterSend();
}

void Session::ProcessDisconnet()
{
	_disconnectEvent.ownerSession = nullptr;

	SocketUtils::CloseSocket(_socket);
	_state = SessionState::DISCONNECT;
	_latestDisconnectedTimeSec = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
	
	OnDisconnected();
}

int32 Session::ParsingPacket(BYTE* buffer, int32 len)
{
	int32 processLen = 0;

	while (true)
	{
		int32 dataSize = len - processLen;

		// 최소한 헤더는 파싱 가능해야 한다.
		if (dataSize < sizeof(PacketHeader))
			break;

		PacketHeader header = *(reinterpret_cast<PacketHeader*>(&buffer[processLen]));
		// 헤더에 기록된 패킷 크기를 파싱할 수 있어야 한다
		if (dataSize < header.size)
			break;

		// 패킷 조립 성공
		OnRecv(&buffer[processLen], header.size);

		processLen += header.size;
	}

	return processLen;
}
#pragma endregion
