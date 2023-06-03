#pragma once
#pragma comment(lib, "ws2_32")
#pragma comment(lib, "mswsock.lib")

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <mstcpip.h>

const UINT32 MAX_SOCKET_BUFFER = 1024;			// ��Ŷ ũ��
const UINT64 REUSE_SESSION_WAIT_TIMESEC = 3;	// ���� ���� ���ð�

enum class IOEvent
{
	ACCEPT,
	RECV,
	SEND
};

struct OverlappedEx
{
	WSAOVERLAPPED	wsaOverlapped;				// Overlapped I/O ����ü
	WSABUF			wsaBuf;						// Overlapped I/O �۾� ����
	IOEvent			ioEvent;					// �۾� ���� ����
	UINT32			sessionId;					// AcceptEvent���� Session�� �о���� ���� key
};