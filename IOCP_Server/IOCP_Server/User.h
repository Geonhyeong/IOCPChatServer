#pragma once
#include "Packet.h"

#include <string>

constexpr UINT32 USER_BUFFER_SIZE = 8096;
enum class USER_DOMAIN_STATE
{
	NONE = 0,
	DISCONNECT = 1,		// TODO : 추후에 연결이 끊겻다가 재접속하면 연결해주는 것 고려...!
	CONNECT = 2,
	LOGIN = 3,
	ROOM = 4,
};

class User
{
public:
	User() = default;
	~User() = default;

	void				Init(UINT32 sessionId);
	void				Clear();
	void				Connect();
	void				Login(const char* userId);
	void				Logout();
	void				EnterRoom(UINT32 roomNumber);
	void				LeaveRoom();

	UINT32				GetSessionId() { return _sessionId; }
	char*				GetUserId() { return _userId; }
	UINT32				GetCurrentRoom() { return _roomNumber; }
	USER_DOMAIN_STATE	GetCurrentDomainState() { return _curDomainState; }

	void				PushPacket(UINT32 packetSize, char* packet);
	PacketInfo			PopPacket();

private:
	UINT32				_sessionId = 0;
	char				_userId[MAX_ID_PWD_BYTE_LENGTH] = { 0, };
	UINT32				_roomNumber = 0;
	USER_DOMAIN_STATE	_curDomainState = USER_DOMAIN_STATE::NONE;

	char*		_ringBuffer = nullptr;		// 패킷을 담을 링버퍼
	UINT32		_writePos = 0;
	UINT32		_readPos = 0;
};