#include "IOCP.h"

const UINT16 SERVER_PORT = 2581;
const UINT16 MAX_CLIENT = 100;

int main()
{
	IOCP iocpServer;

	// 소켓을 초기화
	iocpServer.InitSocket();

	// 소켓과 서버 주소를 연결하고 등록 시킨다.
	iocpServer.BindAndListen(SERVER_PORT);

	iocpServer.StartServer(MAX_CLIENT);

	printf("아무 키나 누를 때까지 대기합니다.\n");
	getchar();

	iocpServer.DestroyThread();
	return 0;
}