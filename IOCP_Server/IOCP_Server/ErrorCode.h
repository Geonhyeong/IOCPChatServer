#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#define CRASH(cause)						\
{											\
	UINT32* crash = nullptr;				\
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

enum class ERROR_CODE : UINT16
{
	NONE = 0,
	INVALID_PACKET = 1,
	INVALID_SESSION = 2,

	LOGIN_SUCCESS = 30,					// �α��� ����
	LOGIN_INVALID_TOKEN = 31,			// ��ȿ���� ���� ��ū
	LOGIN_REDUNDANT_CONNECTION = 32,	// �ߺ� ����
	LOGIN_USED_ALL_OBJ = 33,			// ��� ������ ���� �� �ʰ�

	ROOM_ENTER_SUCCESS = 50,			// �� ���� ����
	ROOM_LEAVE_SUCCESS = 51,			// �� ���� ����
	ROOM_UNKNOWN_NUMBER = 52,			// �������� �ʴ� �� ����
	ROOM_USER_EXCEED = 53,				// �� �ִ� �ο� �� �ʰ�
	ROOM_NONEXIST_USER = 54,			// �濡 �������� �ʴ� ����
};