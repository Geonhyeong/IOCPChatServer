#pragma once
#include "IOCP.h"
#include "Packet.h"
#include "PacketManager.h"

class ChatServer : public IOCP
{
public:
	ChatServer() = default;
	virtual ~ChatServer() = default;

	virtual void OnConnected(const UINT32 sessionId) override
	{
		printf("[OnConnect] Ŭ���̾�Ʈ: Index(%d)\n", sessionId);

		Packet packet{ sessionId, (UINT16)PACKET_ID::SYS_USER_CONNECT, 0 };
		_packetManager->PushPacket(packet);
	}

	virtual void OnDisconnected(const UINT32 sessionId) override
	{
		printf("[OnDisconnected] Ŭ���̾�Ʈ: Index(%d)\n", sessionId);

		Packet packet{ sessionId, (UINT16)PACKET_ID::SYS_USER_DISCONNECT, 0 };
		_packetManager->PushPacket(packet);
	}

	virtual void OnRecv(const UINT32 sessionId, const UINT32 len, char* buf) override
	{	
		printf("[OnReceive] Ŭ���̾�Ʈ: Index(%d), dataSize(%d)\n", sessionId, len);

		_packetManager->PushPacket(sessionId, len, buf);
	}

	void Run(const UINT32 maxClientCount)
	{
		// TODO
		_packetManager = std::make_unique<PacketManager>();
		_packetManager->Init(maxClientCount);
		_packetManager->Run();

		printf("��Ŷ �Ŵ��� ������ ����...\n");

		StartServer(maxClientCount);
	}

	void End()
	{
		_packetManager->End();

		DestroyThread();
	}

private:
	std::unique_ptr<PacketManager> _packetManager;
};