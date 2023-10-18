#include "pch.h"
#include "ServerEntry.h"
#include "User.h"

ServerConf GetServerConf()
{
	auto elem = CONFIG->FirstChildElement("App")->FirstChildElement("Server");

	ServerConf conf;
	conf.IpAddress = Utils::ToWString(elem->FirstChildElement("IP")->GetText());
	conf.Port = (uint16)elem->FirstChildElement("Port")->UnsignedText();
	conf.BackLog = (int32)elem->FirstChildElement("BackLog")->IntText();
	conf.MaxWorkerThreadCount = (uint32)elem->FirstChildElement("MaxWorkerThreadCount")->UnsignedText();
	conf.MaxSessionCount = (uint32)elem->FirstChildElement("MaxSessionCount")->UnsignedText();
	conf.ReuseSessionWaitTimeSec = (uint64)elem->FirstChildElement("ReuseSessionWaitTimeSec")->Int64Text();

	return conf;
}

int main()
{
	ServerConf serverConf = GetServerConf();

	if (SENTRY.Start(serverConf, make_shared<User>) == false)
	{
		SLog(L"Failed to start Server...");
		return 0;
	}

	// TODO : 패킷 매니저 쓰레드들을 실행
	PACKETS.Run(5, 100, 11, 1000);

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

	PACKETS.End();
	if (SENTRY.Close() == false)
		SLog(L"Failed to Server Closed...");

	return 0;
}