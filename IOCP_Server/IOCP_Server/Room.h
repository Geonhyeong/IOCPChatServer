#pragma once
#include "ErrorCode.h"

class User;
class Room
{
public:
	Room(const int32 roomNumber, const int32 maxRoomUserCount);
	~Room();

	uint16 EnterUser(shared_ptr<User> user);
	uint16 LeaveUser(shared_ptr<User> user);
	void Broadcast(const int32 senderId, const uint16 packetSize, BYTE* packet, bool exceptMe);
	void GetUserList(OUT char* userList);
	uint32 GetCurrentUserCount() { return (uint32)_users.size(); }

private:
	int32 _roomNumber = 0;
	int32 _maxRoomUserCount = 0;
	std::vector<shared_ptr<User>> _users;
};