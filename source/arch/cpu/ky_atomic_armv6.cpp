
#include "arch/ky_atomic.h"

#define byte1_t int8
#define byte2_t int16
#define byte4_t int32
#define byte8_t int64

#define asVolatile(T) T volatile * volatile byte = (T volatile * volatile)_v

#if kyArchIsARM
struct arch_arm
{
    static void pause(void)
    {
        kyInlineASM ("" ::: "memory");
    }
    static void memory_fence(eMemoryFences fence = Fence_Acquire)
    {
        kyCompilerBarrier();
        switch (fence)
        {
        case Fence_Relaxed:
            break;
        case Fence_Acquire:
#ifndef kyHasMemoryBarrierSMP
            kyMemoryBarrierReadOnly();
#else
            kyMemoryBarrierReadOnlySMP();
#endif
            break;
        case Fence_Release:
#ifndef kyHasMemoryBarrierSMP
            kyMemoryBarrierWriteOnly();
#else
            kyMemoryBarrierWriteOnlySMP();
#endif
            break;
        case Fence_AcquireRelease:
#ifndef kyHasMemoryBarrierSMP
            kyMemoryBarrierReadWrite();
#else
            kyMemoryBarrierReadWriteSMP();
#endif
            break;
        }
    }
};

struct atomic_arch_1 : public arch_arm
{
    static void load(void volatile *volatile _v, void * val, eMemoryFences fence = Fence_Relaxed)
    {
        asVolatile(byte1_t);
        byte1_t *nval = (byte1_t*)val;
        memory_fence(fence);
        kyInlineASM
                (
                    "ldrexb %[nval], [%[byte]]"
                    : [nval]"=&r" (*nval)
                    : [byte]"r" (byte)
                    : "memory"
                );
    }
    static void store(void volatile *volatile _v, void * nv, eMemoryFences  fence = Fence_Relaxed)
    {
        asVolatile(byte1_t);
        byte1_t *nval = (byte1_t*)nv;
        register byte1_t flag;

        memory_fence(fence);
        kyInlineASM
                (
                    "0: \n"
                    "ldrexb %[flag], [%[byte]]\n"
                    "strexb %[flag], %[nval], [%[byte]]\n"
                    "teq %[flag], #0\n"
                    "bne 0b\n"
                    : [flag]"=&r" (flag), "+m" (*byte)
                    : [byte]"r" (byte), [nval]"r" (*nval)
                    :  "cc", "memory"
                 );
        memory_fence(fence);
    }
    static bool increase(void volatile *volatile _v)
    {
        asVolatile(byte1_t);
        register byte1_t nv;
        register byte1_t temp;
        kyInlineASM
                (
                    "0:\n"
                    "ldrexb %[nv], [%[byte]]\n"
                    "add %[nv], %[nv], #1\n"
                    "strexb %[temp], %[nv], [%[byte]]\n"
                    "teq %[temp], #0\n"
                    "bne 0b\n"
                    : [nv]"=&r" (nv), [temp]"=&r" (temp), "+m" (*byte)
                    : [byte]"r" (byte)
                    : "cc", "memory"
                );
        return nv != 0;
    }
    static bool reduction(void volatile *volatile _v)
    {
        asVolatile(byte1_t);
        register byte1_t nv;
        register byte1_t temp;
        kyInlineASM
                (
                    "0:\n"
                    "ldrexb %[nv], [%[byte]]\n"
                    "sub %[nv], %[nv], #1\n"
                    "strexb %[temp], %[nv], [%[byte]]\n"
                    "teq %[temp], #0\n"
                    "bne 0b\n"
                    : [nv] "=&r" (nv), [temp] "=&r" (temp), "+m" (*byte)
                    : [byte] "r" (byte)
                    : "cc", "memory"
                );
        return nv != 0;
    }
    static void fetch_store(void volatile *volatile _v, void * nfv, void * nnv)
    {
        asVolatile(byte1_t);
        byte1_t *fv = (byte1_t*)nfv;
        byte1_t *nv = (byte1_t*)nnv;
        register byte1_t temp;
        kyInlineASM
                (
                    "0:\n"
                    "ldrexb %[temp], [%[nv]] \n"
                    "ldrexb %[fv], [%[byte]] \n"
                    "strex %[temp], %[temp], [%[byte]] \n"
                    "teq %[temp], #0\n"
                    "bne 0b\n"
                    : [fv] "=&r" (*fv), [temp] "=&r" (temp), "+m" (*byte)
                    : [nv] "r" (nv), [byte] "r" (byte)
                    : "cc"
                );
    }
    static void fetch_add(void volatile *volatile _v, void * nfv, void * nva)
    {
        asVolatile(byte1_t);
        byte1_t *fv = (byte1_t*)nfv;
        *fv = *byte;
        logic_add(_v, nva);
    }
    static bool compare_exchange(void volatile *volatile _v, void * nev, void * nnv)
    {
        asVolatile(byte1_t);
        byte1_t *ev = (byte1_t*)nev;
        byte1_t *nv = (byte1_t*)nnv;
        register byte1_t t1, t2, t3;
        kyInlineASM
                (
                    "0:\n"
                    "ldrexb %[t1], [%[ev]]\n"
                    "ldrexb %[t2], [%[nv]]\n"
                    "ldrexb %[t3], [%[byte]]\n"
                    "eors %[t3], %[t3], %[t1]\n"
                    "itt eq\n"
                    "strexbeq %[t3], %[t2], [%[byte]]\n"
                    "teqeq %[t3], #1\n"
                    "beq 0b\n"
                    : [t1] "=&r" (t1), [t2] "=&r" (t2), [t3] "=&r" (t3),"+m" (*byte)
                    : [ev] "r" (ev), [nv] "r" (nv), [byte] "r" (byte)
                    : "cc"
                );
        return t3 == 0;
    }

