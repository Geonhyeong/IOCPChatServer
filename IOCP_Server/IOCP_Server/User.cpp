#include "User.h"

void User::Init(UINT32 sessionId)
{
	_sessionId = sessionId;
	_ringBuffer = new char[USER_BUFFER_SIZE];
	
	_curDomainState = USER_DOMAIN_STATE::NONE;
}

void User::Clear()
{
	_curDomainState = USER_DOMAIN_STATE::NONE;
	_pingCount = 0;

	_writePos = 0;
	_readPos = 0;
}

void User::Connect()
{
	_curDomainState = USER_DOMAIN_STATE::CONNECT;
	_pingCount = 0;
}

void User::PushPacket(UINT32 packetSize, char* packet)
{
	// lock이 필요 없는 이유
	// - recv는 하나의 쓰레드에서만 실행됨.
	// - 또한 링버퍼이기 때문에 읽고 쓰는데에 멀티쓰레드 이슈는 없다.
	
	// 링버퍼
	UINT32 remainDataSize = _writePos - _readPos;
	if (remainDataSize == 0)
		_writePos = _readPos = 0;

	if ((_writePos + packetSize) >= USER_BUFFER_SIZE)
	{
		CopyMemory(&_ringBuffer[0], &_ringBuffer[_readPos], remainDataSize);
		_readPos = 0;
		_writePos = remainDataSize;
	}

	CopyMemory(&_ringBuffer[_writePos], packet, packetSize);
	_writePos += packetSize;
}

PacketInfo User::PopPacket()
{
	// 버퍼 사이즈 검증
	UINT32 remainDataSize = _writePos - _readPos;
	if (remainDataSize < PACKET_HEADER_SIZE)	// 헤더 사이즈도 안되면 읽을 수 조차 없다.
		return PacketInfo();

	auto header = (PACKET_HEADER*)&_ringBuffer[_readPos];

	if (remainDataSize < header->packetSize)	// 아직 데이터 수신이 덜 됨.
		return PacketInfo();
	
	// PacketInfo 구조체로 wrapping
	PacketInfo packet;
	packet.sessionId = _sessionId;
	packet.packetId = header->packetId;
	packet.dataSize = header->packetSize;
	packet.packetData = &_ringBuffer[_readPos];

	_readPos += header->packetSize;

	return packet;
}
