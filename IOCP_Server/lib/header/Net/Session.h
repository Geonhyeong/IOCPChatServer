#pragma once

enum class SessionState : uint8
{
	DISCONNECT,
	ACCEPT_IN_PROGRESS,
	CONNECT
};

class Session : public enable_shared_from_this<Session>
{
public:
	Session(uint32 maxBufferSize = 65536);
	virtual ~Session();

	SOCKET			GetSocket() { return _socket; }
	NetAddress		GetNetAddress() { return _netAddress; }
	SessionState	GetSessionState() { return _state; }
	UINT64			GetLatestDisconnectedTimeSec() { return _latestDisconnectedTimeSec; }

	void Dispatch(IocpEvent* iocpEvent, int32 numOfBytes = 0);

	/* �ܺο��� ����ϴ� ��Ʈ��ũ �Լ� */
	void Accept(SOCKET acceptSocket);
	void Send();
	void Disconnect();

private:
	/* ���ο��� �۵��ϴ� ��Ʈ��ũ �Լ� */
	void RegisterAccept(SOCKET acceptSocket);
	void RegisterRecv();
	void RegisterSend();
	void RegisterDisconnet();

	void ProcessAccept();
	void ProcessRecv(int32 numOfBytes);
	void ProcessSend(int32 numOfBytes);
	void ProcessDisconnet();

private:
	SOCKET			_socket;
	NetAddress		_netAddress;
	SessionState	_state;
	uint64			_latestDisconnectedTimeSec;

	RingBuffer		_recvBuffer;

	/* Overlapped ����ü ���� */
	AcceptEvent*		_acceptEvent;
	RecvEvent*			_recvEvent;
	SendEvent*			_sendEvent;
	DisconnectEvent*	_disconnectEvent;
};