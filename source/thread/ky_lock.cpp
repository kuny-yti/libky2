#include "ky_lock.h"
#include "ky_thread.h"
#include "ky_timer.h"
#include "ky_debug.h"

#if defined(bsPosixSpinLock)
const pthread_spinlock_t ky_spinlock::_spin_initializer = PTHREAD_SPINLOCK_INITIALIZER;

ky_spinlock::ky_spinlock():
    _spin(_spin_initializer)
{
    _is_init=  true;
    int e = pthread_spin_init(&_spin, PTHREAD_PROCESS_PRIVATE);
    if (e != 0)
        _is_init = false;
}
ky_spinlock::ky_spinlock(const ky_spinlock& ):
    _spin(_spin_initializer)
{
    _is_init=  true;
    int e = pthread_spin_init(&_spin, PTHREAD_PROCESS_PRIVATE);
    if (e != 0)
        _is_init = false;
}

ky_spinlock::~ky_spinlock()
{
    if (_is_init)
        (void)pthread_spin_destroy(&_spin);
}
bool ky_spinlock::lock()
{
    if (!_is_init) return false;
    int e = pthread_spin_lock(&_spin);
    if (e != 0)
        return false;
    return true;
}
bool ky_spinlock::trylock()
{
    if (!_is_init) return false;
    int e = pthread_sp_inrylock(&_spin);
    if (e != 0)
        return false;
    return true;
}
bool ky_spinlock::unlock()
{
    if (!_is_init) return false;
    int e = pthread_spin_unlock(&_spin);
    if (e != 0)
        return false;
    return true;
}
#else
ky_spinlock::ky_spinlock():
    slock(false)
{
}

ky_spinlock::~ky_spinlock()
{
    unlock();
}

// Lock the mutex.
bool ky_spinlock::lock(int64 timeout)
{
    const int64 tick = ky_timer::nanosec ();
    while (!slock.compare_exchange(false, true))
    {
        if (timeout > 0 && (ky_timer::nanosec () - tick) >= 0)
            return false;
        ky_thread::yield();
    }
    return true;
}
// Try to lock the mutex. Return true on success, false otherwise.
bool ky_spinlock::trylock(int64 timeout)
{
    const int64 tick = ky_timer::nanosec ();
    while (!slock.compare_exchange(false, true) && timeout > 0)
    {
        if (timeout > 0 && (ky_timer::nanosec () - tick) >= 0)
            return false;
    }
    return timeout > 0 ? true : false;
}
// Unlock the mutex
bool ky_spinlock::unlock()
{
    return slock.compare_exchange(true, false);
}
#endif

/*
 * Read Write Lock
 *
 */
const pthread_rwlock_t ky_rwlock::_rw_initializer = PTHREAD_RWLOCK_INITIALIZER;


ky_rwlock::ky_rwlock():
    _rw(_rw_initializer)
{
    _is_init=  true;
    int e = pthread_rwlock_init(&_rw, NULL);
    if (e != 0)
        _is_init = false;
}
ky_rwlock::~ky_rwlock()
{
    if (_is_init)
        (void)pthread_rwlock_destroy(&_rw);
}

// Unlock the mutex
bool ky_rwlock::unlock()
{
    if (!_is_init) return false;
    int e = pthread_rwlock_unlock(&_rw);
    if (e != 0)
        return false;
    return true;
}
bool ky_rwlock::lockrd(int64 timeout)
{
    if (!_is_init) return false;
    if (timeout >= 0)
    {
        if (pthread_rwlock_rdlock(&_rw) != 0)
            return false;
    }
    else
    {
        struct timespec ts = ky_datetime::convert_timespec(timeout);
        if (pthread_rwlock_timedrdlock(&_rw, &ts) != 0)
            return false;
    }
    return true;
}
bool ky_rwlock::lockwr(int64 timeout)
{
    if (!_is_init) return false;
    if (timeout >= 0)
    {
        if (pthread_rwlock_wrlock(&_rw) != 0)
            return false;
    }
    else
    {
        struct timespec ts = ky_datetime::convert_timespec(timeout);
        if (pthread_rwlock_timedwrlock(&_rw, &ts) != 0)
            return false;
    }
    return true;
}
bool ky_rwlock::trylockrd(int64 timeout)
{
    if (!_is_init) return false;
    while ((timeout > 0) && (pthread_rwlock_tryrdlock(&_rw) != 0))
    {
        struct timespec ts = ky_datetime::convert_timespec(timeout);
        if (0 != pthread_rwlock_timedrdlock(&_rw, &ts))
            return false;
        else
            return true;
    }
    return (timeout > 0) ? true : (pthread_rwlock_tryrdlock(&_rw) == 0);

}
bool ky_rwlock::trylockwr(int64 timeout)
{
    if (!_is_init) return false;
    while ((timeout > 0) && (pthread_rwlock_trywrlock(&_rw) != 0))
    {
        struct timespec ts = ky_datetime::convert_timespec(timeout);
        if (0 != pthread_rwlock_timedwrlock(&_rw, &ts))
            return false;
        else
            return true;
    }
    return (timeout > 0) ? true : (pthread_rwlock_trywrlock(&_rw) == 0);
}

