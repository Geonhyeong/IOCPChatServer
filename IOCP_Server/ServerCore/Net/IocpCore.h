#pragma once

class IocpCore
{
private:
	IocpCore();
	~IocpCore();

public:
	static IocpCore& GetInstance();

	HANDLE	GetHandle() { return _iocpHandle; }

	bool	Register(SOCKET socket);
	bool	Dispatch(uint32 timeoutMs = INFINITE);

private:
	HANDLE _iocpHandle;
};