#pragma once
#include "IOCP.h"

class ChatServer : public IOCP
{
public:
	virtual void OnConnected(const UINT32 sessionId) override
	{
		printf("技记 (%d) 立加 己傍\n", sessionId);
	}

	virtual void OnDisconnected(const UINT32 sessionId) override
	{
		printf("技记 (%d) 立加 谗辫\n", sessionId);
	}

	virtual void OnRecv(const UINT32 sessionId, char* buf, const UINT32 len) override
	{
		buf[len] = '\0';
		printf("[荐脚] 技记 : %d, bytes : %d, msg : %s\n", sessionId, len, buf);
	}

	virtual void OnSend(const UINT32 sessionId, char* buf, const UINT32 len) override
	{
		printf("[价脚] 技记 : %d, bytes : %d, msg : %s\n", sessionId, len, buf);
	}
};