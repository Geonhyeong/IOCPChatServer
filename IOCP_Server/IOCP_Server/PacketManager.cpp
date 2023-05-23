#include "PacketManager.h"
#include "ErrorCode.h"

void PacketManager::Init(const UINT32 maxClientCount, const std::function<void(UINT32, UINT16, char*)> sendPacketFunc)
{
	// UserManager ���� �� �ʱ�ȭ
	_userManager = std::make_unique<UserManager>();
	_userManager->Init(maxClientCount);
	_userManager->SendPacketFunc = sendPacketFunc;	// TEMP

	_sendPacketFunc = sendPacketFunc;
	_packetFuncDict = std::unordered_map<UINT16, PacketFunction>();
	// �Լ��� �Ҵ�
	_packetFuncDict[(UINT16)PACKET_ID::SYS_USER_CONNECT] = [this](UINT32 sessionId, UINT16 packetSize, char* packet) { return ProcessUserConnect(sessionId, packetSize, packet); };
	_packetFuncDict[(UINT16)PACKET_ID::SYS_USER_DISCONNECT] = [this](UINT32 sessionId, UINT16 packetSize, char* packet) { return ProcessUserDisconnect(sessionId, packetSize, packet); };

	_packetFuncDict[(UINT16)PACKET_ID::CHAT_REQ] = [this](UINT32 sessionId, UINT16 packetSize, char* packet) { return ProcessChat(sessionId, packetSize, packet); };
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
{	// SYSTEM ��Ŷ
	// User Ŭ������ packet ���ۿ� ��Ŷ�� �׾Ƶ�.
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
{	// CONTENT ��Ŷ
	// User Ŭ������ packet ���ۿ� ��Ŷ�� �׾Ƶ�.
	// Ư�� ������ ȥ�ڼ� �ʹ� ���� ��Ŷ�� ������ �� �� ���ذ� �� �������Ը� ������ ��ġ�� �ϱ� ����. Lock�� ���� �ɸ��� �ʰ� �ϱ� ����.
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

	// SessionId�� �̿��ؼ� User ��ü�� �޾ƿ� ��, User�� ��Ŷ ���ۿ� ���� ��Ŷ�� �����´�.
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
	printf("[ProcessUserConnect] Ŭ���̾�Ʈ: sessionId(%d)\n", sessionId);

	_userManager->ConnectUser(sessionId);

	printf("���� ������ Ŭ���̾�Ʈ �� : %d\n", _userManager->GetCurrentUserCount());
}

void PacketManager::ProcessUserDisconnect(UINT32 sessionId, UINT16 packetSize, char* packet)
{
	printf("[ProcessUserDisconnect] Ŭ���̾�Ʈ: sessionId(%d)\n", sessionId);

	_userManager->DisconnectUser(sessionId);
	
	printf("���� ������ Ŭ���̾�Ʈ �� : %d\n", _userManager->GetCurrentUserCount());
}

void PacketManager::ProcessChat(UINT32 sessionId, UINT16 packetSize, char* packet)
{
	CHAT_REQ_PACKET* chatReqPacket = reinterpret_cast<CHAT_REQ_PACKET*>(packet);

	if (packetSize != chatReqPacket->packetSize)
		return;

	{
		CHAT_BROADCAST_PACKET broadcastPacket;
		broadcastPacket.packetSize = sizeof(CHAT_BROADCAST_PACKET);
		broadcastPacket.packetId = (UINT16)PACKET_ID::CHAT_BROADCAST;
		broadcastPacket.type = 0;
		std::memcpy(broadcastPacket.nickname, chatReqPacket->nickname, MAX_NICKNAME_BYTE_LENGTH);
		std::memcpy(broadcastPacket.chatMsg, chatReqPacket->chatMsg, MAX_CHAT_MSG_SIZE);

		_userManager->BroadcastToConnectingUser(sessionId, broadcastPacket.packetSize, (char*)&broadcastPacket);
	}

	{
		CHAT_RES_PACKET chatResPacket;
		chatResPacket.packetSize = sizeof(CHAT_RES_PACKET);
		chatResPacket.packetId = (UINT16)PACKET_ID::CHAT_RES;
		chatResPacket.type = 0;
		chatResPacket.result = (UINT16)ERROR_CODE::NONE;
		chatResPacket.requestTimeTick = chatReqPacket->requestTimeTick;

		_sendPacketFunc(sessionId, chatResPacket.packetSize, (char*)&chatResPacket);
	}
}

#pragma endregion
