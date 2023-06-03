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

	LOGIN_SUCCESS = 30,					// 로그인 성공
	LOGIN_INVALID_PW = 31,				// 유효하지 않은 Password
	LOGIN_REDUNDANT_CONNECTION = 32,	// 중복 접속
	LOGIN_USED_ALL_OBJ = 33,			// 허용 가능한 유저 수 초과
};