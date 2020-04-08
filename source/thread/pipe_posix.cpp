#include "pipe_posix.h"
#include <unistd.h>

enum
{
    IPC_Error,
    IPC_UnixSP,
    IPC_Pipe,
    IPC_Fifo,
    IPC_Socket,
};
pipe_posix::pipe_posix():
    posix_fd()
{
    inl_priv[0] = -1;
    inl_priv[1] = -1;
    inl_flag = IPC_UnixSP;
    if (::socketpair (PF_UNIX, SOCK_STREAM, 0, inl_priv) < 0)
    {
        if (::pipe(inl_priv) < 0)
        {
            inl_flag = IPC_Error;
            inl_priv[0] = -1;
            inl_priv[1] = -1;
        }
        else
            inl_flag = IPC_Pipe;
    }

    if (inl_flag != IPC_Error)
        posix_fd::set(inl_priv[0], Notify_Socket | Notify_Read);
}

pipe_posix::~pipe_posix()
{
    if (inl_priv[0] >= 0)
        ::close(inl_priv[0]);
    if (inl_priv[1] >= 0)
        ::close(inl_priv[1]);
}
bool pipe_posix::active()
{
    ssize_t written;
    while ((written = ::write (inl_priv[1], "W", 1)) != 1)
    {
        if (written == -1 && errno != EAGAIN && errno != EINTR)
            return false;
    }
    return true;
}
bool pipe_posix::cancel()
{
    char buf[1] = {'\0'};
    ssize_t readten;
    while ((readten = ::read (inl_priv[0], buf, 1)) != 1)
    {
        if (readten == -1 && errno != EAGAIN && errno != EINTR)
            return false;
    }
    return true;
}
bool pipe_posix::is_active()const
{
    return wake & Notify_Read;
}
void pipe_posix::despatch()
{
    char buf[1] = {'\0'};
    ssize_t readten;
    if (wake & Notify_Read)
    {
        while ((readten = ::read (inl_priv[0], buf, 1)) != 1)
        {
            if (readten == -1 && errno != EAGAIN && errno != EINTR)
                return ;
        }
    }
}
