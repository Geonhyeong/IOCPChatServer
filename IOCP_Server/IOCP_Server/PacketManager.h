#pragma once
#include "Packet.h"
#include "RoomManager.h"
#include "UserManager.h"
#include "RedisManager.h"
#include "DBConnectionPool.h"
#include "DBInfo.h"

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
	~PacketManager();

	void	Init(const UINT32 maxClientCount);
	void	Run(const UINT32 maxDBThreadCount);
	void	End();

	void	PushSystemPacket(PacketInfo systemPacket);
	void	PushPacket(UINT32 sessionId, UINT32 packetSize, char* packet);

private:
	PacketInfo	PopPacket();
	void		ProcessPacket();

	void			PushChatLog(UINT32 sessionId, UINT32 roomNumber, const char* userId, const char* chatMsg);
	DB_CHATLOG_INFO	PopChatLog();
	void			ProcessDB();

#pragma region HANDLER FUNCTION
	void	ProcessUserConnect(UINT32 sessionId, UINT16 packetSize, char* packet);
	void	ProcessUserDisconnect(UINT32 sessionId, UINT16 packetSize, char* packet);

	void	ProcessLogin(UINT32 sessionId, UINT16 packetSize, char* packet);
	void	ProcessLogout(UINT32 sessionId, UINT16 packetSize, char* packet);
	void	ProcessRoomEnter(UINT32 sessionId, UINT16 packetSize, char* packet);
	void	ProcessRoomLeave(UINT32 sessionId, UINT16 packetSize, char* packet);
	void	ProcessChat(UINT32 sessionId, UINT16 packetSize, char* packet);
#pragma endregion

public:
	std::function<void(UINT32, UINT16, char*)>	SendPacketFunc;

private:
	std::unordered_map<UINT16, PacketFunction>	_packetFuncDict;
	std::unique_ptr<RoomManager>				_roomManager;
	std::unique_ptr<UserManager>				_userManager;
	std::unique_ptr<RedisManager>				_redisManager;
	std::unique_ptr<DBConnectionPool>			_dbConnectionPool;

	bool				_isProcessThread = false;
	std::thread			_packetThread;
	std::thread			_dbThread;
	
	std::mutex					_packetLock, _chatLogLock;
	std::queue<UINT32>			_packetSessionIdQueue;
	std::queue<DB_CHATLOG_INFO>	_chatLogQueue;
};