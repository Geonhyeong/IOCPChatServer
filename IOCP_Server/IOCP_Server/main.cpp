#include "ChatServer.h"

#include <string>
#include <iostream>

const UINT16 SERVER_PORT = 2581;
const UINT16 MAX_CLIENT = 1001;			// 접속 가능한 최대 클라이언트 수
const UINT32 MAX_IO_WORKER_THREAD = 10;	// 쓰레드 풀에 넣을 쓰레드 수
const UINT32 MAX_DB_THREAD = 1;	// DB 작업을 맡을 쓰레드 수

int main()
{
	ChatServer server;
	server.Init(MAX_IO_WORKER_THREAD);
	server.BindAndListen(SERVER_PORT);
	server.Run(MAX_CLIENT, MAX_DB_THREAD);

	printf("아무 키나 누를 때까지 대기합니다.\n");
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