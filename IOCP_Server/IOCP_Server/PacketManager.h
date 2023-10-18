#pragma once
#include "Packet.h"
#include "DBInfo.h"

using PacketFunction = function<void(int32, uint16, BYTE*)>;

class PacketManager
{
private:
	PacketManager();
	~PacketManager();

public:
	static PacketManager& GetInstance();

	void	Run(const uint32 maxDBThreadCount, const int32 maxRoomCount, const int32 maxRoomUserCount, const int32 beginRoomCount);
	void	End();

	void	Push(int32 sessionId);

private:
	void			Init();

	PacketInfo		Pop();
	void			PushChatLog(int32 sessionId, int32 roomNumber, const char* userId, const char* chatMsg);
	DB_CHATLOG_INFO	PopChatLog();

	void			ProcessPacket();
	void			ProcessDB();

#pragma region HANDLER FUNCTION
	void	ProcessLogin(int32 sessionId, uint16 packetSize, BYTE* packet);
	void	ProcessLogout(int32 sessionId, uint16 packetSize, BYTE* packet);
	void	ProcessRoomEnter(int32 sessionId, uint16 packetSize, BYTE* packet);
	void	ProcessRoomLeave(int32 sessionId, uint16 packetSize, BYTE* packet);
	void	ProcessChat(int32 sessionId, uint16 packetSize, BYTE* packet);
#pragma endregion

private:
	unordered_map<uint16, PacketFunction>	_packetFuncDict;

	bool					_isProcessThread = false;
	
	USE_LOCK;
	queue<int32>			_packetSessionIdQueue;
	queue<DB_CHATLOG_INFO>	_chatLogQueue;
};