    static void logic_and(void volatile *volatile _v, void *nva)
    {
        asVolatile(byte1_t);
        byte1_t *va = (byte1_t*)nva;
        register byte1_t nv;
        register byte1_t temp;
        kyInlineASM
                (
                    "0:\n"
                    "ldrexb %[nv], [%[va]]\n"
                    "ldrexb %[temp], [%[byte]]\n"
                    "and %[nv], %[temp], %[nv]\n"
                    "strexb %[temp], %[nv], [%[byte]]\n"
                    "teq %[temp], #0\n"
                    "bne 0b\n"
                    : [nv] "=&r" (nv), [temp] "=&r" (temp), "+m" (*byte)
                    : [va] "r" (va), [byte] "r" (byte)
                    : "cc"
                );
    }
    static void logic_or(void volatile *volatile _v, void * nva)
    {
        asVolatile(byte1_t);
        byte1_t *va = (byte1_t*)nva;
        register byte1_t nv;
        register byte1_t temp;
        kyInlineASM
                (
                    "0:\n"
                    "ldrexb %[nv], [%[va]]\n"
                    "ldrexb %[temp], [%[byte]]\n"
                    "orr %[nv], %[temp], %[nv]\n"
                    "strexb %[temp], %[nv], [%[byte]]\n"
                    "teq %[temp], #0\n"
                    "bne 0b\n"
                    : [nv] "=&r" (nv), [temp] "=&r" (temp), "+m" (*byte)
                    : [va] "r" (va), [byte] "r" (byte)
                    : "cc"
                );
    }
    static void logic_xor(void volatile *volatile _v, void * nva)
    {
        asVolatile(byte1_t);
        byte1_t *va = (byte1_t*)nva;
        register byte1_t nv;
        register byte1_t temp;
        kyInlineASM
                (
                    "0:\n"
                    "ldrexb %[nv], [%[va]]\n"
                    "ldrexb %[temp], [%[byte]]\n"
                    "eor %[nv], %[temp], %[nv]\n"
                    "strexb %[temp], %[nv], [%[byte]]\n"
                    "teq %[temp], #0\n"
                    "bne 0b\n"
                    : [nv] "=&r" (nv), [temp] "=&r" (temp), "+m" (*byte)
                    : [va] "r" (va), [byte] "r" (byte)
                    : "cc"
                );
    }
    static void logic_add(void volatile *volatile _v, void * nva)
    {
        asVolatile(byte1_t);
        byte1_t *va = (byte1_t*)nva;
        register byte1_t nv;
        register byte1_t temp;
        kyInlineASM
                (
                    "0:\n"
                    "ldrexb %[temp], [%[byte]]\n"
                    "add %[nv], %[temp], %[va]\n"
                    "strexb %[temp], %[nv], [%[byte]]\n"
                    "teq %[temp], #0\n"
                    "bne 0b\n"
                    : [nv] "=&r" (nv), [temp] "=&r" (temp), "+m" (*byte)
                    : [va] "r" (*va), [byte] "r" (byte)
                    : "cc"
                );
    }
    static void logic_not(void volatile *volatile _v)
    {
        asVolatile(byte1_t);
        register byte1_t nv;
        register byte1_t temp;
        kyInlineASM
                (
                    "0:\n"
                    "ldrexb %[nv], [%[byte]]\n"
                    "mvn %[nv], %[nv]\n"
                    "strexb %[temp], %[nv], [%[byte]]\n"
                    "teq %[temp], #0\n"
                    "bne 0b\n"
                    : [nv] "=&r" (nv), [temp] "=&r" (temp), "+m" (*byte)
                    : [byte] "r" (byte)
                    : "cc"
                );
    }
    static void logic_inc(void volatile *volatile _v)
    {
        byte1_t onw = 1;
        logic_add(_v, &onw );
    }
    static void logic_dec(void volatile *volatile _v)
    {
        byte1_t onw = -1;
        logic_add(_v, &onw );
    }
};
static const ky_atomic_arch arch_arm_1 =
{
    .load             = atomic_arch_1::load,
    .store            = atomic_arch_1::store ,
    .increase         = atomic_arch_1::increase,
    .reduction        = atomic_arch_1::reduction,
    .fetch_store      = atomic_arch_1::fetch_store,
    .fetch_add        = atomic_arch_1::fetch_add,
    .compare_exchange = atomic_arch_1::compare_exchange,
    .logic_and        = atomic_arch_1::logic_and,
    .logic_or         = atomic_arch_1::logic_or,
    .logic_xor        = atomic_arch_1::logic_xor,
    .logic_add        = atomic_arch_1::logic_add,
    .logic_not        = atomic_arch_1::logic_not,
    .logic_inc        = atomic_arch_1::logic_inc,
    .logic_dec        = atomic_arch_1::logic_dec,
    .pause            = atomic_arch_1::pause,
    .memory_fence     = atomic_arch_1::memory_fence
};

