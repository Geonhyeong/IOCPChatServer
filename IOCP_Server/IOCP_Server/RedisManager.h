#pragma once

#ifdef _DEBUG
#pragma comment(lib, "cpp_redis\\Debug\\cpp_redis.lib")
#pragma comment(lib, "cpp_redis\\Debug\\tacopie.lib")
#else
#pragma comment(lib, "cpp_redis\\Release\\cpp_redis.lib")
#pragma comment(lib, "cpp_redis\\Release\\tacopie.lib")
#endif // _DEBUG

#include <string>
#include <future>
#include <cpp_redis/cpp_redis>

class RedisManager
{
public:
	RedisManager() = default;
	~RedisManager()
	{
		if (_connected == false)
			return;

		_connected = false;
		_redisClient.disconnect();
	}

	void Init()
	{
		if (_connected == true)
			return;

		_redisClient.connect();	// "default : 127.0.0.1:6379"
		_connected = true;
	}

	std::string GetValue(std::string key)
	{
		std::future<cpp_redis::reply> get_reply = _redisClient.get(key);
		_redisClient.sync_commit();

		cpp_redis::reply reply = get_reply.get();

		return reply.as_string();
	}

private:
	bool _connected = false;
	cpp_redis::client _redisClient;
};