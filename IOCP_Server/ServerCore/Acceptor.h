#pragma once

class Acceptor
{
public:
	Acceptor();
	virtual ~Acceptor();

	bool Init(NetAddress address);
	bool BindAndListen(int32 backlog);

	SOCKET GetAcceptSocket() { return _acceptSocket; }

private:
	SOCKET	_acceptSocket;
	NetAddress _netAddress;
};