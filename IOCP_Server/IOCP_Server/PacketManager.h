#pragma once
#include "Packet.h"

#include <functional>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <queue>

using PacketFunction = std::function<void(UINT32, UINT16, char*)>;

class PacketManager
{
public:
	PacketManager() = default;
	~PacketManager() = default;

	void	Init(const UINT32 maxClientCount);
	void	Run();
	void	End();

	void	PushPacket(Packet packet);
	void	PushPacket(UINT32 userId, UINT32 packetSize, char* packet);

private:
	Packet	PopPacket();
	void	ProcessPacket();

private:
	std::unordered_map<UINT16, PacketFunction> _packetFuncDict;

	bool				_isProcessThread = false;
	std::thread			_processThread;
	
	std::mutex			_lock;
	std::queue<UINT32>	_packetUserIdQueue;
};