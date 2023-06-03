#pragma once
#include "Packet.h"

#include <string>

constexpr UINT32 USER_BUFFER_SIZE = 8096;

class User
{
public:
	enum class USER_DOMAIN_STATE
	{
		NONE = 0,
		DISCONNECT = 1,		// TODO : ���Ŀ� ������ ����ٰ� �������ϸ� �������ִ� �� ���...!
		CONNECT = 2,
		LOGIN = 11,
		ROOM = 20,
	};

public:
	void Init(UINT32 sessionId);
	void Clear();

	std::string			GetUserId() { return _userId; }

	void	PushPacket(UINT32 packetSize, char* packet);
	PacketInfo	PopPacket();

public:
	USER_DOMAIN_STATE	curDomainState = USER_DOMAIN_STATE::NONE;

private:
	UINT32				_sessionId;
	std::string			_userId;			// login ID

	char*		_ringBuffer = nullptr;		// ��Ŷ�� ���� ������
	UINT32		_writePos = 0;
	UINT32		_readPos = 0;
};