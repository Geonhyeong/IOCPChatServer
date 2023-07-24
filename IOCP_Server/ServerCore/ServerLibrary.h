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

// TODO: 필수 헤더 파일
//-------------------------------------------------------------------//
#include "Util/Types.h"
#include "Util/ThreadManager.h"
#include "Util/RingBuffer.h"
#include "Util/SendBuffer.h"

#include "Net/NetAddress.h"
#include "Net/SocketUtils.h"

#include "Net/IocpCore.h"
#include "Net/IocpEvent.h"
#include "Net/Session.h"
#include "Net/Acceptor.h"