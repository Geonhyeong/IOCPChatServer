#include "pch.h"
#include "Room.h"
#include "User.h"

Room::Room(const int32 roomNumber, const int32 maxRoomUserCount)
	: _roomNumber(roomNumber), _maxRoomUserCount(maxRoomUserCount)
{
}

Room::~Room()
{
}

uint16 Room::EnterUser(shared_ptr<User> user)
{
	if (_users.size() >= _maxRoomUserCount)
		return (uint16)ERROR_CODE::ROOM_USER_EXCEED;

	if (user->GetCurrentDomainState() != USER_DOMAIN_STATE::LOGIN)
		return (uint16)ERROR_CODE::INVALID_PACKET;

	user->EnterRoom(_roomNumber);
	_users.push_back(user);

	printf("Room(%d) : User(%d) 입장. 현재 인원 : %d\n", _roomNumber, user->GetSessionId(), (int32)_users.size());

	{
		ROOM_USER_LIST_PACKET userListPacket;
		userListPacket.packetSize = sizeof(ROOM_USER_LIST_PACKET);
		userListPacket.packetId = (uint16)PACKET_ID::ROOM_USER_LIST;
		userListPacket.type = 0;
		userListPacket.userCount = GetCurrentUserCount();
		GetUserList(userListPacket.userList);

		Broadcast(user->GetSessionId(), userListPacket.packetSize, (BYTE*)&userListPacket, false);
	}

	return (uint16)ERROR_CODE::ROOM_ENTER_SUCCESS;
}

uint16 Room::LeaveUser(shared_ptr<User> user)
{
	auto it = find(_users.begin(), _users.end(), user);
	if (it == _users.end())
		return (uint16)ERROR_CODE::ROOM_NONEXIST_USER;

	if (user->GetCurrentDomainState() != USER_DOMAIN_STATE::ROOM)
		return (uint16)ERROR_CODE::INVALID_PACKET;

	user->LeaveRoom();
	_users.erase(it);

	printf("Room(%d) : User(%d) 퇴장. 현재 인원 : %d\n", _roomNumber, user->GetSessionId(), (int32)_users.size());

	{
		ROOM_USER_LIST_PACKET userListPacket;
		userListPacket.packetSize = sizeof(ROOM_USER_LIST_PACKET);
		userListPacket.packetId = (uint16)PACKET_ID::ROOM_USER_LIST;
		userListPacket.type = 0;
		userListPacket.userCount = GetCurrentUserCount();
		GetUserList(userListPacket.userList);

		Broadcast(user->GetSessionId(), userListPacket.packetSize, (BYTE*)&userListPacket, true);
	}

	return (uint16)ERROR_CODE::ROOM_LEAVE_SUCCESS;
}

void Room::Broadcast(const int32 senderId, const uint16 packetSize, BYTE* packet, bool exceptMe)
{
	for (auto& user : _users)
	{
		if (user == nullptr)
			continue;

		if (exceptMe && user->GetSessionId() == senderId)
			continue;

		user->SendPacket(packetSize, packet);
	}
}

void Room::GetUserList(OUT char* userList)
{
	uint32 writePos = 0;

	for (auto& user : _users)
	{
		if (user == nullptr)
			continue;

		CopyMemory(&userList[writePos], user->GetNickname(), MAX_ID_PWD_BYTE_LENGTH);
		writePos += MAX_ID_PWD_BYTE_LENGTH;
	}
}