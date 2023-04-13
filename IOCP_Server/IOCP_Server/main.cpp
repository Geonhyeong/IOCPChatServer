#include "ChatServer.h"

const UINT16 SERVER_PORT = 2581;
const UINT16 MAX_CLIENT = 100;

int main()
{
	ChatServer server;

	// 소켓을 초기화
	server.InitSocket();

	// 소켓과 서버 주소를 연결하고 등록 시킨다.
	server.BindAndListen(SERVER_PORT);

	server.Run(MAX_CLIENT);

	printf("아무 키나 누를 때까지 대기합니다.\n");
	getchar();

	server.End();

	return 0;
}