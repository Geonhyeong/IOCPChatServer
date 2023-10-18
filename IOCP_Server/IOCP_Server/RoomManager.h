#pragma once
#include "Room.h"

class RoomManager
{
private:
	RoomManager();
	~RoomManager();

public:
	static RoomManager& GetInstance();

	void Init(const int32 beginRoomCount, const int32 maxRoomCount, const int32 maxRoomUserCount);
	shared_ptr<Room> GetRoomByNumber(const int32 number);

private:
	int32				_beginRoomCount = 0;
	int32				_maxRoomCount = 0;
	
	std::vector<shared_ptr<Room>>	_rooms;
};
