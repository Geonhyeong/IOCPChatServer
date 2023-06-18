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

	LOGIN_SUCCESS = 30,					// 로그인 성공
	LOGIN_INVALID_TOKEN = 31,			// 유효하지 않은 토큰
	LOGIN_REDUNDANT_CONNECTION = 32,	// 중복 접속
	LOGIN_USED_ALL_OBJ = 33,			// 허용 가능한 유저 수 초과

	ROOM_ENTER_SUCCESS = 50,			// 방 입장 성공
	ROOM_LEAVE_SUCCESS = 51,			// 방 퇴장 성공
	ROOM_UNKNOWN_NUMBER = 52,			// 존재하지 않는 방 숫자
	ROOM_USER_EXCEED = 53,				// 방 최대 인원 수 초과
	ROOM_NONEXIST_USER = 54,			// 방에 존재하지 않는 유저
};