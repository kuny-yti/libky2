
/**
 * Basic tool library
 * Copyright (C) 2014 kunyang kunyang.yk@gmail.com
 *
 * @file     ky_thread.h
 * @brief    多线程对象以及锁的实现
 *       1.利用POSIX机制实现线程以及同步.
 *       2.线程支持运行优先级设置
 *       3.线程支持调度策略设置
 *       4.线程可使用TLS
 *       5.线程可支持ms、us、s睡眠机制
 *       6.支持线程安全和非安全类
 *
 * @author   kunyang
 * @email    kunyang.yk@gmail.com
 * @version  1.2.1.1
 * @date     2012/08/12
 * @license  GNU General Public License (GPL)
 *
 * Change History :
 *    Date    |  Version  |  Author  |   Description
 * 2012/08/12 | 1.0.0.1   | kunyang  | 创建文件
 * 2012/08/14 | 1.0.1.0   | kunyang  | 完善线程实现
 * 2013/05/06 | 1.0.2.0   | kunyang  | 实现互斥锁和等待条件
 * 2013/05/08 | 1.0.3.0   | kunyang  | 实现自旋锁
 * 2013/05/10 | 1.0.4.0   | kunyang  | 实现读写锁
 * 2014/03/07 | 1.0.4.1   | kunyang  | 修改互斥锁和等待条件，优化线程对象
 * 2015/01/20 | 1.1.0.1   | kunyang  | 重构线程类并加入运行优先级和调度策略
 * 2015/09/07 | 1.1.1.1   | kunyang  | 修改自旋锁采用原子操作模式
 * 2016/11/18 | 1.1.2.1   | kunyang  | 修改线程实现并加入TLS和睡眠机制，内部加入全局线程管理
 * 2017/08/26 | 1.2.0.1   | kunyang  | 加入线程安全和非安全类
 * 2018/07/01 | 1.2.1.1   | kunyang  | 修改线程安全类使用原子操作
 */
#ifndef KY_THREADS_H
#define KY_THREADS_H
#include "ky_define.h"

#ifndef kyTimeoutIndefinite
#define kyTimeoutIndefinite (-1)
#endif

#define kyThreadInvalid (0)

#if kyOSIsLinux && defined(__GLIBC__)
#define hasThreadLocalStorage
#endif


//! 线程调度的策略
typedef enum
{
    Policy_Default = 0,    //!< 系统默认
    Policy_FIFO = 1,       //!< 先进先出
    Policy_RoundRobin = 2, //!< 轮询
    Policy_Affinity = 3    //!< CPU亲和力，系统自动选择CPU，若系统无可用时，则不响应
}eThreadPolicys;

//! 线程优先权
typedef enum
{
    Priority_Idle,         //!< 空闲
    Priority_Lowest,       //!< 最低
    Priority_Low,          //!< 低
    Priority_Normal,       //!< 正常
    Priority_High,         //!< 高
    Priority_Highest,      //!< 最高

    Priority_TimeCritical, //!< 关键优先权

    Priority_Inherit       //!< 继承父优先权
}eThreadPrioritys;

typedef pthread_t thread_id;
typedef pid_t     process_id;
struct tThreadLocalStorages :public ky_ref
{
    virtual void destroy() = 0;
};

/*!
 * @brief The ky_thread class Thread
 * @class ky_thread
 * Implement the run() function in a subclass.
 */
class ky_thread
{
public:
    explicit ky_thread();
    virtual ~ky_thread();

    //!
    //! \brief policy 线程的策略
    //! \param tp
    //! \return
    //!
    bool set_policy(eThreadPolicys tp);
    eThreadPolicys policy()const;

    //!
    //! \brief priority 线程的优先权
    //! \param prio
    //! \return
    //!
    bool set_priority(eThreadPrioritys prio);
    eThreadPrioritys priority() const;

    //!
    //! \brief is_running 返回线程是否在运行
    //! \return
    //!
    bool is_running()const {return __running;}

    //!
    //! \brief has_affinity 是否可以开启CPU亲和力
    //! \return
    //!
    bool has_affinity()const ;
    //!
    //! \brief is_affinity 是否开启了CPU亲和力
    //! \return
    //!
    bool is_affinity()const;
    //!
    //! \brief set_affinity 开启CPU亲和力
    //! \param idx CPU的索引,若不指定，系统自动选择,除非自己很清楚选择那个
    //! \return
    //! \note 1开始索引
    //!
    bool set_affinity(const uint &idx = 0);

