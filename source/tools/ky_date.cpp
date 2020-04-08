#include "ky_date.h"
#include "ky_timer.h"

/*
ky_date::ky_date(u64 microsec)
{
    static const i32 month_day[2][13] =
    {
        {-1, 0,31,60,91,121,152,182,213,244,274,305,335},
        {-1, 0,31,59,90,120,151,181,212,243,273,304,334}
    };
    const u64 total_sec = (microsec /1000000) + (3600 *kyTimeZone);
    const u64 total_minute = total_sec /60;
    const u64 total_hour = total_minute /60;
    const u64 total_day = total_hour /24;
    _time = ky_time(total_hour %24, total_minute %60,
                    total_sec %60, (microsec /1000) %1000);

    i32 year = kyDateBase + (total_day /366);
    i32 month = 1 ;
    i32 day = 1;
    int diff_day = 0;
    bool leap_year = false;
    while (true)
    {
        int diff = (year - kyDateBase) * 365;
        diff += (year - 1) / 4 -(kyDateBase - 1) /4;
        diff -= (year - 1) / 100 -(kyDateBase - 1) /100;
        diff += (year - 1) / 400 -(kyDateBase - 1) /400;

        diff_day = total_day - diff;
        leap_year = ((year % 4 == 0) && (year %100 != 0)) ||
                (year % 400 == 0);
        if ((!leap_year && (diff_day < 365)) || (leap_year && (diff_day < 366)))
            break;
        else
            year++;
    }

    i32 index = 0;
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

    _day =    day;
    _month =  month;
    _year =   year;
}
*/

