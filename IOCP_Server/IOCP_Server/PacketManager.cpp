#include "PacketManager.h"
#include "ErrorCode.h"
#include <nlohmann/json.hpp>

PacketManager::~PacketManager()
{
	_packetFuncDict.clear();
}

void PacketManager::Init(const UINT32 maxClientCount)
{
	// 함수자 할당
	_packetFuncDict = std::unordered_map<UINT16, PacketFunction>();

	_packetFuncDict[(UINT16)PACKET_ID::SYS_USER_CONNECT] = [this](UINT32 sessionId, UINT16 packetSize, char* packet) { return ProcessUserConnect(sessionId, packetSize, packet); };
	_packetFuncDict[(UINT16)PACKET_ID::SYS_USER_DISCONNECT] = [this](UINT32 sessionId, UINT16 packetSize, char* packet) { return ProcessUserDisconnect(sessionId, packetSize, packet); };

	_packetFuncDict[(UINT16)PACKET_ID::LOGIN_REQ] = [this](UINT32 sessionId, UINT16 packetSize, char* packet) { return ProcessLogin(sessionId, packetSize, packet); };
	_packetFuncDict[(UINT16)PACKET_ID::LOGOUT_REQ] = [this](UINT32 sessionId, UINT16 packetSize, char* packet) { return ProcessLogout(sessionId, packetSize, packet); };
	_packetFuncDict[(UINT16)PACKET_ID::ROOM_ENTER_REQ] = [this](UINT32 sessionId, UINT16 packetSize, char* packet) { return ProcessRoomEnter(sessionId, packetSize, packet); };
	_packetFuncDict[(UINT16)PACKET_ID::ROOM_LEAVE_REQ] = [this](UINT32 sessionId, UINT16 packetSize, char* packet) { return ProcessRoomLeave(sessionId, packetSize, packet); };
	_packetFuncDict[(UINT16)PACKET_ID::CHAT_REQ] = [this](UINT32 sessionId, UINT16 packetSize, char* packet) { return ProcessChat(sessionId, packetSize, packet); };
	
	// DBConnectionPool 생성
	_dbConnectionPool = std::make_unique<DBConnectionPool>();

	// RoomManager 생성 및 초기화
	_roomManager = std::make_unique<RoomManager>();
	_roomManager->SendPacketFunc = SendPacketFunc;
	_roomManager->Init(1000, 100, 11);

	// UserManager 생성 및 초기화
	_userManager = std::make_unique<UserManager>();
	_userManager->Init(maxClientCount);

	// RedisManager 생성 및 초기화
	_redisManager = std::make_unique<RedisManager>();
	_redisManager->Init();
}

void PacketManager::Run(const UINT32 maxDBThreadCount)
{
	ASSERT_CRASH(_dbConnectionPool->Connect(maxDBThreadCount, DB_CONNECTION_STRING));
	if (bool DROP_AND_CREATE_TABLE = false)
	{
		DBConnection* dbConnection = _dbConnectionPool->Pop();
		ASSERT_CRASH(dbConnection->Execute(DB_CREATE_TABLE_QUERY));
		_dbConnectionPool->Push(dbConnection);
	}
	if (bool TRUNCATE_TABLE = true)
	{
		DBConnection* dbConnection = _dbConnectionPool->Pop();
		ASSERT_CRASH(dbConnection->Execute(DB_TRUNCATE_TABLE_QUERY));
		_dbConnectionPool->Push(dbConnection);
	}

	_isProcessThread = true;
	_packetThread = std::thread([this]() { ProcessPacket(); });
	_dbThread = std::thread([this]() { ProcessDB(); });
}

void PacketManager::End()
{
	_isProcessThread = false;

	if (_packetThread.joinable())
		_packetThread.join();

	if (_dbThread.joinable())
		_dbThread.join();
}

void PacketManager::PushSystemPacket(PacketInfo systemPacket)
{	// SYSTEM 패킷
	// User 클래스의 packet 버퍼에 패킷을 쌓아둠.
	PACKET_HEADER packet;
	packet.packetSize = PACKET_HEADER_SIZE;
	packet.packetId = systemPacket.packetId;
	packet.type = 0;

	auto pUser = _userManager->GetUserBySessionId(systemPacket.sessionId);
	pUser->PushPacket(packet.packetSize, (char*)&packet);

	std::lock_guard<std::mutex> guard(_packetLock);
	_packetSessionIdQueue.push(systemPacket.sessionId);
}

void PacketManager::PushPacket(UINT32 sessionId, UINT32 packetSize, char* packet)
{	// CONTENT 패킷
	// User 클래스의 packet 버퍼에 패킷을 쌓아둠.
	// 특정 유저가 혼자서 너무 많은 패킷을 보냈을 때 이 피해가 그 유저에게만 영향을 끼치게 하기 위함. Lock이 자주 걸리지 않게 하기 위함.
	auto pUser = _userManager->GetUserBySessionId(sessionId);
	pUser->PushPacket(packetSize, packet);

	std::lock_guard<std::mutex> guard(_packetLock);
	_packetSessionIdQueue.push(sessionId);
}

