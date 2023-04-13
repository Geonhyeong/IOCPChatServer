#pragma once
#include "Define.h"

#include <thread>
#include <vector>

class IOCP
{
public:
	IOCP() {}
	~IOCP()
	{
		// 윈속의 사용을 끝낸다.
		WSACleanup();
	}
	
	// 소켓을 초기화하는 함수
	bool InitSocket()
	{
		WSADATA wsaData;

		int nRet = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (nRet != 0)
		{
			printf("[에러] WSAStartup()함수 실패 : %d\n", WSAGetLastError());
			return false;
		}

		// 연결지향형 TCP, Overlapped I/O 소켓을 생성
		_listenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);

		if (_listenSocket == INVALID_SOCKET)
		{
			printf("[에러] socket()함수 실패 : %d\n", WSAGetLastError());
			return false;
		}

		printf("소켓 초기화 성공\n");
		return true;
	}

	//---------서버용 함수---------//
	// 서버의 주소정보를 소켓과 연결시키고 접속 요청을 받기 위해 소켓을 등록하는 함수
	bool BindAndListen(int port)
	{
		SOCKADDR_IN serverAddr;
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(port);
		serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

		int nRet = bind(_listenSocket, (SOCKADDR*)&serverAddr, sizeof(SOCKADDR_IN));
		if (nRet != 0)
		{
			printf("[에러] bind()함수 실패 : %d\n", WSAGetLastError());
			return false;
		}

		nRet = listen(_listenSocket, 5);
		if (nRet != 0)
		{
			printf("[에러] listen()함수 실패 : %d\n", WSAGetLastError());
			return false;
		}

		printf("서버 등록 성공..\n");
		return true;
	}

	// 접속 요청을 수락하고 메세지를 받아서 처리하는 함수
	bool StartServer(const UINT32 maxClientCount)
	{
		CreateClient(maxClientCount);

		_iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, MAX_WORKER_THREAD);
		if (_iocpHandle == NULL)
		{
			printf("[에러] CreateIoCompletionPort()함수 실패 : %d\n", GetLastError());
			return false;
		}

		bool bRet = CreateWorkerThread();
		if (bRet == false)
			return false;

		bRet = CreateAcceptThread();
		if (bRet == false)
			return false;

		printf("서버 시작\n");
		return true;
	}

	// 생성되어있는 쓰레드를 파괴하는 함수
	void DestroyThread()
	{
		_isWorkerRun = false;
		CloseHandle(_iocpHandle);

		for (auto& t : _workerThreads)
		{
			if (t.joinable())
				t.join();
		}

		_isAcceptRun = false;
		closesocket(_listenSocket);

		if (_acceptThread.joinable())
			_acceptThread.join();
	}


