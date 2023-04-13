#pragma once
#include "Define.h"

#include <stdio.h>

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
		_socket = socket;

		// IOCP 객체와 소켓을 연결시킨다.
		if (BindIOCP(iocpHandle) == false)
			return false;

		// 연결되면 일단 WSARecv를 예약한다.
		return RegisterRecv();
	}

	void Disconnect(bool isForce = false)
	{
		struct linger stLinger = { 0, 0 };	// SO_DONTLINGER로 설정

		// isForce가 true이면 SO_LINGER, timeout = 0으로 설정하여 강제 종료 시킨다. 주의 : 데이터 손실이 있을 수 있음
		if (isForce)
			stLinger.l_onoff = 1;

		// 소켓의 데이터 송수신을 모두 중단 시킨다.
		shutdown(_socket, SD_BOTH);

		// 소켓 옵션을 설정한다.
		setsockopt(_socket, SOL_SOCKET, SO_LINGER, (char*)&stLinger, sizeof(stLinger));

		// 소켓 연결을 종료 시킨다.
		closesocket(_socket);
		_socket = INVALID_SOCKET;
	}

	bool RegisterRecv()
	{
		DWORD flag = 0;
		DWORD numOfBytes = 0;

		// Overlapped I/O를 위해 각 정보를 세팅한다.
		_recvOverlappedEx.wsaBuf.len = MAX_SOCKET_BUFFER;
		_recvOverlappedEx.wsaBuf.buf = _recvBuffer;
		_recvOverlappedEx.ioEvent = IOEvent::RECV;

		int nRet = WSARecv(_socket, &(_recvOverlappedEx.wsaBuf), 1, &numOfBytes, &flag, (LPWSAOVERLAPPED)&(_recvOverlappedEx), NULL);

		// socket_error이면 client socket이 끊어진걸로 처리한다.
		if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
		{
			printf("[에러] WSARecv()함수 실패 : %d\n", WSAGetLastError());
			return false;
		}

		return true;
	}

	bool RegisterSend(const UINT32 len, char* buf)
	{
		// TODO : 매번 sendOverlappedEx 구조체를 생성한다. 이를 재사용하는 방법을 생각하자.
		OverlappedEx* sendOverlappedEx = new OverlappedEx;
		ZeroMemory(sendOverlappedEx, sizeof(OverlappedEx));
		sendOverlappedEx->wsaBuf.len = len;
		sendOverlappedEx->wsaBuf.buf = new char[len];
		CopyMemory(sendOverlappedEx->wsaBuf.buf, buf, len);
		sendOverlappedEx->ioEvent = IOEvent::SEND;

		DWORD numOfBytes = 0;
		int nRet = WSASend(_socket, &(sendOverlappedEx->wsaBuf), 1, &numOfBytes, 0, (LPWSAOVERLAPPED)sendOverlappedEx, NULL);

		//socket_error이면 client socket이 끊어진걸로 처리한다.
		if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
		{
			printf("[에러] WSASend()함수 실패 : %d\n", WSAGetLastError());
			return false;
		}

		return true;
	}

	void ProcessRecv(const UINT32 len)
	{
		printf("[수신] 세션 : %d, bytes : %d\n", _sessionId, len);

		RegisterRecv();
	}

	void ProcessSend(const UINT32 len)
	{
		printf("[송신] 세션 : %d, bytes : %d\n", _sessionId, len);
	}

private:
	bool BindIOCP(HANDLE iocpHandle_)
	{
		auto iocpHandle = CreateIoCompletionPort((HANDLE)_socket, iocpHandle_, (ULONG_PTR)this, 0);

		if (iocpHandle == INVALID_HANDLE_VALUE)
		{
			printf("[에러] CreateIoCompletionPort()함수 실패 : %d\n", GetLastError());
			return false;
		}

		return true;
	}

private:
	UINT32			_sessionId;
	SOCKET			_socket;
	
	OverlappedEx	_recvOverlappedEx;
	char			_recvBuffer[MAX_SOCKET_BUFFER];
};