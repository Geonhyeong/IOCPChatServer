#include "pch.h"
#include "Session.h"

Session::Session(uint32 maxBufferSize) : _recvBuffer(maxBufferSize)
{
	_socket = SocketUtils::CreateSocket();
	_netAddress = {};
	_state = SessionState::DISCONNECT;
	_latestDisconnectedTimeSec = 0;

	_acceptEvent = new AcceptEvent();
	_recvEvent = new RecvEvent();
	_sendEvent = new SendEvent();
	_disconnectEvent = new DisconnectEvent();
}

Session::~Session()
{
	SocketUtils::CloseSocket(_socket);

	delete _acceptEvent;
	delete _recvEvent;
	delete _sendEvent;
	delete _disconnectEvent;
}

void Session::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
{
	switch (iocpEvent->eventType)
	{
	case EventType::Accept:
		ProcessAccept();
		break;
		break;
	case EventType::Recv:
		ProcessRecv(numOfBytes);
		break;
	case EventType::Send:
		ProcessSend(numOfBytes);
		break;
	case EventType::Disconnect:
		ProcessDisconnet();
	default:
		break;
	}
}

void Session::Accept(SOCKET acceptSocket)
{
	_acceptEvent->Init();
	_acceptEvent->ownerSession = shared_from_this();

	RegisterAccept(acceptSocket);
}

void Session::Send()
{
}

void Session::Disconnect()
{
}


#pragma region 네트워크 함수
void Session::RegisterAccept(SOCKET acceptSocket)
{
	DWORD bytesReceived = 0;
	if (false == SocketUtils::AcceptEx(acceptSocket, _socket, _recvBuffer.WritePos(), 0,
		sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &bytesReceived, static_cast<LPOVERLAPPED>(_acceptEvent)))
	{
		const int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			// TODO : LOG
		}
	}

	_state = SessionState::ACCEPT_IN_PROGRESS;
}

void Session::RegisterRecv()
{
}

void Session::RegisterSend()
{
}

void Session::RegisterDisconnet()
{
}

void Session::ProcessAccept()
{

	// 새로운 소켓을 IOCP에 등록
	
}

void Session::ProcessRecv(int32 numOfBytes)
{
}

void Session::ProcessSend(int32 numOfBytes)
{
}

void Session::ProcessDisconnet()
{
}
#pragma endregion
