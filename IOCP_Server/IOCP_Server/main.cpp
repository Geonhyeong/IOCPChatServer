#include "ChatServer.h"

#include <string>
#include <iostream>

const UINT16 SERVER_PORT = 2581;
const UINT16 MAX_CLIENT = 1001;			// ���� ������ �ִ� Ŭ���̾�Ʈ ��
const UINT32 MAX_IO_WORKER_THREAD = 10;	// ������ Ǯ�� ���� ������ ��
const UINT32 MAX_DB_THREAD = 1;	// DB �۾��� ���� ������ ��

int main()
{
	ChatServer server;
	server.Init(MAX_IO_WORKER_THREAD);
	server.BindAndListen(SERVER_PORT);
	server.Run(MAX_CLIENT, MAX_DB_THREAD);

	printf("�ƹ� Ű�� ���� ������ ����մϴ�.\n");
	while (true)
	{
		std::string inputCmd;
		std::getline(std::cin, inputCmd);

		if (inputCmd == "quit")
		{
			break;
		}
	}

	server.End();
	return 0;
}