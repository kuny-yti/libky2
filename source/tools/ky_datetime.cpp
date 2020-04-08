#include "tools/ky_datetime.h"
#include "tools/ky_string.h"

#include <cerrno>
#ifdef kyHasClockGetTime
# include <time.h>
#else
# include <sys/time.h>
#endif

#if kyOSIsWin32
#include <windows.h>
#elif kyOSIsLinux

#endif

ky_datetime::ky_datetime(ulonglong time_micro):
    time_msec(0),
    time_second(0),
    time_minute(0),
    time_hour(0),
    date_year(0),
    date_month(0),
    date_day(0)
{
    _microsecond_ = 0;
    if (time_micro)
        *this = convert_time(time_micro);
}
ky_datetime::ky_datetime(int y, int mh, int d, int h, int me, int s, int ms):
    time_msec(ms),
    time_second(s),
    time_minute(me),
    time_hour(h),
    date_year(y),
    date_month(mh),
    date_day(d)
{
    _microsecond_ = 0;
}
ky_datetime::ky_datetime(int h, int me, int s, int ms):
    time_msec(ms),
    time_second(s),
    time_minute(me),
    time_hour(h),
    date_year(0),
    date_month(0),
    date_day(0)
{
    _microsecond_ = 0;
}
ky_datetime::ky_datetime(int me, int s, int ms ):
    time_msec(ms),
    time_second(s),
    time_minute(me),
    time_hour(0),
    date_year(0),
    date_month(0),
    date_day(0)
{
    _microsecond_ = 0;
}
ky_datetime::~ky_datetime()
{

}
ky_datetime::ky_datetime(const ky_datetime & rhs):
    time_msec(rhs.time_msec),
    time_second(rhs.time_second),
    time_minute(rhs.time_minute),
    time_hour(rhs.time_hour),
    date_year(rhs.date_year),
    date_month(rhs.date_month),
    date_day(rhs.date_day)
{
    _microsecond_ = rhs._microsecond_;
}
ky_datetime & ky_datetime::operator =(const ky_datetime & rhs)
{
    time_msec = (rhs.time_msec);
    time_second = (rhs.time_second);
    time_minute = (rhs.time_minute);
    time_hour = (rhs.time_hour);
    date_year = (rhs.date_year);
    date_month = (rhs.date_month);
    date_day = (rhs.date_day);
    _microsecond_ = rhs._microsecond_;
    return *this;
}
int ky_datetime::year() const
{
    return date_year;
}
int ky_datetime::month() const
{
    return date_month;
}
int ky_datetime::day() const
{
    return date_day;
}
int ky_datetime::hour() const
{
    return time_hour;
}
int ky_datetime::minute() const
{
    return time_minute;
}
int ky_datetime::second() const
{
    return time_second;
}
int ky_datetime::msec() const
{
    return time_msec;
}
uint64 ky_datetime::to_millisec()const
{
    return _microsecond_ / 1000;
}
ky_string ky_datetime::to_string(const ky_string &form)const
{
    ky_string out;
    out.format(form.to_latin1(), date_year, date_month, date_day,
               time_hour, time_minute, time_second, time_msec);
    return out;
}
void ky_datetime::from_string(const ky_string &s, const ky_string &)
{
    ky_list <ky_string> sp_dt = s.split(" ");
    if (sp_dt.count() ==2)
    {
        if (!sp_dt[0].is_empty())
        {
            ky_list <ky_string> sp_dd = sp_dt[0].split("-");
            if (sp_dd.count() == 3)
            {
                date_year = sp_dd[0].to_uint();
                date_month = sp_dd[1].to_uint();
                date_day = sp_dd[2].to_uint();
            }
        }
        if (!sp_dt[1].is_empty())
        {
            ky_list <ky_string> sp_dd = sp_dt[1].split(":");
            if (sp_dd.count() == 4)
            {
                time_hour = sp_dd[0].to_uint();
                time_minute = sp_dd[1].to_uint();
                time_second = sp_dd[2].to_uint();
                time_msec = sp_dd[3].to_uint();
            }
        }
    }
}

bool ky_datetime::operator !=(const ky_datetime &rhs)
{
    return !(*this == rhs);
}
bool ky_datetime::operator ==(const ky_datetime &rhs)
{
    return _microsecond_ == rhs._microsecond_;
}

