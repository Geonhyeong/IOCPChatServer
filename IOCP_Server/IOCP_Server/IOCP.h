#pragma once
#include "Define.h"

#include <thread>
#include <vector>
#include "Session.h"

class IOCP
{
public:
	IOCP() {}
	~IOCP()
	{
		// ������ ����� ������.
		WSACleanup();
	}
	
	// ������ �ʱ�ȭ�ϴ� �Լ�
	bool InitSocket()
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

		printf("���� ��� ����..\n");
		return true;
	}

	// ���� ��û�� �����ϰ� �޼����� �޾Ƽ� ó���ϴ� �Լ�
	bool StartServer(const UINT32 maxClientCount)
	{
		CreateClient(maxClientCount);

		_iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, MAX_WORKER_THREAD);
		if (_iocpHandle == NULL)
		{
			printf("[����] CreateIoCompletionPort()�Լ� ���� : %d\n", GetLastError());
			return false;
		}

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

	virtual void OnConnected(const UINT32 sessionId) = 0;
	virtual void OnDisconnected(const UINT32 sessionId) = 0;
	virtual void OnRecv(const UINT32 sessionId, const UINT32 len, char* buf) = 0;


private:
	void CreateClient(const UINT32 maxClientCount)
	{
		for (UINT32 i = 0; i < maxClientCount; i++)
		{
			_sessions.emplace_back();
			_sessions[i]->Init(i);
		}
	}

	// WaitingThread Queue���� ����� ��������� ����
	bool CreateWorkerThread()
	{
		// ���� ���� : (cpu���� * 2) + 1
		for (int i = 0; i < MAX_WORKER_THREAD; i++)
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
		Session* session = NULL;			// Key�� ���� ������ ����
		bool bSuccess = TRUE;				// �Լ� ȣ�� ���� ����
		DWORD numOfBytes = 0;				// Overlapped I/O �۾����� ���۵� ������ ũ��
		LPOVERLAPPED lpOverlapped = NULL;	// I/O �۾��� ���� ��û�� Overlapped ����ü�� ���� ������

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
			bSuccess = GetQueuedCompletionStatus(_iocpHandle, &numOfBytes, (PULONG_PTR)&session, &lpOverlapped, INFINITE);

			// ����� ������ ���� �޼��� ó��...
			if (bSuccess == TRUE && numOfBytes == 0 && lpOverlapped == NULL)
			{
				_isWorkerRun = false;
				continue;
			}

			if (lpOverlapped == NULL)
				continue;

			// client�� ������ ������ ��...
			if (bSuccess == FALSE || (numOfBytes == 0 && bSuccess == TRUE))
			{
				CloseSocket(session);
				continue;
			}

			OverlappedEx* overlappedEx = (OverlappedEx*)lpOverlapped;

			if (overlappedEx->ioEvent == IOEvent::RECV)
			{
				session->ProcessRecv(numOfBytes);
				
				OnRecv(session->GetSessionId(), numOfBytes, session->RecvBuffer());
			}
			else if (overlappedEx->ioEvent == IOEvent::SEND)
			{
				session->ProcessSend(numOfBytes);

				// TODO : OverlappedEx ����ü�� ��������� �Ѵ�. ������ �������� �ʱ� ������
				delete[] overlappedEx->wsaBuf.buf;
				delete overlappedEx;
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
		SOCKADDR_IN clientAddr;
		int nAddrLen = sizeof(SOCKADDR_IN);

		while (_isAcceptRun)
		{
			// ������ ���� ����ü�� �ε����� ���´�.
			Session* session = GetEmptySession();
			if (session == NULL)
			{
				printf("[����] Client Full\n");
				return;
			}

			// Ŭ���̾�Ʈ ���� ��û�� ���� ������ ��ٸ���.
			SOCKET clientSocket = accept(_listenSocket, (SOCKADDR*)&clientAddr, &nAddrLen);
			if (clientSocket == INVALID_SOCKET)
				continue;

			if (session->Connect(_iocpHandle, clientSocket) == false)
			{
				session->Disconnect(true);
				return;
			}

			OnConnected(session->GetSessionId());

			//Ŭ���̾�Ʈ ���� ����
			_clientCnt++;
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
	std::vector<Session*>		_sessions;								// Ŭ���̾�Ʈ ���� ���� ����ü
	SOCKET						_listenSocket = INVALID_SOCKET;			// Ŭ���̾�Ʈ�� ������ �ޱ� ���� ���� ����
	int							_clientCnt = 0;							// ���� �Ǿ��ִ� Ŭ���̾�Ʈ ��
	std::vector<std::thread>	_workerThreads;							// IO Worker ������
	std::thread					_acceptThread;							// Accept ������
	HANDLE						_iocpHandle = INVALID_HANDLE_VALUE;		// IOCP ��ü �ڵ�
	bool						_isWorkerRun = true;					// Worker ������ ���� �÷���
	bool						_isAcceptRun = true;					// Accept ������ ���� �÷���
	char						_socketBuf[1024] = { 0, };				// ���� ����
};