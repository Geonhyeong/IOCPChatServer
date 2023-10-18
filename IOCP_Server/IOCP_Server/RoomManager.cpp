#include "pch.h"
#include "RoomManager.h"

RoomManager::RoomManager()
{
	SLog(L"RoomManager Singleton Instance Generated.");
}

RoomManager::~RoomManager()
{
	_rooms.clear();
}

RoomManager& RoomManager::GetInstance()
{
	static RoomManager* instance = new RoomManager();
	return *instance;
}

void RoomManager::Init(const int32 beginRoomCount, const int32 maxRoomCount, const int32 maxRoomUserCount)
{
	_beginRoomCount = beginRoomCount;
	_maxRoomCount = maxRoomCount;
	_rooms.assign(maxRoomCount, NULL);

	for (int32 i = 0; i < maxRoomCount; i++)
	{
		_rooms[i] = make_shared<Room>(i + beginRoomCount, maxRoomUserCount);
	}
}

shared_ptr<Room> RoomManager::GetRoomByNumber(const int32 number)
{
	if (number < _beginRoomCount || number >= _beginRoomCount + _maxRoomCount)
		return nullptr;

	return _rooms[number - _beginRoomCount];
}
