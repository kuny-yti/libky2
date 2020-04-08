#ifndef THREAD_DISPATCH_H
#define THREAD_DISPATCH_H

#include "tools/ky_queue.h"
#include "tools/ky_map.h"
#include "ky_object.h"
#include "ky_lock.h"
#include "event_poll.h"

struct ky_post
{
    ky_object *target; ///< 目标对象[=0 线程内全部对象]
    ievent    *event;  ///< 邮寄的事件

    ky_post()
    {
        event = 0;
        target = 0;
    }
    ky_post(ievent *evt, ky_object *obj = 0)
    {
        event = evt;
        target = obj;
    }
};
struct ky_pair
{
    int        id;
    intptr     hd;
    ky_object* object;
    ky_pair()
    {
        id = 0;
        hd = 0;
        object = 0;
    }
    ky_pair(int a, intptr b, ky_object *o)
    {
        id = a;
        hd = b;
        object = o;
    }
};

class thread_dispatch : public event_poll
{
    friend class main_thread;
    friend class ky_thread;
    friend class ky_object;
    friend class ky_app;

private:
    ky_condition         wait_cond;
    ky_mutex             mutex;       ///< 底层数据互斥锁
    bool                 exited;      ///< 标志已经退出
    int                  exit_code;   ///< 退出时的代码
    bool                 req_quit;    ///< 请求退出

    ky_map<int, ky_pair> object_list; ///< 本线程的所有对象
    ky_queue<ky_post>    post_queue;  ///< 本线程内所有需要寄送的事件

    //! 全局线程列表，整个系统只存在一份列表
    static ky_map<thread_id, ky_thread*> global_thread_list;

private:
    thread_dispatch(ky_thread *self);
    virtual ~thread_dispatch();

    //!
    //! \brief registered 向内部注册需要派遣的对象
    //! \param fd
    //! \param ion [eIoNotifys]
    //! \param o
    //!
    void registered(intptr fd, eNotifyFlags ion, ky_object *o);
    //!
    //! \brief unregister 向内部注销派遣对象
    //! \param fd
    //!
    void unregister(intptr fd);

    //!
    //! \brief posted 事件邮寄
    //! \param e
    //! \param o
    //! \return
    //!
    static bool posted(ievent *e, ky_object *o = 0);

    //!
    //! \brief dispatcher 线程内进行派遣
    //! \return
    //!
    int dispatcher();
};

class main_thread : public ky_thread
{
public:
    main_thread();
    ~main_thread();

    void run();

private:
    process_id pid;
    thread_id  tid;
};

#endif
