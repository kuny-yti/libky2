#include "event_poll.h"
#include "ky_debug.h"
#include "ky_atomic.h"
#include "ky_thread.h"
#include "thread_dispatch.h"

#if kyOSIsWin32

#ifndef TIME_KILL_SYNCHRONOUS
#  define TIME_KILL_SYNCHRONOUS 0x0100
#endif

#ifndef QS_RAWINPUT
#  define QS_RAWINPUT 0x0400
#endif

#ifndef WM_TOUCH
#  define WM_TOUCH 0x0240
#endif
#ifndef WM_GESTURE
#  define WM_GESTURE 0x0119
#endif
#ifndef WM_GESTURENOTIFY
#  define WM_GESTURENOTIFY 0x011A
#endif

#define WM_TimerBase 100
#define WM_MaxPoll MAXIMUM_WAIT_OBJECTS-2

enum
{
    WM_SocketNotice = WM_USER,    ///< socket关联句柄
    WM_SendPostEvent = WM_USER + 1,///<
    WM_ActiveNotice = WM_USER + 2,
    SendPostEventsWindowsTimerId = ~1u
};
LRESULT CALLBACK inl_window_proc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

struct window_register : ky_singleton<window_register>
{
    window_register():
        inl_atom(0)
    {
        memset(inl_ids, 0, sizeof(inl_ids));
        memset(inl_name, 0, sizeof(inl_name));

        wsprintf(inl_name, L"window_register_%d", (intptr_t)inl_window_proc);

        WNDCLASS wc;
        wc.style = 0;
        wc.lpfnWndProc = inl_window_proc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = GetModuleHandle(0);
        wc.hIcon = 0;
        wc.hCursor = 0;
        wc.hbrBackground = 0;
        wc.lpszMenuName = NULL;
        wc.lpszClassName = inl_name;
        inl_atom = RegisterClass(&wc);
    }
    ~window_register()
    {
        if (inl_atom)
            UnregisterClass(inl_name, GetModuleHandle(0));
    }

    int gnid()
    {
        for (int i = 0; i < WM_MaxPoll; ++i)
        {
            if (!inl_ids[i])
            {
                inl_ids[i] = true;
                return i;
            }
        }
        return -1;
    }
    void unid(int id)
    {
        if (id < WM_MaxPoll && id >= 0)
            inl_ids[id] = false;
    }

    wchar_t inl_name[64];
    ATOM    inl_atom;
    bool    inl_ids[WM_MaxPoll];
};
//kyPretreatObject(window_register, 20, "win32 window class register.");

struct poll_t
{
    int      id;
    intptr   hd;
    notify_t ion;
    notify_t wake;
    bool     acitve;
    int      index;
    int      actalw;

    poll_t()
    {
        id = -1;
        hd = 0;
        ion = 0;
        wake = Notify_Not;
        acitve = false;
        index = -1;
        actalw = -1;
    }
    poll_t(intptr a, notify_t m)
    {
        id = -1;
        hd = a;
        ion = m;
        wake = Notify_Not;
        acitve = false;
        index = -1;
        actalw = -1;
    }
};

struct poll_priv
{
    HWND                    inl_hwnd;    ///< 隐藏窗口句柄

    ky_atomic<int>          always;

    ky_atomic<int>          rebuild;     ///< 重新构建轮询
    ky_atomic<int>          pending;     ///< 唤醒记录
    ky_atomic<int>          waiting;     ///< 等待数量
    ky_atomic<int>          flushing;    ///< 完成退出

    ky_map<intptr, poll_t>  inactive;    ///< 未激活列表
    ky_list<poll_t*>        actalw;      ///< 总是激活列表
    ky_list<poll_t*>        active;      ///< 激活列表
    ky_list<poll_t*>        inremove;    ///< 被删除列表
    ky_queue<poll_t*>       wake;        ///< 被唤醒列表

    ky_thread              *inl_thread;  ///< 所属线程

    poll_priv():
        inl_hwnd(0),
        always(0),
        rebuild(1),
        pending(0),
        waiting(0),
        flushing(0),
        inactive(),
        actalw(),
        active(),
        inremove(),
        wake(),
        inl_thread(ky_thread::current())
    {
        if (window_register::instance()->inl_atom)
        {
            inl_hwnd = inl_create_window(window_register::instance()->inl_name,
                                         (void*)this);
            if (!inl_hwnd)
            {
                log_crit("ky_event_poll: win32 window create fail!");
            }
        }
    }
    ~poll_priv()
    {
        inl_destroy_window(inl_hwnd);
    }