private:
	void CreateClient(const UINT32 maxClientCount)
	{
		for (UINT32 i = 0; i < maxClientCount; i++)
		{
			_clientInfos.emplace_back();
		}
	}

	// WaitingThread Queue에서 대기할 쓰레드들을 생성
	bool CreateWorkerThread()
	{
		// 권장 개수 : (cpu개수 * 2) + 1
		for (int i = 0; i < MAX_WORKER_THREAD; i++)
		{
			_workerThreads.emplace_back([this]() { WorkerThread(); });
		}

		printf("WorkerThread 시작...\n");
		return true;
	}

	// accept요청을 처리하는 쓰레드 생성
	bool CreateAcceptThread()
	{
		_acceptThread = std::thread([this]() { AcceptThread(); });

		printf("AcceptThread 시작...\n");
		return true;
	}

	// 사용하지 않는 클라이언트 정보 구조체를 반환한다.
	ClientInfo* GetEmptyClientInfo()
	{
		for (auto& client : _clientInfos)
		{
			if (client.clientSocket == INVALID_SOCKET)
				return &client;
		}

		return nullptr;
	}

	// IOCP 객체와 소켓과 Key를 연결시키는 역할을 한다.
	bool BindIOCP(ClientInfo* clientInfo)
	{
		auto iocpHandle = CreateIoCompletionPort((HANDLE)clientInfo->clientSocket, _iocpHandle, (ULONG_PTR)clientInfo, 0);

		if (iocpHandle == NULL || iocpHandle != _iocpHandle)
		{
			printf("[에러] CreateIoCompletionPort()함수 실패 : %d\n", GetLastError());
			return false;
		}

		return true;
	}

	// WSARecv Overlapped I/O 작업을 시킨다.
	bool BindRecv(ClientInfo* clientInfo)
	{
		DWORD flag = 0;
		DWORD numOfBytes = 0;
		
		// Overlapped I/O를 위해 각 정보를 세팅한다.
		clientInfo->recvOverlappedEx.wsaBuf.len = MAX_SOCKET_BUFFER;
		clientInfo->recvOverlappedEx.wsaBuf.buf = clientInfo->recvBuffer;
		clientInfo->recvOverlappedEx.ioEvent = IOEvent::RECV;

		int nRet = WSARecv(clientInfo->clientSocket, &(clientInfo->recvOverlappedEx.wsaBuf), 1, &numOfBytes, &flag, (LPWSAOVERLAPPED)&(clientInfo->recvOverlappedEx), NULL);

		// socket_error이면 client socket이 끊어진걸로 처리한다.
		if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
		{
			printf("[에러] WSARecv()함수 실패 : %d\n", WSAGetLastError());
			return false;
		}
		
		return true;
	}

	// WSASend Overlapped I/O 작업을 시킨다.
	bool SendMsg(ClientInfo* clientInfo, char* msg, int len)
	{
		DWORD numOfBytes = 0;

		//전송될 메세지를 복사
		CopyMemory(clientInfo->sendBuffer, msg, len);

		//Overlapped I/O을 위해 각 정보를 셋팅해 준다.
		clientInfo->sendOverlappedEx.wsaBuf.len = len;
		clientInfo->sendOverlappedEx.wsaBuf.buf = clientInfo->sendBuffer;
		clientInfo->sendOverlappedEx.ioEvent = IOEvent::SEND;

		int nRet = WSASend(clientInfo->clientSocket, &(clientInfo->sendOverlappedEx.wsaBuf), 1, &numOfBytes, 0,	(LPWSAOVERLAPPED)&(clientInfo->sendOverlappedEx), NULL);

		//socket_error이면 client socket이 끊어진걸로 처리한다.
		if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
		{
			printf("[에러] WSASend()함수 실패 : %d\n", WSAGetLastError());
			return false;
		}

		return true;
	}

	// Overlapped I/O 작업에 대한 완료 통보를 받아 그에 해당하는 처리를 하는 함수
	void WorkerThread()
	{
		ClientInfo* clientInfo = NULL;		// Key를 받을 포인터 변수
		bool bSuccess = TRUE;				// 함수 호출 성공 여부
		DWORD numOfBytes = 0;				// Overlapped I/O 작업에서 전송된 데이터 크기
		LPOVERLAPPED lpOverlapped = NULL;	// I/O 작업을 위해 요청한 Overlapped 구조체를 받을 포인터

		while (_isWorkerRun)
		{
			/*-------------------------------------------------------
				이 함수로 인해 쓰레드들은 WaitingThread Queue에
				대기 상태로 들어가게 된다.
				완료된 Overlapped I/O작업이 발생하면 IOCP Queue에서
				완료된 작업을 가져와 뒤 처리를 한다.
				그리고 GetQueuedCompletionStatus()함수에 의해 사용자
				메세지가 도착되면 쓰레드를 종료한다.
			-------------------------------------------------------*/
			bSuccess = GetQueuedCompletionStatus(_iocpHandle, &numOfBytes, (PULONG_PTR)&clientInfo, &lpOverlapped, INFINITE);

			// 사용자 쓰레드 종료 메세지 처리...
			if (bSuccess == TRUE && numOfBytes == 0 && lpOverlapped == NULL)
			{
				_isWorkerRun = false;
				continue;
			}

			if (lpOverlapped == NULL)
				continue;

			// client가 접속을 끊었을 때...
			if (bSuccess == FALSE || (numOfBytes == 0 && bSuccess == TRUE))
			{
				printf("socket(%d) 접속 끊김\n", (int)clientInfo->clientSocket);
				CloseSocket(clientInfo);
				continue;
			}

			OverlappedEx* overlappedEx = (OverlappedEx*)lpOverlapped;

			// Overlapped I/O Recv 작업 결과 뒤 처리
			if (overlappedEx->ioEvent == IOEvent::RECV)
			{
				clientInfo->recvBuffer[numOfBytes] = '\0';
				printf("[수신] bytes : %d, msg : %s\n", numOfBytes, clientInfo->recvBuffer);

				// 클라이언트에 메세지를 에코한다.
				SendMsg(clientInfo, clientInfo->recvBuffer, numOfBytes);
				BindRecv(clientInfo);
			}
			// Overlapped I/O Send 작업 결과 뒤 처리
			else if (overlappedEx->ioEvent == IOEvent::SEND)
			{
				printf("[송신] bytes : %d, msg : %s\n", numOfBytes, clientInfo->sendBuffer);
			}
			// 예외 상황
			else
			{
				printf("socket(%d)에서 예외상황\n", (int)clientInfo->clientSocket);
			}
		}
	}

	// 사용자의 접속을 받는 쓰레드
	void AcceptThread()
	{
		SOCKADDR_IN clientAddr;
		int nAddrLen = sizeof(SOCKADDR_IN);

		while (_isAcceptRun)
		{
			// 접속을 받을 구조체의 인덱스를 얻어온다.
			ClientInfo* clientInfo = GetEmptyClientInfo();
			if (clientInfo == NULL)
			{
				printf("[에러] Client Full\n");
				return;
			}

			// 클라이언트 접속 요청이 들어올 때까지 기다린다.
			clientInfo->clientSocket = accept(_listenSocket, (SOCKADDR*)&clientAddr, &nAddrLen);
			if (clientInfo->clientSocket == INVALID_SOCKET)
				continue;

			// I/O Completion Port객체와 소켓을 연결시킨다.
			bool bRet = BindIOCP(clientInfo);
			if (bRet == false)
				return;

			// Recv Overlapped I/O작업을 요청해 놓는다.
			bRet = BindRecv(clientInfo);
			if (bRet == false)
				return;

			char clientIP[32] = { 0, };
			inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, 32 - 1);
			printf("클라이언트 접속 : IP(%s) SOCKET(%d)\n", clientIP, (int)clientInfo->clientSocket);

			//클라이언트 갯수 증가
			_clientCnt++;
		}
	}

	// 소켓의 연결을 종료 시킨다.
	void CloseSocket(ClientInfo* clientInfo, bool isForce = false)
	{
		struct linger stLinger = { 0, 0 };	// SO_DONTLINGER로 설정

		// isForce가 true이면 SO_LINGER, timeout = 0으로 설정하여 강제 종료 시킨다. 주의 : 데이터 손실이 있을 수 있음
		if (isForce)
			stLinger.l_onoff = 1;
		
		// 소켓의 데이터 송수신을 모두 중단 시킨다.
		shutdown(clientInfo->clientSocket, SD_BOTH);

		// 소켓 옵션을 설정한다.
		setsockopt(clientInfo->clientSocket, SOL_SOCKET, SO_LINGER, (char*)&stLinger, sizeof(stLinger));

		// 소켓 연결을 종료 시킨다.
		closesocket(clientInfo->clientSocket);

		clientInfo->clientSocket = INVALID_SOCKET;
	}


private:
	std::vector<ClientInfo>		_clientInfos;							// 클라이언트 정보 저장 구조체
	SOCKET						_listenSocket = INVALID_SOCKET;			// 클라이언트의 접속을 받기 위한 리슨 소켓
	int							_clientCnt = 0;							// 접속 되어있는 클라이언트 수
	std::vector<std::thread>	_workerThreads;							// IO Worker 쓰레드
	std::thread					_acceptThread;							// Accept 쓰레드
	HANDLE						_iocpHandle = INVALID_HANDLE_VALUE;		// IOCP 객체 핸들
	bool						_isWorkerRun = true;					// Worker 쓰레드 동작 플래그
	bool						_isAcceptRun = true;					// Accept 쓰레드 동작 플래그
	char						_socketBuf[1024] = { 0, };				// 소켓 버퍼
};