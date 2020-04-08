#include "event_poll.h"
#include "ky_lock.h"
#include "ky_vector.h"
#include "ky_map.h"

#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

#if kyOSIsWin32

#  ifndef EWOULDBLOCK
#  define EWOULDBLOCK EAGAIN
#  endif

typedef struct
{
    int              fd;
    long             event_mask;
    WSANETWORKEVENTS events;
    long             ignored_event_mask;
    HANDLE           hd;
} WinsockFd;
typedef WinsockFd PollFd;

namespace impl
{
struct poll_core
{
    ky_vector<PollFd> active_ignored;
    ky_vector<HANDLE> active_event;
    HANDLE            wakeup_event;    ///< 用于唤醒

    ky_mutex          mutex;           ///< 轮询锁

    bool              controllable;    ///<
    bool              timer;
    ky_atomic<int>    waiting;         ///< 等待数量
    ky_atomic<int>    control_pending; ///< 唤醒记录
    ky_atomic<int>    flushing;        ///< 完成退出
    ky_atomic<int>    rebuild;         ///< 是否需要重新激活轮询

    ky_vector<PollFd> active;          ///< 被激活的轮询
    ky_vector<PollFd> inactive;        ///< 等待被激活的轮询
    ky_vector<PollFd> inremove;        ///< 需要被删除的轮询

    virtual ~poll_core()
    {

    }

    static PollFd *find(ky_vector<PollFd> *vec, int fd, int *inx = 0)
    {
        if (inx)
            *inx = 0;

        foreach (PollFd &var, *vec)
        {
            if (var.fd == fd)
                return &var;
            if (inx)
                (*inx)++;
        }
        if (inx)
            *inx = -1;
        return 0;
    }

    //!
    //! \brief raise_wakeup 提出唤醒
    //! \return
    //!
    bool raise_wakeup ()
    {
        bool result = true;

        mutex.lock ();

        // 未提出唤醒则提出唤醒
        if (control_pending.load () == 0)
            result = wake_event ();

        // 已经提出唤醒则将待唤醒数加一
        if (result)
            ++control_pending;

        mutex.unlock ();

        return result;
    }
    //!
    //! \brief release_wakeup 释放唤醒
    //! \return
    //!
    bool release_wakeup ()
    {
        bool result = false;

        mutex.lock ();

        if (control_pending.load () > 0)
        {
            if (control_pending.load () == 1)
                result = release_event ();
            else
                result = true;

            if (result)
                --control_pending;
            else
                errno = EWOULDBLOCK;
        }

        mutex.unlock ();

        return result;
    }

    //!
    //! \brief release_all_wakeup 释放所有唤醒
    //! \return
    //!
    int release_all_wakeup ()
    {
        int old = -1;

        mutex.lock ();

        if ((old = control_pending.load ()) > 0)
        {
            if (release_event ())
                control_pending.store (0);
            else
                old = 0;
        }

        mutex.unlock ();

        return old;
    }


    bool init (bool ctl)
    {
        rebuild.store (1);

        controllable = ctl;
        control_pending = 0;
        if (ctl)
            timer = true;

        wakeup_event = CreateEvent (NULL, TRUE, FALSE, NULL);
        return wakeup_event;
    }
    void uninit ()
    {
        CloseHandle (wakeup_event);

        foreach (const PollFd &var, active)
        {
            WSAEventSelect (var.fd, var.hd, 0);
            CloseHandle (var.hd);
        }
        foreach (const PollFd &var, inactive)
        {
            WSAEventSelect (var.fd, var.hd, 0);
            CloseHandle (var.hd);
        }
        foreach (const PollFd &var, inremove)
        {
            WSAEventSelect (var.fd, var.hd, 0);
            CloseHandle (var.hd);
        }

        active.clear();
        inactive.clear();
        inremove.clear();
        active_ignored.clear();
    }

    bool wake_event ()
    {
        SetLastError (0);
        errno = 0;
        if (!SetEvent (wakeup_event))
        {
            char msg[1024] = "<unknown>";
            format_last_error (msg, sizeof (msg));
            //log_err ("%p: failed to set wakup_event: %s", this, msg);
            errno = EBADF;
            return false;
        }

        return true;
    }

