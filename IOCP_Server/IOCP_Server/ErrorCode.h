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

	LOGIN_SUCCESS = 30,					// �α��� ����
	LOGIN_INVALID_PW = 31,				// ��ȿ���� ���� Password
	LOGIN_REDUNDANT_CONNECTION = 32,	// �ߺ� ����
	LOGIN_USED_ALL_OBJ = 33,			// ��� ������ ���� �� �ʰ�
};