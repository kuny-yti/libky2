#include "signal_posix.h"
#include <unistd.h>

signal_posix::signal_posix():
    posix_fd()
{
    ::sigemptyset(&mask);
}
signal_posix::~signal_posix()
{
    if (posix_fd::get() >= 0)
        ::close(posix_fd::get());
}
void signal_posix::addset(int signo)
{
    ::sigaddset(&mask, signo);
    update();
}
void signal_posix::delset(int signo)
{
    ::sigdelset(&mask, signo);
    update();
}

void signal_posix::despatch()
{
    signalfd_siginfo info;
    ssize_t readten = 0;
    while ((readten = ::read (posix_fd::get(), &info, sizeof(info))) != sizeof(info))
    {
        if (readten == -1 && errno != EAGAIN && errno != EINTR)
            return ;
    }
    signo = info.ssi_signo;
}

void signal_posix::update()
{
    posix_fd::set(::signalfd(posix_fd::get(), &mask, SFD_NONBLOCK | SFD_CLOEXEC),
                  Notify_Socket | Notify_Read);
    sigset_t allset;
    ::sigfillset(&allset);
    ::sigprocmask(SIG_UNBLOCK, &allset, 0);
    ::sigprocmask(SIG_BLOCK, &mask, 0);
}
