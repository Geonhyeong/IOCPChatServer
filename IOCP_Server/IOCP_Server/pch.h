#pragma once

#define WIN32_LEAN_AND_MEAN // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
#define NOMINMAX

#ifdef _DEBUG
#pragma comment(lib, "ServerCore/Debug/ServerCore.lib")
#pragma comment(lib, "cpp_redis/Debug/cpp_redis.lib")
#pragma comment(lib, "cpp_redis/Debug/tacopie.lib")
#else
#pragma comment(lib, "ServerCore/Release/ServerCore.lib")
#pragma comment(lib, "cpp_redis/Release/cpp_redis.lib")
#pragma comment(lib, "cpp_redis/Release/tacopie.lib")
#endif

#include "ServerLibrary.h"
#include "Config.h"
#include "Utils.h"

#define SENTRY ServerEntry::GetInstance()
#define CONFIG Config::GetInstance().GetConfig()

#include "PacketManager.h"
#include "DBConnectionManager.h"
#include "RedisManager.h"
#include "UserManager.h"
#include "RoomManager.h"

#define PACKETS PacketManager::GetInstance()
#define REDIS RedisManager::GetInstance()
#define DB DBConnectionManager::GetInstance()
#define USERS UserManager::GetInstance()
#define ROOMS RoomManager::GetInstance()