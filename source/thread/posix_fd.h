#ifndef POSIX_FD_H
#define POSIX_FD_H

#include "ky_object.h"

#  define _GNU_SOURCE 1
#  include <sys/poll.h>
#  include <sys/epoll.h>
#  include <sys/time.h>
#  include <sys/socket.h>
#  include <sys/timerfd.h>
#  include <sys/signalfd.h>
#  include <sys/resource.h>

typedef struct pollfd PollFd;

struct posix_fd
{
    virtual ~posix_fd(){}

    posix_fd()
    {
        hd.fd = -1;
        hd.events = 0;
        hd.revents = 0;
        ion = Notify_Not;
        wake = Notify_Not;
        flag = false;
        index = -1;
        always = -1;
    }
    posix_fd(int fd, eNotifyFlags m)
    {
        hd.fd = fd;
        hd.events = 0;
        hd.revents = 0;
        ion = m;
        wake = Notify_Not;
        flag = false;
        index = -1;
        always = -1;
    }

    void set(int fd, eNotifyFlags m) {hd.fd = fd; ion = m;}
    int  get()const {return hd.fd;}
    bool is_valid()const {return hd.fd >= 0;}

    operator int ()const
    {
        return hd.fd;
    }

    //!
    //! \brief despatch 继承者完成触发后的处理
    //!
    virtual void despatch(){}

    PollFd       hd;      ///< 文件描述句柄
    eNotifyFlags ion;     ///< 接收的io通知
    eNotifyFlags wake;    ///< 唤醒的io通知
    bool         flag;    ///< 是否被激活
    int          index;   ///< 内部索引id
    int          always;  ///< 总是通知索引id
};

inline bool operator == (const posix_fd &p1, const posix_fd &p2)
{
    return p1.hd.fd == p2.hd.fd;
}
inline bool operator == (const posix_fd &p1, const int &fd)
{
    return p1.hd.fd == fd;
}
inline bool operator == (const int &fd, const posix_fd &p1)
{
    return p1.hd.fd == fd;
}

#endif
