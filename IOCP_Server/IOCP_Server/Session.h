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
		_socket = INVALID_SOCKET;
	}

	UINT32	GetSessionId() { return _sessionId; }
	UINT32	GetSocket() { return _socket; }
	char*	RecvBuffer() { return _recvBuffer; }
	bool	IsConnected() { return _socket != INVALID_SOCKET; }

	void Init(const UINT32 sessionId)
	{
		_sessionId = sessionId;
	}

	bool Connect(HANDLE iocpHandle, SOCKET socket)
	{
		if (IsConnected())
			return false;

		_socket = socket;

		// IOCP ��ü�� ������ �����Ų��.
		if (BindIOCP(iocpHandle) == false)
			return false;

		// ����Ǹ� �ϴ� WSARecv�� �����Ѵ�.
		return RegisterRecv();
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
	}

	bool Send(const UINT32 len, char* buf)
	{
		OverlappedEx* sendOverlappedEx = new OverlappedEx;
		ZeroMemory(sendOverlappedEx, sizeof(OverlappedEx));
		sendOverlappedEx->wsaBuf.len = len;
		sendOverlappedEx->wsaBuf.buf = new char[len];
		CopyMemory(sendOverlappedEx->wsaBuf.buf, buf, len);
		sendOverlappedEx->ioEvent = IOEvent::SEND;

		std::lock_guard<std::mutex> guard(_lock);
		
		_sendQueue.push(sendOverlappedEx);

		if (_sendQueue.size() == 1)
			RegisterSend();

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
	bool BindIOCP(HANDLE iocpHandle_)
	{
		auto iocpHandle = CreateIoCompletionPort((HANDLE)_socket, iocpHandle_, (ULONG_PTR)this, 0);

		if (iocpHandle == INVALID_HANDLE_VALUE)
		{
			printf("[����] CreateIoCompletionPort()�Լ� ���� : %d\n", GetLastError());
			return false;
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
	
	OverlappedEx				_recvOverlappedEx;
	char						_recvBuffer[MAX_SOCKET_BUFFER];

	std::mutex					_lock;
	std::queue<OverlappedEx*>	_sendQueue;
};