    bool release_event ()
    {
        DWORD status;
        SetLastError (0);
        errno = 0;

        status = WaitForSingleObject (wakeup_event, INFINITE);
        if (status)
        {
            const char *reason = "unknown";
            char msg[1024] = "<unknown>";
            switch (status) {
            case WAIT_ABANDONED:
                reason = "WAIT_ABANDONED";
                break;
            case WAIT_TIMEOUT:
                reason = "WAIT_TIMEOUT";
                break;
            case WAIT_FAILED:
                format_last_error (msg, sizeof (msg));
                reason = msg;
                break;
            default:
                reason = "other";
                break;
            }
            //log_err ("%p: failed to block on wakup_event: %s", this, reason);
            errno = EBADF;
            return false;
        }

        if (!ResetEvent (wakeup_event))
        {
            char msg[1024] = "<unknown>";
            format_last_error (msg, sizeof (msg));
            //log_err ("%p: failed to reset wakup_event: %s", this, msg);
            errno = EBADF;
            return false;
        }

        return true;
    }

    void add(int fd)
    {
        if ((find(&inactive, fd) != 0) || (find(&active, fd) != 0))
            return ;

        PollFd wfd;
        wfd.fd = fd;
        wfd.event_mask = FD_CLOSE;
        memset (&wfd.events, 0, sizeof (wfd.events));
        wfd.ignored_event_mask = 0;
        wfd.hd = WSACreateEvent ();
        inactive.append(wfd);
        rebuild.store (1);
    }
    void del(int fd)
    {
        int inx = -1;
        PollFd *pfd = find(&inactive, fd, &inx);
        if (pfd)
        {
            WSAEventSelect (pfd->fd, pfd->hd, 0);
            CloseHandle (pfd->hd);

            inactive.remove(inx);
            return ;
        }

        pfd = find(&active, fd, &inx);
        if (pfd)
        {
            inremove.append(*pfd);
            active.remove(inx);
            rebuild.store(1);
        }
    }
    bool mod_write(int fd, bool act)
    {
        PollFd *pfd = find(&inactive, fd);
        if (pfd == 0)
            pfd = find(&active, fd);
        if (pfd)
        {
            if (act)
                pfd->event_mask |= FD_WRITE | FD_CONNECT;
            else
                pfd->event_mask &= ~(FD_WRITE | FD_CONNECT);
            if ((pfd->ignored_event_mask & pfd->event_mask) == 0)
                pfd->ignored_event_mask = 0;
            rebuild.store (1);
            return true;
        }
        return false;
    }
    bool mod_read(int fd, bool act)
    {
        PollFd *pfd = find(&inactive, fd);
        if (pfd == 0)
            pfd = find(&active, fd);
        if (pfd)
        {
            if (act)
                pfd->event_mask |= FD_READ | FD_ACCEPT;
            else
                pfd->event_mask &= ~(FD_READ | FD_ACCEPT);
            if ((pfd->ignored_event_mask & pfd->event_mask) == 0)
                pfd->ignored_event_mask = 0;
            rebuild.store (1);
            return true;
        }
        return false;
    }

    bool is_read(int fd)
    {
        PollFd *wfd = find(&active, fd);
        if (wfd)
            return  (wfd->events.lNetworkEvents & (FD_READ | FD_ACCEPT)) != 0;
        return false;
    }
    bool is_write(int fd)
    {
        PollFd *wfd = find(&active, fd);
        if (wfd)
            return (wfd->events.lNetworkEvents & FD_WRITE) != 0;
        return false;
    }
    bool is_error(int fd)
    {
        PollFd *wfd = find(&active, fd);
        if (wfd)
            return (wfd->events.iErrorCode[FD_CLOSE_BIT] != 0) ||
                    (wfd->events.iErrorCode[FD_READ_BIT] != 0) ||
                    (wfd->events.iErrorCode[FD_WRITE_BIT] != 0) ||
                    (wfd->events.iErrorCode[FD_ACCEPT_BIT] != 0) ||
                    (wfd->events.iErrorCode[FD_CONNECT_BIT] != 0);
        return false;
    }
    bool is_close(int fd)
    {
        PollFd *wfd = find(&active, fd);
        if (wfd)
            return (wfd->events.lNetworkEvents & FD_CLOSE) != 0;
        return false;
    }

