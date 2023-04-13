#pragma once
#pragma comment(lib, "ws2_32")
#include <WinSock2.h>
#include <WS2tcpip.h>

#define MAX_SOCKET_BUFFER 1024	// 패킷 크기
#define MAX_WORKER_THREAD 4		// 쓰레드 풀에 넣을 쓰레드 수

enum class IOEvent
{
	RECV,
	SEND
};

struct OverlappedEx
{
	WSAOVERLAPPED	wsaOverlapped;				// Overlapped I/O 구조체
	SOCKET			clientSocket;				// 클라이언트 소켓
	WSABUF			wsaBuf;						// Overlapped I/O 작업 버퍼
	IOEvent			ioEvent;					// 작업 동작 종류
};

struct Session
{
	SOCKET			socket;
	OverlappedEx	recvOverlappedEx;
	OverlappedEx	sendOverlappedEx;

	UINT32			sessionId = 0;
	char			recvBuffer[MAX_SOCKET_BUFFER];
	char			sendBuffer[MAX_SOCKET_BUFFER];

	Session()
	{
		ZeroMemory(&recvOverlappedEx, sizeof(OverlappedEx));
		ZeroMemory(&sendOverlappedEx, sizeof(OverlappedEx));
		socket = INVALID_SOCKET;
	}
};