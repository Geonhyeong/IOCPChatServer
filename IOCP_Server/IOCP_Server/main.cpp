#include "ChatServer.h"

#include <string>
#include <iostream>

const UINT16 SERVER_PORT = 2581;
const UINT16 MAX_CLIENT = 100;			// ���� ������ �ִ� Ŭ���̾�Ʈ ��
const UINT32 MAX_IO_WORKER_THREAD = 4;	// ������ Ǯ�� ���� ������ ��

int main()
{
	ChatServer server;
	server.Init(MAX_IO_WORKER_THREAD);
	server.BindAndListen(SERVER_PORT);
	server.Run(MAX_CLIENT);

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