struct atomic_arch_2 : public arch_arm
{
    static void load(void volatile *volatile _v, void * val, eMemoryFences fence = Fence_Relaxed)
    {
        asVolatile(byte2_t);
        byte2_t *nval = (byte2_t*)val;
        memory_fence(fence);
        kyInlineASM
                (
                    "ldrexh %[nval], [%[byte]]"
                    : [nval]"=&r" (*nval)
                    : [byte]"r" (byte)
                    : "memory"
                );
    }
    static void store(void volatile *volatile _v, void * nv, eMemoryFences  fence = Fence_Relaxed)
    {
        asVolatile(byte2_t);
        byte2_t *nval = (byte2_t*)nv;
        register byte2_t flag;

        memory_fence(fence);
        kyInlineASM
                (
                    "0: \n"
                    "ldrexh %[flag], [%[byte]]\n"
                    "strexh %[flag], %[nval], [%[byte]]\n"
                    "teq %[flag], #0\n"
                    "bne 0b\n"
                    : [flag]"=&r" (flag), "+m" (*byte)
                    : [byte]"r" (byte), [nval]"r" (*nval)
                    :  "cc", "memory"
                 );
        memory_fence(fence);
    }
    static bool increase(void volatile *volatile _v)
    {
        asVolatile(byte2_t);
        register byte2_t nv;
        register byte2_t temp;
        kyInlineASM
                (
                    "0:\n"
                    "ldrexh %[nv], [%[byte]]\n"
                    "add %[nv], %[nv], #1\n"
                    "strexh %[temp], %[nv], [%[byte]]\n"
                    "teq %[temp], #0\n"
                    "bne 0b\n"
                    : [nv]"=&r" (nv), [temp]"=&r" (temp), "+m" (*byte)
                    : [byte]"r" (byte)
                    : "cc", "memory"
                );
        return nv != 0;
    }
    static bool reduction(void volatile *volatile _v)
    {
        asVolatile(byte2_t);
        register byte2_t nv;
        register byte2_t temp;
        kyInlineASM
                (
                    "0:\n"
                    "ldrexh %[nv], [%[byte]]\n"
                    "sub %[nv], %[nv], #1\n"
                    "strexh %[temp], %[nv], [%[byte]]\n"
                    "teq %[temp], #0\n"
                    "bne 0b\n"
                    : [nv] "=&r" (nv), [temp] "=&r" (temp), "+m" (*byte)
                    : [byte] "r" (byte)
                    : "cc", "memory"
                );
        return nv != 0;
    }
    static void fetch_store(void volatile *volatile _v, void * nfv, void * nnv)
    {
        asVolatile(byte2_t);
        byte2_t *fv = (byte2_t*)nfv;
        byte2_t *nv = (byte2_t*)nnv;
        register byte2_t temp;
        kyInlineASM
                (
                    "0:\n"
                    "ldrexh %[temp], [%[nv]] \n"
                    "ldrexh %[fv], [%[byte]] \n"
                    "strex %[temp], %[temp], [%[byte]] \n"
                    "teq %[temp], #0\n"
                    "bne 0b\n"
                    : [fv] "=&r" (*fv), [temp] "=&r" (temp), "+m" (*byte)
                    : [nv] "r" (nv), [byte] "r" (byte)
                    : "cc"
                );
    }
    static void fetch_add(void volatile *volatile _v, void * nfv, void * nva)
    {
        asVolatile(byte2_t);
        byte2_t *fv = (byte2_t*)nfv;
        *fv = *byte;
        logic_add(_v, nva);
    }
    static bool compare_exchange(void volatile *volatile _v, void * nev, void * nnv)
    {
        asVolatile(byte2_t);
        byte2_t *ev = (byte2_t*)nev;
        byte2_t *nv = (byte2_t*)nnv;
        register byte2_t t1, t2, t3;
        kyInlineASM
                (
                    "0:\n"
                    "ldrexh %[t1], [%[ev]]\n"
                    "ldrexh %[t2], [%[nv]]\n"
                    "ldrexh %[t3], [%[byte]]\n"
                    "eors %[t3], %[t3], %[t1]\n"
                    "itt eq\n"
                    "strexbeq %[t3], %[t2], [%[byte]]\n"
                    "teqeq %[t3], #1\n"
                    "beq 0b\n"
                    : [t1] "=&r" (t1), [t2] "=&r" (t2), [t3] "=&r" (t3),"+m" (*byte)
                    : [ev] "r" (ev), [nv] "r" (nv), [byte] "r" (byte)
                    : "cc"
                );
        return t3 == 0;
    }

