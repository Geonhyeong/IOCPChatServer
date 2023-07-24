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
		// TODO : LOG
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
	// TODO : Send�� �� ���� ������ ��� �� ���ΰ�
	// SendQueue�� Ȱ���ؼ� 1-Send�� ������ ��
	// ť�� ���� ���۵��� �ѹ��� ��Ƽ� ������ Scatter-Gather ����� ����� ��
	// SendEvent�� ������ �� (�޸� leak�� ���)
	
	// 1) �̸� Packet Ŭ������ �����ؼ� Packet ��ü�� �޴´�
	//		- ���� : �ټ� �����ϰ� ������ �����ϴ�
	//		- ���� : �̸� ����� ���� Packet Ŭ������ ����ؾ� �ϹǷ� Ȯ�强�� ��������
	// 2) SendBuffer�� �����ؼ� SendBuffer ��ü�� �޴´�
	//		- ���� : Ȯ�强�� ����. �������ܿ��� ��Ŷ�� ��� �����ϰ� ó���ϴ°��� ������� SendBuffer�� ��Ƽ� �Ѱ��ֱ⸸ �ϸ� �ȴ�.
	//				 ����, SendBuffer�� ����Ʈ �����͸� �Ѱ��־ �������� �ּ�ȭ�ϰ� RefCount�� 0�� �Ǹ� �ڵ����� �޸� Ǯ�� �Ѱ��� �� �ִ�.
	//		- ���� : SendBuffer�� ���� �Ҵ�/������ �Ͼ�� �޸� Ǯ ����� ����ؾ� �Ѵ�. ������ ��������.
	// 

	if (_state != SessionState::CONNECT)
	{
		// TODO : LOG
		return;
	}

	bool registerSend = false;
	{
		lock_guard<mutex> lock(_lock);
		
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


#pragma region ��Ʈ��ũ �Լ�
void Session::RegisterAccept()
{
	if (_state != SessionState::DISCONNECT)
	{
		// TODO : LOG
		return;
	}

	DWORD bytesReceived = 0;
	if (false == SocketUtils::AcceptEx(_acceptSocket, _socket, _recvBuffer.WritePos(), 0,
		sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &bytesReceived, &_acceptEvent))
	{
		const int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			// TODO : LOG
			_acceptEvent.ownerSession = nullptr;
			return;
		}
	}

	_state = SessionState::ACCEPT_IN_PROGRESS;
}

void Session::RegisterRecv()
{
	if (_state != SessionState::CONNECT)
	{
		// TODO : LOG
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
		if (::WSAGetLastError() != WSA_IO_PENDING)
		{
			// TODO : LOG
			_recvEvent.ownerSession = nullptr;
		}
	}
}

void Session::RegisterSend()
{
	if (_state != SessionState::CONNECT)
	{
		// TODO : LOG
		return;
	}

	_sendEvent.Init();
	_sendEvent.ownerSession = shared_from_this();

	{
		lock_guard<mutex> lock(_lock);

		while (_sendQueue.empty() == false)
		{
			SendBufferRef sendBuffer = _sendQueue.front();
			_sendQueue.pop();

			_sendEvent.sendBuffers.push_back(sendBuffer);
		}
	}

	// Scatter-Gather (����� �ִ� �����͵��� ��Ƽ� �� �濡 ������)
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
			// TODO : LOG
			_sendEvent.ownerSession = nullptr;
			_sendEvent.sendBuffers.clear(); // RELEASE_REF
			_sendRegistered.store(false);
		}
	}
}

void Session::RegisterDisconnet()
{
	if (_state != SessionState::CONNECT)
	{
		// TODO : LOG
		return;
	}

	_disconnectEvent.Init();
	_disconnectEvent.ownerSession = shared_from_this();

	if (SocketUtils::DisconnectEx(_socket, &_disconnectEvent, TF_REUSE_SOCKET, 0))
	{
		if (::WSAGetLastError() != WSA_IO_PENDING)
		{
			// TODO : LOG
			_disconnectEvent.ownerSession = nullptr;
		}
	}
}

void Session::ProcessAccept()
{
	// SO_KEEPALIVE �ɼ��� ���� �ʱ�� ��. �ֳ��ϸ� ���� �����ϰ� �ڿ��� �����ϰ� ���Ŀ� ����(ex. ������� �߹�)�ϱ� ���ؼ� ���� �����ϱ�� ��.
	if (SocketUtils::SetUpdateAcceptSocket(_socket, _acceptSocket) == false)
	{
		RegisterAccept();
		return;
	}

	// NetAddress �޾ƿ���
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

	// ���ο� ������ IOCP�� ���
	IocpCore::GetInstance().Register(_socket);

	// TODO : ������ �ڵ� ȣ��
	OnConnected();

	// ���� ���
	RegisterRecv();
}

void Session::ProcessRecv(int32 numOfBytes)
{
	_recvEvent.ownerSession = nullptr;

	if (numOfBytes == 0)
	{
		// TODO : LOG
		Disconnect();
		return;
	}

	if (_recvBuffer.OnWrite(numOfBytes) == false)
	{
		// TODO : LOG
		Disconnect();
		return;
	}

	int32 dataSize = _recvBuffer.DataSize();
	int32 processLen = ParsingPacket(_recvBuffer.ReadPos(), dataSize);
	if (processLen < 0 || dataSize < processLen || _recvBuffer.OnRead(processLen) == false)
	{
		// TODO : LOG
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
		// TODO : LOG
		Disconnect();
		return;
	}

	OnSend(numOfBytes);

	lock_guard<mutex> lock(_lock);
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

		// �ּ��� ����� �Ľ� �����ؾ� �Ѵ�.
		if (dataSize < sizeof(PacketHeader))
			break;

		PacketHeader header = *(reinterpret_cast<PacketHeader*>(&buffer[processLen]));
		// ����� ��ϵ� ��Ŷ ũ�⸦ �Ľ��� �� �־�� �Ѵ�
		if (dataSize < header.size)
			break;

		// ��Ŷ ���� ����
		OnRecv(&buffer[processLen], header.size);

		processLen += header.size;
	}

	return processLen;
}
#pragma endregion
