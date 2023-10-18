#pragma once
#include <chrono>
#include <ctime>

#define CLOCK					Clock::GetInstance()
#define NOW_TICK				CLOCK.systemTick
#define NOW_STRING				CLOCK.nowTime

#define TICK_MIN                (60)
#define TICK_HOUR               (TICK_MIN * 60)
#define TICK_DAY                (TICK_HOUR * 24)

#define TICK_TO_MIN(x)          (x / TICK_MIN)
#define MIN_TO_TICK(x)          (x * TICK_MIN)

#define TICK_TO_HOUR(x)         (x / TICK_HOUR)        
#define HOUR_TO_TICK(x)         (x * TICK_HOUR)

#define TICK_TO_DAY(x)          (x / TICK_DAY)
#define DAY_TO_TICK(x)          (x * TICK_DAY)

enum class DayOfTheWeek {
    DAY_SUNDAY = 0,
    DAY_MONDAY = 1,
    DAY_TUESDAY = 2,
    DAY_WEDNESDAY = 3,
    DAY_THURSDAY = 4,
    DAY_FRIDAY = 5,
    DAY_SATURDAY = 6,
};

#define DATETIME_FORMAT         L"D%Y-%m-%dT%H:%M:%S"
#define DATE_FORMAT             L"%Y-%m-%d"
#define TIME_FORMAT             L"%H:%M:%S"
#define DB_TIME_FORMAT          L"%4d-%2d-%2d %2d:%2d:%2d"

using namespace std::chrono;
typedef system_clock::time_point timePoint;

class Clock
{
private:
    Clock();
    ~Clock();

public:
    static Clock& GetInstance();

    time_t	ServerStartTick();
    time_t	SystemTick();
    time_t	StrToTick(wstring str, const WCHAR* fmt = DB_TIME_FORMAT);

    wstring	NowTime(const WCHAR* fmt = DATETIME_FORMAT);
    wstring	NowTimeWithMilliSec(const WCHAR* fmt = DATETIME_FORMAT);

    wstring Today();
    wstring Tomorrow();
    wstring Yesterday();

    DayOfTheWeek TodayOfTheWeek();

private:
    wstring	TickToStr(time_t tick, const WCHAR* fmt = DATETIME_FORMAT);

    time_t	_serverStartTick;
};