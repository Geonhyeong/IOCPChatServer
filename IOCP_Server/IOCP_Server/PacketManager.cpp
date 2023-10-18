#include "pch.h"
#include "PacketManager.h"
#include "User.h"
#include <nlohmann/json.hpp>

PacketManager::PacketManager()
{
	SLog(L"PacketManager Singleton Instance Generated.");
	Init();
}

PacketManager::~PacketManager()
{
	_packetFuncDict.clear();
}

PacketManager& PacketManager::GetInstance()
{
	static PacketManager* instance = new PacketManager();
	return *instance;
}

void PacketManager::Init()
{
	// 함수자 할당
	_packetFuncDict = unordered_map<uint16, PacketFunction>();

	_packetFuncDict[(uint16)PACKET_ID::LOGIN_REQ] = [this](int32 sessionId, uint16 packetSize, BYTE* packet) { return ProcessLogin(sessionId, packetSize, packet); };
	_packetFuncDict[(uint16)PACKET_ID::LOGOUT_REQ] = [this](int32 sessionId, uint16 packetSize, BYTE* packet) { return ProcessLogout(sessionId, packetSize, packet); };
	_packetFuncDict[(uint16)PACKET_ID::ROOM_ENTER_REQ] = [this](int32 sessionId, uint16 packetSize, BYTE* packet) { return ProcessRoomEnter(sessionId, packetSize, packet); };
	_packetFuncDict[(uint16)PACKET_ID::ROOM_LEAVE_REQ] = [this](int32 sessionId, uint16 packetSize, BYTE* packet) { return ProcessRoomLeave(sessionId, packetSize, packet); };
	_packetFuncDict[(uint16)PACKET_ID::CHAT_REQ] = [this](int32 sessionId, uint16 packetSize, BYTE* packet) { return ProcessChat(sessionId, packetSize, packet); };
}

void PacketManager::Run(const uint32 maxDBThreadCount, const int32 maxRoomCount, const int32 maxRoomUserCount, const int32 beginRoomCount = 1)
{
	ASSERT_CRASH(DB.Connect(maxDBThreadCount, DB_CONNECTION_STRING));
#ifdef _DEBUG
	if (bool DROP_AND_CREATE_TABLE = false)
	{
		DBConnection* dbConnection = DB.Pop();
		ASSERT_CRASH(dbConnection->Execute(DB_CREATE_TABLE_QUERY));
		DB.Push(dbConnection);
	}
	if (bool TRUNCATE_TABLE = true)
	{
		DBConnection* dbConnection = DB.Pop();
		ASSERT_CRASH(dbConnection->Execute(DB_TRUNCATE_TABLE_QUERY));
		DB.Push(dbConnection);
	}
#endif
	// RoomManager 초기화
	ROOMS.Init(beginRoomCount, maxRoomCount, maxRoomUserCount);

	_isProcessThread = true;
	ThreadManager::GetInstance().Run([this]() { ProcessPacket(); }, L"LogicThread");

	for (uint32 i = 1; i <= maxDBThreadCount; i++)
		ThreadManager::GetInstance().Run([this]() { ProcessDB(); }, L"DBThread" + to_wstring(i));
}

void PacketManager::End()
{
	_isProcessThread = false;
}

void PacketManager::Push(int32 sessionId)
{
	WRITE_LOCK;
	_packetSessionIdQueue.push(sessionId);
}

PacketInfo PacketManager::Pop()
{
	int32 sessionId = 0;
	{
		WRITE_LOCK;
		if (_packetSessionIdQueue.empty())
			return PacketInfo();

		sessionId = _packetSessionIdQueue.front();
		_packetSessionIdQueue.pop();
	}

	// SessionId를 이용해서 User 객체를 받아온 후, User의 패킷 버퍼에 쌓인 패킷을 가져온다.
	auto pUser = USERS.GetUserRefById(sessionId);
	return pUser->PopPacket();
}

void PacketManager::PushChatLog(int32 sessionId, int32 roomNumber, const char* userId, const char* chatMsg)
{
	// TODO : memory leak problem
	DB_CHATLOG_INFO chatLog;
	chatLog.sessionId = sessionId;
	memcpy(chatLog.userId, userId, MAX_ID_PWD_BYTE_LENGTH);
	memcpy(chatLog.chatMsg, chatMsg, MAX_CHAT_MSG_SIZE);
	chatLog.roomNumber = roomNumber;
	chatLog.dateTime = time(NULL);

	WRITE_LOCK;
	_chatLogQueue.push(chatLog);
}

