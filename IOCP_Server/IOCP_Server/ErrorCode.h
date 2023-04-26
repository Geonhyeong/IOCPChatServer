#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

enum class ERROR_CODE : UINT16
{
	NONE = 0,
	INVALID_PACKET = 1,

	LOGIN_SUCCESS = 30,					// �α��� ����
	LOGIN_INVALID_PW = 31,				// ��ȿ���� ���� Password
	LOGIN_REDUNDANT_CONNECTION = 32,	// �ߺ� ����
	LOGIN_USED_ALL_OBJ = 33,			// ��� ������ ���� �� �ʰ�
};