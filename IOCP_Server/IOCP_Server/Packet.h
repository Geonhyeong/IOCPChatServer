#pragma once

//#define WIN32_LEAN_AND_MEAN
//#include <Windows.h>

// 클라이언트가 보낸 패킷을 저장하는 구조체
struct Packet
{
	UINT32 sessionId = -1;
	UINT32 packetSize = 0;
	char* packetData = nullptr;

	void Set(Packet& value)
	{
		sessionId = value.sessionId;
		packetSize = value.packetSize;

		packetData = new char[packetSize - 8];
		CopyMemory(packetData, value.packetData, packetSize - 8);
	}

	void Set(UINT32 sessionId_, UINT32 dataSize, char* data)
	{
		sessionId = sessionId_;
		packetSize = dataSize + 8;

		packetData = new char[dataSize];
		CopyMemory(packetData, data, dataSize);
	}

	void Release()
	{
		delete packetData;
	}
};