#pragma once

enum class SessionState : uint8
{
	DISCONNECT,
	ACCEPT_IN_PROGRESS,
	CONNECT
};

struct PacketHeader
{
	uint16 size;
	uint16 id;
};

class Session : public enable_shared_from_this<Session>
{
public:
	Session();
	virtual ~Session();

	SOCKET			GetSocket() { return _socket; }
	NetAddress		GetNetAddress() { return _netAddress; }
	SessionState	GetSessionState() { return _state; }
	UINT64			GetLatestDisconnectedTimeSec() { return _latestDisconnectedTimeSec; }

	void Dispatch(IocpEvent* iocpEvent, int32 numOfBytes = 0);

	/* �ܺο��� ����ϴ� ��Ʈ��ũ �Լ� */
	void Accept(SOCKET acceptSocket);
	void Send(SendBufferRef sendBuffer);
	void Disconnect();

private:
	/* ���ο��� �۵��ϴ� ��Ʈ��ũ �Լ� */
	void RegisterAccept();
	void RegisterRecv();
	void RegisterSend();
	void RegisterDisconnet();

	void ProcessAccept();
	void ProcessRecv(int32 numOfBytes);
	void ProcessSend(int32 numOfBytes);
	void ProcessDisconnet();

	int32 ParsingPacket(BYTE* buffer, int32 len);

protected:
	/* ������ �ڵ忡�� ������ */
	virtual void OnConnected() {}
	virtual int32 OnRecv(BYTE* buffer, int32 len) { return len; }
	virtual void OnSend(int32 len) {}
	virtual void OnDisconnected() {}

private:
	SOCKET			_socket;
	SOCKET			_acceptSocket;
	NetAddress		_netAddress;
	SessionState	_state;
	uint64			_latestDisconnectedTimeSec;

	/* ���� ���� */
	mutex					_lock;
	RingBuffer				_recvBuffer;
	queue<SendBufferRef>	_sendQueue;
	atomic_bool				_sendRegistered;

	/* Overlapped ����ü ���� */
	AcceptEvent		_acceptEvent;
	RecvEvent		_recvEvent;
	SendEvent		_sendEvent;
	DisconnectEvent	_disconnectEvent;
};