    static void logic_and(void volatile *volatile _v, void *nva)
    {
        asVolatile(byte2_t);
        byte2_t *va = (byte2_t*)nva;
        register byte2_t nv;
        register byte2_t temp;
        kyInlineASM
                (
                    "0:\n"
                    "ldrexh %[nv], [%[va]]\n"
                    "ldrexh %[temp], [%[byte]]\n"
                    "and %[nv], %[temp], %[nv]\n"
                    "strexh %[temp], %[nv], [%[byte]]\n"
                    "teq %[temp], #0\n"
                    "bne 0b\n"
                    : [nv] "=&r" (nv), [temp] "=&r" (temp), "+m" (*byte)
                    : [va] "r" (va), [byte] "r" (byte)
                    : "cc"
                );
    }
    static void logic_or(void volatile *volatile _v, void * nva)
    {
        asVolatile(byte2_t);
        byte2_t *va = (byte2_t*)nva;
        register byte2_t nv;
        register byte2_t temp;
        kyInlineASM
                (
                    "0:\n"
                    "ldrexh %[nv], [%[va]]\n"
                    "ldrexh %[temp], [%[byte]]\n"
                    "orr %[nv], %[temp], %[nv]\n"
                    "strexh %[temp], %[nv], [%[byte]]\n"
                    "teq %[temp], #0\n"
                    "bne 0b\n"
                    : [nv] "=&r" (nv), [temp] "=&r" (temp), "+m" (*byte)
                    : [va] "r" (va), [byte] "r" (byte)
                    : "cc"
                );
    }
    static void logic_xor(void volatile *volatile _v, void * nva)
    {
        asVolatile(byte2_t);
        byte2_t *va = (byte2_t*)nva;
        register byte2_t nv;
        register byte2_t temp;
        kyInlineASM
                (
                    "0:\n"
                    "ldrexh %[nv], [%[va]]\n"
                    "ldrexh %[temp], [%[byte]]\n"
                    "eor %[nv], %[temp], %[nv]\n"
                    "strexh %[temp], %[nv], [%[byte]]\n"
                    "teq %[temp], #0\n"
                    "bne 0b\n"
                    : [nv] "=&r" (nv), [temp] "=&r" (temp), "+m" (*byte)
                    : [va] "r" (va), [byte] "r" (byte)
                    : "cc"
                );
    }
    static void logic_add(void volatile *volatile _v, void * nva)
    {
        asVolatile(byte2_t);
        byte2_t *va = (byte2_t*)nva;
        register byte2_t nv;
        register byte2_t temp;
        kyInlineASM
                (
                    "0:\n"
                    "ldrexh %[temp], [%[byte]]\n"
                    "add %[nv], %[temp], %[va]\n"
                    "strexh %[temp], %[nv], [%[byte]]\n"
                    "teq %[temp], #0\n"
                    "bne 0b\n"
                    : [nv] "=&r" (nv), [temp] "=&r" (temp), "+m" (*byte)
                    : [va] "r" (*va), [byte] "r" (byte)
                    : "cc"
                );
    }
    static void logic_not(void volatile *volatile _v)
    {
        asVolatile(byte2_t);
        register byte2_t nv;
        register byte2_t temp;
        kyInlineASM
                (
                    "0:\n"
                    "ldrexh %[nv], [%[byte]]\n"
                    "mvn %[nv], %[nv]\n"
                    "strexh %[temp], %[nv], [%[byte]]\n"
                    "teq %[temp], #0\n"
                    "bne 0b\n"
                    : [nv] "=&r" (nv), [temp] "=&r" (temp), "+m" (*byte)
                    : [byte] "r" (byte)
                    : "cc"
                );
    }
    static void logic_inc(void volatile *volatile _v)
    {
        byte2_t onw = 1;
        logic_add(_v, &onw );
    }
    static void logic_dec(void volatile *volatile _v)
    {
        byte2_t onw = -1;
        logic_add(_v, &onw );
    }
};
static const ky_atomic_arch arch_arm_2 =
{
    .load             = atomic_arch_2::load,
    .store            = atomic_arch_2::store ,
    .increase         = atomic_arch_2::increase,
    .reduction        = atomic_arch_2::reduction,
    .fetch_store      = atomic_arch_2::fetch_store,
    .fetch_add        = atomic_arch_2::fetch_add,
    .compare_exchange = atomic_arch_2::compare_exchange,
    .logic_and        = atomic_arch_2::logic_and,
    .logic_or         = atomic_arch_2::logic_or,
    .logic_xor        = atomic_arch_2::logic_xor,
    .logic_add        = atomic_arch_2::logic_add,
    .logic_not        = atomic_arch_2::logic_not,
    .logic_inc        = atomic_arch_2::logic_inc,
    .logic_dec        = atomic_arch_2::logic_dec,
    .pause            = atomic_arch_2::pause,
    .memory_fence     = atomic_arch_2::memory_fence
};

