#include "PacketManager.h"
#include "ErrorCode.h"

void PacketManager::Init(const UINT32 maxClientCount)
{
	_userManager = std::make_unique<UserManager>();
	_userManager->Init(maxClientCount);

	_packetFuncDict = std::unordered_map<UINT16, PacketFunction>();
	// �Լ��� �Ҵ�
	_packetFuncDict[(UINT16)PACKET_ID::SYS_USER_CONNECT] = [this](UINT32 sessionId, UINT16 packetSize, char* packet) { return ProcessUserConnect(sessionId, packetSize, packet); };
	_packetFuncDict[(UINT16)PACKET_ID::SYS_USER_DISCONNECT] = [this](UINT32 sessionId, UINT16 packetSize, char* packet) { return ProcessUserDisconnect(sessionId, packetSize, packet); };

	_packetFuncDict[(UINT16)PACKET_ID::LOGIN_REQUEST] = [this](UINT32 sessionId, UINT16 packetSize, char* packet) { return ProcessLogin(sessionId, packetSize, packet); };
	_packetFuncDict[(UINT16)PACKET_ID::CHAT_ECHO] = [this](UINT32 sessionId, UINT16 packetSize, char* packet) { return ProcessEcho(sessionId, packetSize, packet); };
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
void PacketManager::ProcessUserConnect(UINT32 sessionId, UINT32 packetSize, char* packet)
{
	printf("[ProcessUserConnect] Ŭ���̾�Ʈ: sessionId(%d)\n", sessionId);

	_userManager->ConnectUser(sessionId);
}

void PacketManager::ProcessUserDisconnect(UINT32 sessionId, UINT32 packetSize, char* packet)
{
	printf("[ProcessUserDisconnect] Ŭ���̾�Ʈ: sessionId(%d)\n", sessionId);

	_userManager->DisconnectUser(sessionId);
}

void PacketManager::ProcessLogin(UINT32 sessionId, UINT32 packetSize, char* packet)
{
	printf("[ProcessLogin] Ŭ���̾�Ʈ: sessionId(%d)\n", sessionId);

	auto loginReqPacket = reinterpret_cast<LOGIN_REQUEST_PACKET*>(packet);
	
	LOGIN_RESPONSE_PACKET loginResPacket;
	loginResPacket.packetSize = sizeof(LOGIN_RESPONSE_PACKET);
	loginResPacket.packetId = (UINT16)PACKET_ID::LOGIN_RESPONSE;
	loginResPacket.type = 0;

	if (packetSize != loginReqPacket->packetSize)
	{
		loginResPacket.resultCode = (UINT16)ERROR_CODE::INVALID_PACKET;
		// TODO : SEND
		return;
	}
	
	// TODO : DB���� Id�� Pwd ����

	if (_userManager->GetCurrentUserCount() >= _userManager->GetMaxUserCount())
	{
		loginResPacket.resultCode = (UINT16)ERROR_CODE::LOGIN_USED_ALL_OBJ;
		// TODO : SEND
		return;
	}

	if (_userManager->IsAlreadyLogin(loginReqPacket->userId))
	{
		loginResPacket.resultCode = (UINT16)ERROR_CODE::LOGIN_REDUNDANT_CONNECTION;
		// TODO : SEND
		return;
	}

	_userManager->LoginUser(sessionId, loginReqPacket->userId);
	loginResPacket.resultCode = (UINT16)ERROR_CODE::LOGIN_SUCCESS;
	// TODO : SEND
}

void PacketManager::ProcessEcho(UINT32 sessionId, UINT32 packetSize, char* packet)
{
	auto echoPacket = reinterpret_cast<CHAT_ECHO_PACKET*>(packet);

	if (packetSize != echoPacket->packetSize)
		return;

	echoPacket->chatMsg[packetSize - PACKET_HEADER_SIZE] = '\0';
	printf("[Echo] Ŭ���̾�Ʈ: sessionId(%d) �޽���: %s\n", sessionId, echoPacket->chatMsg);

	// TODO : SEND
}

#pragma endregion
