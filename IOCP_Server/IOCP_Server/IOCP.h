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


private:
	void CreateClient(const UINT32 maxClientCount)
	{
		for (UINT32 i = 0; i < maxClientCount; i++)
		{
			_clientInfos.emplace_back();
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

	// ������� �ʴ� Ŭ���̾�Ʈ ���� ����ü�� ��ȯ�Ѵ�.
	ClientInfo* GetEmptyClientInfo()
	{
		for (auto& client : _clientInfos)
		{
			if (client.clientSocket == INVALID_SOCKET)
				return &client;
		}

		return nullptr;
	}

	// IOCP ��ü�� ���ϰ� Key�� �����Ű�� ������ �Ѵ�.
	bool BindIOCP(ClientInfo* clientInfo)
	{
		auto iocpHandle = CreateIoCompletionPort((HANDLE)clientInfo->clientSocket, _iocpHandle, (ULONG_PTR)clientInfo, 0);

		if (iocpHandle == NULL || iocpHandle != _iocpHandle)
		{
			printf("[����] CreateIoCompletionPort()�Լ� ���� : %d\n", GetLastError());
			return false;
		}

		return true;
	}

	// WSARecv Overlapped I/O �۾��� ��Ų��.
	bool BindRecv(ClientInfo* clientInfo)
	{
		DWORD flag = 0;
		DWORD numOfBytes = 0;
		
		// Overlapped I/O�� ���� �� ������ �����Ѵ�.
		clientInfo->recvOverlappedEx.wsaBuf.len = MAX_SOCKET_BUFFER;
		clientInfo->recvOverlappedEx.wsaBuf.buf = clientInfo->recvBuffer;
		clientInfo->recvOverlappedEx.ioEvent = IOEvent::RECV;

		int nRet = WSARecv(clientInfo->clientSocket, &(clientInfo->recvOverlappedEx.wsaBuf), 1, &numOfBytes, &flag, (LPWSAOVERLAPPED)&(clientInfo->recvOverlappedEx), NULL);

		// socket_error�̸� client socket�� �������ɷ� ó���Ѵ�.
		if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
		{
			printf("[����] WSARecv()�Լ� ���� : %d\n", WSAGetLastError());
			return false;
		}
		
		return true;
	}

	// WSASend Overlapped I/O �۾��� ��Ų��.
	bool SendMsg(ClientInfo* clientInfo, char* msg, int len)
	{
		DWORD numOfBytes = 0;

		//���۵� �޼����� ����
		CopyMemory(clientInfo->sendBuffer, msg, len);

		//Overlapped I/O�� ���� �� ������ ������ �ش�.
		clientInfo->sendOverlappedEx.wsaBuf.len = len;
		clientInfo->sendOverlappedEx.wsaBuf.buf = clientInfo->sendBuffer;
		clientInfo->sendOverlappedEx.ioEvent = IOEvent::SEND;

		int nRet = WSASend(clientInfo->clientSocket, &(clientInfo->sendOverlappedEx.wsaBuf), 1, &numOfBytes, 0,	(LPWSAOVERLAPPED)&(clientInfo->sendOverlappedEx), NULL);

		//socket_error�̸� client socket�� �������ɷ� ó���Ѵ�.
		if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
		{
			printf("[����] WSASend()�Լ� ���� : %d\n", WSAGetLastError());
			return false;
		}

		return true;
	}

	// Overlapped I/O �۾��� ���� �Ϸ� �뺸�� �޾� �׿� �ش��ϴ� ó���� �ϴ� �Լ�
	void WorkerThread()
	{
		ClientInfo* clientInfo = NULL;		// Key�� ���� ������ ����
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
			bSuccess = GetQueuedCompletionStatus(_iocpHandle, &numOfBytes, (PULONG_PTR)&clientInfo, &lpOverlapped, INFINITE);

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
				printf("socket(%d) ���� ����\n", (int)clientInfo->clientSocket);
				CloseSocket(clientInfo);
				continue;
			}

			OverlappedEx* overlappedEx = (OverlappedEx*)lpOverlapped;

			// Overlapped I/O Recv �۾� ��� �� ó��
			if (overlappedEx->ioEvent == IOEvent::RECV)
			{
				clientInfo->recvBuffer[numOfBytes] = '\0';
				printf("[����] bytes : %d, msg : %s\n", numOfBytes, clientInfo->recvBuffer);

				// Ŭ���̾�Ʈ�� �޼����� �����Ѵ�.
				SendMsg(clientInfo, clientInfo->recvBuffer, numOfBytes);
				BindRecv(clientInfo);
			}
			// Overlapped I/O Send �۾� ��� �� ó��
			else if (overlappedEx->ioEvent == IOEvent::SEND)
			{
				printf("[�۽�] bytes : %d, msg : %s\n", numOfBytes, clientInfo->sendBuffer);
			}
			// ���� ��Ȳ
			else
			{
				printf("socket(%d)���� ���ܻ�Ȳ\n", (int)clientInfo->clientSocket);
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
			ClientInfo* clientInfo = GetEmptyClientInfo();
			if (clientInfo == NULL)
			{
				printf("[����] Client Full\n");
				return;
			}

			// Ŭ���̾�Ʈ ���� ��û�� ���� ������ ��ٸ���.
			clientInfo->clientSocket = accept(_listenSocket, (SOCKADDR*)&clientAddr, &nAddrLen);
			if (clientInfo->clientSocket == INVALID_SOCKET)
				continue;

			// I/O Completion Port��ü�� ������ �����Ų��.
			bool bRet = BindIOCP(clientInfo);
			if (bRet == false)
				return;

			// Recv Overlapped I/O�۾��� ��û�� ���´�.
			bRet = BindRecv(clientInfo);
			if (bRet == false)
				return;

			char clientIP[32] = { 0, };
			inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, 32 - 1);
			printf("Ŭ���̾�Ʈ ���� : IP(%s) SOCKET(%d)\n", clientIP, (int)clientInfo->clientSocket);

			//Ŭ���̾�Ʈ ���� ����
			_clientCnt++;
		}
	}

	// ������ ������ ���� ��Ų��.
	void CloseSocket(ClientInfo* clientInfo, bool isForce = false)
	{
		struct linger stLinger = { 0, 0 };	// SO_DONTLINGER�� ����

		// isForce�� true�̸� SO_LINGER, timeout = 0���� �����Ͽ� ���� ���� ��Ų��. ���� : ������ �ս��� ���� �� ����
		if (isForce)
			stLinger.l_onoff = 1;
		
		// ������ ������ �ۼ����� ��� �ߴ� ��Ų��.
		shutdown(clientInfo->clientSocket, SD_BOTH);

		// ���� �ɼ��� �����Ѵ�.
		setsockopt(clientInfo->clientSocket, SOL_SOCKET, SO_LINGER, (char*)&stLinger, sizeof(stLinger));

		// ���� ������ ���� ��Ų��.
		closesocket(clientInfo->clientSocket);

		clientInfo->clientSocket = INVALID_SOCKET;
	}


private:
	std::vector<ClientInfo>		_clientInfos;							// Ŭ���̾�Ʈ ���� ���� ����ü
	SOCKET						_listenSocket = INVALID_SOCKET;			// Ŭ���̾�Ʈ�� ������ �ޱ� ���� ���� ����
	int							_clientCnt = 0;							// ���� �Ǿ��ִ� Ŭ���̾�Ʈ ��
	std::vector<std::thread>	_workerThreads;							// IO Worker ������
	std::thread					_acceptThread;							// Accept ������
	HANDLE						_iocpHandle = INVALID_HANDLE_VALUE;		// IOCP ��ü �ڵ�
	bool						_isWorkerRun = true;					// Worker ������ ���� �÷���
	bool						_isAcceptRun = true;					// Accept ������ ���� �÷���
	char						_socketBuf[1024] = { 0, };				// ���� ����
};