struct atomic_arch_4 : public arch_arm
{
    static void load(void volatile *volatile _v, void * val, eMemoryFences fence = Fence_Relaxed)
    {
        asVolatile(byte4_t);
        byte4_t *nval = (byte4_t*)val;
        memory_fence(fence);
        kyInlineASM
                (
                    "ldrex %[nval], [%[byte]]"
                    : [nval]"=&r" (*nval)
                    : [byte]"r" (byte)
                    : "memory"
                );
    }
    static void store(void volatile *volatile _v, void * nv, eMemoryFences  fence = Fence_Relaxed)
    {
        asVolatile(byte4_t);
        byte4_t *nval = (byte4_t*)nv;
        register byte4_t flag;

        memory_fence(fence);
        kyInlineASM
                (
                    "0: \n"
                    "ldrex %[flag], [%[byte]]\n"
                    "strex %[flag], %[nval], [%[byte]]\n"
                    "teq %[flag], #0\n"
                    "bne 0b\n"
                    : [flag]"=&r" (flag), "+m" (*byte)
                    : [byte]"r" (byte), [nval]"r" (*nval)
                    :  "cc", "memory"
                 );
        memory_fence(fence);
    }
    static bool increase(void volatile *volatile _v)
    {
        asVolatile(byte4_t);
        register byte4_t nv;
        register byte4_t temp;
        kyInlineASM
                (
                    "0:\n"
                    "ldrex %[nv], [%[byte]]\n"
                    "add %[nv], %[nv], #1\n"
                    "strex %[temp], %[nv], [%[byte]]\n"
                    "teq %[temp], #0\n"
                    "bne 0b\n"
                    : [nv]"=&r" (nv), [temp]"=&r" (temp), "+m" (*byte)
                    : [byte]"r" (byte)
                    : "cc", "memory"
                );
        return nv != 0;
    }
    static bool reduction(void volatile *volatile _v)
    {
        asVolatile(byte4_t);
        register byte4_t nv;
        register byte4_t temp;
        kyInlineASM
                (
                    "0:\n"
                    "ldrex %[nv], [%[byte]]\n"
                    "sub %[nv], %[nv], #1\n"
                    "strex %[temp], %[nv], [%[byte]]\n"
                    "teq %[temp], #0\n"
                    "bne 0b\n"
                    : [nv] "=&r" (nv), [temp] "=&r" (temp), "+m" (*byte)
                    : [byte] "r" (byte)
                    : "cc", "memory"
                );
        return nv != 0;
    }
    static void fetch_store(void volatile *volatile _v, void * nfv, void * nnv)
    {
        asVolatile(byte4_t);
        byte4_t *fv = (byte4_t*)nfv;
        byte4_t *nv = (byte4_t*)nnv;
        register byte4_t temp;
        kyInlineASM
                (
                    "0:\n"
                    "ldrex %[temp], [%[nv]] \n"
                    "ldrex %[fv], [%[byte]] \n"
                    "strex %[temp], %[temp], [%[byte]] \n"
                    "teq %[temp], #0\n"
                    "bne 0b\n"
                    : [fv] "=&r" (*fv), [temp] "=&r" (temp), "+m" (*byte)
                    : [nv] "r" (nv), [byte] "r" (byte)
                    : "cc"
                );
    }
    static void fetch_add(void volatile *volatile _v, void * nfv, void * nva)
    {
        asVolatile(byte4_t);
        byte4_t *fv = (byte4_t*)nfv;
        *fv = *byte;
        logic_add(_v, nva);
    }
    static bool compare_exchange(void volatile *volatile _v, void * nev, void * nnv)
    {
        asVolatile(byte4_t);
        byte4_t *ev = (byte4_t*)nev;
        byte4_t *nv = (byte4_t*)nnv;
        register byte4_t t1, t2, t3;
        kyInlineASM
                (
                    "0:\n"
                    "ldrex %[t1], [%[ev]]\n"
                    "ldrex %[t2], [%[nv]]\n"
                    "ldrex %[t3], [%[byte]]\n"
                    "eors %[t3], %[t3], %[t1]\n"
                    "itt eq\n"
                    "strexbeq %[t3], %[t2], [%[byte]]\n"
                    "teqeq %[t3], #1\n"
                    "beq 0b\n"
                    : [t1] "=&r" (t1), [t2] "=&r" (t2), [t3] "=&r" (t3),"+m" (*byte)
                    : [ev] "r" (ev), [nv] "r" (nv), [byte] "r" (byte)
                    : "cc"
                );
        return t3 == 0;
    }

