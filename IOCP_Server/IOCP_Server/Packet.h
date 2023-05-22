#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

enum class PACKET_ID : UINT16
{
	// SYSTEM
	SYS_USER_CONNECT = 11,
	SYS_USER_DISCONNECT = 12,

	// DB

	// CONTENT
	LOGIN_REQUEST = 201,
	LOGIN_RESPONSE = 202,

	CHAT_ECHO = 301,
	C_CHAT = 303,
	S_CHAT = 304,

	DELAY_CHECK = 505,
};

// Ŭ���̾�Ʈ�� ���� ��Ŷ�� Wrapping �ϴ� ����ü
struct PacketInfo
{
	UINT32 sessionId = 0;
	UINT16 packetId = 0;
	UINT16 dataSize = 0;			// packetData�� ������
	char* packetData = nullptr;
};

#pragma pack(push, 1)
struct PACKET_HEADER
{
	UINT16 packetSize;	// ��Ŷ ����� ������ ��ü ������
	UINT16 packetId;
	UINT8 type;			// ���� ����, ��ȣȭ ���� �� �Ӽ��� �˾Ƴ��� ��
};
const UINT32 PACKET_HEADER_SIZE = sizeof(PACKET_HEADER);

#pragma region SYSTEM

#pragma endregion

#pragma region DB

#pragma endregion

#pragma region CONTENT

const int MAX_CHAT_MSG_SIZE = 256;
struct CHAT_ECHO_PACKET : public PACKET_HEADER
{
	char chatMsg[MAX_CHAT_MSG_SIZE] = { 0, };
};

const int MAX_NICKNAME_BYTE_LENGTH = 32;
struct C_CHAT_PACKET : public PACKET_HEADER
{
	char nickname[MAX_NICKNAME_BYTE_LENGTH] = { 0, };
	char chatMsg[MAX_CHAT_MSG_SIZE] = { 0, };
};

struct S_CHAT_PACKET : public PACKET_HEADER
{
	char nickname[MAX_NICKNAME_BYTE_LENGTH] = { 0, };
	char chatMsg[MAX_CHAT_MSG_SIZE] = { 0, };
};

struct PING_CHECK_PACKET : public PACKET_HEADER
{
	INT64 CurrentTimeSpan;
};
#pragma endregion

#pragma pack(pop)