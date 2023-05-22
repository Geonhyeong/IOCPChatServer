#include "PacketManager.h"
#include "ErrorCode.h"

void PacketManager::Init(const UINT32 maxClientCount, const std::function<void(UINT32, UINT16, char*)> sendPacketFunc)
{
	// UserManager 생성 및 초기화
	_userManager = std::make_unique<UserManager>();
	_userManager->Init(maxClientCount);
	_userManager->SendPacketFunc = sendPacketFunc;	// TEMP

	_sendPacketFunc = sendPacketFunc;
	_packetFuncDict = std::unordered_map<UINT16, PacketFunction>();
	// 함수자 할당
	_packetFuncDict[(UINT16)PACKET_ID::SYS_USER_CONNECT] = [this](UINT32 sessionId, UINT16 packetSize, char* packet) { return ProcessUserConnect(sessionId, packetSize, packet); };
	_packetFuncDict[(UINT16)PACKET_ID::SYS_USER_DISCONNECT] = [this](UINT32 sessionId, UINT16 packetSize, char* packet) { return ProcessUserDisconnect(sessionId, packetSize, packet); };

	_packetFuncDict[(UINT16)PACKET_ID::CHAT_ECHO] = [this](UINT32 sessionId, UINT16 packetSize, char* packet) { return ProcessEcho(sessionId, packetSize, packet); };
	_packetFuncDict[(UINT16)PACKET_ID::C_CHAT] = [this](UINT32 sessionId, UINT16 packetSize, char* packet) { return ProcessChat(sessionId, packetSize, packet); };
	_packetFuncDict[(UINT16)PACKET_ID::DELAY_CHECK] = [this](UINT32 sessionId, UINT16 packetSize, char* packet) { return ProcessDelayCheck(sessionId, packetSize, packet); };
}

void PacketManager::Run()
{
	_isProcessThread = true;
	_processThread = std::thread([this]() { ProcessPacket(); });
}

void PacketManager::End()
{
	_isProcessThread = false;

	if (_processThread.joinable())
		_processThread.join();
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

	std::lock_guard<std::mutex> guard(_lock);
	_packetSessionIdQueue.push(systemPacket.sessionId);
}

void PacketManager::PushPacket(UINT32 sessionId, UINT32 packetSize, char* packet)
{	// CONTENT 패킷
	// User 클래스의 packet 버퍼에 패킷을 쌓아둠.
	// 특정 유저가 혼자서 너무 많은 패킷을 보냈을 때 이 피해가 그 유저에게만 영향을 끼치게 하기 위함. Lock이 자주 걸리지 않게 하기 위함.
	auto pUser = _userManager->GetUserBySessionId(sessionId);
	pUser->PushPacket(packetSize, packet);

	std::lock_guard<std::mutex> guard(_lock);
	_packetSessionIdQueue.push(sessionId);
}

PacketInfo PacketManager::PopPacket()
{
	UINT32 sessionId = 0;
	{
		std::lock_guard<std::mutex> guard(_lock);
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

	_userManager->DisconnectUser(sessionId);
	
	printf("현재 접속한 클라이언트 수 : %d\n", _userManager->GetCurrentUserCount());
}

void PacketManager::ProcessEcho(UINT32 sessionId, UINT16 packetSize, char* packet)
{
	auto echoPacket = reinterpret_cast<CHAT_ECHO_PACKET*>(packet);

	if (packetSize != echoPacket->packetSize)
		return;

	_sendPacketFunc(sessionId, echoPacket->packetSize, (char*)echoPacket);

	echoPacket->chatMsg[packetSize - PACKET_HEADER_SIZE] = '\0';
	printf("[Echo] 클라이언트: sessionId(%d) 메시지: %s\n", sessionId, echoPacket->chatMsg);
}

void PacketManager::ProcessChat(UINT32 sessionId, UINT16 packetSize, char* packet)
{
	auto chatPacket = reinterpret_cast<C_CHAT_PACKET*>(packet);

	if (packetSize != chatPacket->packetSize)
		return;

	S_CHAT_PACKET broadcastPacket;
	broadcastPacket.packetSize = sizeof(S_CHAT_PACKET);
	broadcastPacket.packetId = (UINT16)PACKET_ID::S_CHAT;
	broadcastPacket.type = 0;
	std::memcpy(broadcastPacket.nickname, chatPacket->nickname, MAX_NICKNAME_BYTE_LENGTH);
	std::memcpy(broadcastPacket.chatMsg, chatPacket->chatMsg, MAX_CHAT_MSG_SIZE);

	_userManager->BroadcastToConnectingUser(sessionId, broadcastPacket.packetSize, (char*)&broadcastPacket);
}

void PacketManager::ProcessDelayCheck(UINT32 sessionId, UINT16 packetSize, char* packet)
{
	auto delayCheckPacket = reinterpret_cast<PING_CHECK_PACKET*>(packet);

	if (packetSize != delayCheckPacket->packetSize)
		return;

	_sendPacketFunc(sessionId, delayCheckPacket->packetSize, (char*)delayCheckPacket);
}

#pragma endregion
