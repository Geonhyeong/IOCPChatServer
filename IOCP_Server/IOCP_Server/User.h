#pragma once
#include "Packet.h"

#include <string>

constexpr UINT32 USER_BUFFER_SIZE = 8096;

class User
{
public:
	void Init(UINT32 sessionId);

	void	PushPacket(UINT32 packetSize, char* packet);
	PacketInfo	PopPacket();

private:
	UINT32		_sessionId;
	std::string	_userId;		// login ID

	char*		_ringBuffer = nullptr;		// 패킷을 담을 링버퍼
	UINT32		_writePos = 0;
	UINT32		_readPos = 0;
};