    bool build()
    {
        if (rebuild.compare_exchange (1, 0))
        {
            mutex.lock();
            if (active_event.is_empty())
                active_event.append(wakeup_event);

            foreach (const PollFd &wfd, inactive)
            {
                if (wfd.ignored_event_mask == 0)
                {
                    active.append(wfd);
                    active_event.append(wfd.hd);

                    int ret = WSAEventSelect (wfd.fd, wfd.hd, wfd.event_mask);
                    if (kyUnLikely (ret != 0))
                    {
                        errno = error_to_errno (WSAGetLastError ());
                        mutex.unlock();
                        return false;
                    }
                }
                else
                    active_ignored.append(wfd);
            }
            inactive.clear();
            mutex.unlock ();
        }
        return true;
    }
    bool build_reset()
    {
        mutex.lock();
        inactive.append(active);
        inactive.append(active_ignored);
        active.clear();
        active_event.clear();
        active_ignored.clear();

        if (active_event.is_empty())
            active_event.append(wakeup_event);

        foreach (const PollFd &wfd, inactive)
        {
            if (wfd.ignored_event_mask == 0)
            {
                active.append(wfd);
                active_event.append(wfd.hd);

                int ret = WSAEventSelect (wfd.fd, wfd.hd, wfd.event_mask);
                if (kyUnLikely (ret != 0))
                {
                    errno = error_to_errno (WSAGetLastError ());
                    mutex.unlock();
                    return false;
                }
            }
            else
                active_ignored.append(wfd);
        }
        inactive.clear();
        mutex.unlock ();
        rebuild.compare_exchange (1, 0);
        return true;
    }

    DWORD wait(int64 timeout)
    {
        int ignore_count = active_ignored.count();
        DWORD wait_ret, res;

        if (kyLikely (ignore_count == 0))
        {
            if (timeout == -1)
                timeout = INFINITE;
        }
        else
            timeout = 0;

        if (active_event.count() != 0)
        {
            wait_ret = WSAWaitForMultipleEvents (active_event.count(),
                                                 (HANDLE *)active_event.data(),
                                                 FALSE, timeout, FALSE);
        }
        else
        {
            wait_ret = WSA_WAIT_FAILED;
            WSASetLastError (WSA_INVALID_PARAMETER);
        }

        if (ignore_count == 0 && wait_ret == WSA_WAIT_TIMEOUT)
            res = 0;
        else if (wait_ret == WSA_WAIT_FAILED)
        {
            res = -1;
            errno = error_to_errno (WSAGetLastError ());
        }
        else
        {
            if (wait_ret - WSA_WAIT_EVENT_0 >= 1)
                res = collect_events ();
            else
                res = 1;
        }
        return res;
    }

    int collect_events ()
    {
        int res = 0;

        foreach (PollFd &var, active)
        {
            DWORD wait_ret = WaitForSingleObject (var.hd, 0);
            if (wait_ret == WAIT_OBJECT_0)
            {
                int enum_ret = WSAEnumNetworkEvents (var.fd, var.hd, &var.events);

                if (kyUnLikely (enum_ret != 0))
                {
                    res = -1;
                    errno = error_to_errno (WSAGetLastError ());
                    break;
                }

                res++;
            }
            else
                memset (&var.events, 0, sizeof (var.events));
        }

        if (res >= 0)
        {
            res += active_ignored.count();
            foreach (PollFd &var, active_ignored)
                var.ignored_event_mask = 0;

            active_ignored.clear();
        }

        return res;
    }

    static void format_last_error (char * buf, int64 buf_len)
    {
        DWORD flags = FORMAT_MESSAGE_FROM_SYSTEM;
        LPCVOID src = NULL;
        DWORD lang = 0;
        DWORD id;
        id = GetLastError ();
        FormatMessageA (flags, src, id, lang, buf, (DWORD) buf_len, NULL);
        SetLastError (id);
    }

    static int error_to_errno (DWORD last_error)
    {
        switch (last_error)
        {
        case WSA_INVALID_HANDLE:
        case WSAEINVAL:
        case WSAENOTSOCK:
            return EBADF;

        case WSA_NOT_ENOUGH_MEMORY:
            return ENOMEM;

            /*
           * Anything else, including:
           *   WSA_INVALID_PARAMETER, WSAEFAULT, WSAEINPROGRESS, WSAENETDOWN,
           *   WSANOTINITIALISED
           */
        default:
            return EINVAL;
        }
    }

};
}

ky_poll::ky_poll():
    impl(kyNew(impl::poll_core))
{
    impl->mutex.lock ();
    if (!impl->init (controllable))
    {

    }
    impl->mutex.unlock ();
}
ky_poll::~ky_poll()
{
    impl->uninit ();
    kyDelete(impl);
}