const static u32 _LunarTable[199] =
{
    0x04AE53,0x0A5748,0x5526BD,0x0D2650,0x0D9544,0x46AAB9,0x056A4D,0x09AD42,0x24AEB6,0x04AE4A,/*1901-1910*/
    0x6A4DBE,0x0A4D52,0x0D2546,0x5D52BA,0x0B544E,0x0D6A43,0x296D37,0x095B4B,0x749BC1,0x049754,/*1911-1920*/
    0x0A4B48,0x5B25BC,0x06A550,0x06D445,0x4ADAB8,0x02B64D,0x095742,0x2497B7,0x04974A,0x664B3E,/*1921-1930*/
    0x0D4A51,0x0EA546,0x56D4BA,0x05AD4E,0x02B644,0x393738,0x092E4B,0x7C96BF,0x0C9553,0x0D4A48,/*1931-1940*/
    0x6DA53B,0x0B554F,0x056A45,0x4AADB9,0x025D4D,0x092D42,0x2C95B6,0x0A954A,0x7B4ABD,0x06CA51,/*1941-1950*/
    0x0B5546,0x555ABB,0x04DA4E,0x0A5B43,0x352BB8,0x052B4C,0x8A953F,0x0E9552,0x06AA48,0x6AD53C,/*1951-1960*/
    0x0AB54F,0x04B645,0x4A5739,0x0A574D,0x052642,0x3E9335,0x0D9549,0x75AABE,0x056A51,0x096D46,/*1961-1970*/
    0x54AEBB,0x04AD4F,0x0A4D43,0x4D26B7,0x0D254B,0x8D52BF,0x0B5452,0x0B6A47,0x696D3C,0x095B50,/*1971-1980*/
    0x049B45,0x4A4BB9,0x0A4B4D,0xAB25C2,0x06A554,0x06D449,0x6ADA3D,0x0AB651,0x093746,0x5497BB,/*1981-1990*/
    0x04974F,0x064B44,0x36A537,0x0EA54A,0x86B2BF,0x05AC53,0x0AB647,0x5936BC,0x092E50,0x0C9645,/*1991-2000*/
    0x4D4AB8,0x0D4A4C,0x0DA541,0x25AAB6,0x056A49,0x7AADBD,0x025D52,0x092D47,0x5C95BA,0x0A954E,/*2001-2010*/
    0x0B4A43,0x4B5537,0x0AD54A,0x955ABF,0x04BA53,0x0A5B48,0x652BBC,0x052B50,0x0A9345,0x474AB9,/*2011-2020*/
    0x06AA4C,0x0AD541,0x24DAB6,0x04B64A,0x69573D,0x0A4E51,0x0D2646,0x5E933A,0x0D534D,0x05AA43,/*2021-2030*/
    0x36B537,0x096D4B,0xB4AEBF,0x04AD53,0x0A4D48,0x6D25BC,0x0D254F,0x0D5244,0x5DAA38,0x0B5A4C,/*2031-2040*/
    0x056D41,0x24ADB6,0x049B4A,0x7A4BBE,0x0A4B51,0x0AA546,0x5B52BA,0x06D24E,0x0ADA42,0x355B37,/*2041-2050*/
    0x09374B,0x8497C1,0x049753,0x064B48,0x66A53C,0x0EA54F,0x06B244,0x4AB638,0x0AAE4C,0x092E42,/*2051-2060*/
    0x3C9735,0x0C9649,0x7D4ABD,0x0D4A51,0x0DA545,0x55AABA,0x056A4E,0x0A6D43,0x452EB7,0x052D4B,/*2061-2070*/
    0x8A95BF,0x0A9553,0x0B4A47,0x6B553B,0x0AD54F,0x055A45,0x4A5D38,0x0A5B4C,0x052B42,0x3A93B6,/*2071-2080*/
    0x069349,0x7729BD,0x06AA51,0x0AD546,0x54DABA,0x04B64E,0x0A5743,0x452738,0x0D264A,0x8E933E,/*2081-2090*/
    0x0D5252,0x0DAA47,0x66B53B,0x056D4F,0x04AE45,0x4A4EB9,0x0A4D4C,0x0D1541,0x2D92B5          /*2091-2099*/
};
const static i16 _MonthBase[12] = {0,31,59,90,120,151,181,212,243,273,304,334};
const static i8  _DayTable[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
const static i16 _BaseYear = 1901;
// 每个四年一次闰月
// 每个月的天数 (非闰年)
// 一年的小时数 365 * 24 = 8760 (非闰年)
const static i16 _1YearHour = 8760;

ky_date::ky_date():
    _time()
{
    _year   = 0;
    _month  = 0;
    _day    = 0;
}

ky_date::~ky_date()
{

}

ky_date::ky_date(u64 microsec, const i32 timezone)
{
    const u64 total_sec = (microsec /1000000) + (3600 *timezone); // 计算时差
    const u64 total_minute = total_sec /60;
          u64 total_hour = total_minute /60;

    // 取过去多少个四年
    // 每年365天，4年中有一年为366天。
    // 4年的小时数 1461(天) * 24(小时) = 39384(小时)
    const int32 Pass4year = (total_hour /39384);
    //计算年份
    _year = (Pass4year << 2) + kyDateBase;

    //计算星期
    //_wday = (total_hour /24 +4) %7;
    // 四年中剩下的小时数
    total_hour = total_hour %39384;
    //计算在这一年的天数
    //_yday = (total_hour /24) %365;

    //校正闰年影响的年份，计算一年中剩下的小时数
    for (int32 _hpery = 0; total_hour < _hpery; _year++, total_hour -=_hpery)
    {
        //一年的小时数
        _hpery = _1YearHour;

        //判断闰年,是闰年，一年则多24小时，即一天
        if (!(_year & 3))
            _hpery += 24;
    }
    _time = ky_time(total_hour %24, total_minute %60,
                    total_sec %60, (microsec /1000) %1000);

    //一年中剩下的天数, +1为假定闰年
    total_hour = (total_hour /24) +1;

    //校正润年的误差，计算月份，日期
    if (!(_year & 3))
    {
        if (total_hour > 60)
            --total_hour;
        else
        {
            if (total_hour == 60)
            {
                _month = 1;
                _day = 29;
                return;
            }
        }
    }

    //计算月日
    for (_month = 0; _DayTable[_month] < total_hour; ++_month)
        total_hour -= _DayTable[_month];

    _month +=1;
    _day = total_hour;
}

ky_date::ky_date(i32 year, i32 month, i32 day, const ky_time &time):
    _time(time),
    _year(year),
    _month(month),
    _day(day)
{
}

ky_date::ky_date(const ky_date &dt):
    _time(dt.time()),
    _year(dt.year()),
    _month(dt.month()),
    _day(dt.day())
{

}

bool ky_date::is_yleap() const
{
    return (!(year() %400) || (!(year() %4) && (year() %100)));
}

int ky_date::yday() const
{
    if(is_yleap() && (month() >2))
        return _MonthBase[month()] + day() +1;

    return _MonthBase[month()] + day();
}
int ky_date::wday()const
{
    i32 month = month();
    i32 year = year();
    if (month <3)
    {
        month += 12;
        --year;
    }
    // 基姆拉尔森公式
    return (day() +1 + 2* month + 3* (month +1) /5 +
            year + (year >>2) - year /100 + year /400) %7;
}
int ky_date::week() const
{
    const i32 _yday = yday();
    i32 base = 7- (_yday +1 - (wday() +1)) %7;
    if (base == 7)
        base = 0;

    return (base + _yday) /7 +1;
}

bool ky_date::operator ==(const ky_date &rhs)
{
    return (_time == rhs._time) &&
            (_year == rhs._year) &&
            (_month == rhs._month) &&
            (_day == rhs._day);
}
bool ky_date::operator !=(const ky_date &rhs)
{
    return !operator ==(rhs);
}

bool ky_date::operator >(const ky_date &rhs)
{
    if (_time <= rhs._time)
        return false;
    else if (_day <= rhs._day)
        return false;
    else if (_month <= rhs._month)
        return false;
    else if (_year <= rhs._year)
        return false;
    return true;
}

bool ky_date::operator <(const ky_date &rhs)
{
    if (_time >= rhs._time)
        return false;
    else if (_day >= rhs._day)
        return false;
    else if (_month >= rhs._month)
        return false;
    else if (_year >= rhs._year)
        return false;
    return true;
}

bool ky_date::operator >=(const ky_date &rhs)
{
    if (_time < rhs._time)
        return false;
    else if (_day < rhs._day)
        return false;
    else if (_month < rhs._month)
        return false;
    else if (_year < rhs._year)
        return false;
    return true;
}

bool ky_date::operator <=(const ky_date &rhs)
{
    if (_time > rhs._time)
        return false;
    else if (_day > rhs._day)
        return false;
    else if (_month > rhs._month)
        return false;
    else if (_year > rhs._year)
        return false;
    return true;
}

ky_date ky_date::operator +(const ky_date &rhs)
{
    ky_date out;
    out._time  = _time  + rhs._time;
    out._day   = _day   + rhs._day;
    out._month = _month + rhs._month;
    out._year  = _year  + rhs._year;

    // 超过24小时，需要加一天
    if (out._time.hour() /24)
    {
        ++out._day;
        out._time = ky_time(out._time.hour() %24, out._time.minute(), out._time.second(), out._time.msec());
    }
    // 超过一年，需要加一年
    if (out.month() > 12)
    {
        ++out._year;
        out._month = out.month() % 12;
    }
    // 超过一个月，需要加一个月
    out._month += (out._day / _DayTable[out.month()-1]);
    out._day = out._day % _DayTable[out.month()-1];
    out._year += out.month() /12;
    out._month = out.month() %12;

    return out;
}

ky_date ky_date::operator -(const ky_date &rhs)
{
    ky_date out;
    out._time  = _time  - rhs._time;
    out._day   = _day   - rhs._day;
    out._month = _month - rhs._month;
    out._year  = _year  - rhs._year;

    if (out._time.hour() < 0)
    {
        --out._day;
        out._time = ky_time(-out._time.hour(), out._time.minute(), out._time.second(), out._time.msec());
    }
    if (out._day <= 0)
    {
        --out._month;
        if (out.month() <= 0)
        {
            --out._year;
            out._month = 12- out._month;
        }

        out._day = _DayTable[out.month()-1] - out._day;
        // 闰年时若未2月则需加一天
        if (out.is_yleap() && (out.month() == 2))
            ++out._day;
    }
    if (out._month <= 0)
    {
        --out._year;
        out._month = 12- out._month;
    }

    return out;
}

ky_date &ky_date::operator +=(const ky_date &rhs)
{
    *this = operator +(rhs);
    return *this;
}

ky_date &ky_date::operator -=(const ky_date &)
{
    *this = operator +(rhs);
    return *this;
}

void ky_date::swap(ky_date &b)
{
    ky_date tmp = b;
    b = *this;
    *this = tmp;
}

ky_date ky_date::current(const i32 timezone)
{
    return ky_date(ky_timer::microsec(), timezone);
}

u64 ky_date::to_millisec() const
{
    return _time.to_millisec() * (_year - kyDateBase) * yday();
}

ky_date ky_date::to_lunar() const
{
    i32 Spring_NY, Sun_NY, StaticDayCount;
    i32 index, flag;
    ky_date date(*this);

    //Spring_NY 记录春节离当年元旦的天数。
    //Sun_NY 记录阳历日离当年元旦的天数。
    if (((_LunarTable[date.year() -_BaseYear] & 0x0060) >>5) == 1)
        Spring_NY = (_LunarTable[date.year() -_BaseYear] & 0x001F) -1;
    else
        Spring_NY = (_LunarTable[date.year() -_BaseYear] & 0x001F) -1 +31;

    Sun_NY = _MonthAdd[date.month() -1] + date.day() -1;
    if ((!(date.year() %4)) && (date.month() >2))
        Sun_NY++;

    //StaticDayCount记录大小月的天数 29 或30
    //index 记录从哪个月开始来计算。
    //flag 是用来对闰月的特殊处理。
    //判断阳历日在春节前还是春节后
    if (Sun_NY >= Spring_NY)//阳历日在春节后（含春节那天）
    {
        Sun_NY -= Spring_NY;
        date._month = 1;
        index = 1;
        flag = 0;

        if (!(_LunarTable[date.year() -_BaseYear] & (0x80000 >> (index -1))))
            StaticDayCount = 29;
        else
            StaticDayCount = 30;

        while (Sun_NY >= StaticDayCount)
        {
            Sun_NY -= StaticDayCount;
            index++;
            if (date.month() == ((_LunarTable[date.year() -_BaseYear] &0xF00000) >>20))
            {
                flag = ~flag;
                if (flag == 0)
                    date._month++;
            }
            else
                date._month++;
            if ((_LunarTable[date.year() -_BaseYear] & (0x80000 >> (index -1))) ==0)
                StaticDayCount = 29;
            else
                StaticDayCount = 30;
        }
        date._day = Sun_NY + 1;
    }
    else //阳历日在春节前
    {
        Spring_NY -= Sun_NY;
        date._year--;
        date._month = 12;

        if (!((_LunarTable[date.year() -_BaseYear] &0xF00000) >>20))
            index = 12;
        else
            index = 13;

        flag = 0;
        if (!(_LunarTable[date.year() -_BaseYear] & (0x80000 >> (index -1))))
            StaticDayCount = 29;
        else
            StaticDayCount = 30;

        while (Spring_NY > StaticDayCount)
        {
            Spring_NY -= StaticDayCount;
            index--;
            if (flag == 0)
                date._month--;
            if (date.month() == ((_LunarTable[date.year() -_BaseYear] &0xF00000) >>20))
                flag = ~flag;

            if (!(_LunarTable[date.year() -_BaseYear] & (0x80000 >> (index -1))))
                StaticDayCount = 29;
            else
                StaticDayCount = 30;
        }
        date._day = StaticDayCount - Spring_NY +1;
    }

    if (date.month() == ((_LunarTable[date.year() -_BaseYear] &0xF00000) >>20))
        ; // 闰

    return date;
}

ky_string ky_date::to_string(const ky_string &form) const
{
    ky_string out;
    const char *cf = form.to_latin1();
    const char *ce = cf + form.count();
    do
    {
        switch (*cf)
        {
        case 'y' :
        {
            if (*(++cf) == 'y')
                out += ky_string::number(_year);
            break;
        }
        case 'm' :
        {
            if (*(++cf) == 'm')
                out += ky_string::number(_month);
            break;
        }
        case 'd' :
        {
            if (*(++cf) == 'd')
                out += ky_string::number(_day);
            break;
        }
        case 'h' :
        {
            out += _time.to_string(ky_string(cf, ce - cf));
            return out;
        }
        default:
        {
            out += *cf;
            break;
        }
        }
    }while (++cf != ce);

    return out;
}

void ky_date::from_string(const ky_string &s, const ky_string &form)
{
    const char *tf = s.to_latin1();
    const char *tb = tf;
    const char *te = tf + s.count();
    const char *cf = form.to_latin1();
    const char *ce = cf + form.count();
    i32 *addr = 0;
    do
    {
        switch (*cf)
        {
        case 'y' :
        {
            if (*(++cf) == 'y')
               addr = &_year;
            break;
        }
        case 'm' :
        {
            if (*(++cf) == 'm')
                addr = &_month;
            break;
        }
        case 'd' :
        {
            if (*(++cf) == 'd')
                addr = &_day;
            break;
        }
        case ' ':
            break;
        case 'h':
        {
            _time.from_string(ky_string(tf, te - tf), ky_string(cf, ce - cf));
            return ;
        }
        default:
        {
            do
            {
                if ((*tf == *cf) || (cf == ce))
                {
                    if (cf == ce)
                    {
                        ky_string tmp(tb, te - tb);
                        *addr = tmp.to_long();
                    }
                    else
                    {
                        ky_string tmp(tb, tf - tb);
                        *addr = tmp.to_long();
                    }
                    tb = ++tf;
                    break;
                }
            }
            while (++tf != te);
            break;
        }
        }
    }while (cf++ != ce);
}
