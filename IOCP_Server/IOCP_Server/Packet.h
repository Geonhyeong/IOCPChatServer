#pragma once

enum class PACKET_ID : uint16
{
	// SYSTEM
	SYS_USER_CONNECT = 11,
	SYS_USER_DISCONNECT = 12,

	// DB

	// CONTENT
	LOGIN_REQ = 101,
	LOGIN_RES = 102,
	LOGOUT_REQ = 103,
	LOGOUT_RES = 104,

	ROOM_ENTER_REQ = 201,
	ROOM_ENTER_RES = 202,
	ROOM_LEAVE_REQ = 203,
	ROOM_LEAVE_RES = 204,
	ROOM_USER_LIST = 205,

	CHAT_REQ = 303,
	CHAT_RES = 304,
	CHAT_BROADCAST = 305,
};

// 클라이언트가 보낸 패킷을 Wrapping 하는 구조체
struct PacketInfo
{
	int32 sessionId = 0;
	uint16 packetId = 0;
	uint16 dataSize = 0;			// packetData의 사이즈
	BYTE* packetData = nullptr;
};

#pragma pack(push, 1)
struct PACKET_HEADER
{
	uint16 packetSize;	// 패킷 헤더를 포함한 전체 사이즈
	uint16 packetId;
	uint8 type;			// 압축 여부, 암호화 여부 등 속성을 알아내는 값
};
const uint16 PACKET_HEADER_SIZE = sizeof(PACKET_HEADER);

#pragma region SYSTEM

#pragma endregion

#pragma region DB

#pragma endregion

#pragma region CONTENT

const int MAX_ID_PWD_BYTE_LENGTH = 32;
struct LOGIN_REQ_PACKET : public PACKET_HEADER
{
	char userId[MAX_ID_PWD_BYTE_LENGTH] = { 0, };
	int32 accountDbId;
	int32 token;
	int32 isDummy;
};

struct LOGIN_RES_PACKET : public PACKET_HEADER
{
	uint16 result;
};

struct LOGOUT_REQ_PACKET : public PACKET_HEADER
{
};

struct LOGOUT_RES_PACKET : public PACKET_HEADER
{
	uint16 result;
};

struct ROOM_ENTER_REQ_PACKET : public PACKET_HEADER
{
	uint32 roomNumber;
};

struct ROOM_ENTER_RES_PACKET : public PACKET_HEADER
{
	uint16 result;
};

struct ROOM_LEAVE_REQ_PACKET : public PACKET_HEADER
{
};

struct ROOM_LEAVE_RES_PACKET : public PACKET_HEADER
{
	uint16 result;
};

const int MAX_USER_LIST_BYTE_LENGTH = MAX_ID_PWD_BYTE_LENGTH * 128;
struct ROOM_USER_LIST_PACKET : public PACKET_HEADER
{
	uint32 userCount;
	char userList[MAX_USER_LIST_BYTE_LENGTH] = { 0, };
};

const int MAX_CHAT_MSG_SIZE = 256;
struct CHAT_REQ_PACKET : public PACKET_HEADER
{
	char chatMsg[MAX_CHAT_MSG_SIZE] = { 0, };
	int64 requestTimeTick;
};

struct CHAT_RES_PACKET : public PACKET_HEADER
{
	uint16 result;
	int64 requestTimeTick;
};

struct CHAT_BROADCAST_PACKET : public PACKET_HEADER
{
	char userId[MAX_ID_PWD_BYTE_LENGTH] = { 0, };
	char chatMsg[MAX_CHAT_MSG_SIZE] = { 0, };
};
#pragma endregion

#pragma pack(pop)