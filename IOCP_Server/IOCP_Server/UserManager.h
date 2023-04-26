#pragma once
#include "User.h"

#include <vector>
#include <unordered_map>
#include <string>

class UserManager
{
public:
	UserManager() = default;
	~UserManager() = default;

	void Init(const UINT32 maxUserCount)
	{
		_maxUserCount = maxUserCount;
		_users.assign(maxUserCount, new User());

		for (UINT32 i = 0; i < maxUserCount; i++)
		{
			_users[i]->Init(i);
		}
	}

	void ConnectUser(const UINT32 sessionId)
	{
		auto pUser = _users[sessionId];
		if (pUser->curDomainState <= User::USER_DOMAIN_STATE::DISCONNECT)
		{
			pUser->Clear();
			pUser->curDomainState = User::USER_DOMAIN_STATE::CONNECT;

			_userCount++;
		}
	}

	void DisconnectUser(const UINT32 sessionId)
	{
		auto pUser = _users[sessionId];
		if (pUser->curDomainState > User::USER_DOMAIN_STATE::DISCONNECT)
		{
			_userIdDict.erase(pUser->GetUserId());
			pUser->Clear();

			_userCount--;
		}
	}

	void LoginUser(const UINT32 sessionId, std::string userId)
	{
		auto pUser = _users[sessionId];
		if (pUser->curDomainState == User::USER_DOMAIN_STATE::CONNECT)
		{
			_userIdDict.insert(std::make_pair(userId, sessionId));

			_users[sessionId]->Login(userId);
		}
	}

	bool IsAlreadyLogin(char* userId)
	{
		if (_userIdDict.find(userId) != _userIdDict.end())
			return true;

		return false;
	}

	User*	GetUserBySessionId(const UINT32 sessionId) { return _users[sessionId]; }
	UINT32	GetCurrentUserCount() { return _userCount; }
	UINT32	GetMaxUserCount() { return _maxUserCount; }

private:
	UINT32				_userCount = 0;
	UINT32				_maxUserCount = 0;
	
	std::vector<User*>	_users;
	std::unordered_map<std::string, int> _userIdDict;
};