    //!
    //! \brief run 需要执行的线程，继承
    //!
    virtual void run() = 0;

    //!
    //! \brief start 启动一个线程来执行run函数，若在运行则什么都不做
    //! \param tp
    //! \return
    //!
    bool start(eThreadPolicys tp = Policy_Default);

    //!
    //! \brief wait 等待线程运行结束，若没有运行则直接返回
    //! \return
    //!
    bool wait(int64 timeout = kyTimeoutIndefinite);

    //!
    //! \brief finish  等待线程运行结束
    //! \return
    //!
    bool finish();

    //!
    //! \brief cancel  取消线程，不安全请勿使用
    //! \return
    //!
    bool cancel();

    //!
    //! \brief id 回线程id
    //! \return
    //!
    thread_id id()const;

    //!
    //! \brief operator 比较两个线程是否相同
    //! \param rhs
    //! \return
    //!
    bool operator != (const ky_thread &rhs)const;
    bool operator == (const ky_thread &rhs)const;

protected:
    //!
    //! \brief exec 执行事件的派遣工作
    //! \return 返回运行的返回码
    //! \note 注意: 若线程需要派遣事件对象的话，需要在run中执行exec
    //!
    int exec();

public:
    //!
    //! \brief exit 退出exec
    //! \param code exec的返回码
    //!
    void exit(int code);
    //!
    //! \brief quit 退出exec
    //!
    void quit();

public:
    //!
    //! \brief sleep 睡眠s秒
    //! \param s
    //!
    static void sleep(uint s);//s
    //!
    //! \brief msleep 睡眠ms毫秒
    //! \param ms
    //!
    static void msleep(ulong ms);//ms
    //!
    //! \brief usleep 睡眠um微妙
    //! \param um
    //!
    static void usleep(int64 um);//us
    //!
    //! \brief yield 出让执行权
    //!
    static void yield();
    //!
    //! \brief current 返回当前线程
    //! \return
    //!
    static ky_thread *current();
    //!
    //! \brief current_id 返回当前线程id
    //! \return
    //!
    static thread_id current_id ();
    //!
    //! \brief pid 返回当前进程id
    //! \return
    //!
    static process_id pid();

    #ifdef hasThreadLocalStorage
    //!
    //! \brief tls_set 设置线程TLS的数据
    //! \param td
    //!
    static void local_storage_set(tThreadLocalStorages * td);
    //!
    //! \brief tls_get 获取线程TLS的数据
    //! \return
    //!
    static tThreadLocalStorages *local_storage_get();
    //!
    //! \brief tls_clear 清除线程TLS数据
    //!
    static void local_storage_clear();
    #endif
    //!
    //! \brief en_thread_affinity 将当前线程限制为指定CPU执行
    //! \param idx CPU索引处理器idx
    //! \return
    //!
    static bool en_thread_affinity(const uint &idx);
    //!
    //! \brief de_thread_affinity 将线程所限定的CPU执行关闭
    //! \param idx
    //! \return
    //!
    static bool de_thread_affinity(const uint &idx);

public:
    static const int priority_default;

protected:
    thread_id       __thread_id;
    ky_atomic<bool> __joinable;
    ky_atomic<bool> __running;

    static void* __run(void* p);

private:
    eThreadPrioritys    __priority;
    int                 __priority_min;
    int                 __priority_max;
    eThreadPolicys      __policy;
    process_id          __process_idx;

    friend class ky_app;
    friend class ky_object;
    friend class thread_dispatch;
    class thread_dispatch *dispatch;
};

//!
//! \brief The ky_nonthreadsafe class
//! @class ky_nonthreadsafe
//! 线程非安全指示
//!
class ky_nonthreadsafe
{
public:
    virtual ~ky_nonthreadsafe(){}

    bool is_safe(){return true;}
    bool move_thread(ky_thread *){return true;}

protected:
    ky_nonthreadsafe(){}
    void detach(){}
    void ensure_id_valid(){}
};

//!
//! \brief The ky_threadsafe class
//! @class ky_threadsafe
//! 线程安全检测
//!
class ky_threadsafe
{
public:
    virtual ~ky_threadsafe(){}

    bool move_thread(ky_thread *t);

    bool is_safe();

protected:
    ky_threadsafe();
    void detach();
    void ensure_id_valid();

private:
    ky_atomic<thread_id> valid_id;
};


#endif
