
#include "ky_lock.h"
#include "ky_vector.h"
#include "ky_map.h"
#include "ky_queue.h"
#include "ky_debug.h"

#include "event_poll.h"
#include "posix_fd.h"
#include "signal_posix.h"
#include "timer_posix.h"
#include "pipe_posix.h"

#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

#if kyOSIsLinux

#  define _GNU_SOURCE 1
#  include <sys/poll.h>
#  include <sys/epoll.h>
#  include <sys/time.h>
#  include <sys/socket.h>
#  include <sys/timerfd.h>
#  include <sys/eventfd.h>
#  include <sys/resource.h>

//!
//! \brief system_limit 当前系统的句柄限制
//! \return
//!
int64 event_poll::system_limit()
{
    static struct rlimit rbuf;
    if(::getrlimit(RLIMIT_NOFILE, &rbuf) < 0)
        return -1;
    return rbuf.rlim_cur;
}
//!
//! \brief hard_limit 本机硬件句柄限制
//! \return
//!
int64 event_poll::hard_limit()
{
    static struct rlimit rbuf;
    if(::getrlimit(RLIMIT_NOFILE, &rbuf) < 0)
        return -1;
    return rbuf.rlim_max;
}

typedef enum
{
    Mode_Select,
    Mode_PSelect,
    Mode_Poll,
    Mode_PPoll,
    Mode_EPoll,
    Mode_Auto
} ePollModes;

struct poll_priv
{
    ePollModes        mode;            ///< 轮询模式
    ky_mutex          mutex;           ///< 轮询锁

    ky_atomic<int>    always;
    ky_atomic<int>    rebuild;         ///< 是否需要重新激活轮询
    ky_atomic<int>    pending;         ///< 唤醒记录
    ky_atomic<int>    waiting;         ///< 等待数量
    ky_atomic<int>    flushing;        ///< 完成退出

    ky_map<intptr, posix_fd*> inactive;   ///< 未激活列表
    ky_list<posix_fd*>   active;         ///< 被激活的轮询
    ky_list<posix_fd*>   actalw;         ///< 总是激活列表
    ky_list<posix_fd*>   inremove;       ///< 需要被删除的轮询
    ky_queue<posix_fd>   wake;           ///< 被唤醒列表
    ky_vector<PollFd>  inl_active;     ///< 用于存储激活的轮询对象
    ky_thread         *inl_thread;     ///< 所属线程

    pipe_posix        *pipe;
    int                epoll;

    poll_priv():
        mode(Mode_Auto),
        mutex(),
        always(0),
        rebuild(1),
        pending(0),
        waiting(0),
        flushing(0),
        inactive(),
        active(),
        actalw(),
        inremove(),
        wake(),
        inl_thread(ky_thread::current()),
        pipe(kyNew(pipe_posix())),
        epoll(-1)
    {
        if (pipe->is_valid())
        {
            inactive[pipe->get()] = pipe;
            mode = choose_mode (-1);

#  if defined(kyHasEPoll)
            if (mode == Mode_EPoll)
            {
                epoll = epoll_create1(EPOLL_CLOEXEC);
                //epoll = epoll_create(kyMode_Auto_event_poll::system_limit());
                if (epoll < 0)
                {
                    log_warn ("epoll_create fail!");
                    mode = Mode_PPoll;
                }
            }
#  endif
        }
        else
            log_err ("can't create pipe!");
    }

    virtual ~poll_priv()
    {
        if ((mode == Mode_EPoll) && (epoll >=0 ))
            ::close(epoll);

        foreach (posix_fd *fd, inactive)
            kyDelete(fd);
    }

