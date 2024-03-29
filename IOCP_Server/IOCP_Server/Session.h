#pragma once
#include "Define.h"

#include <stdio.h>
#include <queue>
#include <mutex>

class Session
{
	friend class IOCP;
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

		//printf("Accept %d...\n", _sessionId);

		_latestDisconnectedTimeSec = UINT32_MAX;

		return RegisterAccept(listenSocket);
	}

	bool Disconnect(bool isForce = false)
	{
		if (IsConnected() == false)
			return false;

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
		
		_isConnected = false;
		_latestDisconnectedTimeSec = std::chrono::duration_cast<std::chrono::seconds>
										(std::chrono::steady_clock::now().time_since_epoch()).count();
	}

	bool Send(const UINT32 len, char* buf)
	{
		// TODO : memory leak problem
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

private:
	bool RegisterAccept(SOCKET listenSocket)
	{
		_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (_socket == INVALID_SOCKET)
		{
			printf("[에러] WSASocket()함수 실패 : %d\n", GetLastError());
			return false;
		}

		DWORD bytes = 0;
		DWORD flags = 0;
		_acceptOverlappedEx.wsaBuf.len = 0;
		_acceptOverlappedEx.wsaBuf.buf = nullptr;
		_acceptOverlappedEx.ioEvent = IOEvent::ACCEPT;
		_acceptOverlappedEx.sessionId = _sessionId;

		if (FALSE == AcceptEx(listenSocket, _socket, _acceptBuffer, 0, 
			sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &bytes, (LPWSAOVERLAPPED)&_acceptOverlappedEx))
		{
			if (WSAGetLastError() != WSA_IO_PENDING)
			{
				printf("[에러] AcceptEx()함수 실패 : %d\n", GetLastError());
				return false;
			}
		}

		return true;
	}

	bool RegisterRecv()
	{
		DWORD flag = 0;
		DWORD numOfBytes = 0;

		// Overlapped I/O를 위해 각 정보를 세팅한다.
		_recvOverlappedEx.wsaBuf.len = MAX_SOCKET_BUFFER;
		_recvOverlappedEx.wsaBuf.buf = _recvBuffer;
		_recvOverlappedEx.ioEvent = IOEvent::RECV;
		_recvOverlappedEx.sessionId = _sessionId;

		int nRet = WSARecv(_socket, &(_recvOverlappedEx.wsaBuf), 1, &numOfBytes, &flag, (LPWSAOVERLAPPED)&_recvOverlappedEx, NULL);

		// socket_error이면 client socket이 끊어진걸로 처리한다.
		if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
		{
			printf("[에러] WSARecv()함수 실패 : %d\n", WSAGetLastError());
			return false;
		}

		return true;
	}

	bool RegisterSend()
	{
		auto sendOverlappedEx = _sendQueue.front();

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

	bool ProcessAccept(SOCKET listenSocket)
	{
		_isConnected = true;

		// SO_KEEPALIVE 옵션 설정
		BOOL optval = TRUE;
		setsockopt(_socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (const char*)&listenSocket, sizeof(SOCKET));
		setsockopt(_socket, SOL_SOCKET, SO_KEEPALIVE, (const char*)&optval, sizeof(BOOL));

		tcp_keepalive sKA_Settings = { 0 };
		sKA_Settings.onoff = 1;
		sKA_Settings.keepalivetime = 5000;        // Keep Alive in 5 sec.
		sKA_Settings.keepaliveinterval = 1000;        // Resend if No-Reply

		DWORD dwBytes;
		if (WSAIoctl(_socket, SIO_KEEPALIVE_VALS, &sKA_Settings, sizeof(sKA_Settings), 0, 0, &dwBytes, NULL, NULL) == SOCKET_ERROR)
		{
			printf("[에러] WSAIoctl()함수 SO_KEEPALIVE 옵션 설정 실패 : %d\n", WSAGetLastError());
			return false;
		}

		// IOCP 객체와 소켓을 연결시킨다.
		auto iocpHandle = CreateIoCompletionPort((HANDLE)_socket, _iocpHandle, 0, 0);
		if (iocpHandle == INVALID_HANDLE_VALUE)
		{
			printf("[에러] CreateIoCompletionPort()함수 실패 : %d\n", GetLastError());
			return false;
		}

		// 연결되면 일단 WSARecv를 예약한다.
		if (RegisterRecv() == false)
			return false;

		// 클라이언트 IP와 Port 출력
		PSOCKADDR pRemoteSocketAddr = nullptr;
		PSOCKADDR pLocalSocketAddr = nullptr;
		int pRemoteSocketAddrLen = 0;
		int pLocalSocketAddrLen = 0;
		GetAcceptExSockaddrs(_acceptBuffer, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &pLocalSocketAddr, &pLocalSocketAddrLen, &pRemoteSocketAddr, &pRemoteSocketAddrLen);

		SOCKADDR_IN remoteAddr = *(reinterpret_cast<PSOCKADDR_IN>(pRemoteSocketAddr));
		printf("클라이언트 접속 IP(%s), Port(%d)\n", inet_ntoa(remoteAddr.sin_addr), ntohs(remoteAddr.sin_port));

		return true;
	}

	bool ProcessRecv(const UINT32 len)
	{
		//printf("[수신] 세션 : %d, bytes : %d\n", _sessionId, len);

		return RegisterRecv();
	}

	bool ProcessSend(const UINT32 len)
	{
		//printf("[송신] 세션 : %d, bytes : %d\n", _sessionId, len);

		std::lock_guard<std::mutex> guard(_lock);

		delete[] _sendQueue.front()->wsaBuf.buf;
		delete _sendQueue.front();

		_sendQueue.pop();

		if (_sendQueue.empty() == false)
			RegisterSend();

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