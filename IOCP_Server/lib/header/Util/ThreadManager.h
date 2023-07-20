#pragma once

extern thread_local uint32 LThreadId;
extern thread_local wstring LThreadName;

class ThreadManager
{
private:
	ThreadManager();
	~ThreadManager();

public:
	static ThreadManager& GetInstance();

	void Run(function<void(void)> callback, wstring name);
	void Join();

private:
	void InitTLS(wstring name);
	void DestroyTLS();

private:
	mutex _lock;
	
	atomic<uint32> _threadId;
	list<thread> _threads;
};