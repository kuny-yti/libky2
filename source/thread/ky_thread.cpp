
#include <cerrno>
#include <cstring>
#include <pthread.h>
#include <sched.h>
#include <unistd.h>
#include <time.h>
#include <sched.h>
#include <errno.h>

#include "ky_thread.h"
#include "ky_lock.h"
#include "ky_object.h"
#include "tools/ky_datetime.h"
#include "tools/ky_map.h"
#include "tools/ky_stack.h"
#include "arch/ky_cpu.h"
#include "tools/ky_bitset.h"
#include "ky_debug.h"
#include "thread_dispatch.h"

#if kyOSIsLinux
#  include <sys/prctl.h>
#  include <linux/futex.h>
#  include <sys/syscall.h>
#  ifndef SCHED_IDLE
     // from linux/sched.h
#    define SCHED_IDLE    5
#  endif
#endif

#define MAX_CPUS  1024
// 全局共享处理器位掩码
struct cpu_affinity
{
    ky_bitset<MAX_CPUS/ sizeof(uintptr)*8, uintptr> prevmask;
    ky_bitset<MAX_CPUS/ sizeof(uintptr)*8, uintptr> sysmask;
    //!
    //! \brief has_affinity idx亲和力是否被占用， < 0是否有可用
    //! \param idx
    //! \return
    //!
    bool has_affinity(const int &idx = -1)const
    {
        if (idx >= 0 && idx < ky_cpu::count())
            return prevmask[idx];

        // 自动选择
        return find() >= 0;
    }
    int  find()const
    {
        for (uint i = 0; i < ky_cpu::count(); ++i)
            if (!prevmask[i])
                return i;
        return -1;
    }
#if kyOSIsLinux || kyOSIsUnix
    cpu_affinity()
    {
        // 获取系统启动前的设置
        uint mask[MAX_CPUS / (sizeof(uint)*8)];
        ky_memory::zero(&mask, sizeof(mask));
        kyASSERT(::syscall(__NR_sched_getaffinity, 0, sizeof(mask), &mask) == 0);

        for (uint i = 0; i < ky_cpu::count(); ++i)
        {
            const uint u = i / (sizeof(uint)*8);
            if(mask[u] & (1 << i))
                sysmask.set(i);
        }
        prevmask = sysmask;
    }
    ~cpu_affinity()
    {
        // 还原系统设置
        if (sysmask.any() || prevmask.any())
        {
            uint mask[MAX_CPUS / (sizeof(uint)*8)];
            ky_memory::zero(&mask, sizeof(mask));
            for (uint i = 0; i < ky_cpu::count(); ++i)
            {
                const uint u = i / (sizeof(uint)*8);
                const uint x = i % (sizeof(uint)*8);
                if(sysmask[i])
                    mask[u] |= (1 << x);
            }
            kyASSERT(::syscall(__NR_sched_setaffinity, 0, sizeof(mask), &mask) == 0);
        }
    }

