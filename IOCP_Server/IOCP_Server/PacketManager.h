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

	void	Init(const UINT32 maxClientCount, std::function<void(UINT32, UINT16, char*)> sendPacketFunc);
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

	void	ProcessLogin(UINT32 sessionId, UINT16 packetSize, char* packet);
	void	ProcessEcho(UINT32 sessionId, UINT16 packetSize, char* packet);
#pragma endregion

private:
	std::unordered_map<UINT16, PacketFunction>	_packetFuncDict;
	std::unique_ptr<UserManager>				_userManager;
	std::function<void(UINT32, UINT16, char*)>	_sendPacketFunc;

	bool				_isProcessThread = false;
	std::thread			_processThread;
	
	std::mutex			_lock;
	std::queue<UINT32>	_packetSessionIdQueue;
};