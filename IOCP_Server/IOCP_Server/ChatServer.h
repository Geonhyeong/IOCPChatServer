#pragma once
#include "IOCP.h"
#include "Packet.h"

#include <queue>
#include <mutex>

class ChatServer : public IOCP
{
public:
	virtual void OnConnected(const UINT32 sessionId) override
	{
		printf("技记 (%d) 立加 己傍\n", sessionId);
	}

	virtual void OnDisconnected(const UINT32 sessionId) override
	{
		printf("技记 (%d) 立加 谗辫\n", sessionId);
	}

	virtual void OnRecv(const UINT32 sessionId, const UINT32 len, char* buf) override
	{	
		Packet packet;
		packet.Set(sessionId, len, buf);
		
		std::lock_guard<std::mutex> guard(_lock);
		_packetQueue.push(packet);
	}

	void Run(const UINT32 maxClientCount)
	{
		_isProcessThread = true;
		_processThread = std::thread([this]() { ProcessPacket(); });

		printf("菩哦 静饭靛 矫累...\n");

		StartServer(maxClientCount);
	}

	void End()
	{
		_isProcessThread = false;

		if (_processThread.joinable())
			_processThread.join();

		DestroyThread();
	}


private:
	void ProcessPacket()
	{
		while (_isProcessThread)
		{
			Packet packet = PopPacket();
			if (packet.packetSize != 0)
			{
				// Echo
				Send(packet.sessionId, packet.packetSize - 8, packet.packetData);
			}
			else
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
		}
	}

	Packet PopPacket()
	{
		std::lock_guard<std::mutex> guard(_lock);

		if (_packetQueue.empty())
			return Packet();
		
		Packet packet;
		packet.Set(_packetQueue.front());
		
		_packetQueue.front().Release();
		_packetQueue.pop();

		return packet;
	}

private:
	bool				_isProcessThread = false;
	std::thread			_processThread;

	std::mutex			_lock;
	std::queue<Packet>	_packetQueue;
};