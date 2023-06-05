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
	};

public:
	void				Init(UINT32 sessionId);
	void				Clear();
	void				Connect();

	USER_DOMAIN_STATE	GetCurrentDomainState() { return _curDomainState; }

	void				PushPacket(UINT32 packetSize, char* packet);
	PacketInfo			PopPacket();

	UINT16				AddPingCount() { return ++_pingCount; }
	void				ResetPingCount() { _pingCount = 0; }

private:
	UINT32				_sessionId;
	USER_DOMAIN_STATE	_curDomainState = USER_DOMAIN_STATE::NONE;
	UINT16				_pingCount = 0;

	char*		_ringBuffer = nullptr;		// ��Ŷ�� ���� ������
	UINT32		_writePos = 0;
	UINT32		_readPos = 0;
};