module;
#include <Windows.h>
#include <thread>
#include <string>
#include <list>

export module lang.thread;
import util;

export class Thread
{
public:
	Thread(std::thread* thread, std::wstring name);
	~Thread();

	size_t			GetId() { return _id; }
	std::wstring&	GetName() { return _name; }

private:
	std::thread*	_thread;
	std::wstring	_name;
	size_t			_id;
};

export class ThreadManager
{
public:
	static ThreadManager& GetInstance()
	{
		static ThreadManager* instance = new ThreadManager();
		return *instance;
	}

	void Join()
	{
		std::list<HANDLE>::iterator pos, posPrev;

		pos = _listThreadHandle.begin();
		while (pos != _listThreadHandle.end())
		{
			posPrev = pos++;
			::WaitForSingleObject(*posPrev, INFINITE);

			_listThreadHandle.erase(posPrev);
		}
	}

	HANDLE Spawn(LPTHREAD_START_ROUTINE startAddress, LPVOID parameter, DWORD* threadId)
	{
		HANDLE threadHandle;

		threadHandle = ::CreateThread(0, 0, startAddress, parameter, 0, threadId);

		_listThreadHandle.push_back(threadHandle);

		return threadHandle;
	}

	void Spawn(Thread* thread)
	{

	}

private:
	ThreadManager() {}
	virtual ~ThreadManager()
	{
		std::list<HANDLE>::iterator pos;

		while (_listThreadHandle.size() > 0)
			_listThreadHandle.erase(_listThreadHandle.begin());
	}
	
private:
	std::list<HANDLE> _listThreadHandle;
};


/*-------------
	Thread
-------------*/

Thread::Thread(std::thread* thread, std::wstring name)
{
	_name = name;
	_thread = thread;
	_id = std::hash<std::thread::id>{}(_thread->get_id());

	ThreadManager::GetInstance().Spawn(this);
}

Thread::~Thread()
{
	if (_thread->joinable())
		_thread->join();

	if (_thread)
		delete _thread;
}
