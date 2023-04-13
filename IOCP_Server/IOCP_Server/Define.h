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
	WSABUF			wsaBuf;						// Overlapped I/O 작업 버퍼
	IOEvent			ioEvent;					// 작업 동작 종류
};