bool ky_datetime::operator >(const ky_datetime &rhs)
{
    return _microsecond_ > rhs._microsecond_;
}
bool ky_datetime::operator <(const ky_datetime &rhs)
{
    return _microsecond_ < rhs._microsecond_;
}
bool ky_datetime::operator >=(const ky_datetime &rhs)
{
    return _microsecond_ >= rhs._microsecond_;
}
bool ky_datetime::operator <=(const ky_datetime &rhs)
{
    return _microsecond_ <= rhs._microsecond_;
}
ky_datetime ky_datetime::operator +(const ky_datetime &rhs)
{
    ky_datetime out;
    out.time_msec = rhs.time_msec + time_msec;
    out.time_second = rhs.time_second + time_second;
    out.time_minute = rhs.time_minute  + time_minute;
    out.time_hour = rhs.time_hour + time_hour;
    out.date_year  = rhs.date_year  + date_year;
    out.date_month  = rhs.date_month  + date_month;
    out.date_day  = rhs.date_day  + date_day;
    out._microsecond_ = rhs._microsecond_+ _microsecond_;
    return out;
}
ky_datetime ky_datetime::operator -(const ky_datetime &rhs)
{
    ky_datetime out;
    out.time_msec = time_msec - rhs.time_msec;
    out.time_second = time_second - rhs.time_second;
    out.time_minute = time_minute  - rhs.time_minute;
    out.time_hour = time_hour - rhs.time_hour;
    out.date_year  = date_year  - rhs.date_year;
    out.date_month  = date_month  - rhs.date_month;
    out.date_day  = date_day  - rhs.date_day;
    out._microsecond_ = _microsecond_ - rhs._microsecond_;
    return out;
}
ky_datetime &ky_datetime::operator +=(const ky_datetime &rhs)
{
    time_msec += rhs.time_msec;
    time_second += rhs.time_second;
    time_minute  += rhs.time_minute;
    time_hour += rhs.time_hour;
    date_year  += rhs.date_year;
    date_month  += rhs.date_month;
    date_day  += rhs.date_day;
    _microsecond_ += rhs._microsecond_;
    return *this;
}
ky_datetime &ky_datetime::operator -=(const ky_datetime &rhs)
{
    time_msec -= rhs.time_msec;
    time_second -= rhs.time_second;
    time_minute  -= rhs.time_minute;
    time_hour -= rhs.time_hour;
    date_year  -= rhs.date_year;
    date_month  -= rhs.date_month;
    date_day  -= rhs.date_day;
    _microsecond_ -= rhs._microsecond_;
    return *this;
}
ky_datetime ky_datetime::convert_time(uint64 total_us)
{
    ky_datetime out;
    const int timezone = 8;
    uint64 total_s = total_us / (1000 *1000);
    total_s += 60 *60 *timezone;
    int second = total_s % 60;
    uint64 total_minute = total_s / 60;
    int minute = total_minute % 60;
    uint64 total_hour = total_minute /60;
    int hour = total_hour % 24;
    uint64 total_day = total_hour /24;

    int year = 1970 + (total_day / 366);
    int month = 1 ;
    int day = 1;

    int diff_day = 0; bool leap_year = false;
    while (true)
    {
        int diff = (year - 1970) * 365;
        diff += (year - 1) / 4 -(1970 - 1) /4;
        diff -= (year - 1) / 100 -(1970 - 1) /100;
        diff += (year - 1) / 400 -(1970 - 1) /400;

        diff_day = total_day - diff;
        leap_year = ((year % 4 == 0) && (year %100 != 0)) ||
                (year % 400 == 0);
        if ((!leap_year && (diff_day < 365)) || (leap_year && (diff_day < 366)))
            break;
        else
            year++;
    }
    int index = 0;
    int month_day[2][13] =
    {
        {-1, 0,31,60,91,121,152,182,213,244,274,305,335},
        {-1, 0,31,59,90,120,151,181,212,243,273,304,334}
    };

    if (diff_day >= 59 && leap_year)
        index = 0;
    else
        index = 1;

    for (int i = 13 -1; i >= 1; i--)
    {
        if (diff_day >= month_day[index][i])
        {
            month = i;
            day = diff_day - month_day[index][i] +1;
            break;
        }
    }
    out._microsecond_ = total_us ;
    out.time_msec =   (total_us / 1000) % 1000;
    out.time_second = second ;
    out.time_minute = minute;
    out.time_hour =   hour;

    out.date_day =    day;
    out.date_month =  month;
    out.date_year =   year;
    return out;
}
ky_datetime ky_datetime::current_time()
{
    uint64 total_us = microsec();
    return convert_time(total_us);
}
uint64 ky_datetime::millisec()
{
    return microsec() / 1000;
}
uint64 ky_datetime::microsec()
{
    return nanosec() / 1000;
}
uint64 ky_datetime::nanosec(eTypes t)
{
#ifdef kyHasClockGetTime
    struct timespec time;
    int r;
    if (t == RealTime)
    {
        r = clock_gettime(CLOCK_REALTIME, &time);
    }
    else if (t == Monotonic)
    {
        r = clock_gettime(CLOCK_MONOTONIC, &time);
    }
    else if (t == Process)
    {
#if defined(CLOCK_PROCESS_CPUTIME_ID)
        r = clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time);
#elif defined(CLOCK_PROF)
        r = clock_gettime(CLOCK_PROF, &time);
#else
        r = -1;
        errno = ENOSYS;
#endif
    }
    else /* t == thread_cpu */
    {
#ifdef CLOCK_PROCESS_CPUTIME_ID
        r = clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time);
