#include "ky_poll.h"
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

//#if kyPlatformIsLinux

#  define _GNU_SOURCE 1
#  include <sys/poll.h>
#  include <sys/epoll.h>
#  include <sys/time.h>
#  include <sys/socket.h>

typedef struct pollfd PollFd;


bool operator == (const PollFd &p1, const PollFd &p2)
{
    return p1.fd == p2.fd;
}
bool operator == (const PollFd &p1, const int &fd)
{
    return p1.fd == fd;
}
bool operator == (const int &fd, const PollFd &p1)
{
    return p1.fd == fd;
}
namespace  impl
{

typedef enum
{
#if kyOSIsWin32
    Mode_Windows,
#else
    Mode_Select,
    Mode_PSelect,
    Mode_Poll,
    Mode_PPoll,
    Mode_EPoll,
#endif
    Mode_Auto
} ePollModes;

struct poll_core
{
    ePollModes        mode;            ///< 轮询模式
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

    virtual ~poll_core(){}

    //!
    //! \brief wake_event  唤醒事件
    //! \return
    //!
    virtual bool wake_event () = 0;
    //!
    //! \brief release_event  释放事件
    //! \return
    //!
    virtual bool release_event () = 0;

    virtual void add(int fd) = 0;
    virtual void del(int fd) = 0;
    virtual bool mod_write(int fd, bool active) = 0;
    virtual bool mod_read(int fd, bool active) = 0;
    virtual bool is_read(int fd) = 0;
    virtual bool is_write(int fd) = 0;
    virtual bool is_error(int fd) = 0;
    virtual bool is_close(int fd) = 0;

    //!
    //! \brief init 初始化轮询核心
    //! \param ctl
    //! \return
    //!
    virtual bool init (bool ctl)
    {
        mode = Mode_Auto;
        rebuild.store (1);

        controllable = ctl;
        control_pending = 0;
        if (ctl)
            timer = true;
        return true;
    }
    //!
    //! \brief uninit
    //!
    virtual void uninit()
    {

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

};

//#if kyPlatformIsWin32

struct poll_win32 : poll_core
{
    ky_vector<PollFd>  active_ignored;
    ky_vector<HANDLE>  active_event;
    ky_vector<HANDLE>  event;
    HANDLE             wakeup_event;

    virtual bool init (bool ctl)
    {
        mode = Mode_Windows;
        wakeup_event = CreateEvent (NULL, TRUE, FALSE, NULL);
        return poll_core::init (ctl) && wakeup_event;
    }
    virtual void uninit ()
    {
        CloseHandle (wakeup_event);

        for (fdMap_i i = active.begin(); i != active.end(); ++i)
        {
            const PollFd &wfd = i.value();

            WSAEventSelect (wfd.fd, wfd.hd, 0);
            CloseHandle (wfd.hd);
        }
        for (fdMap_i i = inactive.begin(); i != inactive.end(); ++i)
        {
            const PollFd &wfd = i.value();

            WSAEventSelect (wfd.fd, wfd.hd, 0);
            CloseHandle (wfd.hd);
        }
        for (fdMap_i i = inremove.begin(); i != inremove.end(); ++i)
        {
            const PollFd &wfd = i.value();

            WSAEventSelect (wfd.fd, wfd.hd, 0);
            CloseHandle (wfd.hd);
        }

        active.clear();
        inactive.clear();
        inremove.clear();

        poll_core::uninit ();
    }

    virtual bool wake_event ()
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

    virtual bool release_event ()
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

    virtual void add(int fd)
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
    virtual void del(int fd)
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

