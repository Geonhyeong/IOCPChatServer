#include "pch.h"
#include "IocpCore.h"

IocpCore::IocpCore()
{
	_iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	ASSERT_CRASH(_iocpHandle != INVALID_HANDLE_VALUE);
}

IocpCore::~IocpCore()
{
	::CloseHandle(_iocpHandle);
}

IocpCore& IocpCore::GetInstance()
{
	static IocpCore* instance = new IocpCore();
	return *instance;
}

bool IocpCore::Register(SOCKET socket)
{
	if (::CreateIoCompletionPort(reinterpret_cast<HANDLE>(socket), _iocpHandle, /*key*/0, 0) == NULL)
		return false;
	return true;
}

bool IocpCore::Dispatch(uint32 timeoutMs)
{
	DWORD numOfBytes = 0;
	ULONG_PTR key = 0;
	IocpEvent* iocpEvent = nullptr;

	bool bSuccess = ::GetQueuedCompletionStatus(_iocpHandle, &numOfBytes, &key, reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), timeoutMs);
	
	if (bSuccess == TRUE && key != 0 && numOfBytes != 0 && iocpEvent != NULL)
	{
		SessionRef session = iocpEvent->ownerSession;
		session->Dispatch(iocpEvent, numOfBytes);
	}
	else
	{
		int32 errCode = ::WSAGetLastError();
		switch (errCode)
		{
		case WAIT_TIMEOUT:
			return false;
		default:
			// TODO : LOG
			SessionRef session = iocpEvent->ownerSession;
			session->Dispatch(iocpEvent, numOfBytes);
			break;
		}
	}

	return true;
}