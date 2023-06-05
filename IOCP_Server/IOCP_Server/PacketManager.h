#pragma once
#include "Packet.h"
#include "UserManager.h"
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
	~PacketManager() = default;

	void	Init(const UINT32 maxClientCount, const std::function<void(UINT32, UINT16, char*)> sendPacketFunc, const std::function<void(UINT32)> disconnectFunc);
	void	Run(const UINT32 maxDBThreadCount);
	void	End();

	void	PushSystemPacket(PacketInfo systemPacket);
	void	PushPacket(UINT32 sessionId, UINT32 packetSize, char* packet);

private:
	PacketInfo	PopPacket();
	void		ProcessPacket();

	void			PushChatLog(UINT32 sessionId, const char* nickname, const char* chatMsg);
	DB_CHATLOG_INFO	PopChatLog();
	void			ProcessDB();

	void			ProcessPing();

#pragma region HANDLER FUNCTION
	void	ProcessUserConnect(UINT32 sessionId, UINT16 packetSize, char* packet);
	void	ProcessUserDisconnect(UINT32 sessionId, UINT16 packetSize, char* packet);

	void	ProcessChat(UINT32 sessionId, UINT16 packetSize, char* packet);
	void	ProcessPong(UINT32 sessionId, UINT16 packetSize, char* packet);
#pragma endregion

private:
	std::unordered_map<UINT16, PacketFunction>	_packetFuncDict;
	std::function<void(UINT32, UINT16, char*)>	_sendPacketFunc;
	std::function<void(UINT32)>					_disconnectFunc;
	std::unique_ptr<UserManager>				_userManager;
	std::unique_ptr<DBConnectionPool>			_dbConnectionPool;

	bool				_isProcessThread = false;
	std::thread			_packetThread;
	std::thread			_dbThread;
	std::thread			_pingThread;
	
	std::mutex					_packetLock, _chatLogLock;
	std::queue<UINT32>			_packetSessionIdQueue;
	std::queue<DB_CHATLOG_INFO>	_chatLogQueue;
};