    void wait_prepare()
    {
        // 删除句柄
        foreach (posix_fd *var, inremove)
        {
            // 删除socket
            if ((var->ion & Notify_Socket) && (mode == Mode_EPoll))
            {
                epoll_event ev;
                epoll_ctl (epoll, EPOLL_CTL_DEL, var->hd.fd, &ev);
            }

            // 在列表中将内存删除
            inactive.remove(var->get());
            kyDelete(var);
        }

        // 将删除列表清空
        inremove.clear();
        active.clear ();
        actalw.clear ();
        inl_active.clear ();
        always.store (0);

        // 处理套接字关联
        ky_map<intptr, posix_fd*>::iterator ite = inactive.begin();
        for (;ite != inactive.end(); ++ite)
        {
            posix_fd* pt = *ite;

            // 设置为非阻塞
            const int old_flags = ::fcntl(pt->get(), F_GETFL);
            ::fcntl(pt->get(), F_SETFL, old_flags | O_NONBLOCK);

            if (!(pt->ion & Notify_Always))
                pt->index = active.count();

            if (pt->ion & Notify_Always)
            {
                pt->always = actalw.count();
                actalw.append(pt);
                always.increase();
            }
            // 不为消息对象，并且未被激活[修改也是如此]
            else if (pt->ion & Notify_Socket)
            {
                pt->hd.events = 0;
                pt->hd.revents = 0;
                if (pt->ion & Notify_Close)
                    pt->hd.events |= POLLHUP;//POLLRDHUP;
                if ((pt->ion & Notify_Read) || (pt->ion & Notify_Accept))
                    pt->hd.events |= (POLLIN | POLLPRI/*带外数据*/);
                if (pt->ion & Notify_Write)
                    pt->hd.events |= POLLOUT;

                if (mode == Mode_EPoll)
                {
                    epoll_event ev;
                    ky_memory::zero(&ev, sizeof(ev));
                    ev.data.fd = pt->hd.fd;
                    ev.events |= (/*EPOLLET | */EPOLLHUP | EPOLLERR | EPOLLONESHOT);

                    //EPOLLRDHUP ;-检测EPOLLRDHUP就可以知道是对方关闭(检测不到时读写会产生EPOLLERR)
                    if (pt->ion & Notify_Close)
                        ev.events |= EPOLLRDHUP;
                    if ((pt->ion & Notify_Read) || (pt->ion & Notify_Accept))
                        ev.events |= (EPOLLIN | EPOLLPRI/*带外数据*/);
                    if (pt->ion & Notify_Write)
                        ev.events |= EPOLLOUT;

                    if (pt->flag)
                        epoll_ctl (epoll, EPOLL_CTL_MOD, ev.data.fd, &ev);
                    else
                    {
                        pt->flag = true;

                        epoll_ctl (epoll, EPOLL_CTL_ADD, ev.data.fd, &ev);
                        active.append(pt);
                    }
                }
                else
                {
                    active.append(pt);
                    inl_active.append (pt->hd);
                }
            }
        }
    }

    int wait_epoll(int64 timeout)
    {
        const int fd_size = active.count ();
        struct epoll_event events[fd_size];
        ky_memory::zero(events, sizeof(events));
        int   wake_count = 0;
        const int res = ::epoll_wait(epoll, events, fd_size, timeout);

        for (int i = 0; i < res; ++i)
        {
            const int fd = events[i].data.fd;
            if (inactive.contains ((intptr)fd))
            {
                posix_fd* pt = inactive[(intptr)fd];

                pt->hd.revents = 0;
                pt->wake = 0;
                if (events[i].events & EPOLLIN)
                {
                    pt->hd.revents |= POLLIN;
                    pt->wake |= Notify_Read | Notify_Accept;
                }
                if (events[i].events & EPOLLPRI)
                {
                    pt->hd.revents |= POLLPRI;
                    pt->wake |= Notify_Read | Notify_Accept;
                }
                if (events[i].events & EPOLLOUT)
                {
                    pt->hd.revents |= POLLOUT;
                    pt->wake |= Notify_Write;
                }
                if (events[i].events & EPOLLERR)
                    pt->hd.revents |= POLLERR;
                if (events[i].events & EPOLLHUP)
                {
                    pt->hd.revents |= POLLHUP;
                    pt->wake |= Notify_Close;
                }
                if (events[i].events & EPOLLRDHUP)
                    pt->wake |= Notify_Close;

                pt->despatch();

                // 加入唤醒列表中
                wake.push(*pt);
                wake_count++;
            }
        }
        return wake_count;
    }

    bool is_wakeup()
    {
        if (pipe->is_active())
            return pipe->hd.revents & (POLLIN | POLLPRI);
        return false;
    }

    //!
    //! \brief wake_event  唤醒事件
    //! \return
    //!
    virtual bool wake_event ()
    {
        return pipe->active();
    }
    //!
    //! \brief release_event  释放事件
    //! \return
    //!
    virtual bool release_event ()
    {
        return pipe->cancel();
    }

    //!
    //! \brief raise_wakeup 提出唤醒
    //! \return
    //!
    bool raise_wakeup ()
    {
        bool result = true;

        // 未提出唤醒则提出唤醒
        if (pending.load () == 0)
            result = wake_event ();

        // 已经提出唤醒则将待唤醒数加一
        if (result)
            ++pending;

        return result;
    }
    //!
    //! \brief release_wakeup 释放唤醒
    //! \return
    //!
    bool release_wakeup ()
    {
        bool result = false;

        if (pending.load () > 0)
        {
            if (pending.load () == 1)
                result = release_event ();
            else
                result = true;

            if (result)
                --pending;
            else
                errno = EWOULDBLOCK;
        }

        return result;
    }

