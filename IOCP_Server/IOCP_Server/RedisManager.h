#pragma once
#include <cpp_redis/cpp_redis>

class RedisManager
{
private:
	RedisManager()
	{
		SLog(L"RedisManager Singleton Instance Generated.");
		Init();
	}

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

public:
	static RedisManager& GetInstance()
	{
		static RedisManager* instance = new RedisManager();
		return *instance;
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