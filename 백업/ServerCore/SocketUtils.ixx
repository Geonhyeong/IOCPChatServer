#include "..\..\IOCP_Server\ServerCore\Net\SocketUtils.h"
module;
#pragma once
#pragma comment(lib, "ws2_32")
#pragma comment(lib, "mswsock.lib")

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <mstcpip.h>
#include <assert.h>

export module net.socketutils;

export class SocketUtils
{
public:
	static LPFN_CONNECTEX		ConnectEx;
	static LPFN_DISCONNECTEX	DisconnectEx;
	static LPFN_ACCEPTEX		AcceptEx;

public:
	static void Init()
	{
		WSADATA wsaData;
		assert(::WSAStartup(MAKEWORD(2, 2), &wsaData) == 0 && "[에러] WSAStartup()함수 실패");

		/* 런타임에 주소를 얻어오는 API */
		SOCKET dummySocket = CreateSocket();
		assert(BindWindowsFunction(dummySocket, WSAID_CONNECTEX, reinterpret_cast<LPVOID*>(&ConnectEx)));
		assert(BindWindowsFunction(dummySocket, WSAID_DISCONNECTEX, reinterpret_cast<LPVOID*>(&DisconnectEx)));
		assert(BindWindowsFunction(dummySocket, WSAID_ACCEPTEX, reinterpret_cast<LPVOID*>(&AcceptEx)));
		CloseSocket(dummySocket);
	}

	static void Clear()
	{
		::WSACleanup();
	}

	static bool BindWindowsFunction(SOCKET socket, GUID guid, LPVOID* fn)
	{
		DWORD bytes = 0;
		return SOCKET_ERROR != ::WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(guid), fn, sizeof(*fn), &bytes, NULL, NULL);
	}

	static SOCKET CreateSocket()
	{
		return ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);
	}

	static bool SetLinger(SOCKET socket, u_short onoff, u_short linger);
	static bool SetReuseAddress(SOCKET socket, bool flag);
	static bool SetRecvBufferSize(SOCKET socket, int size);
	static bool SetSendBufferSize(SOCKET socket, int size);
	static bool SetTcpNoDelay(SOCKET socket, bool flag);
	static bool SetUpdateAcceptSocket(SOCKET socket, SOCKET acceptSocket);

	static bool Bind(SOCKET socket, SOCKADDR_IN* sockAddr)
	{
		return SOCKET_ERROR != ::bind(socket, reinterpret_cast<const SOCKADDR*>(sockAddr), sizeof(SOCKADDR_IN));
	}

	bool SocketUtils::Bind(SOCKET socket, NetAddress netAddr)
	{
		return false;
	}

	static bool Listen(SOCKET socket, int backlog)
	{
		return SOCKET_ERROR != ::listen(socket, backlog);
	}

	static void CloseSocket(SOCKET& socket)
	{
		if (socket != INVALID_SOCKET)
			::closesocket(socket);
		socket = INVALID_SOCKET;
	}
};

LPFN_CONNECTEX		SocketUtils::ConnectEx = nullptr;
LPFN_DISCONNECTEX	SocketUtils::DisconnectEx = nullptr;
LPFN_ACCEPTEX		SocketUtils::AcceptEx = nullptr;

template<typename T>
static inline bool SetSockOpt(SOCKET socket, int level, int optName, T optVal)
{
	return SOCKET_ERROR != ::setsockopt(socket, level, optName, reinterpret_cast<char*>(&optVal), sizeof(T));
}

bool SocketUtils::SetLinger(SOCKET socket, u_short onoff, u_short linger)
{
	LINGER option;
	option.l_onoff = onoff;
	option.l_linger = linger;
	return SetSockOpt(socket, SOL_SOCKET, SO_LINGER, option);
}

bool SocketUtils::SetReuseAddress(SOCKET socket, bool flag)
{
	return SetSockOpt(socket, SOL_SOCKET, SO_REUSEADDR, flag);
}

bool SocketUtils::SetRecvBufferSize(SOCKET socket, int size)
{
	return SetSockOpt(socket, SOL_SOCKET, SO_RCVBUF, size);
}

bool SocketUtils::SetSendBufferSize(SOCKET socket, int size)
{
	return SetSockOpt(socket, SOL_SOCKET, SO_SNDBUF, size);
}

bool SocketUtils::SetTcpNoDelay(SOCKET socket, bool flag)
{
	return SetSockOpt(socket, SOL_SOCKET, TCP_NODELAY, flag);
}

bool SocketUtils::SetUpdateAcceptSocket(SOCKET socket, SOCKET acceptSocket)
{
	return SetSockOpt(socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, acceptSocket);
}