
#include "arch/ky_timer.h"

#if kyArchIsX86
#include "cpu/x86_cpu_ability.h"
#endif

#ifndef kyHasGetTimeOfDay
#  if kyOSIsLinux || kyOSIsWin32 || kyOSIsApple
#    define kyHasGetTimeOfDay
#  endif
#endif

#ifndef kyHasClockGetTime
#  if (defined(_POSIX_TIMERS) && _POSIX_TIMERS > 0) || kyOSIsWin32
#    define kyHasClockGetTime
#  endif
#endif

ky_timer::ky_timer()
{

}
ky_timer::~ky_timer()
{

}

u64 ky_timer::millisec()
{
    return microsec() / 1000;
}
u64 ky_timer::microsec()
{
    return nanosec() / 1000;
}
u64 ky_timer::nanosec(eTypes t)
{
#if defined(kyHasClockGetTime)
    struct timespec time;
    int clk = CLOCK_REALTIME;
    switch ((int)t)
    {
    case Monotonic: clk = CLOCK_MONOTONIC;
    case Process:
#  if defined(CLOCK_PROCESS_CPUTIME_ID)
        clk = CLOCK_PROCESS_CPUTIME_ID;
#  elif defined(CLOCK_PROF)
        clk = CLOCK_PROF;
#  else
        errno = ENOSYS;
        return 0;
#  endif
    case Thread:
#  ifdef CLOCK_PROCESS_CPUTIME_ID
        clk = CLOCK_PROCESS_CPUTIME_ID;
#  else
        errno = ENOSYS;
        return 0;
#  endif
    }

    if (::clock_gettime(clk, &time) != 0)
    {
        //throw ky_string(("Cannot get time."));
        return 0;
    }
    const u64 ret = u64(time.tv_sec) * 1000 * 1000 * 1000;
    return ret + (time.tv_nsec); //flt64 = (ret * 1e-9);

#elif defined(kyHasGetTimeOfDay)
    switch ((int)t)
    {
    case RealTime :
    case Monotonic :
    {
        struct timeval tv;
        if (::gettimeofday(&tv, 0) != 0)
        {
            //throw ky_string("Cannot get time.");
        }
        const uint64 ret = u64(tv.tv_sec) * 1000 * 1000 * 1000;
        return ret + tv.tv_usec * 1000; //flt64 = (ret * 1e-6);
    }
    case Process:
    {
        clock_t c = ::clock();
        return (c * u64(1000000000) / CLOCKS_PER_SEC);
    }
    default:
    {
        //throw ky_string("Cannot get time.");
    }
    }

#endif
}


ky_timer_scope::ky_timer_scope(volatile uint64 &elapsed) :
    _elapsed(elapsed)
{
    _t0 = ky_timer::nanosec();
}

ky_timer_scope::~ky_timer_scope()
{
    const flt64 t1 = ky_timer::nanosec();
    _elapsed = t1 - _t0;
}

u64 ky_timer_scope::elapsed()const
{
    const flt64 t1 = ky_timer::nanosec();
    return t1 - _t0;
}


ky_timer_unit::ky_timer_unit()
{
    zero();
}
ky_timer_unit::~ky_timer_unit()
{

}
void ky_timer_unit::zero()
{
    _cycles = 0;
}


void ky_timer_unit::set_timer()
{
#  if kyArchIs64Bit && kyArchIsX86
    _cycles = cpu_ability::rdtsc();
#  else
    _cycles = ky_timer::nanosec();
#  endif
}

void ky_timer_unit::add_diff(const ky_timer_unit &t0, const ky_timer_unit &t1)
{
    _cycles += t1._cycles - t0._cycles;
}

void ky_timer_unit::sub(const ky_timer_unit &t)
{
    _cycles -= t._cycles;
}

flt64 ky_timer_unit::to_sec() const
{
#if kyArchIsX86
    return (flt64)_cycles / cpu_ability::clock_frequency();
#endif
}

