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
		// ������ ����� ������.
		WSACleanup();
	}
	
	// ���� �� ������ �ʱ�ȭ�ϴ� �Լ�
	bool Init(const UINT32 maxWorkerThreadCount)
	{
		WSADATA wsaData;

		int nRet = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (nRet != 0)
		{
			printf("[����] WSAStartup()�Լ� ���� : %d\n", WSAGetLastError());
			return false;
		}

		// ���������� TCP, Overlapped I/O ������ ����
		_listenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);

		if (_listenSocket == INVALID_SOCKET)
		{
			printf("[����] socket()�Լ� ���� : %d\n", WSAGetLastError());
			return false;
		}

		_maxWorkerThreadCount = maxWorkerThreadCount;

		printf("���� �ʱ�ȭ ����\n");
		return true;
	}

	//---------������ �Լ�---------//
	// ������ �ּ������� ���ϰ� �����Ű�� ���� ��û�� �ޱ� ���� ������ ����ϴ� �Լ�
	bool BindAndListen(int port)
	{
		SOCKADDR_IN serverAddr;
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(port);
		serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

		int nRet = bind(_listenSocket, (SOCKADDR*)&serverAddr, sizeof(SOCKADDR_IN));
		if (nRet != 0)
		{
			printf("[����] bind()�Լ� ���� : %d\n", WSAGetLastError());
			return false;
		}

		nRet = listen(_listenSocket, 5);
		if (nRet != 0)
		{
			printf("[����] listen()�Լ� ���� : %d\n", WSAGetLastError());
			return false;
		}

		_iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, _maxWorkerThreadCount);
		if (_iocpHandle == NULL)
		{
			printf("[����] CreateIoCompletionPort()�Լ� ���� : %d\n", GetLastError());
			return false;
		}

		auto iocpHandle = CreateIoCompletionPort((HANDLE)_listenSocket, _iocpHandle, 0, 0);
		if (iocpHandle == NULL)
		{
			printf("[����] listen socket IOCP bind ���� : %d\n", GetLastError());
			return false;
		}

		printf("���� ��� ����..\n");
		return true;
	}

	// ���� ��û�� �����ϰ� �޼����� �޾Ƽ� ó���ϴ� �Լ�
	bool StartServer(const UINT32 maxClientCount)
	{
		CreateClient(maxClientCount);

		bool bRet = CreateWorkerThread();
		if (bRet == false)
			return false;

		bRet = CreateAcceptThread();
		if (bRet == false)
			return false;

		printf("���� ����\n");
		return true;
	}

	// �����Ǿ��ִ� �����带 �ı��ϴ� �Լ�
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

	// WaitingThread Queue���� ����� ��������� ����
	bool CreateWorkerThread()
	{
		// ���� ���� : (cpu���� * 2) + 1
		for (UINT32 i = 0; i < _maxWorkerThreadCount; i++)
		{
			_workerThreads.emplace_back([this]() { WorkerThread(); });
		}

		printf("WorkerThread ����...\n");
		return true;
	}

	// accept��û�� ó���ϴ� ������ ����
	bool CreateAcceptThread()
	{
		_acceptThread = std::thread([this]() { AcceptThread(); });

		printf("AcceptThread ����...\n");
		return true;
	}

	// ������� �ʴ� ������ ��ȯ�Ѵ�.
	Session* GetEmptySession()
	{
		for (auto& session : _sessions)
		{
			if (session->IsConnected() == false)
				return session;
		}

		return nullptr;
	}

	// Overlapped I/O �۾��� ���� �Ϸ� �뺸�� �޾� �׿� �ش��ϴ� ó���� �ϴ� �Լ�
	void WorkerThread()
	{
		ULONG_PTR key = 0;				// Key�� ���� ������ ����
		bool bSuccess = TRUE;				// �Լ� ȣ�� ���� ����
		DWORD numOfBytes = 0;				// Overlapped I/O �۾����� ���۵� ������ ũ��
		LPOVERLAPPED lpOverlapped = NULL;	// I/O �۾��� ���� ��û�� Overlapped ����ü�� ���� ������
		Session* session = NULL;

		while (_isWorkerRun)
		{
			/*-------------------------------------------------------
				�� �Լ��� ���� ��������� WaitingThread Queue��
				��� ���·� ���� �ȴ�.
				�Ϸ�� Overlapped I/O�۾��� �߻��ϸ� IOCP Queue����
				�Ϸ�� �۾��� ������ �� ó���� �Ѵ�.
				�׸��� GetQueuedCompletionStatus()�Լ��� ���� �����
				�޼����� �����Ǹ� �����带 �����Ѵ�.
			-------------------------------------------------------*/
			bSuccess = GetQueuedCompletionStatus(_iocpHandle, &numOfBytes, &key, &lpOverlapped, INFINITE);

			// ����� ������ ���� �޼��� ó��...
			if (bSuccess == TRUE && numOfBytes == 0 && lpOverlapped == NULL)
			{
				_isWorkerRun = false;
				continue;
			}

			if (lpOverlapped == NULL)
				continue;

			OverlappedEx* overlappedEx = (OverlappedEx*)lpOverlapped;
			session = _sessions[overlappedEx->sessionId];
			
			// client�� ������ ������ ��...
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

					//Ŭ���̾�Ʈ ���� ����
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
			// ���� ��Ȳ
			else
			{
				printf("socket(%d)���� ���ܻ�Ȳ\n", (int)session->GetSocket());
			}
		}
	}

	// ������� ������ �޴� ������
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

	// ������ ������ ���� ��Ų��.
	void CloseSocket(Session* session, bool isForce = false)
	{
		auto sessionId = session->GetSessionId();

		session->Disconnect(isForce);

		OnDisconnected(sessionId);
	}


private:
	SOCKET						_listenSocket = INVALID_SOCKET;			// Ŭ���̾�Ʈ�� ������ �ޱ� ���� ���� ����
	HANDLE						_iocpHandle = INVALID_HANDLE_VALUE;		// IOCP ��ü �ڵ�
	
	std::vector<Session*>		_sessions;								// Ŭ���̾�Ʈ ���� ���� ����ü
	int							_clientCnt = 0;							// ���� �Ǿ��ִ� Ŭ���̾�Ʈ ��

	std::vector<std::thread>	_workerThreads;							// IO Worker ������
	std::thread					_acceptThread;							// Accept ������
	bool						_isWorkerRun = true;					// Worker ������ ���� �÷���
	bool						_isAcceptRun = true;					// Accept ������ ���� �÷���
	UINT32						_maxWorkerThreadCount = 0;
};