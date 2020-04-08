
/**
 * Basic tool library
 * Copyright (C) 2014 kunyang kunyang.yk@gmail.com
 *
 * @file     ky_lock.h
 * @brief    定义POSIX类型线程锁
 *       1.支持自旋锁(原子实现)
 *       2.支持互斥锁
 *       3.支持读写锁
 *       4.支持条件等待
 *       5.支持自动加解锁
 *
 * @author   kunyang
 * @email    kunyang.yk@gmail.com
 * @version  1.1.0.1
 * @date     2012/10/02
 * @license  GNU General Public License (GPL)
 *
 * Change History :
 *    Date    |  Version  |  Author  |   Description
 * 2012/10/02 | 1.0.0.1   | kunyang  | 创建文件定义键盘事件
 * 2017/06/17 | 1.1.0.1   | kunyang  | 修改自旋锁为原子实现
 *
 */
#ifndef KY_LOCK_H
#define KY_LOCK_H
#include "ky_define.h"
#include "arch/ky_atomic.h"

#ifndef kyTimeoutIndefinite
#define kyTimeoutIndefinite (-1)
#endif

#ifdef kyPosixSpinLock
/*!
 * Spin
 *
 */
class ky_spinlock
{
private:
    static const pthread_spinlock_t _spin_initializer;
    pthread_spinlock_t _spin;
    bool _is_init;
private:
    ky_spinlock(const ky_spinlock &) = delete;
    ky_spinlock &operator=(const ky_spinlock &) = delete;

public:
    explicit ky_spinlock();
    ~ky_spinlock();

    // Lock the mutex.
    bool lock();
    // Try to lock the mutex. Return true on success, false otherwise.
    bool trylock();
    // Unlock the mutex
    bool unlock();

};

#else
/*!
 * @brief The ky_spinlock class Spin (atomic)
 * @class ky_spinlock
 */
class ky_spinlock
{
private:
    ky_atomic<bool> slock;

private:
    ky_spinlock(const ky_spinlock &) = delete;
    ky_spinlock &operator=(const ky_spinlock &) = delete;

public:
    explicit ky_spinlock();
    ~ky_spinlock();

    // Lock the mutex.
    bool lock(int64 timeout = kyTimeoutIndefinite);
    // Try to lock the mutex. Return true on success, false otherwise.
    bool trylock(int64 timeout = kyTimeoutIndefinite);
    // Unlock the mutex
    bool unlock();
};
#endif

/*!
 * @brief The ky_rwlock class  Read Write Lock (posix)
 * @class ky_rwlock
 */
class ky_rwlock
{
private:
    static const pthread_rwlock_t _rw_initializer;
    pthread_rwlock_t _rw;
    bool _is_init;

private:
    ky_rwlock(const ky_rwlock &) = delete;
    ky_rwlock &operator=(const ky_rwlock &) = delete;

public:
    explicit ky_rwlock();
    ~ky_rwlock();

    // Lock the mutex.
    bool lockrd(int64 timeout = kyTimeoutIndefinite);
    bool lockwr(int64 timeout = kyTimeoutIndefinite);
    // Try to lock the mutex. Return true on success, false otherwise.
    bool trylockrd(int64 timeout = kyTimeoutIndefinite);
    bool trylockwr(int64 timeout = kyTimeoutIndefinite);
    // Unlock the mutex
    bool unlock();

};

/*!
 * @brief The ky_mutex class  Mutex Lock (posix)
 * @class ky_mutex
 */
class ky_mutex
{
private:
    static const pthread_mutex_t _mutex_initializer;
    pthread_mutex_t _mutex;
    bool _is_init;

private:
    ky_mutex(const ky_mutex &) = delete;
    ky_mutex &operator=(const ky_mutex &) = delete;

public:
    //!
    //! \brief ky_mutex
    //! \param is_reentrant 是否可重用
    //!
    explicit ky_mutex(bool is_reentrant = false);
    ~ky_mutex();

    // Lock the mutex.
    bool lock(int64 timeout = kyTimeoutIndefinite);
    // Try to lock the mutex. Return true on success, false otherwise.
    bool trylock(int64 timeout = kyTimeoutIndefinite);

    // Unlock the mutex
    bool unlock();

    friend class ky_condition;
    friend class ky_moment_lock;
};

/*!
 * @brief The ky_scopelock class 范围锁
 * @class ky_scopelock
 */
class ky_scopelock
{
public:
    explicit ky_scopelock(ky_mutex *m);
    explicit ky_scopelock(ky_mutex &m);
    ~ky_scopelock();

    void unlock();

private:
    ky_mutex *mutex;
};

/*!
 * @brief The ky_condition class Wait condition
 * @class ky_condition
 */
class ky_condition
{
private:
    static const pthread_cond_t _cond_initializer;
    ky_mutex* _mutex;
    pthread_cond_t _cond;
    bool _is_init;

private:
    ky_condition(const ky_condition &) = delete;
    ky_condition &operator=(const ky_condition &) = delete;

public:
    // Constructor / Destructor
    explicit ky_condition();
    ~ky_condition();

    // ms Wait for the condition. The calling thread must have the mutex locked.
    bool wait(ky_mutex& m, int64 timeout = kyTimeoutIndefinite);
    // Wake one thread that waits on the condition.
    bool wake_one(); //- signal
    // Wake all threads that wait on the condition.
    bool wake_all(); //- broadcast

};

#endif // KY_LOCK_H
