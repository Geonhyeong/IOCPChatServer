#pragma once

#define DB_CONNECTION_STRING L"Driver={SQL Server Native Client 11.0};Server=(localdb)\\MSSQLLocalDB;Database=ChatDB;Trusted_Connection=Yes;"
#define DB_CREATE_TABLE_QUERY L"					\
		DROP TABLE IF EXISTS [dbo].[ChatLog];		\
		CREATE TABLE [dbo].[ChatLog]				\
		(											\
			[Id] INT NOT NULL PRIMARY KEY IDENTITY,	\
			[SessionId] INT NULL,					\
			[UserId] NVARCHAR(50) NULL,				\
			[ChatMsg] NVARCHAR(500) NULL,			\
			[RoomNumber] INT NULL,					\
			[DateTime] DATETIME NULL				\
		); "
#define DB_TRUNCATE_TABLE_QUERY L"TRUNCATE TABLE [dbo].[ChatLog];"

struct DB_CHATLOG_INFO
{
	int32 sessionId = 0;
    WCHAR userId[50] = { 0, };
    WCHAR chatMsg[500] = { 0, };
	int32 roomNumber = 0;
    time_t dateTime = 0;
};