    static void logic_and(void volatile *volatile _v, void *nva)
    {
        asVolatile(byte4_t);
        byte4_t *va = (byte4_t*)nva;
        register byte4_t nv;
        register byte4_t temp;
        kyInlineASM
                (
                    "0:\n"
                    "ldrex %[nv], [%[va]]\n"
                    "ldrex %[temp], [%[byte]]\n"
                    "and %[nv], %[temp], %[nv]\n"
                    "strex %[temp], %[nv], [%[byte]]\n"
                    "teq %[temp], #0\n"
                    "bne 0b\n"
                    : [nv] "=&r" (nv), [temp] "=&r" (temp), "+m" (*byte)
                    : [va] "r" (va), [byte] "r" (byte)
                    : "cc"
                );
    }
    static void logic_or(void volatile *volatile _v, void * nva)
    {
        asVolatile(byte4_t);
        byte4_t *va = (byte4_t*)nva;
        register byte4_t nv;
        register byte4_t temp;
        kyInlineASM
                (
                    "0:\n"
                    "ldrex %[nv], [%[va]]\n"
                    "ldrex %[temp], [%[byte]]\n"
                    "orr %[nv], %[temp], %[nv]\n"
                    "strex %[temp], %[nv], [%[byte]]\n"
                    "teq %[temp], #0\n"
                    "bne 0b\n"
                    : [nv] "=&r" (nv), [temp] "=&r" (temp), "+m" (*byte)
                    : [va] "r" (va), [byte] "r" (byte)
                    : "cc"
                );
    }
    static void logic_xor(void volatile *volatile _v, void * nva)
    {
        asVolatile(byte4_t);
        byte4_t *va = (byte4_t*)nva;
        register byte4_t nv;
        register byte4_t temp;
        kyInlineASM
                (
                    "0:\n"
                    "ldrex %[nv], [%[va]]\n"
                    "ldrex %[temp], [%[byte]]\n"
                    "eor %[nv], %[temp], %[nv]\n"
                    "strex %[temp], %[nv], [%[byte]]\n"
                    "teq %[temp], #0\n"
                    "bne 0b\n"
                    : [nv] "=&r" (nv), [temp] "=&r" (temp), "+m" (*byte)
                    : [va] "r" (va), [byte] "r" (byte)
                    : "cc"
                );
    }
    static void logic_add(void volatile *volatile _v, void * nva)
    {
        asVolatile(byte4_t);
        byte4_t *va = (byte4_t*)nva;
        register byte4_t nv;
        register byte4_t temp;
        kyInlineASM
                (
                    "0:\n"
                    "ldrex %[temp], [%[byte]]\n"
                    "add %[nv], %[temp], %[va]\n"
                    "strex %[temp], %[nv], [%[byte]]\n"
                    "teq %[temp], #0\n"
                    "bne 0b\n"
                    : [nv] "=&r" (nv), [temp] "=&r" (temp), "+m" (*byte)
                    : [va] "r" (*va), [byte] "r" (byte)
                    : "cc"
                );
    }
    static void logic_not(void volatile *volatile _v)
    {
        asVolatile(byte4_t);
        register byte4_t nv;
        register byte4_t temp;
        kyInlineASM
                (
                    "0:\n"
                    "ldrex %[nv], [%[byte]]\n"
                    "mvn %[nv], %[nv]\n"
                    "strex %[temp], %[nv], [%[byte]]\n"
                    "teq %[temp], #0\n"
                    "bne 0b\n"
                    : [nv] "=&r" (nv), [temp] "=&r" (temp), "+m" (*byte)
                    : [byte] "r" (byte)
                    : "cc"
                );
    }
    static void logic_inc(void volatile *volatile _v)
    {
        byte4_t onw = 1;
        logic_add(_v, &onw );
    }
    static void logic_dec(void volatile *volatile _v)
    {
        byte4_t onw = -1;
        logic_add(_v, &onw );
    }
};
static const ky_atomic_arch arch_arm_4 =
{
    .load             = atomic_arch_4::load,
    .store            = atomic_arch_4::store ,
    .increase         = atomic_arch_4::increase,
    .reduction        = atomic_arch_4::reduction,
    .fetch_store      = atomic_arch_4::fetch_store,
    .fetch_add        = atomic_arch_4::fetch_add,
    .compare_exchange = atomic_arch_4::compare_exchange,
    .logic_and        = atomic_arch_4::logic_and,
    .logic_or         = atomic_arch_4::logic_or,
    .logic_xor        = atomic_arch_4::logic_xor,
    .logic_add        = atomic_arch_4::logic_add,
    .logic_not        = atomic_arch_4::logic_not,
    .logic_inc        = atomic_arch_4::logic_inc,
    .logic_dec        = atomic_arch_4::logic_dec,
    .pause            = atomic_arch_4::pause,
    .memory_fence     = atomic_arch_4::memory_fence
};