DB_CHATLOG_INFO PacketManager::PopChatLog()
{
	DB_CHATLOG_INFO chatLog;
	{
		WRITE_LOCK;
		if (_chatLogQueue.empty())
			return DB_CHATLOG_INFO();

		chatLog = _chatLogQueue.front();
		_chatLogQueue.pop();
	}

	return chatLog;
}

void PacketManager::ProcessPacket()
{
	while (_isProcessThread)
	{
		bool isIdle = true;

		PacketInfo packet = Pop();
		if (packet.packetId != 0)
		{
			isIdle = false;
			_packetFuncDict[packet.packetId](packet.sessionId, packet.dataSize, packet.packetData);
		}

		if (isIdle)
			this_thread::sleep_for(chrono::milliseconds(1));
	}
}

void PacketManager::ProcessDB()
{
	DBConnection* dbConnection = DB.Pop();
	
	while (_isProcessThread)
	{
		bool isIdle = true;

		DB_CHATLOG_INFO chatLog = PopChatLog();
		if (chatLog.dateTime != 0)
		{
			isIdle = false;

			SQLLEN len = 0;
			SQLLEN len2 = 0;
			SQLLEN len3 = 0;
			SQLLEN len4 = 0;

			TIMESTAMP_STRUCT ts = {};
			struct tm dateTime;
			localtime_s(&dateTime, &chatLog.dateTime);
			ts.year = dateTime.tm_year + 1900;
			ts.month = dateTime.tm_mon + 1;
			ts.day = dateTime.tm_mday;
			ts.hour = dateTime.tm_hour;
			ts.minute = dateTime.tm_min;
			ts.second = dateTime.tm_sec;
			SQLLEN len5 = 0;

			ASSERT_CRASH(dbConnection->BindParam(1, &chatLog.sessionId, &len));
			ASSERT_CRASH(dbConnection->BindParam(2, chatLog.userId, &len2));
			ASSERT_CRASH(dbConnection->BindParam(3, chatLog.chatMsg, &len3));
			ASSERT_CRASH(dbConnection->BindParam(4, &chatLog.roomNumber, &len4));
			ASSERT_CRASH(dbConnection->BindParam(5, &ts, &len5));

			ASSERT_CRASH(dbConnection->Execute(L"INSERT INTO [dbo].[ChatLog]([SessionId], [UserId], [ChatMsg], [RoomNumber], [DateTime]) VALUES(?, ?, ?, ?, ?)"));
		}

		if (isIdle)
			this_thread::sleep_for(chrono::milliseconds(1));
	}
	
	DB.Push(dbConnection);
}

#pragma region HANDLER FUNCTION
void PacketManager::ProcessLogin(int32 sessionId, uint16 packetSize, BYTE* packet)
{
	LOGIN_REQ_PACKET* loginReqPacket = reinterpret_cast<LOGIN_REQ_PACKET*>(packet);
	auto user = USERS.GetUserRefById(sessionId);

	if (packetSize != loginReqPacket->packetSize)
		return;

	LOGIN_RES_PACKET resPacket;
	resPacket.packetSize = sizeof(LOGIN_RES_PACKET);
	resPacket.packetId = (uint16)PACKET_ID::LOGIN_RES;
	resPacket.type = 0;
	resPacket.result = (uint16)ERROR_CODE::LOGIN_SUCCESS;

	// Validation 체크 (RedisDB에 token 검증)
	if (loginReqPacket->isDummy == false)
	{
		string value = REDIS.GetValue(to_string(loginReqPacket->accountDbId));
		nlohmann::json j = nlohmann::json::parse(value);
	
		if (loginReqPacket->accountDbId != j["AccountDbId"] || loginReqPacket->token != j["Token"] || j["Expired"] < CLOCK.SystemTick())
		{
			resPacket.result = (uint16)ERROR_CODE::LOGIN_INVALID_TOKEN;
			user->SendPacket(resPacket.packetSize, (BYTE*)&resPacket);
			return;
		}
	}

	resPacket.result = user->Login(loginReqPacket->userId);
	user->SendPacket(resPacket.packetSize, (BYTE*)&resPacket);
}

void PacketManager::ProcessLogout(int32 sessionId, uint16 packetSize, BYTE* packet)
{
	LOGOUT_REQ_PACKET* logoutReqPacket = reinterpret_cast<LOGOUT_REQ_PACKET*>(packet);
	auto user = USERS.GetUserRefById(sessionId);

	if (packetSize != logoutReqPacket->packetSize)
		return;

	if (user->GetCurrentRoom() != 0)
		ROOMS.GetRoomByNumber(user->GetCurrentRoom())->LeaveUser(user);
	user->Logout();

	LOGOUT_RES_PACKET resPacket;
	resPacket.packetSize = sizeof(LOGOUT_RES_PACKET);
	resPacket.packetId = (uint16)PACKET_ID::LOGOUT_RES;
	resPacket.type = 0;
	resPacket.result = (uint16)ERROR_CODE::NONE;

	user->SendPacket(resPacket.packetSize, (BYTE*)&resPacket);
}

