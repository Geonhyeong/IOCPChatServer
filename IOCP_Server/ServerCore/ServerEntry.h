#pragma once

struct ServerConf
{
	wstring IpAddress;
	uint16 Port;
	int32 BackLog;

	uint32 MaxWorkerThreadCount;
	uint32 MaxSessionCount;
	uint64 ReuseSessionWaitTimeSec;
};

class ServerEntry
{
private:
	ServerEntry();
	~ServerEntry();

public:
	static ServerEntry& GetInstance();

	bool Start(ServerConf serverConf);
	bool Close();

private:
	void CreateSessions(uint32 maxSessionCount);
	void AcceptThread();
	void WorkerThread();

private:
	ServerConf _serverConf;
	unique_ptr<Acceptor> _acceptor;
	vector<SessionRef> _sessions;

	bool _isStart;
	bool _isAcceptRun;
	bool _isWorkerRun;
};