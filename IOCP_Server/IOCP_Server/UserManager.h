#pragma once
#include "User.h"

#include <vector>
#include <unordered_map>
#include <string>
#include <functional>

class UserManager
{
public:
	UserManager() = default;
	~UserManager() = default;

	void Init(const UINT32 maxUserCount)
	{
		_maxUserCount = maxUserCount;
		_users.assign(maxUserCount, NULL);

		for (UINT32 i = 0; i < maxUserCount; i++)
		{
			_users[i] = new User();
			_users[i]->Init(i);
		}
	}

	void ConnectUser(const UINT32 sessionId)
	{
		auto pUser = _users[sessionId];
		if (pUser->GetCurrentDomainState() <= User::USER_DOMAIN_STATE::DISCONNECT)
		{
			pUser->Clear();
			pUser->Connect();

			_userCount++;
		}
	}

	void DisconnectUser(const UINT32 sessionId)
	{
		auto pUser = _users[sessionId];
		if (pUser->GetCurrentDomainState() > User::USER_DOMAIN_STATE::DISCONNECT)
		{
			pUser->Clear();

			_userCount--;
		}
	}

	void BroadcastToConnectingUser(UINT32 senderId, UINT16 packetSize, char* packet)
	{
		// 현재로서는 packetManager도 싱글 쓰레드로 동작하고 userManager도 unique_ptr로 인해 싱글쓰레드로 동작한다.
		int broadcastCnt = 0;
		for (UINT32 i = 0; i < _maxUserCount; i++)
		{
			if (i == senderId)
				continue;

			auto pUser = _users[i];
			if (pUser->GetCurrentDomainState() == User::USER_DOMAIN_STATE::CONNECT)
			{
				SendPacketFunc(i, packetSize, packet);
				broadcastCnt++;
			}
		}

		printf("Broacast to %d Users...", broadcastCnt);
	}

	User*	GetUserBySessionId(const UINT32 sessionId) { return _users[sessionId]; }
	UINT32	GetCurrentUserCount() { return _userCount; }
	UINT32	GetMaxUserCount() { return _maxUserCount; }

private:
	UINT32				_userCount = 0;
	UINT32				_maxUserCount = 0;
	
	std::vector<User*>	_users;

public:
	std::function<void(UINT32, UINT16, char*)>	SendPacketFunc;
};