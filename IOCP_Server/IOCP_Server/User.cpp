#include "pch.h"
#include "User.h"
#include "UserManager.h"

User::User() : _packetQueue(USER_BUFFER_SIZE)
{
}

User::~User()
{
}

void User::SendPacket(uint16 size, BYTE* packet)
{
	SendBufferRef sendBuffer = SendBufferManager::GetInstance().Open(size);
	CopyMemory(sendBuffer->Buffer(), &packet[0], size);
	sendBuffer->Close(size);

	Send(sendBuffer);
}

uint16 User::Login(const char* nickname)
{
	if (GetCurrentDomainState() >= USER_DOMAIN_STATE::LOGIN)
		return (UINT16)ERROR_CODE::LOGIN_REDUNDANT_CONNECTION;

	_curDomainState = USER_DOMAIN_STATE::LOGIN;
	std::memcpy(_nickname, nickname, MAX_ID_PWD_BYTE_LENGTH);
	
	printf("sessionId(%d) is login\n", _sessionId);

	return (UINT16)ERROR_CODE::LOGIN_SUCCESS;
}

void User::Logout()
{
	if (GetCurrentDomainState() >= USER_DOMAIN_STATE::LOGIN)
	{
		memset(_nickname, 0, sizeof(_nickname));
		_roomNumber = 0;
		_curDomainState = USER_DOMAIN_STATE::CONNECT;
	
		printf("sessionId(%d) is logout\n", _sessionId);
	}
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

void User::PushPacket(UINT32 packetSize, BYTE* packet)
{
	WRITE_LOCK;

	CopyMemory(_packetQueue.WritePos(), packet, packetSize);
	if (_packetQueue.OnWrite(packetSize) == false)
	{
		SLog(L"User::PushPacket()::OnWrite() => PacketQueue is overflowed.");
		Disconnect();
		return;
	}

	_packetQueue.Clean();
}

PacketInfo User::PopPacket()
{
	WRITE_LOCK;

	// 최소한 헤더는 파싱 가능해야 한다.
	if (_packetQueue.DataSize() < PACKET_HEADER_SIZE)
		return PacketInfo();

	PACKET_HEADER header = *(reinterpret_cast<PACKET_HEADER*>(_packetQueue.ReadPos()));

	if (_packetQueue.DataSize() < header.packetSize)	// 아직 데이터 수신이 덜 됨.
		return PacketInfo();
	
	// PacketInfo 구조체로 wrapping
	PacketInfo packet;
	packet.sessionId = _sessionId;
	packet.packetId = header.packetId;
	packet.dataSize = header.packetSize;
	packet.packetData = _packetQueue.ReadPos();

	if (_packetQueue.OnRead(header.packetSize) == false)
	{
		SLog(L"User::PopPacket()::OnRead() => PacketQueue is overflowed.");
		Disconnect();
		return PacketInfo();
	}

	_packetQueue.Clean();
	return packet;
}

void User::OnConnected()
{
	_sessionId = USERS.Add(static_pointer_cast<User>(shared_from_this()));

	_curDomainState = USER_DOMAIN_STATE::CONNECT;
}

int32 User::OnRecv(BYTE* buffer, int32 len)
{
	PACKETS.Push(_sessionId);
	PushPacket(len, buffer);
	
	return len;
}

void User::OnSend(int32 len)
{
}

void User::OnDisconnected()
{
	USERS.Remove(_sessionId);
	
	_curDomainState = USER_DOMAIN_STATE::DISCONNECT;
}
