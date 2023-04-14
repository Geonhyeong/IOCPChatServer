#pragma once
#include "Define.h"

#include <stdio.h>
#include <queue>
#include <mutex>

class Session
{
public:
	Session()
	{
		ZeroMemory(&_recvOverlappedEx, sizeof(OverlappedEx));
		ZeroMemory(&_acceptOverlappedEx, sizeof(OverlappedEx));
		_socket = INVALID_SOCKET;
		_iocpHandle = INVALID_HANDLE_VALUE;
	}

	UINT32	GetSessionId() { return _sessionId; }
	UINT32	GetSocket() { return _socket; }
	char*	RecvBuffer() { return _recvBuffer; }
	bool	IsConnected() { return _isConnected; }
	UINT64	GetLatestDisconnectedTimeSec() { return _latestDisconnectedTimeSec; }

	void Init(const UINT32 sessionId, HANDLE iocpHandle)
	{
		_sessionId = sessionId;
		_iocpHandle = iocpHandle;
	}

	bool Accept(SOCKET listenSocket)
	{
		if (IsConnected())
			return false;

		return RegisterAccept(listenSocket);
	}

	void Disconnect(bool isForce = false)
	{
		if (IsConnected() == false)
			return;

		struct linger stLinger = { 0, 0 };	// SO_DONTLINGER�� ����

		// isForce�� true�̸� SO_LINGER, timeout = 0���� �����Ͽ� ���� ���� ��Ų��. ���� : ������ �ս��� ���� �� ����
		if (isForce)
			stLinger.l_onoff = 1;

		// ������ ������ �ۼ����� ��� �ߴ� ��Ų��.
		shutdown(_socket, SD_BOTH);

		// ���� �ɼ��� �����Ѵ�.
		setsockopt(_socket, SOL_SOCKET, SO_LINGER, (char*)&stLinger, sizeof(stLinger));

		// ���� ������ ���� ��Ų��.
		closesocket(_socket);
		_socket = INVALID_SOCKET;
		
		_isConnected = false;
		_latestDisconnectedTimeSec = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
	}

	bool Send(const UINT32 len, char* buf)
	{
		OverlappedEx* sendOverlappedEx = new OverlappedEx;
		ZeroMemory(sendOverlappedEx, sizeof(OverlappedEx));
		sendOverlappedEx->wsaBuf.len = len;
		sendOverlappedEx->wsaBuf.buf = new char[len];
		CopyMemory(sendOverlappedEx->wsaBuf.buf, buf, len);
		sendOverlappedEx->ioEvent = IOEvent::SEND;
		sendOverlappedEx->sessionId = _sessionId;

		std::lock_guard<std::mutex> guard(_lock);
		
		_sendQueue.push(sendOverlappedEx);

		if (_sendQueue.size() == 1)
			RegisterSend();

		return true;
	}

	bool ProcessAccept()
	{
		_isConnected = true;

		// IOCP ��ü�� ������ �����Ų��.
		auto iocpHandle = CreateIoCompletionPort((HANDLE)_socket, _iocpHandle, 0, 0);
		if (iocpHandle == INVALID_HANDLE_VALUE)
		{
			printf("[����] CreateIoCompletionPort()�Լ� ���� : %d\n", GetLastError());
			return false;
		}

		// ����Ǹ� �ϴ� WSARecv�� �����Ѵ�.
		if (RegisterRecv() == false)
			return false;

		SOCKADDR_IN clientAddr;
		int addrLen = sizeof(SOCKADDR_IN);
		char clientIP[32] = { 0, };
		inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, 32 - 1);
		
		printf("Ŭ���̾�Ʈ ���� : IP(%s) SOCKET(%d)\n", clientIP, (int)_socket);

		return true;
	}

	void ProcessRecv(const UINT32 len)
	{
		printf("[����] ���� : %d, bytes : %d\n", _sessionId, len);

		RegisterRecv();
	}

	void ProcessSend(const UINT32 len)
	{
		printf("[�۽�] ���� : %d, bytes : %d\n", _sessionId, len);

		std::lock_guard<std::mutex> guard(_lock);

		delete[] _sendQueue.front()->wsaBuf.buf;
		delete _sendQueue.front();

		_sendQueue.pop();

		if (_sendQueue.empty() == false)
			RegisterSend();
	}

private:
	bool RegisterAccept(SOCKET listenSocket)
	{
		_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (_socket == INVALID_SOCKET)
		{
			printf("[����] WSASocket()�Լ� ���� : %d\n", GetLastError());
			return false;
		}

		DWORD bytes = 0;
		DWORD flags = 0;
		_acceptOverlappedEx.wsaBuf.len = 0;
		_acceptOverlappedEx.wsaBuf.buf = nullptr;
		_acceptOverlappedEx.ioEvent = IOEvent::ACCEPT;
		_acceptOverlappedEx.sessionId = _sessionId;

		if (FALSE == AcceptEx(listenSocket, _socket, _acceptBuffer, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &bytes, (LPWSAOVERLAPPED)&_acceptOverlappedEx))
		{
			if (WSAGetLastError() != WSA_IO_PENDING)
			{
				printf("[����] AcceptEx()�Լ� ���� : %d\n", GetLastError());
				return false;
			}
		}

		return true;
	}

	bool RegisterRecv()
	{
		DWORD flag = 0;
		DWORD numOfBytes = 0;

		// Overlapped I/O�� ���� �� ������ �����Ѵ�.
		_recvOverlappedEx.wsaBuf.len = MAX_SOCKET_BUFFER;
		_recvOverlappedEx.wsaBuf.buf = _recvBuffer;
		_recvOverlappedEx.ioEvent = IOEvent::RECV;
		_recvOverlappedEx.sessionId = _sessionId;

		int nRet = WSARecv(_socket, &(_recvOverlappedEx.wsaBuf), 1, &numOfBytes, &flag, (LPWSAOVERLAPPED)&(_recvOverlappedEx), NULL);

		// socket_error�̸� client socket�� �������ɷ� ó���Ѵ�.
		if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
		{
			printf("[����] WSARecv()�Լ� ���� : %d\n", WSAGetLastError());
			return false;
		}

		return true;
	}

	bool RegisterSend()
	{
		auto sendOverlappedEx = _sendQueue.front();

		DWORD numOfBytes = 0;
		int nRet = WSASend(_socket, &(sendOverlappedEx->wsaBuf), 1, &numOfBytes, 0, (LPWSAOVERLAPPED)sendOverlappedEx, NULL);

		//socket_error�̸� client socket�� �������ɷ� ó���Ѵ�.
		if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
		{
			printf("[����] WSASend()�Լ� ���� : %d\n", WSAGetLastError());
			return false;
		}

		return true;
	}

private:
	UINT32						_sessionId;
	SOCKET						_socket;
	HANDLE						_iocpHandle;
	bool						_isConnected = false;
	UINT64						_latestDisconnectedTimeSec = 0;
	
	OverlappedEx				_recvOverlappedEx;
	OverlappedEx				_acceptOverlappedEx;
	char						_recvBuffer[MAX_SOCKET_BUFFER];
	char						_acceptBuffer[64];

	std::mutex					_lock;
	std::queue<OverlappedEx*>	_sendQueue;
};