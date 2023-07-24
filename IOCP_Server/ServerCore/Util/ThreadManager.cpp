#include "pch.h"
#include "ThreadManager.h"

thread_local uint32 LThreadId = 0;
thread_local wstring LThreadName = L"INVALID_THREAD";
thread_local SendBufferChunkRef	LSendBufferChunk;

ThreadManager::ThreadManager()
{
    _threadId = 1;
}

ThreadManager::~ThreadManager()
{
    Join();
}

ThreadManager& ThreadManager::GetInstance()
{
    static ThreadManager* instance = new ThreadManager();
    return *instance;
}

void ThreadManager::Run(function<void(void)> callback, wstring name)
{
    lock_guard<mutex> lock(_lock);

    _threads.emplace_back(thread([=]()
        {
            InitTLS(name);
            callback();
            DestroyTLS();
        }));
}

void ThreadManager::Join()
{
    for (thread& t : _threads)
    {
        if (t.joinable())
            t.join();
    }
    _threads.clear();
}

void ThreadManager::InitTLS(wstring name)
{
    LThreadId = _threadId.fetch_add(1);
    LThreadName.assign(name);
}

void ThreadManager::DestroyTLS()
{
    // TLS 영역 내 동적으로 할당된 객체를 해제
}
