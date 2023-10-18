#pragma once
#include "Packet.h"

constexpr int32 USER_BUFFER_SIZE = 8096;
enum class USER_DOMAIN_STATE
{
	NONE = 0,
	DISCONNECT = 1,		// TODO : 추후에 연결이 끊겻다가 재접속하면 연결해주는 것 고려...!
	CONNECT = 2,
	LOGIN = 3,
	ROOM = 4,
};

class User : public Session
{
public:
	User();
	~User();

	void				SendPacket(uint16 size, BYTE* packet);

	uint16				Login(const char* nickname);
	void				Logout();
	void				EnterRoom(UINT32 roomNumber);
	void				LeaveRoom();

	int32				GetSessionId() { return _sessionId; }
	char*				GetNickname() { return _nickname; }
	UINT32				GetCurrentRoom() { return _roomNumber; }
	USER_DOMAIN_STATE	GetCurrentDomainState() { return _curDomainState; }

	PacketInfo			PopPacket();

protected:
	virtual void OnConnected() override;
	virtual int32 OnRecv(BYTE* buffer, int32 len) override;
	virtual void OnSend(int32 len) override;
	virtual void OnDisconnected() override;

private:
	void				PushPacket(UINT32 packetSize, BYTE* packet);

private:
	int32				_sessionId = 0;
	char				_nickname[MAX_ID_PWD_BYTE_LENGTH] = { 0, };
	int32				_roomNumber = 0;
	USER_DOMAIN_STATE	_curDomainState = USER_DOMAIN_STATE::NONE;

	USE_LOCK;
	RingBuffer	_packetQueue;
};