#if kyArchIs64Bit
struct atomic_arch_8 : public arch_arm
{
    static void load(void volatile *volatile _v, void * val, eMemoryFences fence = Fence_Relaxed)
    {
        asVolatile(byte8_t);
        byte8_t *nval = (byte8_t*)val;
        memory_fence(fence);

        kyInlineASM
                (
                    "ldrd %[nval], %H[nval], [%[byte]]"
                    : [nval]"=&r" (nval)
                    : [byte]"r" (*byte)
                    : "memory"
                );
    }
    static void store(void volatile *volatile _v, void * nv, eMemoryFences  fence = Fence_Relaxed)
    {
        asVolatile(byte8_t);
        byte8_t *nval = (byte8_t*)nv;
        byte8_t flag;
        /*kyInlineASM
                (
                    "strd %[nval], %H[nval], [%[byte]]"
                    : [nval]"=&r" (*nval)
                    : [byte]"r" (*byte)
                    : "memory"
                );*/
        kyInlineASM
                (
                    "0: \n"
                    "ldrexd %[flag], [%[byte]]\n"
                    "strexd %[flag], %[nval], [%[byte]]\n"
                    "teq %[flag], #0\n"
                    "bne 0b\n"
                    : [flag]"=&r" (flag), "+m" (*byte)
                    : [byte]"r" (byte), [nval]"r" (nval)
                    :  "cc", "memory"
                 );
        memory_fence(fence);
    }
    static bool increase(void volatile *volatile _v)
    {
        asVolatile(byte8_t);
        byte8_t nv;
        int temp;
        kyInlineASM
                (
                    "0:\n"
                    "ldrexd %[nv], %H[nv], [%[byte]]\n"
                    "adds %Q[nv], %Q[nv], #1\n"
                    "adc %R[nv], %R[nv], #0\n"
                    "strexd %[temp], %[nv], %H[nv], [%[byte]]\n"
                    "teq %[temp], #0\n"
                    "bne 0b\n"
                    : [nv] "=&r" (nv), [temp] "=&r" (temp), "+m" (byte)
                    : [byte] "r" (&byte)
                    : "cc", "memory"
                );
        return nv != 0;
    }
    static bool reduction(void volatile *volatile _v)
    {
        asVolatile(byte8_t);
        byte8_t nv;
        int temp;
        kyInlineASM
                (
                    "0:\n"
                    "ldrexd %[nv], %H[nv], [%[byte]]\n"
                    "subs %Q[nv], %Q[nv], #1\n"
                    "sbc %R[nv], %R[nv], #0\n"
                    "strexd %[temp], %[nv], %H[nv], [%[byte]]\n"
                    "teq %[temp], #0\n"
                    "bne 0b\n"
                    : [nv] "=&r" (nv), [temp] "=&r" (temp), "+m" (byte)
                    : [byte] "r" (&byte)
                    : "cc", "memory"
                );
        return nv != 0;
    }
    static void fetch_store(void volatile *volatile _v, void * fv, void * nnv)
    {
        asVolatile(byte8_t);
        byte8_t *nfv = (byte8_t*)fv;
        byte8_t *nv = (byte8_t*)nnv;
        int temp;
        kyInlineASM
                (
                    "0:\n"
                    "ldrexd %[nfv], %H[nfv], [%[byte]]\n"
                    "strexd %[temp], %[nv], %H[nv], [%[byte]]\n"
                    "teq %[temp], #0\n"
                    "bne 0b\n"
                    : [nfv] "=&r" (*nfv), [temp] "=&r" (temp), "+m" (byte)
                    : [nv] "r" (*nv), [byte] "r" (&byte)
                    : "cc"
                );
    }
    static void fetch_add(void volatile *volatile _v, void * fv, void * nva)
    {
        asVolatile(byte8_t);
        byte8_t *nfv = (byte8_t*)fv;
        byte8_t *va = (byte8_t*)nva;
        byte8_t nv;
        int temp;
        kyInlineASM
                (
                    "0:\n"
                    "ldrexd %[nfv], %H[nfv], [%[byte]]\n"
                    "adds %Q[nv], %Q[nfv], %Q[va]\n"
                    "adc %R[nv], %R[nfv], %R[va]\n"
                    "strexd %[temp], %[nv], %H[nv], [%[byte]]\n"
                    "teq %[temp], #0\n"
                    "bne 0b\n"
                    : [nfv] "=&r" (*nfv), [nv] "=&r" (nv), [temp] "=&r" (temp), "+m" (byte)
                    : [va] "r" (*va), [byte] "r" (&byte)
                    : "cc"
                );
    }
    static bool compare_exchange(void volatile *volatile _v, void * nev, void * nnv)
    {
        asVolatile(byte8_t);
        byte8_t *ev = (byte8_t*)nev;
        byte8_t *nv = (byte8_t*)nnv;
        byte8_t temp;
        kyInlineASM
                (
                    "0:\n"
                    "ldrexd %[temp], %H[temp], [%[byte]]\n"
                    "eor %[temp], %[temp], %[ev]\n"
                    "eor %H[temp], %H[temp], %H[ev]\n"
                    "orrs %[temp], %[temp], %H[temp]\n"
                    "itt eq\n"
                    "strexdeq %[temp], %[nv], %H[nv], [%[byte]]\n"
                    "teqeq %[temp], #1\n"
                    "beq 0b\n"
                    : [temp] "=&r" (temp), "+m" (*byte)
                    : [ev] "r" (*ev), [nv] "r" (*nv), [byte] "r" (byte)
                    : "cc"
                );
        return byte4_t(temp) == 0;
    }