PacketInfo PacketManager::PopPacket()
{
	UINT32 sessionId = 0;
	{
		std::lock_guard<std::mutex> guard(_packetLock);
		if (_packetSessionIdQueue.empty())
			return PacketInfo();

		sessionId = _packetSessionIdQueue.front();
		_packetSessionIdQueue.pop();
	}

	// SessionId를 이용해서 User 객체를 받아온 후, User의 패킷 버퍼에 쌓인 패킷을 가져온다.
	auto pUser = _userManager->GetUserBySessionId(sessionId);
	return pUser->PopPacket();
}

void PacketManager::ProcessPacket()
{
	while (_isProcessThread)
	{
		bool isIdle = true;

		PacketInfo packet = PopPacket();
		if (packet.packetId != 0)
		{
			isIdle = false;
			_packetFuncDict[packet.packetId](packet.sessionId, packet.dataSize, packet.packetData);
		}

		if (isIdle)
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void PacketManager::PushChatLog(UINT32 sessionId, UINT32 roomNumber, const char* userId, const char* chatMsg)
{
	// TODO : memory leak problem
	DB_CHATLOG_INFO chatLog;
	chatLog.sessionId = sessionId;
	std::memcpy(chatLog.userId, userId, MAX_ID_PWD_BYTE_LENGTH);
	std::memcpy(chatLog.chatMsg, chatMsg, MAX_CHAT_MSG_SIZE);
	chatLog.roomNumber = roomNumber;
	chatLog.dateTime = time(NULL);

	std::lock_guard<std::mutex> guard(_chatLogLock);
	_chatLogQueue.push(chatLog);
}

DB_CHATLOG_INFO PacketManager::PopChatLog()
{
	std::lock_guard<std::mutex> guard(_chatLogLock);
	if (_chatLogQueue.empty())
		return DB_CHATLOG_INFO();

	DB_CHATLOG_INFO chatLog = _chatLogQueue.front();
	_chatLogQueue.pop();

	return chatLog;
}

void PacketManager::ProcessDB()
{
	DBConnection* dbConnection = _dbConnectionPool->Pop();
	
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
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	
	_dbConnectionPool->Push(dbConnection);
}

#pragma region HANDLER FUNCTION
void PacketManager::ProcessUserConnect(UINT32 sessionId, UINT16 packetSize, char* packet)
{
	printf("[ProcessUserConnect] 클라이언트: sessionId(%d)\n", sessionId);

	_userManager->ConnectUser(sessionId);

	printf("현재 접속한 클라이언트 수 : %d\n", _userManager->GetCurrentUserCount());
}

void PacketManager::ProcessUserDisconnect(UINT32 sessionId, UINT16 packetSize, char* packet)
{
	printf("[ProcessUserDisconnect] 클라이언트: sessionId(%d)\n", sessionId);

	User* pUser = _userManager->GetUserBySessionId(sessionId);
	_roomManager->LeaveUser(pUser, pUser->GetCurrentRoom());
	_userManager->DisconnectUser(sessionId);
	
	printf("현재 접속한 클라이언트 수 : %d\n", _userManager->GetCurrentUserCount());
}

void PacketManager::ProcessLogin(UINT32 sessionId, UINT16 packetSize, char* packet)
{
	LOGIN_REQ_PACKET* loginReqPacket = reinterpret_cast<LOGIN_REQ_PACKET*>(packet);

	if (packetSize != loginReqPacket->packetSize)
		return;

	LOGIN_RES_PACKET resPacket;
	resPacket.packetSize = sizeof(LOGIN_RES_PACKET);
	resPacket.packetId = (UINT16)PACKET_ID::LOGIN_RES;
	resPacket.type = 0;
	resPacket.result = (UINT16)ERROR_CODE::LOGIN_SUCCESS;

	// Validation 체크 (RedisDB에 token 검증)
	if (loginReqPacket->isDummy == false)
	{
		std::string value = _redisManager->GetValue(std::to_string(loginReqPacket->accountDbId));
		nlohmann::json j = nlohmann::json::parse(value);
	
		if (loginReqPacket->accountDbId != j["AccountDbId"] || loginReqPacket->token != j["Token"] || j["Expired"] < ::GetTickCount64())
		{
			resPacket.result = (UINT16)ERROR_CODE::LOGIN_INVALID_TOKEN;
			SendPacketFunc(sessionId, resPacket.packetSize, (char*)&resPacket);
			return;
		}
	}

	resPacket.result = _userManager->LoginUser(sessionId, loginReqPacket->userId);
	SendPacketFunc(sessionId, resPacket.packetSize, (char*)&resPacket);
}

void PacketManager::ProcessLogout(UINT32 sessionId, UINT16 packetSize, char* packet)
{
	LOGOUT_REQ_PACKET* logoutReqPacket = reinterpret_cast<LOGOUT_REQ_PACKET*>(packet);

	if (packetSize != logoutReqPacket->packetSize)
		return;

	User* pUser = _userManager->GetUserBySessionId(sessionId);
	_roomManager->LeaveUser(pUser, pUser->GetCurrentRoom());
	_userManager->LogoutUser(sessionId);

	LOGOUT_RES_PACKET resPacket;
	resPacket.packetSize = sizeof(LOGOUT_RES_PACKET);
	resPacket.packetId = (UINT16)PACKET_ID::LOGOUT_RES;
	resPacket.type = 0;
	resPacket.result = (UINT16)ERROR_CODE::NONE;

	SendPacketFunc(sessionId, resPacket.packetSize, (char*)&resPacket);
}

void PacketManager::ProcessRoomEnter(UINT32 sessionId, UINT16 packetSize, char* packet)
{
	ROOM_ENTER_REQ_PACKET* roomEnterReqPacket = reinterpret_cast<ROOM_ENTER_REQ_PACKET*>(packet);

	if (packetSize != roomEnterReqPacket->packetSize)
		return;

	User* pUser = _userManager->GetUserBySessionId(sessionId);
	UINT16 result = _roomManager->EnterUser(pUser, roomEnterReqPacket->roomNumber);

	ROOM_ENTER_RES_PACKET resPacket;
	resPacket.packetSize = sizeof(ROOM_ENTER_RES_PACKET);
	resPacket.packetId = (UINT16)PACKET_ID::ROOM_ENTER_RES;
	resPacket.type = 0;
	resPacket.result = result;

	SendPacketFunc(sessionId, resPacket.packetSize, (char*)&resPacket);
}

void PacketManager::ProcessRoomLeave(UINT32 sessionId, UINT16 packetSize, char* packet)
{
	ROOM_LEAVE_REQ_PACKET* roomLeaveReqPacket = reinterpret_cast<ROOM_LEAVE_REQ_PACKET*>(packet);

	if (packetSize != roomLeaveReqPacket->packetSize)
		return;

	User* pUser = _userManager->GetUserBySessionId(sessionId);
	Room* room = _roomManager->GetRoomByNumber(pUser->GetCurrentRoom());
	UINT16 result = _roomManager->LeaveUser(pUser, pUser->GetCurrentRoom());

	ROOM_LEAVE_RES_PACKET resPacket;
	resPacket.packetSize = sizeof(ROOM_LEAVE_RES_PACKET);
	resPacket.packetId = (UINT16)PACKET_ID::ROOM_LEAVE_RES;
	resPacket.type = 0;
	resPacket.result = result;

	SendPacketFunc(sessionId, resPacket.packetSize, (char*)&resPacket);
}

void PacketManager::ProcessChat(UINT32 sessionId, UINT16 packetSize, char* packet)
{
	CHAT_REQ_PACKET* chatReqPacket = reinterpret_cast<CHAT_REQ_PACKET*>(packet);

	if (packetSize != chatReqPacket->packetSize)
		return;
	
	CHAT_RES_PACKET chatResPacket;
	chatResPacket.packetSize = sizeof(CHAT_RES_PACKET);
	chatResPacket.packetId = (UINT16)PACKET_ID::CHAT_RES;
	chatResPacket.type = 0;
	chatResPacket.result = (UINT16)ERROR_CODE::NONE;
	chatResPacket.requestTimeTick = chatReqPacket->requestTimeTick;

	User* pUser = _userManager->GetUserBySessionId(sessionId);
	if (pUser == nullptr)
	{
		chatResPacket.result = (UINT16)ERROR_CODE::INVALID_SESSION;
		SendPacketFunc(sessionId, chatResPacket.packetSize, (char*)&chatResPacket);
		return;
	}

	Room* room = _roomManager->GetRoomByNumber(pUser->GetCurrentRoom());
	if (room == nullptr)
	{
		chatResPacket.result = (UINT16)ERROR_CODE::ROOM_UNKNOWN_NUMBER;
		SendPacketFunc(sessionId, chatResPacket.packetSize, (char*)&chatResPacket);
		return;
	}
	
	SendPacketFunc(sessionId, chatResPacket.packetSize, (char*)&chatResPacket);
	
	CHAT_BROADCAST_PACKET broadcastPacket;
	broadcastPacket.packetSize = sizeof(CHAT_BROADCAST_PACKET);
	broadcastPacket.packetId = (UINT16)PACKET_ID::CHAT_BROADCAST;
	broadcastPacket.type = 0;
	std::memcpy(broadcastPacket.userId, pUser->GetUserId(), MAX_ID_PWD_BYTE_LENGTH);
	std::memcpy(broadcastPacket.chatMsg, chatReqPacket->chatMsg, MAX_CHAT_MSG_SIZE);

	room->Broadcast(sessionId, broadcastPacket.packetSize, (char*)&broadcastPacket, false);

	PushChatLog(sessionId, pUser->GetCurrentRoom(), pUser->GetUserId(), chatReqPacket->chatMsg);
}

#pragma endregion
