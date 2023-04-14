#include "ChatServer.h"

#include <string>
#include <iostream>

const UINT16 SERVER_PORT = 2581;
const UINT16 MAX_CLIENT = 100;

int main()
{
	ChatServer server;

	// ������ �ʱ�ȭ
	server.InitSocket();

	// ���ϰ� ���� �ּҸ� �����ϰ� ��� ��Ų��.
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