    static void logic_and(void volatile *volatile _v, void *va)
    {
        asVolatile(byte8_t);
        byte8_t *nva = (byte8_t*)va;
    }
    static void logic_or(void volatile *volatile _v, void * va)
    {
        asVolatile(byte8_t);
        byte8_t *nva = (byte8_t*)va;
    }
    static void logic_xor(void volatile *volatile _v, void * va)
    {
        asVolatile(byte8_t);
        byte8_t *nva = (byte8_t*)va;
    }
    static void logic_add(void volatile *volatile _v, void * nva)
    {
        asVolatile(byte8_t);
        byte8_t *va = (byte8_t*)nva;
        byte8_t nv;
        int temp;
        kyInlineASM
                (
                    "0:\n"
                    "ldrexd %[nv], %H[nv], [%[byte]]\n"
                    "adds %Q[nv], %Q[nv], %Q[va]\n"
                    "adc %R[nv], %R[nv], %R[va]\n"
                    "strexd %[temp], %[nv], %H[nv], [%[byte]]\n"
                    "teq %[temp], #0\n"
                    "bne 0b\n"
                    : [nv] "=&r" (nv), [temp] "=&r" (temp), "+m" (*byte)
                    : [va] "r" (*va), [byte] "r" (byte)
                    : "cc"
                );
    }
    static void logic_not(void volatile *volatile _v)
    {
        asVolatile(byte8_t);
    }
    static void logic_inc(void volatile *volatile _v)
    {
        asVolatile(byte8_t);
    }
    static void logic_dec(void volatile *volatile _v)
    {
        asVolatile(byte8_t);
    }
};
static const ky_atomic_arch arch_arm_8 =
{
    .load             = atomic_arch_8::load,
    .store            = atomic_arch_8::store ,
    .increase         = atomic_arch_8::increase,
    .reduction        = atomic_arch_8::reduction,
    .fetch_store      = atomic_arch_8::fetch_store,
    .fetch_add        = atomic_arch_8::fetch_add,
    .compare_exchange = atomic_arch_8::compare_exchange,
    .logic_and        = atomic_arch_8::logic_and,
    .logic_or         = atomic_arch_8::logic_or,
    .logic_xor        = atomic_arch_8::logic_xor,
    .logic_add        = atomic_arch_8::logic_add,
    .logic_not        = atomic_arch_8::logic_not,
    .logic_inc        = atomic_arch_8::logic_inc,
    .logic_dec        = atomic_arch_8::logic_dec,
    .pause            = atomic_arch_8::pause,
    .memory_fence     = atomic_arch_8::memory_fence
};
#endif

struct ky_atomic_arch * const kyArchAtomics[9] =
{
    0,
    (ky_atomic_arch *const)&arch_arm_1,
    (ky_atomic_arch *const)&arch_arm_2,
    0,
    (ky_atomic_arch *const)&arch_arm_4,
    0,
    0,
    0,
#if kyArchIs64Bit
    (ky_atomic_arch *const)&arch_arm_8
#else
    0
#endif
};
#endif
