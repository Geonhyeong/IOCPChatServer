#pragma once
#include "Define.h"

#include <thread>
#include <vector>
#include "Session.h"

class IOCP
{
public:
	IOCP() {}
	virtual ~IOCP()
	{
		// 윈속의 사용을 끝낸다.
		WSACleanup();
	}
	
	// 소켓 및 설정을 초기화하는 함수
	bool Init(const UINT32 maxWorkerThreadCount)
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

		_maxWorkerThreadCount = maxWorkerThreadCount;

		printf("서버 초기화 성공\n");
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

		_iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, _maxWorkerThreadCount);
		if (_iocpHandle == NULL)
		{
			printf("[에러] CreateIoCompletionPort()함수 실패 : %d\n", GetLastError());
			return false;
		}

		auto iocpHandle = CreateIoCompletionPort((HANDLE)_listenSocket, _iocpHandle, 0, 0);
		if (iocpHandle == NULL)
		{
			printf("[에러] listen socket IOCP bind 실패 : %d\n", GetLastError());
			return false;
		}

		printf("서버 등록 성공..\n");
		return true;
	}

	// 접속 요청을 수락하고 메세지를 받아서 처리하는 함수
	bool StartServer(const UINT32 maxClientCount)
	{
		CreateClient(maxClientCount);

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

	bool Send(const UINT32 sessionId, const UINT32 len, char* buf)
	{
		Session* session = _sessions[sessionId];
		return session->Send(len, buf);
	}

	void Disconnect(const UINT32 sessionId)
	{
		Session* session = _sessions[sessionId];
		CloseSocket(session, true);
	}

	virtual void OnConnected(const UINT32 sessionId) = 0;
	virtual void OnDisconnected(const UINT32 sessionId) = 0;
	virtual void OnRecv(const UINT32 sessionId, const UINT32 len, char* buf) = 0;

private:
	void CreateClient(const UINT32 maxClientCount)
	{
		for (UINT32 i = 0; i < maxClientCount; i++)
		{
			Session* session = new Session();
			session->Init(i, _iocpHandle);

			_sessions.push_back(session);
		}
	}

	// WaitingThread Queue에서 대기할 쓰레드들을 생성
	bool CreateWorkerThread()
	{
		// 권장 개수 : (cpu개수 * 2) + 1
		for (UINT32 i = 0; i < _maxWorkerThreadCount; i++)
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

	// 사용하지 않는 세션을 반환한다.
	Session* GetEmptySession()
	{
		for (auto& session : _sessions)
		{
			if (session->IsConnected() == false)
				return session;
		}

		return nullptr;
	}

	// Overlapped I/O 작업에 대한 완료 통보를 받아 그에 해당하는 처리를 하는 함수
	void WorkerThread()
	{
		ULONG_PTR key = 0;				// Key를 받을 포인터 변수
		bool bSuccess = TRUE;				// 함수 호출 성공 여부
		DWORD numOfBytes = 0;				// Overlapped I/O 작업에서 전송된 데이터 크기
		LPOVERLAPPED lpOverlapped = NULL;	// I/O 작업을 위해 요청한 Overlapped 구조체를 받을 포인터
		Session* session = NULL;

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
			bSuccess = GetQueuedCompletionStatus(_iocpHandle, &numOfBytes, &key, &lpOverlapped, INFINITE);

			// 사용자 쓰레드 종료 메세지 처리...
			if (bSuccess == TRUE && numOfBytes == 0 && lpOverlapped == NULL)
			{
				_isWorkerRun = false;
				continue;
			}

			if (lpOverlapped == NULL)
				continue;

			OverlappedEx* overlappedEx = (OverlappedEx*)lpOverlapped;
			session = _sessions[overlappedEx->sessionId];
			
			// client가 접속을 끊었을 때...
			if (bSuccess == FALSE || (numOfBytes == 0 && overlappedEx->ioEvent != IOEvent::ACCEPT))
			{
				CloseSocket(session);
				continue;
			}

			if (overlappedEx->ioEvent == IOEvent::ACCEPT)
			{
				if (session->ProcessAccept(_listenSocket))
				{
					OnConnected(session->GetSessionId());

					//클라이언트 갯수 증가
					_clientCnt++;
				}
				else
				{
					CloseSocket(session, true);
				}
			}
			else if (overlappedEx->ioEvent == IOEvent::RECV)
			{
				OnRecv(session->GetSessionId(), numOfBytes, session->RecvBuffer());
				
				session->ProcessRecv(numOfBytes);
			}
			else if (overlappedEx->ioEvent == IOEvent::SEND)
			{
				session->ProcessSend(numOfBytes);
			}
			// 예외 상황
			else
			{
				printf("socket(%d)에서 예외상황\n", (int)session->GetSocket());
			}
		}
	}

	// 사용자의 접속을 받는 쓰레드
	void AcceptThread()
	{
		while (_isAcceptRun)
		{
			auto curTimeSec = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now().time_since_epoch()).count();

			for (auto& session : _sessions)
			{
				if (session->IsConnected())
					continue;

				if ((UINT64)curTimeSec < session->GetLatestDisconnectedTimeSec())
					continue;

				auto diff = curTimeSec - session->GetLatestDisconnectedTimeSec();
				if (diff <= REUSE_SESSION_WAIT_TIMESEC)
					continue;

				session->Accept(_listenSocket);
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(32));
		}
	}

	// 소켓의 연결을 종료 시킨다.
	void CloseSocket(Session* session, bool isForce = false)
	{
		auto sessionId = session->GetSessionId();

		session->Disconnect(isForce);

		OnDisconnected(sessionId);
	}


private:
	SOCKET						_listenSocket = INVALID_SOCKET;			// 클라이언트의 접속을 받기 위한 리슨 소켓
	HANDLE						_iocpHandle = INVALID_HANDLE_VALUE;		// IOCP 객체 핸들
	
	std::vector<Session*>		_sessions;								// 클라이언트 정보 저장 구조체
	int							_clientCnt = 0;							// 접속 되어있는 클라이언트 수

	std::vector<std::thread>	_workerThreads;							// IO Worker 쓰레드
	std::thread					_acceptThread;							// Accept 쓰레드
	bool						_isWorkerRun = true;					// Worker 쓰레드 동작 플래그
	bool						_isAcceptRun = true;					// Accept 쓰레드 동작 플래그
	UINT32						_maxWorkerThreadCount = 0;
};