void PacketManager::ProcessRoomEnter(int32 sessionId, uint16 packetSize, BYTE* packet)
{
	ROOM_ENTER_REQ_PACKET* roomEnterReqPacket = reinterpret_cast<ROOM_ENTER_REQ_PACKET*>(packet);
	auto user = USERS.GetUserRefById(sessionId);

	if (packetSize != roomEnterReqPacket->packetSize)
		return;

	auto room = ROOMS.GetRoomByNumber(roomEnterReqPacket->roomNumber);
	uint16 result;
	if (room == nullptr)
		result = (uint16)ERROR_CODE::ROOM_UNKNOWN_NUMBER;
	else
		result = room->EnterUser(user);

	ROOM_ENTER_RES_PACKET resPacket;
	resPacket.packetSize = sizeof(ROOM_ENTER_RES_PACKET);
	resPacket.packetId = (uint16)PACKET_ID::ROOM_ENTER_RES;
	resPacket.type = 0;
	resPacket.result = result;

	user->SendPacket(resPacket.packetSize, (BYTE*)&resPacket);
}

void PacketManager::ProcessRoomLeave(int32 sessionId, uint16 packetSize, BYTE* packet)
{
	ROOM_LEAVE_REQ_PACKET* roomLeaveReqPacket = reinterpret_cast<ROOM_LEAVE_REQ_PACKET*>(packet);
	auto user = USERS.GetUserRefById(sessionId);

	if (packetSize != roomLeaveReqPacket->packetSize)
		return;

	auto room = ROOMS.GetRoomByNumber(user->GetCurrentRoom());
	uint16 result = room->LeaveUser(user);

	ROOM_LEAVE_RES_PACKET resPacket;
	resPacket.packetSize = sizeof(ROOM_LEAVE_RES_PACKET);
	resPacket.packetId = (uint16)PACKET_ID::ROOM_LEAVE_RES;
	resPacket.type = 0;
	resPacket.result = result;

	user->SendPacket(resPacket.packetSize, (BYTE*)&resPacket);
}

void PacketManager::ProcessChat(int32 sessionId, uint16 packetSize, BYTE* packet)
{
	CHAT_REQ_PACKET* chatReqPacket = reinterpret_cast<CHAT_REQ_PACKET*>(packet);
	shared_ptr<User> user = USERS.GetUserRefById(sessionId);

	if (packetSize != chatReqPacket->packetSize)
		return;
	
	CHAT_RES_PACKET chatResPacket;
	chatResPacket.packetSize = sizeof(CHAT_RES_PACKET);
	chatResPacket.packetId = (uint16)PACKET_ID::CHAT_RES;
	chatResPacket.type = 0;
	chatResPacket.result = (uint16)ERROR_CODE::NONE;
	chatResPacket.requestTimeTick = chatReqPacket->requestTimeTick;

	if (user == nullptr)
	{
		chatResPacket.result = (uint16)ERROR_CODE::INVALID_SESSION;
		user->SendPacket(chatResPacket.packetSize, (BYTE*)&chatResPacket);
		return;
	}

	auto room = ROOMS.GetRoomByNumber(user->GetCurrentRoom());
	if (room == nullptr)
	{
		chatResPacket.result = (uint16)ERROR_CODE::ROOM_UNKNOWN_NUMBER;
		user->SendPacket(chatResPacket.packetSize, (BYTE*)&chatResPacket);
		return;
	}
	
	user->SendPacket(chatResPacket.packetSize, (BYTE*)&chatResPacket);
	
	CHAT_BROADCAST_PACKET broadcastPacket;
	broadcastPacket.packetSize = sizeof(CHAT_BROADCAST_PACKET);
	broadcastPacket.packetId = (uint16)PACKET_ID::CHAT_BROADCAST;
	broadcastPacket.type = 0;
	memcpy(broadcastPacket.userId, user->GetNickname(), MAX_ID_PWD_BYTE_LENGTH);
	memcpy(broadcastPacket.chatMsg, chatReqPacket->chatMsg, MAX_CHAT_MSG_SIZE);

	room->Broadcast(sessionId, broadcastPacket.packetSize, (BYTE*)&broadcastPacket, false);

	PushChatLog(sessionId, user->GetCurrentRoom(), user->GetNickname(), chatReqPacket->chatMsg);
}

#pragma endregion
