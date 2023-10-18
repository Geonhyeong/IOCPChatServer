#include "pch.h"
#include "ThreadManager.h"

thread_local uint32 LThreadId = 0;
thread_local wstring LThreadName = L"INVALID_THREAD";
thread_local SendBufferChunkRef	LSendBufferChunk;

ThreadManager::ThreadManager()
{
    _threadId = 0;
    InitTLS(L"Main Thread");
}

ThreadManager::~ThreadManager()
{
    Join();
    DestroyTLS();
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

    SLog(L"All thread is joined.")
}

void ThreadManager::InitTLS(wstring name)
{
    LThreadId = _threadId.fetch_add(1);
    LThreadName.assign(name);

    SLog(L"Thread is running...");
}

void ThreadManager::DestroyTLS()
{
    // TLS 영역 내 동적으로 할당된 객체를 해제
    SLog(L"Thread is destroyed...");
}
