#pragma once
#include "DBConnection.h"

#include <vector>
#include <mutex>

class DBConnectionPool
{
public:
	DBConnectionPool() = default;
	~DBConnectionPool()
	{
		Clear();
	}

	bool Connect(UINT32 connectionCount, const WCHAR* connectionString)
	{
		// 서버가 시작할 때 딱 한번만 실행된다.
		std::lock_guard<std::mutex> guard(_lock);

		if (::SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &_environment) != SQL_SUCCESS)
			return false;

		if (::SQLSetEnvAttr(_environment, SQL_ATTR_ODBC_VERSION, reinterpret_cast<SQLPOINTER>(SQL_OV_ODBC3), 0) != SQL_SUCCESS)
			return false;

		for (UINT32 i = 0; i < connectionCount; i++)
		{
			DBConnection* connection = new DBConnection();

			if (connection->Connect(_environment, connectionString) == false)
				return false;

			_connections.push_back(connection);
		}

		return true;
	}

	void Clear()
	{
		std::lock_guard<std::mutex> guard(_lock);

		if (_environment != SQL_NULL_HANDLE)
		{
			::SQLFreeHandle(SQL_HANDLE_ENV, _environment);
			_environment = SQL_NULL_HANDLE;
		}

		for (DBConnection* connection : _connections)
			delete connection;

		_connections.clear();
	}

	DBConnection* Pop()
	{
		std::lock_guard<std::mutex> guard(_lock);

		if (_connections.empty())
			return nullptr;
		
		DBConnection* connection = _connections.back();
		_connections.pop_back();

		return connection;
	}

	void Push(DBConnection* connection)
	{
		std::lock_guard<std::mutex> guard(_lock);

		_connections.push_back(connection);
	}

private:
	SQLHENV						_environment = SQL_NULL_HANDLE;
	std::vector<DBConnection*>	_connections;
	std::mutex	_lock;
};