    //!
    //! \brief en_thread_affinity 根据idx开启线程亲和力
    //! \param idx
    //! \return false 失败
    //!
    bool en_thread_affinity(const uint &idx)
    {
        uint mask[MAX_CPUS / (sizeof(uint)*8)];
        ky_memory::zero(&mask, sizeof(mask));
        for (uint i = 0; i < ky_cpu::count(); ++i)
        {
            const uint u = i / (sizeof(uint)*8);
            const uint x = i % (sizeof(uint)*8);
            if(prevmask[i] || (i == idx))
                mask[u] |= (1 << x);
        }
        if (::syscall(__NR_sched_setaffinity, 0, sizeof(mask), &mask) != 0)
            return false;
        prevmask.set(idx);
        return true;
    }
    //!
    //! \brief de_thread_affinity 根据idx关闭线程亲和力
    //! \param idx
    //! \return
    //!
    bool de_thread_affinity(const uint &idx)
    {
        if (!prevmask[idx])
            return false;

        prevmask.unset(idx);
        uint mask[MAX_CPUS / (sizeof(uint)*8)];
        ky_memory::zero(&mask, sizeof(mask));
        for (uint i = 0; i < ky_cpu::count(); ++i)
        {
            const uint u = i / (sizeof(uint)*8);
            const uint x = i % (sizeof(uint)*8);
            if(prevmask[i])
                mask[u] |= (1 << x);
        }
        if (::syscall(__NR_sched_setaffinity, 0, sizeof(mask), &mask) != 0)
            return false;
        return true;
    }
    //!
    //! \brief set_thread_affinity 自动选择设置
    //! \return >=0 返回成功的cpu索引，<0则失败
    //!
    int set_thread_affinity()
    {
        int sel_idx = -1;
        uint mask[MAX_CPUS / (sizeof(uint)*8)];
        ky_memory::zero(&mask, sizeof(mask));
        for (uint i = 0; i < ky_cpu::count(); ++i)
        {
            const uint u = i / (sizeof(uint)*8);
            const uint x = i % (sizeof(uint)*8);
            if(prevmask[i] || (sel_idx < 0))
            {
                mask[u] |= (1 << x);
                if (sel_idx < 0)
                    sel_idx = i;
            }
        }
        if (::syscall(__NR_sched_setaffinity, 0, sizeof(mask), &mask) != 0)
            return -1;
        prevmask.set(sel_idx);
        return sel_idx;
    }
#else
    cpu_affinity()
    {
        // 获取系统启动前的设置
        const uint size = CPU_ALLOC_SIZE(ky_cpu::count());
        cpu_set_t* set = CPU_ALLOC(ky_cpu::count());
        CPU_ZERO_S(size, set);
        kyASSERT(::sched_getaffinity(0, size, set) == 0);

        for (uint i = 0; i < ky_cpu::count(); ++i)
        {
            if(CPU_ISSET_S(i, size, set))
                sysmask.set(i);
        }
        prevmask = sysmask;
        CPU_FREE(set);
    }
    ~cpu_affinity()
    {
        // 还原系统设置
        if (sysmask.any() || prevmask.any())
        {
            const uint size = CPU_ALLOC_SIZE(ky_cpu::count());
            cpu_set_t* set = CPU_ALLOC(ky_cpu::count());
            CPU_ZERO_S(size, set);
            for (uint i = 0; i < ky_cpu::count(); ++i)
            {
                if(sysmask[i])
                    CPU_SET_S(i, size, set);
            }
            CPU_FREE(set);
            kyASSERT(::sched_setaffinity(0, size, set) == 0);
        }
    }

    //!
    //! \brief en_thread_affinity 根据idx开启线程亲和力
    //! \param idx
    //! \return false 失败
    //!
    bool en_thread_affinity(const uint &idx)
    {
        const uint size = CPU_ALLOC_SIZE(ky_cpu::count());
        cpu_set_t* set = CPU_ALLOC(ky_cpu::count());
        CPU_ZERO_S(size, set);
        for (uint i = 0; i < ky_cpu::count(); ++i)
        {
            if(prevmask[i] || (i == idx))
                CPU_SET_S(i, size, set);
        }
        if (::sched_setaffinity(0, size, set) != 0)
            return false;
        CPU_FREE(set);
        return true;
    }
    //!
    //! \brief de_thread_affinity 根据idx关闭线程亲和力
    //! \param idx
    //! \return
    //!
    bool de_thread_affinity(const uint &idx)
    {
        if (!prevmask[idx])
            return false;

        prevmask.unset(idx);
        const uint size = CPU_ALLOC_SIZE(ky_cpu::count());
        cpu_set_t* set = CPU_ALLOC(ky_cpu::count());
        CPU_ZERO_S(size, set);
        for (uint i = 0; i < ky_cpu::count(); ++i)
        {
            if(prevmask[i])
                CPU_SET_S(i, size, set);
        }
        if (::sched_setaffinity(0, size, set) != 0)
            return false;
        CPU_FREE(set);
        return true;
    }
    //!
    //! \brief set_thread_affinity 自动选择设置
    //! \return >=0 返回成功的cpu索引，<0则失败
    //!
    int set_thread_affinity()
    {
        int sel_idx = -1;
        const uint size = CPU_ALLOC_SIZE(ky_cpu::count());
        cpu_set_t* set = CPU_ALLOC(ky_cpu::count());
        CPU_ZERO_S(size, set);
        for (uint i = 0; i < ky_cpu::count(); ++i)
        {
            if(prevmask[i] || (sel_idx < 0))
            {
                CPU_SET_S(i, size, set);
                if (sel_idx < 0)
                    sel_idx = i;
            }
        }
        if (::sched_setaffinity(0, size, set) != 0)
            return false;
        CPU_FREE(set);
        prevmask.set(sel_idx);
        return sel_idx;
    }
#endif
};
static cpu_affinity __affinity;

