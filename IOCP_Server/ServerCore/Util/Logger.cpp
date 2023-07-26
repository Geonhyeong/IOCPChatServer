#include "pch.h"
#include "Logger.h"

LogPrintf::LogPrintf()
{
}

void LogPrintf::Log(WCHAR* logStr)
{
	printf("%ws", logStr);
}

/*----------------
	LogWriter
----------------*/

LogWriter::LogWriter()
{
	_base = nullptr;
}

LogWriter::~LogWriter()
{
	if (_base)
		delete _base;
}

void LogWriter::SetLogBase(LogBase* base)
{
	_base = base;
	_base->Initialize();
}

void LogWriter::Log(const WCHAR* fmt, va_list args)
{
	wstring logMessage = CLOCK.NowTimeWithMilliSec();
	
	logMessage += L"\t";

	// 쓰레드 정보 넣기
	logMessage += LThreadName;

	array<WCHAR, 16> threadIdStr;
	_snwprintf_s(threadIdStr.data(), threadIdStr.size(), _TRUNCATE, L"0x%X", LThreadId);

	logMessage += L":";
	logMessage += threadIdStr.data();
	logMessage += L"\t";

	array<WCHAR, 1024> logStr;

	vswprintf_s(logStr.data(), logStr.size(), fmt, args);

	logMessage += logStr.data();
	logMessage += L"\n";
	_base->Log((WCHAR*)logMessage.c_str());
}

/*-------------
	Logger
-------------*/

Logger::Logger()
{
	this->Init();
}

Logger::~Logger()
{
}

Logger& Logger::GetInstance()
{
	static Logger* instance = new Logger();
	return *instance;
}

void Logger::Init()
{
	// TODO : LogBase 설정
	LogBase* base = new LogPrintf();

	_logWriter.SetLogBase(base);
}

void Logger::Log(const WCHAR* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	_logWriter.Log(fmt, args);
	va_end(args);
}