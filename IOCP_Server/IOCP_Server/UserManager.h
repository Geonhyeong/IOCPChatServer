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
		if (pUser->GetCurrentDomainState() <= USER_DOMAIN_STATE::DISCONNECT)
		{
			pUser->Clear();
			pUser->Connect();

			_userCount++;
		}
	}

	void DisconnectUser(const UINT32 sessionId)
	{
		auto pUser = _users[sessionId];
		if (pUser->GetCurrentDomainState() > USER_DOMAIN_STATE::DISCONNECT)
		{
			pUser->Clear();

			_userCount--;
		}
	}

	UINT16 LoginUser(const UINT32 sessionId, const char* userId)
	{
		auto pUser = _users[sessionId];
		if (pUser->GetCurrentDomainState() >= USER_DOMAIN_STATE::LOGIN)
			return (UINT16)ERROR_CODE::LOGIN_REDUNDANT_CONNECTION;
		
		if (pUser->GetCurrentDomainState() == USER_DOMAIN_STATE::CONNECT)
		{
			pUser->Login(userId);
			return (UINT16)ERROR_CODE::LOGIN_SUCCESS;
		}

		return (UINT16)ERROR_CODE::LOGIN_USED_ALL_OBJ;
	}

	void LogoutUser(const UINT32 sessionId)
	{
		auto pUser = _users[sessionId];
		if (pUser->GetCurrentDomainState() >= USER_DOMAIN_STATE::LOGIN)
		{
			pUser->Logout();
		}
	}

	User*	GetUserBySessionId(const UINT32 sessionId) { return _users[sessionId]; }
	UINT32	GetCurrentUserCount() { return _userCount; }
	UINT32	GetMaxUserCount() { return _maxUserCount; }

private:
	UINT32				_userCount = 0;
	UINT32				_maxUserCount = 0;
	
	std::vector<User*>	_users;
};