const int ky_thread::priority_default = Priority_Idle;

static int policy_int(eThreadPolicys tp)
{
    return tp == Policy_Default ?
            #ifdef SCHED_IDLE
                SCHED_IDLE
            #else
                SCHED_OTHER
            #endif
              : tp == Policy_FIFO
              ? SCHED_FIFO : SCHED_RR;
}
static eThreadPolicys int_policy (int v)
{
    return v ==
        #ifdef SCHED_IDLE
            SCHED_IDLE
        #else
            SCHED_OTHER
        #endif
            ? Policy_Default : v == SCHED_FIFO
            ? Policy_FIFO : Policy_RoundRobin;
}


#ifdef hasThreadLocalStorage
struct tThreadTlss
{
    static __thread tThreadLocalStorages *tls_tdata ;
    static pthread_once_t thread_data_once ;
    static pthread_key_t thread_data_key;

    static void set(tThreadLocalStorages *data)
    {
#ifdef hasThreadLocalStorage
        tls_tdata = data;
#endif
        pthread_once(&thread_data_once, create_thread_data_key);
        pthread_setspecific(thread_data_key, data);
    }
    static tThreadLocalStorages * get()
    {
#ifdef hasThreadLocalStorage
        return tls_tdata;
#else
        pthread_once(&thread_data_once, create_thread_data_key);
        return reinterpret_cast<tThreadDatas *>(pthread_getspecific(thread_data_key));
#endif
    }
    static void clear()
    {
#ifdef hasThreadLocalStorage
        tls_tdata = 0;
#endif
        pthread_setspecific(thread_data_key, 0);
    }

    static void destroy_thread_data_key()
    {
        pthread_once(&thread_data_once, create_thread_data_key);
        pthread_key_delete(thread_data_key);

        pthread_once_t pthread_once_init = PTHREAD_ONCE_INIT;
        thread_data_once = pthread_once_init;
    }
    static void create_thread_data_key()
    {
        pthread_key_create(&thread_data_key, destroy_thread_data);
    }
    static void destroy_thread_data(void *p)
    {
        pthread_setspecific(thread_data_key, p);
        tThreadLocalStorages *data = static_cast<tThreadLocalStorages *>(p);
        if (data->lessref ())
            data->destroy ();

        pthread_setspecific(thread_data_key, 0);
    }
};
__thread tThreadLocalStorages *tThreadTlss::tls_tdata = NULL;
pthread_once_t tThreadTlss::thread_data_once  = PTHREAD_ONCE_INIT;
pthread_key_t tThreadTlss::thread_data_key;

static tThreadTlss gttls;
void ky_thread::local_storage_set (tThreadLocalStorages * td)
{
    gttls.set (td);
}
tThreadLocalStorages *ky_thread::local_storage_get ()
{
    return gttls.get ();
}
void ky_thread::local_storage_clear()
{
    gttls.clear ();
}
#endif


