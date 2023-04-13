#include "IOCP.h"

const UINT16 SERVER_PORT = 2581;
const UINT16 MAX_CLIENT = 100;

int main()
{
	IOCP iocpServer;

	// ������ �ʱ�ȭ
	iocpServer.InitSocket();

	// ���ϰ� ���� �ּҸ� �����ϰ� ��� ��Ų��.
	iocpServer.BindAndListen(SERVER_PORT);

	iocpServer.StartServer(MAX_CLIENT);

	printf("�ƹ� Ű�� ���� ������ ����մϴ�.\n");
	getchar();

	iocpServer.DestroyThread();
	return 0;
}