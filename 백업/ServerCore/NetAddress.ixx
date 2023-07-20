module;
#pragma once
#pragma comment(lib, "ws2_32")

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>

export module net.netaddress;

export class NetAddress
{
public:
	NetAddress() = default;
	NetAddress(SOCKADDR_IN sockAddr) : _sockAddr(sockAddr) {}
	NetAddress(std::wstring ip, u_short port)
	{
		::memset(&_sockAddr, 0, sizeof(_sockAddr));
		_sockAddr.sin_family = AF_INET;
		::InetPtonW(AF_INET, ip.c_str(), &_sockAddr.sin_addr);
		_sockAddr.sin_port = ::htons(port);
	}

	SOCKADDR_IN& GetSockAddr() { return _sockAddr; }
	u_short GetPort() { return ::ntohs(_sockAddr.sin_port); }
	std::wstring GetIpAddress()
	{
		WCHAR buffer[100];
		::InetNtopW(AF_INET, &_sockAddr.sin_addr, buffer, sizeof(buffer) / sizeof(buffer[0]));
		return std::wstring(buffer);
	}

private:
	SOCKADDR_IN _sockAddr = {};
};