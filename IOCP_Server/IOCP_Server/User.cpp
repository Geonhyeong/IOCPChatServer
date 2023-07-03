#include "User.h"

User::~User()
{
	delete[] _userId;
	delete[] _ringBuffer;
}

void User::Init(UINT32 sessionId)
{
	_sessionId = sessionId;

	_ringBuffer = new char[USER_BUFFER_SIZE];
}

void User::Clear()
{
	memset(_userId, 0, sizeof(_userId));
	_roomNumber = 0;
	_curDomainState = USER_DOMAIN_STATE::NONE;

	_writePos = 0;
	_readPos = 0;
}

void User::Connect()
{
	_curDomainState = USER_DOMAIN_STATE::CONNECT;
}

void User::Login(const char* userId)
{
	_curDomainState = USER_DOMAIN_STATE::LOGIN;
	std::memcpy(_userId, userId, MAX_ID_PWD_BYTE_LENGTH);
	
	printf("sessionId(%d) is login\n", _sessionId);
}

void User::Logout()
{
	memset(_userId, 0, sizeof(_userId));
	_roomNumber = 0;
	_curDomainState = USER_DOMAIN_STATE::CONNECT;
	
	_writePos = 0;
	_readPos = 0;

	printf("sessionId(%d) is logout\n", _sessionId);
}

void User::EnterRoom(UINT32 roomNumber)
{
	_roomNumber = roomNumber;
	_curDomainState = USER_DOMAIN_STATE::ROOM;
}

void User::LeaveRoom()
{
	_roomNumber = 0;
	_curDomainState = USER_DOMAIN_STATE::LOGIN;
}

void User::PushPacket(UINT32 packetSize, char* packet)
{
	// lock�� �ʿ� ���� ����
	// - recv�� �� ���� �ϳ��� �����忡���� �����.
	// - ���� �������̱� ������ �а� ���µ��� ��Ƽ������ �̽��� ����.
	
	// ������
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
	// ���� ������ ����
	UINT32 remainDataSize = _writePos - _readPos;
	if (remainDataSize < PACKET_HEADER_SIZE)	// ��� ����� �ȵǸ� ���� �� ���� ����.
		return PacketInfo();

	auto header = (PACKET_HEADER*)&_ringBuffer[_readPos];

	if (remainDataSize < header->packetSize)	// ���� ������ ������ �� ��.
		return PacketInfo();
	
	// PacketInfo ����ü�� wrapping
	PacketInfo packet;
	packet.sessionId = _sessionId;
	packet.packetId = header->packetId;
	packet.dataSize = header->packetSize;
	packet.packetData = &_ringBuffer[_readPos];

	_readPos += header->packetSize;

	return packet;
}
