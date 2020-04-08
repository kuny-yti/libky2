/**
 * Basic tool library
 * Copyright (C) 2014 kunyang kunyang.yk@gmail.com
 *
 * @file     ky_event_poll.h
 * @brief    fd事件轮询对象
 *       1.支持windows
         2.支持类unix内核(linux)
 *       3.可检测fd是否被唤醒
 *       4.可控制轮询者的状态
 *
 * @author   kunyang
 * @email    kunyang.yk@gmail.com
 * @version  1.2.1.1
 * @date     2017/09/18
 * @license  GNU General Public License (GPL)
 *
 * Change History :
 *    Date    |  Version  |  Author  |   Description
 * 2017/09/18 | 1.0.0.1   | kunyang  | 创建文件
 * 2018/06/27 | 1.0.1.1   | kunyang  | 修改fd唤醒设置及检测唤醒
 * 2018/07/02 | 1.1.0.1   | kunyang  | 加入epoll模式
 * 2019/01/20 | 1.2.0.1   | kunyang  | 分离Windows和POSIX模式实现
 * 2019/02/03 | 1.2.1.1   | kunyang  | 加入事件通知枚举
 * 2019/04/21 | 1.2.2.1   | kunyang  | 修复类unix的epoll模式出现无响应
 * 2019/04/29 | 1.3.0.1   | kunyang  | 加入定时器实现
 */
#ifndef KY_EVENT_POLL_H
#define KY_EVENT_POLL_H

#include "ky_define.h"
#include "ky_object.h"

//!
//! \brief The ky_event_poll class
//!
class event_poll
{
public:
    event_poll();
    virtual ~event_poll();

    //!
    //! \brief registered 注册需要轮询的对象(消息、事件、套接字、异步IO)
    //! \param fd 句柄
    //! \param im 能被唤醒的标签-eIoNotifys
    //! \return 返回注册的id
    //! \note
    //!     1.(im == Io_NotifyMsg) fd = HANDLE(windows)
    //!     2.(im == Io_NotifyIo)  fd = HANDLE(windows)
    //!     3.(im == Io_NotifySocket)  fd = sockhd
    //!     4.(im == Io_NotifyTimer)  fd = interval(millisec)
    //!
    int registered (intptr fd, eNotifyFlags im = Notify_Not);
    //!
    //! \brief unregister 注销轮询的对象
    //! \param id 注册时id
    //! \return
    //!
    bool unregister (int id);

    //!
    //! \brief modify 修改轮询对象唤醒标签
    //! \param id 注册时id
    //! \param im 唤醒的标签-eIoNotifys
    //! \param active 指定标签是激活还是删除
    //! \return
    //! \note 只有 Notify_Io 类型的才可修改
    //!
    bool modify(int id, eNotifyFlags im, bool active);

    //!
    //! \brief can_exit 是否可以退出(无对象轮询)
    //! \return
    //!
    bool can_exit();

    //!
    //! \brief can_error 检测是否被错误唤醒
    //! \param id 注册时id
    //! \return
    //!
    bool can_error (int id);

    //!
    //! \brief can_wake 是否被唤醒
    //! \param id 注册时id
    //! \param im 检测的唤醒标签-eIoNotifys
    //! \return
    //!
    bool can_wake(int id, eNotifyFlags im);

    //!
    //! \brief wait 等待fd唤醒活动,直至超时或退出
    //! \param timeout 以毫秒为单位的超时。
    //! \note 此函数只能是一次从一个线程调用
    //! \return -1 error(flushing), -2 从多个线程调用.
    //!        返回活动的数量
    //!
    int wait (int64 timeout = - 1);

    //!
    //! \brief restart 重新启动 wait
    //!                这个函数通常在注册/注销后使用。
    //!
    void restart ();

    //!
    //! \brief flushing 直接完成
    //! \param flush true:对 wait 调用将返回-1
    //!
    void flushing (bool flush);

    //!
    //! \brief wakeup 唤醒等待
    //! \return
    //!
    bool wakeup ();

    //!
    //! \brief release 释放唤醒
    //! \return
    //!
    bool release ();
    //!
    //! \brief is_wakeup 是否被唤醒
    //! \return
    //!
    bool is_wakeup();

public:
    //!
    //! \brief system_limit 当前系统的句柄限制
    //! \return
    //!
    static int64 system_limit();
    //!
    //! \brief hard_limit 本机硬件句柄限制
    //! \return
    //!
    static int64 hard_limit();

protected:
    //!
    //! \brief fetch_wake 取出唤醒id(在列表中删除)
    //! \return 注册的id, -1无可用
    //!
    int fetch_wake()const;
    //!
    //! \brief peek_wake 查看唤醒id(不会删除id)
    //! \return 注册的id, -1无可用
    //!
    int peek_wake()const;

protected:
    explicit event_poll(ky_thread *self);

private:
    struct poll_priv *priv;
};

#endif // KY_EVENT_POLL_H
