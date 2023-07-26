#pragma once

#define SLog(arg, ...)		Logger::GetInstance().Log(arg, __VA_ARGS__);
#define SLogErr(arg, ...)	Logger::GetInstance().Log(arg, __VA_ARGS__); ::ExitProcess(0);

class LogBase
{
public:
	LogBase() {}
	virtual ~LogBase() {}
	virtual void Initialize() {}
	virtual void UnInitialize() {}
	virtual void Log(WCHAR* logStr) = 0;
};

class LogPrintf : public LogBase
{
public:
	LogPrintf();
	void Log(WCHAR* logStr);
};

/*--------------------
	�α� ���� ��ü 
--------------------*/

class LogWriter
{
public:
	LogWriter();
	~LogWriter();

	void SetLogBase(LogBase* base);
	void Log(const WCHAR* fmt, va_list args);
private:
	LogBase* _base;

};

/*------------------
	�α� �ý���
------------------*/

class Logger
{
private:
	Logger();
	~Logger();

public:
	static Logger& GetInstance();

	void Init();
	void Log(const WCHAR* fmt, ...);

private:
	LogWriter _logWriter;
};

