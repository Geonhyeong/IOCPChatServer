#pragma once
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "mswsock.lib")

#include <Ws2tcpip.h>
#include <winsock2.h>
#include <mswsock.h>
#include <Mstcpip.h>

#include <Windows.h>
#include <iostream>
#include <assert.h> 
#include <algorithm>  
#include <functional>
#include <thread>
#include <mutex>
#include <memory>

#include <string>
#include <list>
#include <stack>
#include <queue>
#include <array>
#include <map>
#include <set>
#include <unordered_map>

using namespace std;

// TODO: 공용 매크로
#define CRASH(cause)						\
{											\
	uint32* crash = nullptr;				\
	__analysis_assume(crash != nullptr);	\
	*crash = 0xDEADBEEF;					\
}

#define ASSERT_CRASH(expr)			\
{									\
	if (!(expr))					\
	{								\
		CRASH("ASSERT_CRASH");		\
		__analysis_assume(expr);	\
	}								\
}

#define USE_LOCK	Lock _lock;
#define READ_LOCK	ReadLockGuard readLockGuard(_lock, typeid(this).name());
#define WRITE_LOCK	WriteLockGuard writeLockGuard(_lock, typeid(this).name());

// TODO: 필수 헤더 파일
//-------------------------------------------------------------------//
#include "Types.h"
#include "Clock.h"
#include "Logger.h"
#include "Lock.h"
#include "ThreadManager.h"
#include "RingBuffer.h"
#include "SendBuffer.h"

#include "NetAddress.h"
#include "SocketUtils.h"
#include "IocpCore.h"
#include "IocpEvent.h"
#include "Session.h"
#include "Acceptor.h"