#pragma once
#pragma comment(lib, "ws2_32")
#include <WinSock2.h>
#include <WS2tcpip.h>

#define MAX_SOCKET_BUFFER 1024	// ��Ŷ ũ��
#define MAX_WORKER_THREAD 4		// ������ Ǯ�� ���� ������ ��

enum class IOEvent
{
	RECV,
	SEND
};

struct OverlappedEx
{
	WSAOVERLAPPED	wsaOverlapped;				// Overlapped I/O ����ü
	SOCKET			clientSocket;				// Ŭ���̾�Ʈ ����
	WSABUF			wsaBuf;						// Overlapped I/O �۾� ����
	IOEvent			ioEvent;					// �۾� ���� ����
};

struct ClientInfo
{
	SOCKET			clientSocket;
	OverlappedEx	recvOverlappedEx;
	OverlappedEx	sendOverlappedEx;

	char			recvBuffer[MAX_SOCKET_BUFFER];
	char			sendBuffer[MAX_SOCKET_BUFFER];

	ClientInfo()
	{
		ZeroMemory(&recvOverlappedEx, sizeof(OverlappedEx));
		ZeroMemory(&sendOverlappedEx, sizeof(OverlappedEx));
		clientSocket = INVALID_SOCKET;
	}
};