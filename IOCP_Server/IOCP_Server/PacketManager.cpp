#include "PacketManager.h"

void PacketManager::Init(const UINT32 maxClientCount)
{
	_packetFuncDict = std::unordered_map<UINT16, PacketFunction>();

	// TODO : �Լ��� �Ҵ�
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
{	// SYSTEM ��Ŷ+@
	// TODO : User Ŭ������ packet ���ۿ� ��Ŷ�� �׾Ƶ�.

	std::lock_guard<std::mutex> guard(_lock);
	_packetUserIdQueue.push(packet.sessionId);
}

void PacketManager::PushPacket(UINT32 userId, UINT32 packetSize, char* packet)
{	// CONTENT ��Ŷ
	// TODO : User Ŭ������ packet ���ۿ� ��Ŷ�� �׾Ƶ�.
	// Ư�� ������ ȥ�ڼ� �ʹ� ���� ��Ŷ�� ������ �� �� ���ذ� �� �������Ը� ������ ��ġ�� �ϱ� ����. Lock�� ���� �ɸ��� �ʰ� �ϱ� ����.

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

	// TODO : UserId�� �̿��ؼ� User ��ü�� �޾ƿ� ��, User�� ��Ŷ ���ۿ� ���� ��Ŷ�� �����´�.
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
