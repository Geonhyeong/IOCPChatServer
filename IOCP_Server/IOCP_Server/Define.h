#pragma once
#pragma comment(lib, "ws2_32")
#pragma comment(lib, "mswsock.lib")

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <mstcpip.h>

const UINT32 MAX_SOCKET_BUFFER = 1024;			// 패킷 크기
const UINT64 REUSE_SESSION_WAIT_TIMESEC = 3;	// 소켓 재사용 대기시간

enum class IOEvent
{
	ACCEPT,
	RECV,
	SEND
};

struct OverlappedEx
{
	WSAOVERLAPPED	wsaOverlapped;				// Overlapped I/O 구조체
	WSABUF			wsaBuf;						// Overlapped I/O 작업 버퍼
	IOEvent			ioEvent;					// 작업 동작 종류
	UINT32			sessionId;					// AcceptEvent에서 Session을 읽어오기 위한 key
};