ky_thread::ky_thread() :
    __thread_id(0),
    __joinable(false),
    __running(false)
{
    dispatch = kyNew(thread_dispatch(this));
    __priority = (eThreadPrioritys)priority_default;
    __policy = Policy_Default;

    pthread_attr_t thread_attr;
    struct sched_param param_;
    pthread_attr_init(&thread_attr);

    pthread_attr_getschedpolicy(&thread_attr, (int*)&__policy);
    pthread_attr_getschedparam(&thread_attr, &param_);
    __priority = (eThreadPrioritys)param_.sched_priority;
    pthread_attr_destroy(&thread_attr);

    __priority_max = sched_get_priority_max(__policy);
    __priority_min = sched_get_priority_min(__policy);

    __policy = int_policy(__policy);
    __process_idx = 0;
}

ky_thread::~ky_thread()
{
    if (__joinable)
        (void)pthread_detach(__thread_id);

    thread_dispatch::global_thread_list.remove (__thread_id);
    kyDelete(dispatch);
}

void* ky_thread::__run(void* p)
{
    ky_thread* t = static_cast<ky_thread*>(p);

    // 线程开启CPU亲和力，可自动选择
    bool have_aff = t->__process_idx != 0;
    if (!have_aff && (t->__policy == Policy_Affinity))
        have_aff = t->set_affinity();

    if (have_aff)
        t->en_thread_affinity(t->__process_idx-1);

    t->run();

    // 还原
    if (have_aff)
    {
        t->de_thread_affinity(t->__process_idx-1);
        if (t->__policy == Policy_Affinity)
            t->__process_idx = 0;
    }
    t->__running = false;
    return 0;
}
bool ky_thread::has_affinity()const
{
    return __affinity.has_affinity();
}
bool ky_thread::is_affinity()const
{
    return __process_idx != 0;
}
bool ky_thread::set_affinity(const uint &idx)
{
    if (!has_affinity())
        return false;

    if (idx != 0)
    {
        if (__affinity.has_affinity(idx))
        {
            __process_idx = idx;
            return true;
        }
        return false;
    }
    __process_idx = __affinity.find() +1;
    return __process_idx >= 0;
}

bool ky_thread::en_thread_affinity(const uint &idx)
{
    return __affinity.en_thread_affinity(idx);
}
bool ky_thread::de_thread_affinity(const uint &idx)
{
    return __affinity.de_thread_affinity(idx);
}
bool ky_thread::set_policy(eThreadPolicys tp)
{
    if (tp != __policy)
    {
        if (!is_running())
        {
            __policy = tp;
            return true;
        }
        log_warning("Cannot set policy, thread is running");
        return false;
    }
    return true;
}
eThreadPolicys ky_thread::policy()const
{
    return __policy;
}

