#include "pch.h"
#include <ServerEntry.h>

int main()
{
	ServerConf conf;
	conf.IpAddress = L"127.0.0.1";
	conf.Port = 2581;
	conf.BackLog = INFINITE;
	conf.MaxWorkerThreadCount = 3;
	conf.MaxSessionCount = 100;
	conf.ReuseSessionWaitTimeSec = 10;

	ServerEntry::GetInstance().Start(conf);
	
	SLog(L"Enter 'quit' command to close server...");
	while (true)
	{
		string inputCmd;
		::getline(cin, inputCmd);

		if (inputCmd == "quit")
		{
			break;
		}
	}

	if (ServerEntry::GetInstance().Close() == false)
		SLogErr(L"Failed to server closed...");
	
	return 0;
}