#else
        r = -1;
        errno = ENOSYS;
#endif
    }
    if (r != 0)
        throw ky_string(("Cannot get time."));
    uint64 ret = uint64(time.tv_sec) * 1000 * 1000 * 1000;

    return ret + (time.tv_nsec);

#else

    if (t == RealTime || t == Monotonic)
    {
        struct timeval tv;
        int r = gettimeofday(&tv, 0);
        if (r != 0)
            throw ky_string("Cannot get time.");
        return static_cast<uint64>(tv.tv_sec) * 1000 * 1000 * 1000 + tv.tv_usec * 1000;
    }
    else if (t == Process)
    {
        clock_t c = clock();
        return (c * static_cast<uint64>(1000000000) / CLOCKS_PER_SEC);
    }
    else
    {
        throw ky_string(("Cannot get time."));
    }

#endif
}
timespec ky_datetime::convert_timespec(uint64 milliSeconds )
{
    timespec ts = { 0, 0 };
    ts.tv_sec  = static_cast<int>( milliSeconds / 1000 );
    ts.tv_nsec = ( milliSeconds % 1000 ) * 1000000;
    return ts;
}
timespec ky_datetime::convert_timespec()
{
    timespec ts = { 0, 0 };
    ulonglong micro = microsec();
    ts.tv_sec  = static_cast<int>( micro / 1000000 );
    ts.tv_nsec = ( micro % 1000  ) * 100000;
    return ts;
}


#include "tools/ky_stream.h"
ky_streamb &operator << (ky_streamb &in, const ky_datetime &v)
{
    in << v.year();
    in << v.month();
    in << v.day();
    in << v.hour();
    in << v.minute();
    in << v.second();
    in << v.msec();
    return in;
}
ky_streamb &operator >> (ky_streamb &out, ky_datetime &v)
{
    int tmp_year, tmp_month, tmp_day, tmp_hour, tmp_minute, tmp_second, tmp_msec;
    out >> tmp_year ;
    out >> tmp_month ;
    out >> tmp_day ;
    out >> tmp_hour ;
    out >> tmp_minute ;
    out >> tmp_second ;
    out >> tmp_msec;
    v = ky_datetime(tmp_year, tmp_month, tmp_day, tmp_hour, tmp_minute, tmp_second, tmp_msec);
    return out;
}
ky_streamt &operator << (ky_streamt &in, const ky_datetime &v)
{
    in << v.year();
    in << v.month();
    in << v.day();
    in << v.hour();
    in << v.minute();
    in << v.second();
    in << v.msec();
    return in;
}
ky_streamt &operator >> (ky_streamt &out, ky_datetime &v)
{
    int tmp_year, tmp_month, tmp_day, tmp_hour, tmp_minute, tmp_second, tmp_msec;
    out >> tmp_year ;
    out >> tmp_month ;
    out >> tmp_day ;
    out >> tmp_hour ;
    out >> tmp_minute ;
    out >> tmp_second ;
    out >> tmp_msec;
    v = ky_datetime(tmp_year, tmp_month, tmp_day, tmp_hour, tmp_minute, tmp_second, tmp_msec);
    return out;
}
ky_variant &operator << (ky_variant &va, const ky_datetime &col)
{
    va.set(kyDateTimeCode, 0, (void*)&col, sizeof(ky_datetime));
    return va;
}

ky_variant &operator >> (ky_variant &va, ky_datetime &col)
{
    col = va.get<ky_datetime>(kyDateTimeCode);
    return va;
}
