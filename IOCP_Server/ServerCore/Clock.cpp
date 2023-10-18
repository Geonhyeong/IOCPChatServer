#include "pch.h"
#include "Clock.h"

Clock::Clock()
{
    _serverStartTick = this->SystemTick();
}

Clock::~Clock()
{
}

Clock& Clock::GetInstance()
{
    static Clock* instance = new Clock();
    return *instance;
}

time_t Clock::ServerStartTick()
{
    return _serverStartTick;
}

time_t Clock::SystemTick()
{
    return system_clock::to_time_t(system_clock::now());
}

time_t Clock::StrToTick(wstring str, const WCHAR* fmt)
{
    int	year = 0;
    int	month = 0;
    int	day = 0;
    int	hour = 0;
    int	minute = 0;
    int	second = 0;

    swscanf_s(str.c_str(), fmt, &year, &month, &day, &hour, &minute, &second);

    //		   초,		분,    시,  일,  월(0부터시작), 년(1900년부터시작)
    tm time = { second, minute, hour, day, month - 1, year - 1900 };

    return mktime(&time);
}

wstring Clock::NowTime(const WCHAR* fmt)
{
    return this->TickToStr(this->SystemTick(), fmt);
}

wstring Clock::NowTimeWithMilliSec(const WCHAR* fmt)
{
    high_resolution_clock::time_point point = high_resolution_clock::now();
    milliseconds ms = duration_cast<milliseconds>(point.time_since_epoch());

    seconds s = duration_cast<seconds>(ms);
    time_t t = s.count();
    std::size_t fractionalSeconds = ms.count() % 1000;
    array<WCHAR, 8> milliStr;
    _snwprintf_s(milliStr.data(), milliStr.size(), _TRUNCATE, L"%03d", (int)(fractionalSeconds));

    wstring timeString = this->TickToStr(this->SystemTick(), fmt);
    timeString += L".";
    timeString += milliStr.data();
    return timeString;
}

wstring Clock::Today()
{
    return this->TickToStr(this->SystemTick(), DATE_FORMAT);
}

wstring Clock::Tomorrow()
{
    return this->TickToStr(this->SystemTick() + DAY_TO_TICK(1), DATE_FORMAT);
}

wstring Clock::Yesterday()
{
    return this->TickToStr(this->SystemTick() - DAY_TO_TICK(1), DATE_FORMAT);
}

DayOfTheWeek Clock::TodayOfTheWeek()
{
    tm time;
    time_t tick = this->SystemTick();
    localtime_s(&time, &tick);

    return static_cast<DayOfTheWeek>(time.tm_wday);
}

wstring Clock::TickToStr(time_t tick, const WCHAR* fmt)
{
    array<WCHAR, 128> timeStr;

    tm time;
    localtime_s(&time, &tick);
    wcsftime(timeStr.data(), timeStr.size(), fmt, &time);

    return timeStr.data();
}