#include "PacketManager.h"

void PacketManager::Init(const UINT32 maxClientCount)
{
	_userManager = std::make_unique<UserManager>();
	_userManager->Init(maxClientCount);

	_packetFuncDict = std::unordered_map<UINT16, PacketFunction>();
	// �Լ��� �Ҵ�
	_packetFuncDict[(UINT16)PACKET_ID::SYS_USER_CONNECT] = [this](UINT32 sessionId, UINT16 packetSize, char* packet) { return ProcessUserConnect(sessionId, packetSize, packet); };
	_packetFuncDict[(UINT16)PACKET_ID::SYS_USER_DISCONNECT] = [this](UINT32 sessionId, UINT16 packetSize, char* packet) { return ProcessUserDisconnect(sessionId, packetSize, packet); };

	_packetFuncDict[(UINT16)PACKET_ID::CHAT_ECHO_REQUEST] = [this](UINT32 sessionId, UINT16 packetSize, char* packet) { return ProcessEcho(sessionId, packetSize, packet); };
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
			// Handle Packet
			_packetFuncDict[packet.packetId](packet.sessionId, packet.dataSize, packet.packetData);
		}

		if (isIdle)
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

#pragma region HANDLER FUNCTION
void PacketManager::ProcessUserConnect(UINT32 sessionId, UINT32 packetSize, char* packet)
{
	// TODO
}

void PacketManager::ProcessUserDisconnect(UINT32 sessionId, UINT32 packetSize, char* packet)
{
	// TODO
}

void PacketManager::ProcessEcho(UINT32 sessionId, UINT32 packetSize, char* packet)
{
	// TODO
}

#pragma endregion
