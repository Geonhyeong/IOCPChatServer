#include "PacketManager.h"

void PacketManager::Init(const UINT32 maxClientCount)
{
	_packetFuncDict = std::unordered_map<UINT16, PacketFunction>();

	// TODO : 함수자 할당
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

void PacketManager::PushPacket(Packet packet)
{	// SYSTEM 패킷+@
	// TODO : User 클래스의 packet 버퍼에 패킷을 쌓아둠.

	std::lock_guard<std::mutex> guard(_lock);
	_packetUserIdQueue.push(packet.sessionId);
}

void PacketManager::PushPacket(UINT32 userId, UINT32 packetSize, char* packet)
{	// CONTENT 패킷
	// TODO : User 클래스의 packet 버퍼에 패킷을 쌓아둠.
	// 특정 유저가 혼자서 너무 많은 패킷을 보냈을 때 이 피해가 그 유저에게만 영향을 끼치게 하기 위함. Lock이 자주 걸리지 않게 하기 위함.

	std::lock_guard<std::mutex> guard(_lock);
	_packetUserIdQueue.push(userId);
}

Packet PacketManager::PopPacket()
{
	UINT32 userId = 0;
	{
		std::lock_guard<std::mutex> guard(_lock);
		if (_packetUserIdQueue.empty())
			return Packet();

		userId = _packetUserIdQueue.front();
		_packetUserIdQueue.pop();
	}

	// TODO : UserId를 이용해서 User 객체를 받아온 후, User의 패킷 버퍼에 쌓인 패킷을 가져온다.
	return Packet();
}

void PacketManager::ProcessPacket()
{
	while (_isProcessThread)
	{
		// TODO
	}
}


#pragma region HANDLER FUNCTION

#pragma endregion
