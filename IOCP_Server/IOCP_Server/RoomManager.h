#pragma once
#include "Room.h"

#include <functional>
#include <vector>

class RoomManager
{
public:
	RoomManager() = default;
	~RoomManager() = default;

	void Init(const UINT32 beginRoomCount, const UINT32 maxRoomCount, const UINT32 maxRoomUserCount)
	{
		_beginRoomCount = beginRoomCount;
		_maxRoomCount = maxRoomCount;
		_rooms.assign(maxRoomCount, NULL);

		for (UINT32 i = 0; i < maxRoomCount; i++)
		{
			_rooms[i] = new Room();
			_rooms[i]->SendPacketFunc = SendPacketFunc;
			_rooms[i]->Init(i + beginRoomCount, maxRoomUserCount);
		}
	}

	UINT16 EnterUser(User* user, const UINT32 roomNumber)
	{
		if (user == nullptr)
			return (UINT16)ERROR_CODE::INVALID_SESSION;

		Room* room = GetRoomByNumber(roomNumber);
		if (room == nullptr)
			return (UINT16)ERROR_CODE::ROOM_UNKNOWN_NUMBER;

		return room->EnterUser(user);
	}

	UINT16 LeaveUser(User* user, const UINT32 roomNumber)
	{
		if (user == nullptr)
			return (UINT16)ERROR_CODE::INVALID_SESSION;

		Room* room = GetRoomByNumber(roomNumber);
		if (room == nullptr)
			return (UINT16)ERROR_CODE::ROOM_UNKNOWN_NUMBER;

		return room->LeaveUser(user);
	}

	Room* GetRoomByNumber(const UINT32 number)
	{
		if (number < _beginRoomCount || number >= _beginRoomCount + _maxRoomCount)
			return nullptr;

		return _rooms[number - _beginRoomCount];
	}

public:
	std::function<void(UINT32, UINT16, char*)>	SendPacketFunc;

private:
	UINT32				_beginRoomCount = 0;
	UINT32				_maxRoomCount = 0;
	
	std::vector<Room*>	_rooms;
};
