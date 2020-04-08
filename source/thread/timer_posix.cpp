#include "timer_posix.h"
#include "ky_date.h"
#include <sys/timerfd.h>
#include <unistd.h>

timer_posix::timer_posix(bool is_rel):
    posix_fd(),
    inl_relatively(is_rel),
    inl_timeout(-1),
    inl_total(0)
{
    const int clockid = is_rel ? CLOCK_REALTIME : CLOCK_MONOTONIC;
    posix_fd::set(::timerfd_create(clockid, TFD_NONBLOCK | TFD_CLOEXEC),
                  Notify_Socket | Notify_Read | Notify_Timer);
    if (posix_fd::is_valid())
        start(-1);
}
timer_posix::~timer_posix()
{
    if (posix_fd::is_valid())
        ::close(posix_fd::get());
}

bool timer_posix::start (int64 ms)
{
    if (!posix_fd::is_valid())
        return false;

    if (ms >= 0)
        inl_timeout = ms;
    itimerspec its {{0,0},{0,0}};
    its.it_value = ky_time::convert_timespec(inl_timeout);
    its.it_interval.tv_sec = its.it_value.tv_sec;
    its.it_interval.tv_nsec = its.it_value.tv_nsec;
    inl_total = 0;
    inl_past = 0;
    inl_base = ky_time::millisec();

    if (::timerfd_settime(posix_fd::get(), 0, &its, 0) < 0)
        return false;
    return true;
}
int64 timer_posix::elapsed()const
{
    return inl_past;
}
bool timer_posix::stop ()
{
    itimerspec its {{0,0},{0,0}};
    if (::timerfd_settime(posix_fd::get(), 0, &its, 0) < 0)
        return false;
    inl_total = 0;
    return true;
}

void timer_posix::despatch()
{
    int64 exp = 0, readten = 0;
    while ((readten = ::read(posix_fd::get(), &exp, sizeof(exp))) != sizeof(exp))
    {
        if (readten == -1 && errno != EAGAIN && errno != EINTR)
            return ;
    }
    inl_total += exp;
    inl_past += ky_time::millisec() - inl_base;
}

