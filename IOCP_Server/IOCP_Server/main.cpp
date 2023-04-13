#include "ChatServer.h"

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
	getchar();

	server.End();

	return 0;
}