    //!
    //! \brief release_all_wakeup 释放所有唤醒
    //! \return
    //!
    int release_all_wakeup ()
    {
        int old = -1;

        if ((old = pending.load ()) > 0)
        {
            if (release_event ())
                pending.store (0);
            else
                old = 0;
        }

        return old;
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
            PollFd &pfd = (active[i])->hd;
            if (pfd.fd < FD_SETSIZE)
            {
                if (pfd.events & POLLIN)
                    FD_SET (pfd.fd, readfds);
                if (pfd.events & POLLOUT)
                    FD_SET (pfd.fd, writefds);
                if (pfd.events)
                    FD_SET (pfd.fd, errorfds);
                if (pfd.fd > max_fd && (pfd.events & (POLLIN | POLLOUT)))
                    max_fd = pfd.fd;
            }
        }

        return max_fd;
    }
    // 非epoll模式的fd转换,是否可读已经设置
    void fd_conv_pollfd (fd_set * readfds, fd_set * writefds, fd_set * errorfds)
    {
        for (int i = 0; i < active.count (); ++i)
        {
            PollFd &pfd = active[i]->hd;
            if (pfd.fd < FD_SETSIZE)
            {
                pfd.revents = 0;
                if (FD_ISSET (pfd.fd, readfds))
                    pfd.revents |= POLLIN;
                if (FD_ISSET (pfd.fd, writefds))
                    pfd.revents |= POLLOUT;
                if (FD_ISSET (pfd.fd, errorfds))
                    pfd.revents |= POLLERR;
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

event_poll::event_poll():
    priv(kyNew(poll_priv()))
{

}
event_poll::event_poll(ky_thread *self):
    priv(kyNew(poll_priv()))
{
    priv->inl_thread = self;
}
event_poll::~event_poll()
{
    kyDelete(priv);
}

int event_poll::registered (intptr fd, eNotifyFlags im)
{
    if (fd < 0 && (im & Notify_Socket))
    {
        log_err("event_poll: Can't register, fd error.");
        return -1;
    }
    if (ky_thread::current() != priv->inl_thread)
    {
        log_warn("event_poll: Not thread safe operation.");
        return -1;
    }
    // 已经注册，但是通知标志不同，则修改
    if ((im & Notify_Socket) && priv->inactive.contains(fd))
    {
        posix_fd *pt = priv->inactive[fd];
        if (pt->ion != im)
            pt->ion = im;
        wakeup();
        return pt->hd.fd;
    }

    posix_fd *pfd = 0;
    if (im & Notify_Timer)
    {
        timer_posix *timer = kyNew(timer_posix());
        timer->start((int64)fd);
        pfd = timer;
    }
    else if (im & Notify_Socket)
        pfd = kyNew(posix_fd(fd, im));
    else if (im & Notify_Signal)
    {
        signal_posix *signal = kyNew(signal_posix());
        signal->addset(fd);
        pfd = signal;
    }

    if (!pfd)
    {
        log_err("event_poll: Can't register.");
        return -1;
    }

    // 注册一个通知
    priv->inactive[pfd->get()] = pfd;
    priv->rebuild.store(1);
    wakeup();

    return pfd->get();
}

//!
//! \brief unregister 注销轮询的对象
//! \param id 句柄
//! \return
//!
bool event_poll::unregister (int id)
{
    if (id < 0)
        return false;
    if (ky_thread::current() != priv->inl_thread)
    {
        log_warn("event_poll: Not thread safe operation.");
        return false;
    }

    if (!priv->inactive.contains((intptr)id))
        return false;

    posix_fd *fd = priv->inactive.value((intptr)id);
    priv->inremove.append(fd);
    wakeup();
    return true;
}

//!
//! \brief modify 修改轮询对象唤醒标签
//! \param fd 句柄
//! \param im 唤醒的标签-eIoModes
//! \return
//!
bool event_poll::modify(int id, eNotifyFlags im, bool active)
{
    if (id < 0 || !(im & Notify_Io))
        return false;
    if (ky_thread::current() != priv->inl_thread)
    {
        log_warn("event_poll: Not thread safe operation.");
        return false;
    }

    if (!priv->inactive.contains((intptr)id))
        return false;
    posix_fd *pt = priv->inactive.value((intptr)id);

    if (pt && pt->get() == id)
    {
        if (!(pt->ion & Notify_Io))
            return false;

        if (active)
            pt->ion |= im;
        else
            pt->ion &= ~im;
        pt->flag = false;
        priv->rebuild.store(1);
        wakeup();
        return true;
    }
    return false;
}

//!
//! \brief can_exit 是否可以退出(无对象轮询)
//! \return
//!
bool event_poll::can_exit()
{
    return priv->active.is_empty() || (priv->flushing.load() > 0);
}

//!
//! \brief can_error 检测是否被错误唤醒
//! \param fd 句柄
//! \return
//!
bool event_poll::can_error (int id)
{
    foreach (posix_fd *pfd, priv->active)
    {
        if (id == pfd->hd.fd)
            return (pfd->hd.revents & (POLLERR | POLLNVAL)) != 0;
    }
    return false;
}

//!
//! \brief can_wake 是否被唤醒
//! \param fd 句柄
//! \param im 检测的唤醒标签-eIoModes
//! \return
//!
bool event_poll::can_wake(int id, eNotifyFlags im)
{
    foreach (posix_fd *var, priv->active)
    {
        if (var->hd.fd == id)
            return var->flag && (var->wake & im);
    }
    return false;
}

//!
//! \brief wait 等待fd唤醒活动,直至超时或退出
//! \param timeout 以毫秒为单位的超时。
//! \note 此函数只能是一次从一个线程调用
//! \return -1 error(flushing), -2 从多个线程调用.
//!        返回活动的数量
//!
int event_poll::wait (int64 timeout)
{
    const int old_waiting = priv->waiting.fetch_add (1);
    /* 我们不能等待多个线程 */
    if (kyUnLikely (old_waiting > 0))
    {
        log_err ("event_poll: we are already waiting");
        --priv->waiting;
        return -2;
    }
    /* 立即退出 */
    if (kyUnLikely (priv->flushing.load ()))
    {
        log_err ("event_poll: we are flushing");
        --priv->waiting;
        return -1;
    }

    priv->release_all_wakeup();

    // 重新构建
    if (priv->rebuild.compare_exchange (1, 0))
        priv->wait_prepare();

    // 若总是处理则将超时时间设置为0ms
    if (priv->always.load () > 0)
        timeout = 0;

    if (!priv->active.is_empty ())
    {
        switch ((int)priv->mode)
        {
        case Mode_Auto:
            log_err("event_poll::Wait no polling mode!");
            break;
        case Mode_EPoll:
        {
            priv->wait_epoll(timeout);
            break;
        }
        case Mode_PPoll:
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

            ::ppoll ((struct pollfd *)priv->inl_active.data (),
                     priv->inl_active.count (), tsptr, 0);
#  endif
            break;
        }
        case Mode_Poll:
        {
#ifdef kyHasPoll
            ::poll ((struct pollfd *) priv->inl_active.data (),
                    priv->inl_active.count (), timeout);
#endif
            break;
        }
        case Mode_PSelect:
#ifndef kyHasPSelect
            break;
#endif
        case Mode_Select:
        {
            fd_set readfds;
            fd_set writefds;
            fd_set errorfds;

            int res = 0;
            int max_fd = priv->pollfd_conv_fd (&readfds, &writefds, &errorfds);

            if (priv->mode == Mode_Select)
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
                priv->fd_conv_pollfd (&readfds, &writefds, &errorfds);

            break;
        }
        }

        if (priv->mode != Mode_EPoll)
        {
            foreach (const PollFd &var, priv->inl_active)
            {
                if (var.revents != 0 && priv->inactive.contains(var.fd))
                {
                    posix_fd *pt = priv->inactive[var.fd];
                    pt->hd.revents = var.revents;
                    if (var.revents & POLLIN)
                        pt->wake |= Notify_Read | Notify_Accept;
                    if (var.revents & POLLPRI)
                        pt->wake |= Notify_Read | Notify_Accept;
                    if (var.revents & POLLOUT)
                        pt->wake |= Notify_Write;
                    if (var.revents & POLLHUP)
                        pt->wake |= Notify_Close;

                    pt->despatch();
                    priv->wake.push(*pt);
                }
            }
        }
    }

    foreach (posix_fd *var, priv->actalw)
    {
        if (var->ion & Notify_Always)
        {
            var->wake |= Notify_Always;
            var->despatch();
            priv->wake.push(*var);
        }
    }

    --priv->waiting;
    return priv->wake.count();
}

void event_poll::restart ()
{
    if (priv->waiting.load () > 0)
        priv->raise_wakeup ();
}

void event_poll::flushing (bool flush)
{
    priv->flushing.store (flush);

    if (flush && priv->waiting.load () > 0)
        priv->raise_wakeup ();
}

bool event_poll::wakeup ()
{
    return priv->raise_wakeup ();
}

bool event_poll::release ()
{
    return priv->release_wakeup ();
}
bool event_poll::is_wakeup()
{
    return priv->is_wakeup ();
}
//!
//! \brief fetch_wake 取出唤醒id(在列表中删除)
//! \return 注册的id, -1无可用
//!
int event_poll::fetch_wake()const
{
    if (!priv->wake.is_empty ())
        return priv->wake.pop ().get();
    return -1;
}
//!
//! \brief peek_wake 查看唤醒id(不会删除id)
//! \return 注册的id, -1无可用
//!
int event_poll::peek_wake()const
{
    if (!priv->wake.is_empty ())
        return priv->wake.front ().get();
    return -1;
}


#endif
