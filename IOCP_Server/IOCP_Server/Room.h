#pragma once
#include "ErrorCode.h"
#include "User.h"

#include <functional>
#include <vector>

class Room
{
public:
	Room() = default;
	~Room() = default;

	void Init(const UINT32 roomNumber, const UINT32 maxRoomUserCount)
	{
		_roomNumber = roomNumber;
		_maxRoomUserCount = maxRoomUserCount;
	}

	UINT16 EnterUser(User* user)
	{
		if (_users.size() >= _maxRoomUserCount)
			return (UINT16)ERROR_CODE::ROOM_USER_EXCEED;

		if (user->GetCurrentDomainState() != USER_DOMAIN_STATE::LOGIN)
			return (UINT16)ERROR_CODE::INVALID_PACKET;

		user->EnterRoom(_roomNumber);
		_users.push_back(user);

		printf("Room(%d) : User(%d) 입장. 현재 인원 : %d\n", _roomNumber, user->GetSessionId(), _users.size());

		{
			ROOM_USER_LIST_PACKET userListPacket;
			userListPacket.packetSize = sizeof(ROOM_USER_LIST_PACKET);
			userListPacket.packetId = (UINT16)PACKET_ID::ROOM_USER_LIST;
			userListPacket.type = 0;
			userListPacket.userCount = GetCurrentUserCount();
			memcpy(userListPacket.userList, GetUserList(), MAX_USER_LIST_BYTE_LENGTH);

			Broadcast(user->GetSessionId(), userListPacket.packetSize, (char*)&userListPacket, false);
		}

		return (UINT16)ERROR_CODE::ROOM_ENTER_SUCCESS;
	}

	UINT16 LeaveUser(User* user)
	{
		auto it = find(_users.begin(), _users.end(), user);
		if (it == _users.end())
			return (UINT16)ERROR_CODE::ROOM_NONEXIST_USER;

		if (user->GetCurrentDomainState() != USER_DOMAIN_STATE::ROOM)
			return (UINT16)ERROR_CODE::INVALID_PACKET;

		user->LeaveRoom();
		_users.erase(it);

		printf("Room(%d) : User(%d) 퇴장. 현재 인원 : %d\n", _roomNumber, user->GetSessionId(), _users.size());

		{
			ROOM_USER_LIST_PACKET userListPacket;
			userListPacket.packetSize = sizeof(ROOM_USER_LIST_PACKET);
			userListPacket.packetId = (UINT16)PACKET_ID::ROOM_USER_LIST;
			userListPacket.type = 0;
			userListPacket.userCount = GetCurrentUserCount();
			memcpy(userListPacket.userList, GetUserList(), MAX_USER_LIST_BYTE_LENGTH);

			Broadcast(user->GetSessionId(), userListPacket.packetSize, (char*)&userListPacket, true);
		}
		
		return (UINT16)ERROR_CODE::ROOM_LEAVE_SUCCESS;
	}

	void Broadcast(const UINT32 senderId, const UINT16 packetSize, char* packet, bool exceptMe)
	{
		for (auto& user : _users)
		{
			if (user == nullptr)
				continue;

			if (exceptMe && user->GetSessionId() == senderId)
				continue;

			SendPacketFunc(user->GetSessionId(), packetSize, packet);
		}
	}

	char* GetUserList()
	{
		char* userList = new char[MAX_USER_LIST_BYTE_LENGTH];
		UINT32 writePos = 0;

		for (auto& user : _users)
		{
			if (user == nullptr)
				continue;

			CopyMemory(&userList[writePos], user->GetUserId(), MAX_ID_PWD_BYTE_LENGTH);
			writePos += MAX_ID_PWD_BYTE_LENGTH;
		}

		return userList;
	}

	UINT32 GetCurrentUserCount() { return (UINT32)_users.size(); }

public:
	std::function<void(UINT32, UINT16, char*)>	SendPacketFunc;

private:
	UINT32				_roomNumber = 0;
	UINT32				_maxRoomUserCount = 0;

	std::vector<User*>	_users;
};