    //!
    //! \brief wake_event 进行唤醒
    //! \return
    //!
    bool wake_event ()
    {
        if (inl_hwnd)
            PostMessage(inl_hwnd, WM_SendPostEvent, 0, 0);

        return true;
    }
    //!
    //! \brief release_event 释放唤醒
    //! \return
    //!
    bool release_event ()
    {
        return true;
    }
    //!
    //! \brief raise_wakeup 提出唤醒
    //! \return
    //!
    bool raise_wakeup ()
    {
        bool result = true;

        // 未提出唤醒则提出唤醒
        if (!pending.load ())
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

    //!
    //! \brief wsaSelect windows socket关联句柄
    //! \param fd
    //! \param ion
    //! \return
    //!
    bool wsa_select(int fd, notify_t ion)
    {
        int event = 0;
        if (ion & Notify_Socket)
            return false;

        if (ion & Notify_Accept)
            event |= FD_ACCEPT;
        if (ion & Notify_Close)
            event |= FD_CLOSE;
        if (ion & Notify_Read)
            event |= FD_READ;
        if (ion & Notify_Write)
            event |= FD_WRITE | FD_CONNECT;

        WSAAsyncSelect(fd, inl_hwnd, event ? int(WM_SocketNotice) : 0, event);
        return true;
    }

    void set_timer(int id, intptr elapse)
    {
        SetTimer(inl_hwnd, WM_TimerBase+id,elapse, 0);
    }
    void kill_timer(int id)
    {
        KillTimer(inl_hwnd, WM_TimerBase+id);
    }

    static HWND inl_create_window(wchar_t *nm, void *ptr)
    {
        HWND wnd = CreateWindow(nm, nm, 0, 0, 0, 0, 0, HWND_MESSAGE, 0,
                                 GetModuleHandle(0), 0);
        if (!wnd)
            return 0;

    #ifdef GWLP_USERDATA
        SetWindowLongPtr(wnd, GWLP_USERDATA, (LONG_PTR)ptr);
    #else
        SetWindowLong(wnd, GWL_USERDATA, (LONG)ptr);
    #endif

        return wnd;
    }
    static void inl_destroy_window(HWND hwnd)
    {
        if (hwnd)
            DestroyWindow(hwnd);
    }

    friend LRESULT CALLBACK inl_window_proc(HWND, UINT, WPARAM, LPARAM);
};

ky_event_poll::ky_event_poll():
    priv(kyNew(poll_priv()))
{

}
ky_event_poll::ky_event_poll(ky_thread *self):
    priv(kyNew(poll_priv()))
{
    priv->inl_thread = self;
}
ky_event_poll::~ky_event_poll()
{
    kyDelete(priv);
}

//!
//! \brief registered 注册需要轮询的对象(消息、事件、套接字、异步IO)
//! \param fd 句柄
//! \param im 能被唤醒的标签-eIoModes
//! \return
//!
int ky_event_poll::registered (intptr fd, notify_t im)
{
    if (!fd)
    {
        log_warn("ky_event_poll: Can't register, handle error.");
        return -1;
    }
    if (ky_thread::current() != priv->inl_thread)
    {
        log_warn("ky_event_poll: Not thread safe operation.");
        return -1;
    }
    // 已经注册，但是通知标志不同，则修改
    if (priv->inactive.contains(fd))
    {
        poll_t &pt = priv->inactive[fd];
        if (pt.ion != im)
            pt.ion = im;
        priv->raise_wakeup();
        return pt.id;
    }

    // 产生一个id来表示注册位置
    int poll_id = window_register::instance()->gnid();
    if (poll_id < 0)
    {
        log_warn("ky_event_poll: Can't register, has exceeded.");
        return -1;
    }
    // 注册一个通知
    poll_t &pt = priv->inactive[fd];
    pt.hd = fd;
    pt.id = poll_id;
    pt.ion = im;
    pt.acitve = false;
    pt.wake = Notify_Not;
    priv->rebuild.store(1);
    priv->raise_wakeup();

    return poll_id;
}

//!
//! \brief unregister 注销轮询的对象
//! \param id 句柄
//! \return
//!
bool ky_event_poll::unregister (int id)
{
    if (id < 0)
        return false;
    if (ky_thread::current() != priv->inl_thread)
    {
        log_warn("ky_event_poll: Not thread safe operation.");
        return false;
    }
    intptr fd = 0;
    ky_map<intptr, poll_t>::iterator ite = priv->inactive.begin();
    for (;ite != priv->inactive.end(); ++ite)
    {
        const poll_t & pt = ite.value();
        if (pt.id == id)
        {
            fd = pt.hd;
            break;
        }
    }
    if (fd)
    {
        priv->inactive.remove(fd);
        priv->wake_event();
        return true;
    }
    return false;
}

//!
//! \brief modify 修改轮询对象唤醒标签
//! \param fd 句柄
//! \param im 唤醒的标签-eIoModes
//! \return
//!
bool ky_event_poll::modify(int id, notify_t im, bool active)
{
    if (id < 0 || !(im & Notify_Io))
        return false;
    if (ky_thread::current() != priv->inl_thread)
    {
        log_warn("ky_event_poll: Not thread safe operation.");
        return false;
    }

    ky_map<intptr, poll_t>::iterator ite = priv->inactive.begin();
    for (;ite != priv->inactive.end(); ++ite)
    {
        poll_t & pt = *ite;
        if (pt.id == id)
        {
            if (!(pt.ion & Notify_Io))
                return false;

            if (active)
                pt.ion |= im;
            else
                pt.ion &= ~im;
            pt.acitve = false;
            priv->rebuild.store(1);
            priv->raise_wakeup();
            return true;
        }
    }
    return false;
}

//!
//! \brief can_exit 是否可以退出(无对象轮询)
//! \return
//!
bool ky_event_poll::can_exit()
{
    return priv->active.is_empty() || (priv->flushing.load() > 0);
}

//!
//! \brief can_error 检测是否被错误唤醒
//! \param fd 句柄
//! \return
//!
bool ky_event_poll::can_error (int )
{
    return false;
}

//!
//! \brief can_wake 是否被唤醒
//! \param fd 句柄
//! \param im 检测的唤醒标签-eIoModes
//! \return
//!
bool ky_event_poll::can_wake(int id, notify_t im)
{
    foreach (poll_t *var, priv->active)
    {
        if (var->id == id)
            return var->acitve && (var->wake & im);
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
int ky_event_poll::wait (int64 timeout)
{
    const int old_waiting = priv->waiting.fetch_add (1);
    /* 我们不能等待多个线程 */
    if (kyUnLikely (old_waiting > 0))
    {
#ifdef kyHasDebug
        log_err ("ky_event_poll: we are already waiting");
#endif
        --priv->waiting;
        return -2;
    }
    /* 立即退出 */
    if (kyUnLikely (priv->flushing.load ()))
    {
#  ifdef kyHasDebug
        log_err ("ky_event_poll: we are flushing");
#  endif
        --priv->waiting;
        return -1;
    }

    priv->release_all_wakeup();

    // 重新构建
    if (priv->rebuild.compare_exchange (1, 0))
    {
        // 重新构建
        if (priv->active.is_empty())
        {
            ky_map<intptr, poll_t>::iterator ite = priv->inactive.begin();
            for (;ite != priv->inactive.end(); ++ite)
            {
                poll_t & pt = *ite;
                if (pt.index < 0)
                    pt.index = priv->active.count();
                // 不为消息对象，并且未被激活[修改也是如此]
                if ((pt.ion & Notify_Socket))
                {
                    pt.acitve = true;
                    priv->wsa_select(pt.hd, pt.ion);
                    priv->active.append(&pt);
                }
                // (事件、消息、异步IO)均不需要绑定句柄
                else if (pt.ion & Notify_Msg)
                {
                    pt.acitve = true;
                    priv->active.append(&pt);
                }
                else if (pt.ion & Notify_Timer)
                {
                    pt.acitve = true;
                    priv->set_timer(pt.id, pt.hd);
                    priv->active.append(&pt);
                }
                if (pt.ion & Notify_Always)
                {
                    if (pt.actalw < 0)
                        pt.actalw = priv->actalw.count();
                    priv->actalw.append(&pt);
                    priv->always.increase();
                }
            }
        }
        else
        {
            // 处理套接字关联
            ky_map<intptr, poll_t>::iterator ite = priv->inactive.begin();
            for (;ite != priv->inactive.end(); ++ite)
            {
                poll_t & pt = *ite;
                if (pt.index < 0)
                    pt.index = priv->active.count();
                // 不为消息对象，并且未被激活[修改也是如此]
                if ((pt.ion & Notify_Socket) && !pt.acitve)
                {
                    pt.acitve = true;
                    priv->wsa_select(pt.hd, pt.ion);
                    priv->active.append(&pt);
                }
                // (事件、消息、计时器、异步IO)均不需要绑定句柄
                else if (pt.ion & Notify_Msg)
                {
                    pt.acitve = true;
                    priv->active.append(&pt);
                }
                else if (pt.ion & Notify_Timer)
                {
                    pt.acitve = true;
                    priv->set_timer(pt.id, pt.hd);
                    priv->active.append(&pt);
                }
                if (pt.ion & Notify_Always)
                {
                    if (pt.actalw < 0)
                        pt.actalw = priv->actalw.count();
                    priv->actalw.append(&pt);
                    priv->always.increase();
                }
            }
            // 删除句柄
            foreach (poll_t *var, priv->inremove)
            {
                if (var->ion & Notify_Socket)
                    priv->wsa_select(var->hd, 0);
                else if (var->ion & Notify_Timer)
                    priv->kill_timer(var->id);
                priv->inactive.remove(var->hd);
                if (var->index >= 0)
                    priv->active.remove(var->index);

                if (var->ion & Notify_Always)
                {
                    priv->actalw.remove(var->actalw);
                    priv->always.reduction();
                }
            }
            priv->inremove.clear();
        }
    }

    DWORD waitRet = WAIT_OBJECT_0;
    if (priv->always.load() < 1)
        waitRet = MsgWaitForMultipleObjectsEx(0, 0, timeout, QS_ALLEVENTS,
                                              MWMO_ALERTABLE | MWMO_INPUTAVAILABLE);
    if (waitRet == WAIT_FAILED)
    {
        errno = GetLastError();
#ifdef kyHasDebug
        log_crit("ky_event_poll: wait error(code=%d)!", GetLastError());
#endif
        --priv->waiting;
        return -3;
    }
    else if (waitRet == WAIT_OBJECT_0)
    {
        MSG msg = {0};
        while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            if (msg.hwnd != priv->inl_hwnd)
            {
                foreach (poll_t *var, priv->active)
                {
                    if ((var->ion & Notify_Msg) && (var->hd == (intptr)msg.hwnd))
                    {
                        priv->wake.push(var);
                        break;
                    }
                }
            }
            else if (msg.message == WM_QUIT)
            {
                priv->flushing.store(1);
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        foreach (poll_t *var, priv->actalw)
        {
            if (var->ion & Notify_Always)
                priv->wake.push(var);
        }
    }

    --priv->waiting;
    return priv->wake.count();
}

//!
//! \brief restart 重新启动 wait
//!                这个函数通常在注册/注销后使用。
//!
void ky_event_poll::restart ()
{
    priv->active.clear();
    priv->rebuild.store(1);
    if (priv->waiting.load() > 0)
        priv->raise_wakeup();
}

//!
//! \brief flushing 直接完成
//! \param flush true:对 wait 调用将返回-1
//!
void ky_event_poll::flushing (bool flush)
{
    priv->flushing.store (flush);
    if (flush && priv->waiting.load () > 0)
        priv->raise_wakeup ();
}

//!
//! \brief wakeup 唤醒等待
//! \return
//!
bool ky_event_poll::wakeup ()
{
    return priv->wake_event();
}

//!
//! \brief release 释放唤醒
//! \return
//!
bool ky_event_poll::release ()
{
    return priv->release_wakeup();
}

int ky_event_poll::fetch_wake()const
{
    if (priv->wake.is_empty())
        return -1;
    poll_t *pt = priv->wake.pop();
    return pt->id;
}
int ky_event_poll::peek_wake()const
{
    if (priv->wake.is_empty())
        return -1;
    poll_t *pt = priv->wake.front();
    return pt->id;
}

LRESULT CALLBACK inl_window_proc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_NCCREATE)
        return true;

    poll_priv *priv = 0;
#  ifdef GWLP_USERDATA
    priv = (poll_priv*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
#  else
    priv = (poll_priv*)GetWindowLong(hwnd, GWL_USERDATA);
#  endif
    MSG msg = {0};
    msg.hwnd = hwnd;
    msg.message = message;
    msg.wParam = wParam;
    msg.lParam = lParam;

    // socket 通知
    if (message == WM_SocketNotice)
    {
        notify_t notify = 0;
        int cmd = WSAGETSELECTEVENT(lParam);
        if (cmd == FD_READ)
            notify = Notify_Read;
        else if (cmd == FD_ACCEPT)
            notify = Notify_Accept;
        else if (cmd == FD_WRITE || cmd == FD_CONNECT)
            notify = Notify_Write;
        else if (cmd == FD_OOB)
        {

        }
        else if (cmd == FD_CLOSE)
            notify = Notify_Close;

        if (notify != 0 && priv)
        {
            // 查询轮询者是否可被响应
            foreach (poll_t *var, priv->active)
            {
                if ((var->hd == wParam) && (var->ion & notify))
                {
                    priv->wake.push(var);
                    break;
                }
            }
        }
        return 0;
    }
    // message 通知
    else if (priv && (message == WM_ActiveNotice))
    {

        return 0;
    }
    // post 通知
    else if (message == WM_SendPostEvent)
    {

        return 0;
    }
    else if (priv && (message == WM_TIMER))
    {
        foreach (poll_t *var, priv->active)
        {
            if ((var->hd == wParam) && (var->ion & Notify_Timer))
            {
                priv->wake.push(var);
                break;
            }
        }
        return 0;
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}

#endif
