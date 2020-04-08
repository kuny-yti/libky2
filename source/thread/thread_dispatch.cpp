#include "thread_dispatch.h"
#include "ky_debug.h"
#include "ky_event.h"

//! 全局线程列表实例
ky_map<thread_id, ky_thread*> thread_dispatch::global_thread_list;


main_thread::main_thread():
    ky_thread()
{
    pid = ky_thread::pid ();
    tid = ky_thread::current_id ();
    this->__running = true;
    thread_dispatch::global_thread_list.append (tid, this);
}
main_thread::~main_thread()
{
    thread_dispatch::global_thread_list.remove (tid);
}

void main_thread::run()
{
    this->exec();
}

kyPretreatObject(main_thread, 10, "system main thread pretreat.");

thread_dispatch::thread_dispatch(ky_thread *self):
    ky_event_poll(self)
{
    mutex.lock ();
    exited = false;
    exit_code = 0;
    req_quit = false;
    mutex.unlock ();
}
thread_dispatch::~thread_dispatch()
{
}

bool thread_dispatch::posted (ievent *e, ky_object *o)
{
    // 事件派遣所有对象
    if (0 == o)
    {
        ky_map<thread_id, ky_thread*>::iterator ite =
                thread_dispatch::global_thread_list.begin();
        for (; ite != thread_dispatch::global_thread_list.end(); ++ite)
        {
            ky_thread* thr = ite.value ();

            thr->dispatch->mutex.lock ();
            thr->dispatch->post_queue.push(ky_post(e, o));
            thr->dispatch->mutex.unlock ();
            thr->dispatch->wakeup ();
        }
        return true;
    }

    // 指定对象派遣
    ky_thread *othr = o->thread ();
    if (othr && othr->dispatch)
    {
        othr->dispatch->mutex.lock ();
        othr->dispatch->post_queue.push(ky_post(e, o));
        othr->dispatch->mutex.unlock ();
        othr->dispatch->wakeup ();
        return true;
    }
    return false;
}

void thread_dispatch::registered(intptr fd, eNotifyFlags ion, ky_object *o)
{
    if (o && o->thread() != ky_thread::current())
    {
        log_err("ky_object: Non-thread safe operation");
        return ;
    }
    const int id = ky_event_poll::registered(fd, ion);
    if (id >= 0)
    {
        object_list[id] = ky_pair(id, fd, o);
    }
}

void thread_dispatch::unregister(intptr fd)
{
    int id = -1;
    ky_map<int, ky_pair>::iterator ite = object_list.begin();
    for (;ite != object_list.end(); ++ite)
    {
        if (ite.value().hd == fd)
        {
            id = ite.key();
            break;
        }
    }
    if (id >= 0)
    {
        ky_event_poll::unregister(id);
        object_list.remove(id);
    }
}
#include "ky_datetime.h"
int thread_dispatch::dispatcher()
{
    do
    {
        const bool is_lave_posted = !post_queue.is_empty();

        // 本线程内有事件需要寄送
        if (is_lave_posted)
        {
            // 取出需要寄送的事件
            ky_post ep = post_queue.pop();

            // 寄送到指定目标
            if (ep.target)
                ep.target->event (ep.event);
            // 无寄送目标，则事件不为空，需要寄送本线程内所有对象
            else if (ep.event)
            {
                ky_map<int, ky_pair>::iterator ite = object_list.begin();
                for (;ite != object_list.end(); ++ite)
                {
                    ky_pair &pair = *ite;
                    pair.object->event(ep.event);
                }
            }
            // 若本线程内无对象，请求退出派遣
            else
            {
                req_quit = object_list.is_empty();
            }
        }

        // 还有事件寄送时，则检查是否有新事件到来。无对象事件寄送，则进入等待
        const int64 timeout = is_lave_posted ? 0 : -1;
        // 论巡是否需要寄送事件
        int num = this->wait (timeout);
        // 检测到事件需要派遣
        if (num > 0)
        {
            // 取出需要派遣的对象
            while ((num = this->fetch_wake()) >= 0 && object_list.contains (num))
            {
                const ky_pair pair = object_list.value(num);
                ky_event notify = ky_event::make_notify(pair.hd, ky_datetime::millisec());
                pair.object->event(&notify);
            }
        }
        // 出错
        else if (num == -1)
        {
            req_quit = this->can_exit();
        }
        // 多个线程轮询了
        else if (num == -2)
        {

        }
    }while (!req_quit);

    return exit_code;
}
