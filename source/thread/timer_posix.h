#ifndef KY_TIMER_POSIX_H
#define KY_TIMER_POSIX_H

#include "posix_fd.h"

struct timer_posix : posix_fd
{
    //!
    //! \brief timer_posix 构造函数
    //! \param is_relatively 相对时间定时器
    //!
    explicit timer_posix(bool is_relatively = false);
    virtual ~timer_posix();

    //!
    //! \brief start 启动定时器
    //! \param timeout 定时超时时间(毫秒)
    //! \return
    //!
    bool start (int64 timeout = -1);
    //!
    //! \brief elapsed 已过去的时间
    //! \return
    //!
    int64 elapsed()const;
    //!
    //! \brief stop 停止定时器
    //!
    bool stop();

    //!
    //! \brief despatch 线程派遣
    //! \return
    //!
    void despatch();

    bool inl_relatively;
    i64  inl_timeout;
    i64  inl_total;
    u64  inl_base;
    u64  inl_past;
};

#endif // KY_TIMER_POSIX_H
