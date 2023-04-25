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
		_users.assign(maxUserCount, new User());

		for (UINT32 i = 0; i < maxUserCount; i++)
		{
			_users[i]->Init(i);
		}
	}

	User* GetUserBySessionId(const UINT32 sessionId)
	{
		return _users[sessionId];
	}

private:
	UINT32				_userCount = 0;
	
	std::vector<User*>	_users;
	std::unordered_map<std::string, int> _userIdDict;
};