const pthread_mutex_t ky_mutex::_mutex_initializer = PTHREAD_MUTEX_INITIALIZER;

ky_mutex::ky_mutex(bool is_reentrant) :
    _mutex(_mutex_initializer)
{
    _is_init=  true;
    pthread_mutexattr_t *pattr = 0, attr;
    if (is_reentrant)
    {
        pattr = &attr;
        if((pthread_mutexattr_init(pattr)) != 0)
            log_err("create mutex reentrant error.");
        else
            pthread_mutexattr_settype(pattr, PTHREAD_MUTEX_RECURSIVE);
    }
    int e = pthread_mutex_init(&_mutex, pattr);
    if (e != 0)
        _is_init = false;

}

ky_mutex::~ky_mutex()
{
    if (_is_init)
    (void)pthread_mutex_destroy(&_mutex);
}

bool ky_mutex::lock(int64 timeout)
{
    if (!_is_init) return false;
    if (timeout <= 0)
    {
        if (0 != pthread_mutex_lock(&_mutex))
            return false;
    }
    else
    {
        struct timespec ts = ky_datetime::convert_timespec(timeout);
        if (0 != pthread_mutex_timedlock(&_mutex, &ts))
            return false;
    }
    return true;
}

bool ky_mutex::trylock(int64 timeout)
{
    if (!_is_init) return false;

    while ((timeout > 0) && (pthread_mutex_trylock(&_mutex) != 0))
    {
        struct timespec ts = ky_datetime::convert_timespec(timeout);
        if (0 != pthread_mutex_timedlock(&_mutex, &ts))
            return false;
        else
            return true;
    }
    return (timeout > 0) ? true : (pthread_mutex_trylock(&_mutex) == 0);
}

bool ky_mutex::unlock()
{
    if (!_is_init) return false;
    int e = pthread_mutex_unlock(&_mutex);
    if (e != 0)
        return false;
    return true;
}


ky_scopelock::ky_scopelock(ky_mutex *m):
    mutex(m)
{
    if (mutex)
        mutex->lock();
}
ky_scopelock::ky_scopelock(ky_mutex &m)
{
   mutex = &m;
   mutex->lock();
}
ky_scopelock::~ky_scopelock()
{
    if (mutex)
        mutex->unlock();
}

void ky_scopelock::unlock()
{
    if (mutex)
        mutex->unlock();
    mutex = 0;
}

const pthread_cond_t ky_condition::_cond_initializer = PTHREAD_COND_INITIALIZER;

ky_condition::ky_condition() :
    _mutex(0),
    _cond(_cond_initializer),
    _is_init(true)
{
    pthread_condattr_t condattr, *pattr = &condattr;
    if (pthread_condattr_init(&condattr) != 0)
        log_err("create condition attr error.");
#  if !kyOSIsWin32 ///> windows 未定义 CLOCK_MONOTONIC
    else if (pthread_condattr_setclock(&condattr, CLOCK_MONOTONIC) != 0)
        log_err("set condition clock MONOTONIC error.");
#  endif
    else
        pattr = 0;

    int e = pthread_cond_init(&_cond, pattr);
    if (e != 0)
        _is_init = false;

}

ky_condition::~ky_condition()
{
    if (!_is_init)
        return ;
    (void)pthread_cond_destroy(&_cond);
}

bool ky_condition::wait(ky_mutex& m, int64 timeout )
{
    if (!_is_init) return false;
    if( timeout == kyTimeoutIndefinite)
        return pthread_cond_wait(&_cond, &m._mutex) == 0;

    const timespec delta = ky_datetime::convert_timespec(timeout);
    if( pthread_cond_timedwait( &_cond, &m._mutex, &delta ) == ETIMEDOUT )
        return false;
    return true;
}

bool ky_condition::wake_one()
{
    if (!_is_init) return false;
    int e = pthread_cond_signal(&_cond);
    if (e != 0)
        return false;
    return true;
}

bool ky_condition::wake_all()
{
    if (!_is_init) return false;
    int e = pthread_cond_broadcast(&_cond);
    if (e != 0)
        return false;
    return true;
}
