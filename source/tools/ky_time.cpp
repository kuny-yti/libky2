#include "ky_time.h"
#include "ky_timer.h"

ky_time::ky_time()
{
    _second   = 0;
    _minute   = 0;
    _hour     = 0;
    _millisec = 0;
}

ky_time::~ky_time()
{

}

ky_time::ky_time(u64 microsec, const i32 timezone)
{
    const u64 total_sec = (microsec /1000000) + (3600 *timezone);
    const u64 total_minute = total_sec /60;
    const u64 total_hour = total_minute /60;

    _millisec = (microsec / 1000) % 1000;
    _second   = total_sec %60;
    _minute   = total_minute %60;
    _hour     = total_hour /*%24*/;
}

ky_time::ky_time(i32 hour, i32 minute, i32 second, i32 millisec)
{
    _millisec = millisec % 1000;
    _second   = second % 60;
    _minute   = minute % 60;
    _hour     = hour % 24;
}
bool ky_time::operator ==(const ky_time &rhs)
{
    return (_hour == rhs._hour) &&
            (_minute == rhs._minute) &&
            (_second == rhs._second) &&
            (_millisec == rhs._millisec);
}
bool ky_time::operator !=(const ky_time &rhs)
{
    return !operator ==(rhs);
}


bool ky_time::operator >(const ky_time &rhs)
{
    if (_hour <= rhs._hour)
        return false;
    else if (_minute <= rhs._minute)
        return false;
    else if (_second <= rhs._second)
        return false;
    else if (_millisec <= rhs._millisec)
        return false;

    return true;
}

bool ky_time::operator <(const ky_time &)
{
    if (_hour >= rhs._hour)
        return false;
    else if (_minute >= rhs._minute)
        return false;
    else if (_second >= rhs._second)
        return false;
    else if (_millisec >= rhs._millisec)
        return false;

    return true;
}

bool ky_time::operator >=(const ky_time &)
{
    if (_hour < rhs._hour)
        return false;
    else if (_minute < rhs._minute)
        return false;
    else if (_second < rhs._second)
        return false;
    else if (_millisec < rhs._millisec)
        return false;

    return true;
}

bool ky_time::operator <=(const ky_time &)
{
    if (_hour > rhs._hour)
        return false;
    else if (_minute > rhs._minute)
        return false;
    else if (_second > rhs._second)
        return false;
    else if (_millisec > rhs._millisec)
        return false;

    return true;
}

ky_time ky_time::operator +(const ky_time &rhs)
{
    ky_time out;
    out._millisec = _millisec + rhs._millisec;
    out._second   = _second   + rhs._second;
    out._minute   = _minute   + rhs._minute;
    out._hour     = _hour     + rhs._hour;

    out._second += (out._millisec / 1000);
    out._minute += (out._second / 60);
    out._hour   += (out._minute / 60);

    out._millisec = out._millisec % 1000;
    out._second   = out._second % 60;
    out._minute   = out._minute % 60;

    return out;
}

ky_time ky_time::operator -(const ky_time &rhs)
{
    ky_time out;
    out._millisec = _millisec - rhs._millisec;
    out._second   = _second   - rhs._second;
    out._minute   = _minute   - rhs._minute;
    out._hour     = _hour     - rhs._hour;

    if (out._millisec < 0)
    {
        --out._second;
        out._millisec = 1000 - out._millisec;
    }
    if (out._second < 0)
    {
        --out._minute;
        out._second = 60 - out._second;
    }
    if (out._minute < 0)
    {
        --out._hour;
        out._minute = 60 - out._minute;
    }

    return out;
}

ky_time &ky_time::operator +=(const ky_time &rhs)
{
    *this = operator +(rhs);
    return *this;
}

ky_time &ky_time::operator -=(const ky_time &)
{
    *this = operator +(rhs);
    return *this;
}

void ky_time::swap(ky_time &b)
{
    ky_time tmp = b;
    b = *this;
    *this = tmp;
}

u64 ky_time::millisec()
{
    return ky_timer::millisec();
}

timespec ky_time::convert_timespec(const u64 millisec)
{
    timespec ts = { 0, 0 };
    ts.tv_sec  = static_cast<int>( millisec / 1000 );
    ts.tv_nsec = ( millisec % 1000 ) * 1000000;
    return ts;
}

timespec ky_time::convert_timespec(const ky_time &time)
{
    timespec ts = { 0, 0 };
    ts.tv_sec  = static_cast<int>(time._microsec / 1000000);
    ts.tv_nsec = (time._microsec % 1000) * 1000;
    return ts;
}

ky_time ky_time::current()
{
    return ky_time(ky_timer::microsec());
}

u64 ky_time::to_millisec() const
{
    return (u64)_millisec + (u64)_second *1000 +
            (u64)_minute *60000 + (u64)_hour *3600000;
}

ky_string ky_time::to_string(const ky_string &form) const
{
    ky_string out;
    const char *cf = form.to_latin1();
    const char *ce = cf + form.count();
    do
    {
        switch (*cf)
        {
        case 'h' :
        {
            if (*(++cf) == 'h')
                out += ky_string::number(_hour);
            break;
        }
        case 'm' :
        {
            if (*(cf+1) == 'm')
                out += ky_string::number(_minute);
            else if (*(cf+1) == 's')
                out += ky_string::number(_millisec);
            ++cf;
            break;
        }
        case 's' :
        {
            if (*(++cf) == 's')
                out += ky_string::number(_second);
            break;
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

void ky_time::from_string(const ky_string &s, const ky_string &form = ky_string("hh:mm:ss:ms"))
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
        case 'h' :
        {
            if (*(++cf) == 'h')
               addr = &_hour;
            break;
        }
        case 'm' :
        {
            if (*(cf+1) == 'm')
                addr = &_minute;
            else if (*(cf+1) == 's')
                addr = &_millisec;
            ++cf;
            break;
        }
        case 's' :
        {
            if (*(++cf) == 's')
                addr = &_second;
            break;
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