bool ky_poll::append (int fd)
{
    if  (fd < 0)
        return false;

    impl->mutex.lock ();
    impl->add (fd);
    impl->mutex.unlock ();

    return true;
}

bool ky_poll::remove (int fd)
{
    if (fd < 0)
        return false;

    impl->mutex.lock();
    impl->del(fd);
    impl->mutex.unlock ();

    return true;
}

bool ky_poll::can_exit()
{
    impl->mutex.lock ();
    int co = impl->inactive.count ();
    int ci = impl->active.count ();
    impl->mutex.unlock ();
    return (ci + co) > 1;
}

bool ky_poll::ctl_write (int fd, bool active)
{
    if (fd < 0)
        return false;

    impl->mutex.lock();
    impl->mod_write(fd, active);
    impl->mutex.unlock ();

    return true;
}

bool ky_poll::ctl_read (int fd, bool active)
{
    if (fd < 0)
        return false;

    impl->mutex.lock();
    int ret = impl->mod_read (fd, active);
    impl->mutex.unlock ();

    return ret;
}

void ky_poll::ignored (int fd)
{
    if (fd < 0)
        return ;

    impl->mutex.lock ();

    PollFd *pfd = impl->find(&impl->inactive, fd);
    if (pfd == 0)
        impl->find(&impl->active, fd);
    if (pfd)
    {
        pfd->ignored_event_mask = pfd->event_mask & (FD_READ | FD_WRITE);
        impl->rebuild.store (1);
    }

    impl->mutex.unlock ();
}

bool ky_poll::can_close (int fd)
{
    if (fd < 0)
        return false;

    bool res = false;
    impl->mutex.lock ();
    res = impl->is_close(fd);
    impl->mutex.unlock ();

    return res;
}

bool ky_poll::can_error (int fd)
{
    if (fd < 0)
        return false;

    bool res = false;
    impl->mutex.lock ();

    res = impl->is_error(fd);
    impl->mutex.unlock ();

    return res;
}

bool ky_poll::can_read (int fd)
{
    if (fd < 0)
        return false;

    bool res = false;

    impl->mutex.lock ();
    res = impl->is_read (fd);
    impl->mutex.unlock ();

    return res;
}

bool ky_poll::can_write (int fd)
{
    if (fd < 0)
        return false;

    bool res = false;
    impl->mutex.lock ();

    res = impl->is_write(fd);
    impl->mutex.unlock ();

    return res;
}

bool ky_poll::controllable (bool ctl)
{
    if (impl->timer)
        return false;

  impl->controllable = ctl;

  return true;
}

void ky_poll::restart ()
{
    if (impl->timer)
        return ;

    if (impl->controllable && impl->waiting.load () > 0)
        impl->raise_wakeup ();
}

void ky_poll::flushing (bool flush)
{
    if (impl->timer)
        return ;

    impl->flushing.store (flush);

    if (flush && impl->controllable && impl->waiting.load () > 0)
        impl->raise_wakeup ();
}

bool ky_poll::wakeup ()
{
  if (!impl->timer)
      return false;

  return impl->raise_wakeup ();
}

bool ky_poll::release ()
{
  if (!impl->timer)
      return false;

  return impl->release_wakeup ();
}
bool ky_poll::is_wakeup()
{
    return false;
}

int ky_poll::wait (int64 timeout)
{
    bool restarting = false;
    int res = -1;

    const bool is_timer = impl->timer;
    const int old_waiting = impl->waiting.fetch_add (1);

    /* 不能在多个线程等待，除非是一个计时器 */
    if (kyUnLikely (old_waiting > 0 && !is_timer))
    {
        //log_err ("we are already waiting");
        --impl->waiting;
        return -2;
    }

    /* 立即退出*/
    if (kyUnLikely (impl->flushing.load ()))
    {
        //log_err ("we are flushing");
        --impl->waiting;
        return -1;
    }

    do
    {
        /* 我们需要重新建立轮询列表 */
        if (!impl->build())
        {
            //log_err (" winsock error");
            --impl->waiting;
            return -1;
        }

        res = impl->wait(timeout);

        if (!is_timer)
        {
            // 唤醒事件则进入重新等待
            if (impl->release_all_wakeup () > 0 && res == 1)
                restarting = true;
        }

        if (kyUnLikely (impl->flushing.load ()))
        {
            --impl->waiting;
            return -1;
        }

    } while (kyUnLikely (restarting));

    --impl->waiting;
    return res;
}


#endif