bool ky_thread::set_priority(eThreadPrioritys prio)
{
    int sch_level = 0;
    int sch_policy =
        #ifdef SCHED_IDLE
            SCHED_IDLE;
        #else
            SCHED_OTHER;
        #endif
    sched_param param;

    ky_scopelock mlock(&dispatch->mutex);kyUnused2(mlock);

    if (!is_running ())
    {
        log_warning("Cannot set priority, thread is not running");
        return false;
    }

#ifdef SCHED_IDLE
    if (prio == Priority_Idle)
    {
        set_policy (Policy_Default);
        sch_policy = SCHED_IDLE; // SCHED_IDLE
        sch_level = 0;
    }

    const int lowestPriority = Priority_Lowest;
#else
    const int lowestPriority = Priority_Idle;
#endif
    const int highestPriority = Priority_TimeCritical;

    sch_policy = policy_int(policy());
    if (pthread_getschedparam(__thread_id, &sch_policy, &param) != 0)
    {
        log_warning("Cannot get scheduler parameters");
        return false;
    }
    __priority_max = sched_get_priority_max(sch_policy);
    __priority_min = sched_get_priority_min(sch_policy);
    if (__priority_min == -1 || __priority_max == -1)
        return false;
    if (prio != Priority_Idle)
    {
        sch_level = ((prio - lowestPriority) * (__priority_max - __priority_min) / highestPriority) + __priority_min;
        sch_level = ky_bound(__priority_min, sch_level, __priority_max);
    }

    param.sched_priority = sch_level;
    int status = pthread_setschedparam(__thread_id, sch_policy, &param);
# ifdef SCHED_IDLE
    // were we trying to set to idle priority and failed?
    if (status == -1 && sch_policy == SCHED_IDLE && errno == EINVAL)
    {
        pthread_getschedparam(__thread_id, &sch_policy, &param);
        param.sched_priority = sched_get_priority_min(sch_policy);
        pthread_setschedparam(__thread_id, sch_policy, &param);
        return false;
    }
# else
    if (status != 0)
        return false;
# endif

    __priority = prio;
    return true;
}
eThreadPrioritys ky_thread::priority()const
{
    return __priority;
}
bool ky_thread::start(eThreadPolicys tp)
{
    if (__running.compare_exchange (false, true))
    {
        wait();
        pthread_attr_t priority_thread_attr;
        struct sched_param param_;

        pthread_attr_init(&priority_thread_attr);
        pthread_attr_setdetachstate(&priority_thread_attr, PTHREAD_CREATE_DETACHED);

        // set policy
        if (policy() != tp || tp != Policy_Default)
        {
            if(pthread_attr_setschedpolicy(&priority_thread_attr, policy_int (__policy)) == 0)
            {
                __priority_max = sched_get_priority_max(policy_int (__policy));
                __priority_min = sched_get_priority_min(policy_int (__policy));
                __policy = tp;
            }
        }
        else
        {
            int sch_policy = Policy_Default;
            if (pthread_attr_getschedpolicy(&priority_thread_attr, &sch_policy) != 0)
            {
#ifdef kyHasDebug
                log_warning("Cannot determine default scheduler policy");
#endif
            }
            else
                __policy = int_policy (sch_policy);
        }

        // set priority
        if (priority() == Priority_Inherit)
            pthread_attr_setinheritsched(&priority_thread_attr, PTHREAD_INHERIT_SCHED);
        else
        {

#if SCHED_IDLE
                const int lowestPriority = Priority_Lowest;
#else
                const int lowestPriority = Priority_Idle;
#endif
                const int highestPriority = Priority_TimeCritical;
                if (__priority_min == -1 || __priority_max == -1)
                    param_.sched_priority = 0;
                else
                {
                    int level = ((priority() - lowestPriority) *
                                 (__priority_max - __priority_min) /
                                 highestPriority) + __priority_min;
                    param_.sched_priority = ky_bound(__priority_min, level,
                                                     __priority_max);
                }

                if ((pthread_attr_setinheritsched(&priority_thread_attr,
                                   PTHREAD_EXPLICIT_SCHED) != 0) ||
                     (pthread_attr_setschedparam(&priority_thread_attr, &param_)
                         != 0))
                {
                    pthread_attr_setinheritsched(&priority_thread_attr,
                                                 PTHREAD_INHERIT_SCHED);
                    set_priority(Priority_Idle);
                }
        }

        // create thread
        int e = pthread_create(&__thread_id, &priority_thread_attr, __run, this);
        if (e == EPERM)
        {
            pthread_attr_setinheritsched(&priority_thread_attr, PTHREAD_INHERIT_SCHED);
            e = pthread_create(&__thread_id, &priority_thread_attr, __run, this);
        }
        pthread_attr_destroy(&priority_thread_attr);
        if (e != 0)
        {
            __running = false;
            return false;
        }

        thread_dispatch::global_thread_list[__thread_id] = this;
        __joinable = true;
    }
    return true;
}

bool ky_thread::wait(int64 timeout)
{
    ky_scopelock alock(&dispatch->mutex);kyUnused2(alock);
    if (id() == current_id ())
    {
#ifdef kyHasDebug
        log_warning("Thread tried to wait on itself");
#endif
        return false;
    }

    if (!this->is_running ())
        return true;

    if (timeout != kyTimeoutIndefinite)
    {
        while (this->is_running ())
        {
            if (!dispatch->wait_cond.wait(dispatch->mutex, timeout))
                return false;
        }
        return true;
    }
    else if (__joinable.compare_exchange (true, false))
    {
        int e = pthread_join(__thread_id, NULL);
        if (e != 0)
            return false;
    }
    return true;
}

