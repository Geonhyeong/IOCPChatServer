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
	CHAT_ECHO_REQUEST = 301,
	CHAT_ECHO_RESPONSE = 302,
};

// Ŭ���̾�Ʈ�� ���� ��Ŷ�� �����ϴ� ����ü
struct Packet
{
	UINT32 sessionId = 0;
	UINT16 packetId = 0;
	UINT16 dataSize = 0;			// ��Ŷ ����� ����� �� packetData�� ������
	char* packetData = nullptr;
};

#pragma pack(push, 1)
struct PACKET_HEADER
{
	UINT16 dataSize;
	UINT16 packetId;
	UINT8 type;			// ���� ����, ��ȣȭ ���� �� �Ӽ��� �˾Ƴ��� ��
};
const UINT32 PACKET_HEADER_SIZE = sizeof(PACKET_HEADER);

#pragma region SYSTEM

#pragma endregion

#pragma region DB

#pragma endregion

#pragma region CONTENT
const UINT32 MAX_CHAT_MSG_SIZE = 256;
struct CHAT_ECHO_REQUEST_PACKET : public PACKET_HEADER
{
	char chatMsg[MAX_CHAT_MSG_SIZE] = { 0, };
};

struct CHAT_ECHO_RESPONSE_PACKET : public PACKET_HEADER
{
	char echoMsg[MAX_CHAT_MSG_SIZE] = { 0, 1 };
};
#pragma endregion

#pragma pack(pop)