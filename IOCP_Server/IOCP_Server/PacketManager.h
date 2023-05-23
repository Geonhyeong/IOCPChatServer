#pragma once
#include "Packet.h"
#include "UserManager.h"

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

	void	Init(const UINT32 maxClientCount, const std::function<void(UINT32, UINT16, char*)> sendPacketFunc);
	void	Run();
	void	End();

	void	PushSystemPacket(PacketInfo systemPacket);
	void	PushPacket(UINT32 sessionId, UINT32 packetSize, char* packet);

private:
	PacketInfo	PopPacket();
	void		ProcessPacket();

#pragma region HANDLER FUNCTION
	void	ProcessUserConnect(UINT32 sessionId, UINT16 packetSize, char* packet);
	void	ProcessUserDisconnect(UINT32 sessionId, UINT16 packetSize, char* packet);

	void	ProcessChat(UINT32 sessionId, UINT16 packetSize, char* packet);
#pragma endregion

private:
	std::unordered_map<UINT16, PacketFunction>	_packetFuncDict;
	std::function<void(UINT32, UINT16, char*)>	_sendPacketFunc;
	std::unique_ptr<UserManager>				_userManager;

	bool				_isProcessThread = false;
	std::thread			_processThread;
	
	std::mutex			_lock;
	std::queue<UINT32>	_packetSessionIdQueue;
};