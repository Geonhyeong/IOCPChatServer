#pragma once

#define DB_CONNECTION_STRING L"Driver={SQL Server Native Client 11.0};Server=(localdb)\\MSSQLLocalDB;Database=ChatDB;Trusted_Connection=Yes;"
#define DB_CREATE_TABLE_QUERY L"					\
		DROP TABLE IF EXISTS [dbo].[ChatLog];		\
		CREATE TABLE [dbo].[ChatLog]				\
		(											\
			[Id] INT NOT NULL PRIMARY KEY IDENTITY,	\
			[SessionId] INT NULL,					\
			[Nickname] NVARCHAR(50) NULL,			\
			[ChatMsg] NVARCHAR(500) NULL,			\
			[DateTime] DATETIME NULL				\
		); "
#define DB_TRUNCATE_TABLE_QUERY L"TRUNCATE TABLE [dbo].[ChatLog];"

struct DB_CHATLOG_INFO
{
	INT32 sessionId = 0;
    WCHAR nickname[50] = { 0, };
    WCHAR chatMsg[500] = { 0, };
    time_t dateTime = 0;
};