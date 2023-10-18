#include "pch.h"
#include "ServerEntry.h"

ServerEntry::ServerEntry()
{
    _isStart = false;
    _isAcceptRun = false;
    _isWorkerRun = false;

    SocketUtils::Init();
}

ServerEntry::~ServerEntry()
{
    Close();

    SocketUtils::Clear();
}

ServerEntry& ServerEntry::GetInstance()
{
    static ServerEntry* instance = new ServerEntry();
    return *instance;
}

bool ServerEntry::Start(ServerConf serverConf, SessionFactory sessionFactory)
{
    _serverConf = serverConf;
    _sessionFactory = sessionFactory;

    if (_isStart == true)
        return false;

    _acceptor = make_unique<Acceptor>();
    if (_acceptor == nullptr)
        return false;

    if (_acceptor->Init(NetAddress(_serverConf.IpAddress, _serverConf.Port)) == false)
        return false;

    if (_acceptor->BindAndListen(_serverConf.BackLog) == false)
        return false;

    CreateSessions(_serverConf.MaxSessionCount);

    _isAcceptRun = true;
    ThreadManager::GetInstance().Run([this]() { AcceptThread(); }, L"AcceptThread");

    _isWorkerRun = true;
    for (uint32 i = 1; i <= _serverConf.MaxWorkerThreadCount; i++)
        ThreadManager::GetInstance().Run([this]() { WorkerThread(); }, L"WorkerThread_" + to_wstring(i));

    _isStart = true;
    return true;
}

bool ServerEntry::Close()
{
    if (_isStart == false)
        return false;

    _isAcceptRun = false;
    _isWorkerRun = false;

    ThreadManager::GetInstance().Join();

    return true;
}

void ServerEntry::CreateSessions(uint32 maxSessionCount)
{
    _sessions.reserve(maxSessionCount);
    
    for (uint32 i = 0; i < maxSessionCount; i++)
    {
        // 나중에 Session을 상속받는 컨텐츠단의 Session에 접근하기 위해서 SessionFactory를 적용하여야 한다. 
        SessionRef session = _sessionFactory();

        _sessions.push_back(session);
    }
}

void ServerEntry::AcceptThread()
{
    SOCKET acceptSocket = _acceptor->GetAcceptSocket();
    
    while (_isAcceptRun)
    {
        auto curTimeSec = chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now().time_since_epoch()).count();

        for (SessionRef& session : _sessions)
        {
            if (session->GetSessionState() >= SessionState::ACCEPT_IN_PROGRESS)
                continue;

            if ((uint64)curTimeSec < session->GetLatestDisconnectedTimeSec())
                continue;

            uint64 diff = curTimeSec - session->GetLatestDisconnectedTimeSec();
            if (diff <= _serverConf.ReuseSessionWaitTimeSec)
                continue;
            
            session->Accept(acceptSocket);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(32));
    }
}

void ServerEntry::WorkerThread()
{
    while (_isWorkerRun)
    {
        IocpCore::GetInstance().Dispatch(10);
    }
}