        if (pfd = find(&active, fd, &inx))
        {
            inremove.append(*pfd);
            active.remove(inx);
            rebuild.store(1);
        }
    }
    virtual bool mod_write(int fd, bool active)
    {
        PollFd *pfd = find(&inactive, fd);
        if (pfd == 0)
            pfd = find(&active, fd);
        if (pfd)
        {
            if (active)
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
    virtual bool mod_read(int fd, bool active)
    {
        PollFd *pfd = find(&inactive, fd);
        if (pfd == 0)
            pfd = find(&active, fd);
        if (pfd)
        {
            if (active)
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

    virtual bool is_read(int fd)
    {
        if (PollFd *wfd = find(&active, fd))
            return  (wfd->events.lNetworkEvents & (FD_READ | FD_ACCEPT)) != 0;
        return false;
    }
    virtual bool is_write(int fd)
    {
        if (PollFd *wfd = find(&active, fd))
            return (wfd->events.lNetworkEvents & FD_WRITE) != 0;
        return false;
    }
    virtual bool is_error(int fd)
    {
        if (PollFd *wfd = find(&active, fd))
            return (wfd->events.iErrorCode[FD_CLOSE_BIT] != 0) ||
                    (wfd->events.iErrorCode[FD_READ_BIT] != 0) ||
                    (wfd->events.iErrorCode[FD_WRITE_BIT] != 0) ||
                    (wfd->events.iErrorCode[FD_ACCEPT_BIT] != 0) ||
                    (wfd->events.iErrorCode[FD_CONNECT_BIT] != 0);
        return false;
    }
    virtual bool is_close(int fd)
    {
        if (PollFd *wfd = find(&active, fd))
            return (wfd->events.lNetworkEvents & FD_CLOSE) != 0;
        return false;
    }

    bool prepare_active_sets ()
    {
        active.clear ();
        active_ignored.clear();

        for (fdMap_i i = inactive.begin(); i != inactive.end(); ++i)
        {
            PollFd &wfd = i.value();

            if (wfd.ignored_event_mask == 0)
            {
                active[wfd.fd] = wfd;

                int ret = WSAEventSelect (wfd.fd, wfd.hd, wfd.event_mask);
                if (kyUnLikely (ret != 0))
                {
                    errno = error_to_errno (WSAGetLastError ());
                    return false;
                }
            }
            else
                active_ignored.append(*wfd);
        }

        return true;
    }

    int collect_events ()
    {
        int res = 0;

        for (fdMap_i i = active.begin(); i != active.end(); ++i)
        {
            PollFd &wfd = active[i];

            DWORD wait_ret = WaitForSingleObject (wfd.hd, 0);
            if (wait_ret == WAIT_OBJECT_0)
            {
                int enum_ret = WSAEnumNetworkEvents (wfd.fd, wfd.hd, &wfd.events);

                if (kyUnLikely (enum_ret != 0))
                {
                    res = -1;
                    errno = error_to_errno (WSAGetLastError ());
                    break;
                }

                res++;
            }
            else
                memset (&wfd.events, 0, sizeof (wfd.events));
        }

        if (res >= 0)
        {
            res += active_ignored.count();
            for (ky_vector<PollFd>::iterator p = active_ignored.begin ();
                 p != active_ignored.end (); ++p)
            {
                p->ignored_event_mask = 0;
            }

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

#  define impl_as(impl) ((impl::poll_win32 *)impl)


//#elif kyPlatformIsLinux

struct poll_linux : poll_core
{
    int        control_read;
    int        control_write;
    int        epoll;
    virtual ~poll_linux(){}
    virtual bool init (bool ctl)
    {
        control_read = -1;
        control_write = -1;
        epoll = -1;

        int control_sock[2];

        if (socketpair (PF_UNIX, SOCK_STREAM, 0, control_sock) >= 0)
        {
            control_read = control_sock[0];
            control_write = control_sock[1];

            add_fd (control_read);
            ctl_read (control_read, true);
        }
        //else log_err ("can't create socket pair !");
        poll_core::init (ctl);
        mode = choose_mode (-1);
#  if defined(kyHasEPoll)
        epoll = epoll_create(1);
        if (epoll < 0)
        {
            perror ("epoll_create ");
            mode = Mode_Poll;
        }
#  endif
        return (control_read != -1) && (control_write != -1);
    }
    virtual void uninit ()
    {
        if (control_write >= 0)
            ::close (control_write);
        if (control_read >= 0)
            ::close (control_read);
        if (epoll >=0 )
            ::close(epoll);
        poll_core::uninit ();
    }

    virtual void add(int fd)
    {
        // 已经存在
        if ((find(&inactive, fd) != 0) || (find(&active, fd) != 0))
            return ;

        // 加入fd
        PollFd nfd;
        nfd.fd = fd;
        nfd.events = POLLERR | POLLNVAL | POLLHUP;
        nfd.revents = 0;
        inactive.append (nfd);

        // 需要从新建立
        rebuild.store (1);
    }
    virtual void del(int fd)
    {
        int inx = -1;
        PollFd *pfd = find(&inactive, fd, &inx);
        if (pfd)
        {
            inactive.remove(inx);
            return ;
        }

        if (pfd = find(&active, fd, &inx))
        {
            inremove.append(*pfd);
            active.remove(inx);
            rebuild.store(1);
        }
    }
    virtual bool mod_write(int fd, bool active)
    {
        PollFd *pfd = find(&inactive, fd);
        if (pfd == 0)
            pfd = find(&active, fd);
        if (pfd)
        {
            if (active)
                pfd->events |= POLLOUT;
            else
                pfd->events &= ~POLLOUT;
            rebuild.store (1);
            return true;
        }
        return false;
    }
    virtual bool mod_read(int fd, bool active)
    {
        PollFd *pfd = find(&inactive, fd);
        if (pfd == 0)
            pfd = find(&active, fd);
        if (pfd)
        {
            if (active)
                pfd->events |= (POLLIN | POLLPRI);
            else
                pfd->events &= ~(POLLIN | POLLPRI);
            rebuild.store (1);
            return true;
        }
        return false;
    }

    // 非epoll模式检测fd是否可读
    virtual bool is_read(int fd)
    {
        if (PollFd *pfd = find(&active, fd))
            return (pfd->revents & (POLLIN | POLLPRI)) != 0;
        return false;
    }
    virtual bool is_write(int fd)
    {
        if (PollFd *pfd = find(&active, fd))
            return (pfd->revents & POLLOUT) != 0;
        return false;
    }
    virtual bool is_error(int fd)
    {
        if (PollFd *pfd = find(&active, fd))
            return (pfd->revents & (POLLERR | POLLNVAL)) != 0;
        return false;
    }
    virtual bool is_close(int fd)
    {
        if (PollFd *pfd = find(&active, fd))
            return (pfd->revents & POLLHUP) != 0;
        return false;
    }

    // 非epoll模式的fd转换
    int pollfd_conv_fd (fd_set * readfds, fd_set * writefds, fd_set * errorfds)
    {
        int max_fd = -1;

        FD_ZERO (readfds);
        FD_ZERO (writefds);
        FD_ZERO (errorfds);

        for (int i = 0; i < active.count (); ++i)
        {
            PollFd *pfd = &active[i];
            if (pfd->fd < FD_SETSIZE)
            {
                if (pfd->events & POLLIN)
                    FD_SET (pfd->fd, readfds);
                if (pfd->events & POLLOUT)
                    FD_SET (pfd->fd, writefds);
                if (pfd->events)
                    FD_SET (pfd->fd, errorfds);
                if (pfd->fd > max_fd && (pfd->events & (POLLIN | POLLOUT)))
                    max_fd = pfd->fd;
            }
        }

        return max_fd;
    }
    // 非epoll模式的fd转换,是否可读已经设置
    void fd_conv_pollfd (fd_set * readfds, fd_set * writefds, fd_set * errorfds)
    {
        for (int i = 0; i < active.count (); ++i)
        {
            PollFd *pfd = &active[i];
            if (pfd->fd < FD_SETSIZE)
            {
                pfd->revents = 0;
                if (FD_ISSET (pfd->fd, readfds))
                    pfd->revents |= POLLIN;
                if (FD_ISSET (pfd->fd, writefds))
                    pfd->revents |= POLLOUT;
                if (FD_ISSET (pfd->fd, errorfds))
                    pfd->revents |= POLLERR;
            }
        }
    }

    // 选择linux最优的轮询模式
    ePollModes choose_mode (int64_t timeout)
    {
        if (mode == Mode_Auto)
        {
#  if defined(kyHasEPoll)
            return Mode_EPoll;
#  elif defined(kyHasPPoll)
            return Mode_PPoll;
#  elif defined(kyHasPoll)
            if (!selectable_fds () || pollable_timeout (timeout))
                return Mode_Poll;
            else
            {
#    if defined(kyHasPSelect)
                return Mode_PSelect;
#    else
                return Mode_Select;
#    endif
            }
#  elif defined(kyHasPSelect)
            return Mode_PSelect;
#  else
            return Mode_Select;
#  endif
        }
        return mode;
    }

    // 继承实现
    bool wake_event ()
    {
        ssize_t num_written;
        while ((num_written = ::write (control_write, "W", 1)) != 1)
        {
            if (num_written == -1 && errno != EAGAIN && errno != EINTR)
                return false;
        }
        return true;
    }
    bool release_event ()
    {
        char buf[1] = {'\0'};
        ssize_t num_read;
        while ((num_read = ::read (control_read, buf, 1)) != 1)
        {
            if (num_read == -1 && errno != EAGAIN && errno != EINTR)
                return false;
        }
        return true;
    }

#if !defined(kyHasPPoll) && defined(kyHasPoll)
    bool selectable_fds ()
    {
        mutex.lock();
        for (int i = 0; i< inactive.count (); ++i)
        {
            PollFd *pfd = &active[i];
            if (pfd->pfd.fd >= FD_SETSIZE)
            {
                mutex.unlock();
                return false;
            }
        }
        mutex.unlock();
        return true;
    }

    bool pollable_timeout (int64_t timeout)
    {
        if (timeout == -1)
            return true;

        /* not a nice multiple of milliseconds */
        if (timeout % 1000000)
            return false;

        return true;
    }
#endif

};

#  define impl_as(impl) ((impl::poll_linux *)impl)

#endif

}

#define dptr (impl_as(impl))


ky_poll::ky_poll(bool controllable):
    impl(0)
{
    impl =
#  if kyPlatformIsWin32
            kyNew (impl::poll_win32);
#  elif kyPlatformIsLinux
            kyNew (impl::poll_linux);
#  endif

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
    bool ret = dptr->add (fd);
    impl->mutex.unlock ();

    return ret;
}

bool ky_poll::remove (int fd)
{
    if (fd < 0)
        return false;

    impl->mutex.lock();
    dptr->del(fd);
    impl->mutex.unlock ();

    return true;
}

bool ky_poll::can_exit()
{
    impl->mutex.lock ();
    int co = dptr->inactive.count ();
    int ci = dptr->active.count ();
    impl->mutex.unlock ();
    return (ci + co) > 1;
}

bool ky_poll::ctl_write (int fd, bool active)
{
    if (fd < 0)
        return false;

    impl->mutex.lock();
    dptr->mod_write(fd, active);
    impl->mutex.unlock ();

    return true;
}

bool ky_poll::ctl_read (int fd, bool active)
{
    if (fd < 0)
        return false;

    impl->mutex.lock();
    int ret = dptr->ctl_read (fd, active);
    impl->mutex.unlock ();

    return ret;
}

void ky_poll::ignored (int fd)
{
#if kyPlatformIsWin32
    if (fd < 0)
        return ;

    impl->mutex.lock ();

    PollFd *pfd = dptr->find(&dptr->inactive, fd);
    if (pfd == 0)
        dptr->find(&dptr->active, fd);
    if (pfd)
    {
        pfd->ignored_event_mask = pfd->event_mask & (FD_READ | FD_WRITE);
        impl->rebuild.store (1);
    }

    impl->mutex.unlock ();
#endif
}

bool ky_poll::can_close (int fd)
{
    if (fd < 0)
        return false;

    bool res = false;
    impl->mutex.lock ();
    res = dptr->is_close(fd);
    impl->mutex.unlock ();

    return res;
}

bool ky_poll::can_error (int fd)
{
    if (fd < 0)
        return false;

    bool res = false;
    impl->mutex.lock ();

    res = dptr->is_error(fd);
    impl->mutex.unlock ();

    return res;
}

bool ky_poll::can_read (int fd)
{
    if (fd < 0)
        return false;

    bool res = false;

    impl->mutex.lock ();
    res = dptr->can_read (fd);
    impl->mutex.unlock ();

    return res;
}

bool ky_poll::can_write (int fd)
{
    if (fd < 0)
        return false;

    bool res = false;
    impl->mutex.lock ();

    res = dptr->is_write(fd);
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

int ky_poll::wait (int64 timeout)
{
    bool restarting = false;
    int res = -1;

    const bool is_timer = impl->timer;
    const int old_waiting = impl->waiting.fetch_add (1);

    /* 我们不能等待多个线程，除非我们是一个计时器 */
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
        if (impl->rebuild.compare_exchange (1, 0))
        {
            impl->mutex.lock();
#  if kyPlatformIsLinux
            if (impl->mode == impl::Mode_EPoll)
            {
                // 删除处理
                foreach (const PollFd &var, dptr->inremove)
                {
                    epoll_event ev;
                    epoll_ctl (dptr->epoll, EPOLL_CTL_DEL, var.fd, &ev);
                }
                dptr->inremove.clear ();

                // 添加或修改处理
                foreach (const PollFd &var, dptr->inactive)
                {
                    epoll_event ev;
                    ev.data.fd = var.fd;
                    ev.events = (var.events & POLLERR) || (var.events & POLLNVAL) ?
                                (EPOLLERR/* | EPOLLPRI*/): 0;
                    ev.events |= (var.events & POLLIN) ? EPOLLIN : 0;
                    ev.events |= (var.events & POLLOUT)? EPOLLOUT : 0;
                    ev.events |= (EPOLLET | EPOLLHUP);

                    PollFd *pfd = dptr->find (&active, var.fd);
                    if (pfd)
                    {
                        epoll_ctl (dptr->epoll, EPOLL_CTL_MOD, pfd->fd, &ev);
                        *pfd = var;
                    }
                    else
                    {
                        epoll_ctl (dptr->epoll, EPOLL_CTL_ADD, var.fd, &ev);
                        dptr->active.append (var);
                    }
                }
            }
            else
                dptr->active.append (dptr->inactive);
#  else
            if (!dptr->prepare_active_sets ())
            {
                //log_err (" winsock error");
                impl->mutex.unlock ();
                --impl->waiting;
                return -1;
            }
#  endif
            dptr->inactive.clear ();
            impl->mutex.unlock ();
        }

#if kyPlatformIsLinux
        switch (impl->mode)
        {
        case impl::Mode_Auto:
            //log_err("ky_poll::wait No polling mode!");
            break;
        case impl::Mode_EPoll:
        {
            int fd_size = dptr->active.count ();
            struct epoll_event events[fd_size];
            res = ::epoll_wait(dptr->epoll, events, fd_size, timeout);

            for (int i = 0; i < res; ++i)
            {
                PollFd nfd;
                nfd.fd = events[i].data.fd;
                int idx = dptr->active.find (nfd);
                if (idx >= 0)
                {
                    PollFd *fd = &dptr->active[idx];
                    fd->revents = (events[i].events & EPOLLIN) ? POLLIN : 0;
                    fd->revents |= (events[i].events & EPOLLOUT) ? POLLOUT : 0;
                    fd->revents |= (events[i].events & EPOLLERR) ? POLLERR : 0;
                }
            }
        }
            break;
        case impl::Mode_PPoll:
        {
#  ifdef kyHasPPoll
            struct timespec ts;
            struct timespec *tsptr = 0;

            if (timeout != -1)
            {
                ts.tv_sec  =  (long) ((timeout) / 1000);
                ts.tv_nsec = (long) (((timeout) - ts.tv_sec * 1000) * 1000000);
                tsptr = &ts;
            }

            res = ::ppoll ((struct pollfd *)dptr->active.data (),
                           dptr->active.count (), tsptr, 0);
#  endif
            break;
        }
        case impl::Mode_Poll:
        {
#ifdef kyHasPoll

            res = ::poll ((struct pollfd *) dptr->active.data (),
                          dptr->active.count (), timeout);
#endif
            break;
        }
        case impl::Mode_PSelect:
#ifndef kyHasPSelect
            break;
#endif
        case impl::Mode_Select:
        {
#if !kyPlatformIsWin32
            fd_set readfds;
            fd_set writefds;
            fd_set errorfds;

            int max_fd = dptr->pollfd_conv_fd (&readfds, &writefds, &errorfds);

            if (impl->mode == impl::Mode_Select)
            {
                struct timeval tv;
                struct timeval *tvptr = 0;

                if (timeout != -1)
                {
                    tv.tv_sec  = (long) (timeout / 1000);
                    tv.tv_usec = (long) ((timeout - (tv.tv_sec) * 1000) * 1000);
                    tvptr = &tv;
                }

                res = ::select (max_fd + 1, &readfds, &writefds, &errorfds, tvptr);
            }
            else
            {
#ifdef kyHasPSelect
                struct timespec ts;
                struct timespec *tsptr = 0;

                if (timeout != -1)
                {
                    ts.tv_sec  =  (long) ((timeout) / 1000);
                    ts.tv_nsec = (long) (((timeout) - ts.tv_sec * 1000) * 1000000);
                    tsptr = &ts;
                }

                res = ::pselect (max_fd + 1, &readfds, &writefds, &errorfds, tsptr, 0);
#endif
            }

            if (res >= 0)
                dptr->fd_conv_pollfd (&readfds, &writefds, &errorfds);
#else /* WIN32 */
            errno = ENOSYS;
#endif
            break;
        }

            break;
        }

#elif kyPlatformIsWin32
        int ignore_count = dptr->active_ignored.count();
        DWORD t, wait_ret;

        if (kyLikely (ignore_count == 0))
        {
            if (timeout == -1)
                timeout = INFINITE;
        }
        else
            timeout = 0;

        if (dptr->active_events.count() != 0)
        {
            wait_ret = WSAWaitForMultipleEvents (dptr->active_events.count(),
                                                 (HANDLE *)dptr->active_events.data(), FALSE, t, FALSE);
        }
        else
        {
            wait_ret = WSA_WAIT_FAILED;
            WSASetLastError (WSA_INVALID_PARAMETER);
        }

        if (ignore_count == 0 && wait_ret == WSA_WAIT_TIMEOUT)
        {
            res = 0;
        }
        else if (wait_ret == WSA_WAIT_FAILED)
        {
            res = -1;
            errno = dptr->error_to_errno (WSAGetLastError ());
        }
        else
        {
            /* the first entry is the wakeup event */
            if (wait_ret - WSA_WAIT_EVENT_0 >= 1)
            {
                res = dptr->collect_events ();
            }
            else
            {
                res = 1;            /* wakeup event */
            }
        }

#endif

        if (!is_timer)
        {
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
