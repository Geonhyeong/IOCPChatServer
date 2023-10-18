#include "pch.h"
#include "Acceptor.h"

Acceptor::Acceptor()
{
	_acceptSocket = INVALID_SOCKET;
	_netAddress = {};
}

Acceptor::~Acceptor()
{
	SocketUtils::CloseSocket(_acceptSocket);
}

bool Acceptor::Init(NetAddress address)
{
	_acceptSocket = SocketUtils::CreateSocket();
	if (_acceptSocket == INVALID_SOCKET)
		return false;

	if (SocketUtils::SetReuseAddress(_acceptSocket, true) == false)
		return false;

	if (SocketUtils::SetLinger(_acceptSocket, 0, 0) == false)
		return false;

	_netAddress = address;
	
	return true;
}

bool Acceptor::BindAndListen(int32 backlog)
{
	if (IocpCore::GetInstance().Register(_acceptSocket) == false)
		return false;

	if (SocketUtils::Bind(_acceptSocket, _netAddress) == false)
		return false;

	if (SocketUtils::Listen(_acceptSocket, backlog) == false)
		return false;

	return true;
}