bool ky_thread::finish()
{
    __running = false;
    return wait();
}
thread_id ky_thread::id()const
{
    return __thread_id;
}
bool ky_thread::operator != (const ky_thread &rhs)const
{
    return pthread_equal(__thread_id, rhs.__thread_id) == 0;
}
bool ky_thread::operator == (const ky_thread &rhs)const
{
    return pthread_equal(__thread_id, rhs.__thread_id) != 0;
}
bool ky_thread::cancel()
{
    dispatch->mutex.lock();
    int e = pthread_cancel(__thread_id);
    if (e != 0)
    {
        dispatch->mutex.unlock();
        return false;
    }
    dispatch->mutex.unlock();
    return true;
}

void ky_thread::quit()
{
    exit(0);
}
void ky_thread::exit(int code)
{
    dispatch->mutex.lock ();

    dispatch->exited = false;
    dispatch->exit_code = code;
    dispatch->req_quit = true;

    dispatch->mutex.unlock ();
}
int ky_thread::exec()
{
    // 刚进入请求退出
    dispatch->mutex.lock ();
    if (dispatch->req_quit)
    {
        dispatch->req_quit = false;
        dispatch->exited = true;
        dispatch->mutex.unlock();
        return dispatch->exit_code;
    }
    dispatch->exited = false;
    dispatch->exit_code = 0;
    dispatch->mutex.unlock ();

    // 进行派遣
    const int ret_code = dispatch->dispatcher ();

    // 设置退出标志
    dispatch->mutex.lock ();
    dispatch->exited = true;
    dispatch->exit_code = 0;
    dispatch->mutex.unlock ();

    return ret_code;
}

void ky_thread::yield()
{
#if kyCompilerIsMSVC
    SwitchToThread();
    //::Sleep( 0 ); // sleeps thread
#else
#   if kyOSIsApple
    ::pthread_yield_np();
#   else
    ::sched_yield();
#   endif
#endif
}
void ky_thread::sleep(uint s)
{
#if kyOSIsWin32
    ::Sleep(s * 1000);
#else
#   if kyOSIsUnix || kyOSIsAndroid || kyOSIsLinux
    ::sleep(s);
#   endif
#endif
}
void ky_thread::msleep(ulong ms)
{
#if kyOSIsWin32
    ::Sleep(ms);
#elif kyOSIsUnix || kyOSIsAndroid || kyOSIsLinux
    ::usleep(ms*1000);
#endif

}

void ky_thread::usleep(int64 um)
{
#if kyOSIsWin32
    ::Sleep((um / 1000) <= 0 ? 1 : um / 1000);
#else
#   if kyOSIsUnix || kyOSIsAndroid || kyOSIsLinux
    ::usleep(um);
#   endif
#endif
}
thread_id ky_thread::current_id()
{
#if kyOSIsLinux
    return syscall(__NR_gettid);
#elif kyOSIsAndroid
    return gettid();
#else
    return pthread_self();
#endif
}
process_id ky_thread::pid()
{
    return getpid();
}
ky_thread *ky_thread::current()
{
    thread_id ids = current_id();
    if (thread_dispatch::global_thread_list.contains(ids))
        return thread_dispatch::global_thread_list.value (ids);
    return 0;
}


ky_threadsafe::ky_threadsafe()
{
    valid_id.store (kyThreadInvalid);
    ensure_id_valid();
}
bool ky_threadsafe::is_safe()
{
    if (valid_id.load () == kyThreadInvalid)
        valid_id.store (ky_thread::current_id());
    return valid_id.load () == ky_thread::current_id ();
}
bool ky_threadsafe::move_thread(ky_thread *t)
{
    valid_id.store (t->id ());
    return valid_id.load () == ky_thread::current_id ();
}
void ky_threadsafe::detach()
{
    valid_id.store (kyThreadInvalid);
}
void ky_threadsafe::ensure_id_valid()
{
    if (valid_id.load () != kyThreadInvalid)
        return;
    valid_id.store (ky_thread::current_id());
}
