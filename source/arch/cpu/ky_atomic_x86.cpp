#include "arch/ky_atomic.h"

#define byte1_t int8
#define byte2_t int16
#define byte4_t int32
#define byte8_t int64

#define asVolatile(T) T volatile * volatile byte = (T volatile * volatile)_v

#if kyArchIsX86

struct arch_x86
{
    static void pause(void) {
        kyInlineASM ("pause" ::: "memory");
        //kyInlineASM ("rep; nop" ::: "memory");
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

struct atomic_arch_1 : public arch_x86
{
    static void load(void volatile *volatile _v, void * val, eMemoryFences fence = Fence_Relaxed)
    {
        asVolatile(byte1_t);
        byte1_t *nval = (byte1_t*)val;
        memory_fence(fence);

        kyInlineASM
                (
                    "movb %1, %0"
                    : "=r" (*nval)
                    : "m" (*byte)
                    : "memory"
                );
    }
    static void store(void volatile *volatile _v, void * nv, eMemoryFences  fence = Fence_Relaxed)
    {
        asVolatile(byte1_t);
        byte1_t *nval = (byte1_t*)nv;
        kyInlineASM
                (
                    "movb %1, %0"
                    : "=m" (*byte)
                    : "ir" (*nval)
                    : "memory"
                );
        memory_fence(fence);
    }
    static bool increase(void volatile *volatile _v)
    {
        asVolatile(byte1_t);
        byte1_t r = 0;
        kyInlineASM
                (
                    "lock\n"
                    "addb  $1, %0\n"
                    "setne %1\n"
                    : "=m" (*byte), "=qm" (r)
                    : "m" (*byte)
                    : "memory"
                );
        return r != 0;
    }
    static bool reduction(void volatile *volatile _v)
    {
        asVolatile(byte1_t);
        byte1_t r;
        kyInlineASM
                (
                    "lock\n"
                    "subb $1, %0\n"
                    "setne %1\n"
                    : "=m" (*byte), "=qm" (r)
                    : "m" (*byte)
                    : "memory"
                );
        return r != 0;
    }
    static void fetch_store(void volatile *volatile _v, void * fv, void * ev)
    {
        asVolatile(byte1_t);
        byte1_t *nval = (byte1_t*)ev;
        byte1_t *nfv = (byte1_t*)fv;
        kyInlineASM
                (
                    "xchgb %0,%1"
                    : "=q" (*nval), "+m" (*byte)
                    : "0" (*nval)
                    : "memory"
                );
        *nfv = *nval;
    }
    static void fetch_add(void volatile *volatile _v, void * fv, void * ev)
    {
        asVolatile(byte1_t);
        byte1_t *nval = (byte1_t*)ev;
        byte1_t *nfv = (byte1_t*)fv;
        kyInlineASM
                (
                    "lock\n"
                    "xadd %0,%1"
                    : "=q" (*nval), "+m" (*byte)
                    : "0" (*nval)
                    : "memory"
                );
        *nfv = *nval;
    }
    static bool compare_exchange(void volatile *volatile _v, void * ev, void * nv)
    {
        asVolatile(byte1_t);
        byte1_t *nev = (byte1_t*)ev;
        byte1_t *nnv = (byte1_t*)nv;
        byte1_t r;
        kyInlineASM
                (
                    "lock\n"
                    "cmpxchgb %3,%2\n"
                    "sete %1\n"
                    : "=a" (*nnv), "=qm" (r), "+m" (*byte)
                    : "r" (*nnv), "0" (*nev)
                    : "memory"
                );
        return r != 0;
    }
    static bool compare_exchange(void volatile *volatile _v, void * ev, void * nv, void *cv)
    {
        asVolatile(byte1_t);
        byte1_t *nev = (byte1_t*)ev;
        byte1_t *nnv = (byte1_t*)nv;
        byte1_t *ncv = (byte1_t*)cv;
        byte1_t r;
        kyInlineASM
                (
                    "lock\n"
                    "cmpxchgb %3,%2\n"
                    "sete %1\n"
                    : "=a" (*nnv), "=qm" (r), "+m" (*byte)
                    : "r" (*nnv), "0" (*nev)
                    : "memory"
                );
        *ncv = *nnv;
        return r != 0;
    }
    static void logic_and(void volatile *volatile _v, void *va)
    {
        asVolatile(byte1_t);
        byte1_t *nva = (byte1_t*)va;
        kyInlineASM
                (
                    "lock \n"
                    "and %1,%0"
                    : "+m" (*byte)
                    : "ir" (*nva)
                    : "memory"
                );
    }
    static void logic_or(void volatile *volatile _v, void * va)
    {
        asVolatile(byte1_t);
        byte1_t *nva = (byte1_t*)va;
        kyInlineASM
                (
                    "lock \n"
                    "orb %1,%0"
                    : "+m" (*byte)
                    : "ir" (*nva)
                    : "memory"
                );
    }
    static void logic_xor(void volatile *volatile _v, void * va)
    {
        asVolatile(byte1_t);
        byte1_t *nva = (byte1_t*)va;
        kyInlineASM
                (
                    "lock \n"
                    "xorb %1,%0"
                    : "+m" (*byte)
                    : "ir" (*nva)
                    : "memory"
                );
    }
    static void logic_add(void volatile *volatile _v, void * va)
    {
        asVolatile(byte1_t);
        byte1_t *nva = (byte1_t*)va;
        kyInlineASM
                (
                    "lock \n"
                    "addb %1,%0"
                    : "+m" (*byte)
                    : "ir" (*nva)
                    : "memory"
                );
    }
    static void logic_not(void volatile *volatile _v)
    {
        asVolatile(byte1_t);
        kyInlineASM
                (
                    "lock \n"
                    "notb %0"
                    : "+m" (*byte)
                    :
                    : "memory"
                );
    }
    static void logic_inc(void volatile *volatile _v)
    {
        asVolatile(byte1_t);
        kyInlineASM
                (
                    "lock \n"
                    "incb %0"
                    : "+m" (*byte)
                    :
                    : "memory"
                );
    }
    static void logic_dec(void volatile *volatile _v)
    {
        asVolatile(byte1_t);
        kyInlineASM
                (
                    "lock \n"
                    "decb %0"
                    : "+m" (*byte)
                    :
                    : "memory"
                );
    }
};
static const ky_atomic_arch arch_x86_1 =
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

struct atomic_arch_2 : public arch_x86
{
    static void load(void volatile *volatile _v, void * val, eMemoryFences fence = Fence_Relaxed)
    {
        asVolatile(byte2_t);
        byte2_t *nval = (byte2_t*)val;
        memory_fence(fence);

        kyInlineASM
                (
                    "movw %1, %0"
                    : "=r" (*nval)
                    : "m" (*byte)
                    : "memory"
                );
    }
    static void store(void volatile *volatile _v, void * nv, eMemoryFences  fence = Fence_Relaxed)
    {
        asVolatile(byte2_t);
        byte2_t *nval = (byte2_t*)nv;
        kyInlineASM
                (
                    "movw %1, %0"
                    : "=m" (*byte)
                    : "ir" (*nval)
                    : "memory"
                );
        memory_fence(fence);
    }
    static bool increase(void volatile *volatile _v)
    {
        asVolatile(byte2_t);
        byte1_t r = 0;
        kyInlineASM
                (
                    "lock\n"
                    "addw  $1, %0\n"
                    "setne %1\n"
                    : "=m" (*byte), "=qm" (r)
                    : "m" (*byte)
                    : "memory"
                );
        return r != 0;
    }
    static bool reduction(void volatile *volatile _v)
    {
        asVolatile(byte2_t);
        byte1_t r;
        kyInlineASM
                (
                    "lock\n"
                    "subw $1, %0\n"
                    "setne %1\n"
                    : "=m" (*byte), "=qm" (r)
                    : "m" (*byte)
                    : "memory"
                );
        return r != 0;
    }
    static void fetch_store(void volatile *volatile _v, void * fv, void * ev)
    {
        asVolatile(byte2_t);
        byte2_t *nval = (byte2_t*)ev;
        byte2_t *nfv = (byte2_t*)fv;
        kyInlineASM
                (
                    "xchgw %0,%1"
                    : "=q" (*nval), "+m" (*byte)
                    : "0" (*nval)
                    : "memory"
                );
        *nfv = *nval;
    }
    static void fetch_add(void volatile *volatile _v, void * fv, void * ev)
    {
        asVolatile(byte2_t);
        byte2_t *nval = (byte2_t*)ev;
        byte2_t *nfv = (byte2_t*)fv;
        kyInlineASM
                (
                    "lock\n"
                    "xadd %0,%1"
                    : "=q" (*nval), "+m" (*byte)
                    : "0" (*nval)
                    : "memory"
                );
        *nfv = *nval;
    }
    static bool compare_exchange(void volatile *volatile _v, void * ev, void * nv)
    {
        asVolatile(byte2_t);
        byte2_t *nev = (byte2_t*)ev;
        byte2_t *nnv = (byte2_t*)nv;
        byte1_t r;
        kyInlineASM
                (
                    "lock\n"
                    "cmpxchgw %3,%2\n"
                    "sete %1\n"
                    : "=a" (*nnv), "=qm" (r), "+m" (*byte)
                    : "r" (*nnv), "0" (*nev)
                    : "memory"
                );
        return r != 0;
    }
    static bool compare_exchange(void volatile *volatile _v, void * ev, void * nv, void *cv)
    {
        asVolatile(byte2_t);
        byte2_t *nev = (byte2_t*)ev;
        byte2_t *nnv = (byte2_t*)nv;
        byte2_t *ncv = (byte2_t*)cv;
        byte1_t r;
        kyInlineASM
                (
                    "lock\n"
                    "cmpxchgw %3,%2\n"
                    "sete %1\n"
                    : "=a" (*nnv), "=qm" (r), "+m" (*byte)
                    : "r" (*nnv), "0" (*nev)
                    : "memory"
                );
        *ncv = *nnv;
        return r != 0;
    }
    static void logic_and(void volatile *volatile _v, void *va)
    {
        asVolatile(byte2_t);
        byte2_t *nva = (byte2_t*)va;
        kyInlineASM
                (
                    "lock \n"
                    "and %1,%0"
                    : "+m" (*byte)
                    : "ir" (*nva)
                    : "memory"
                );
    }
    static void logic_or(void volatile *volatile _v, void * va)
    {
        asVolatile(byte2_t);
        byte2_t *nva = (byte2_t*)va;
        kyInlineASM
                (
                    "lock \n"
                    "orw %1,%0"
                    : "+m" (*byte)
                    : "ir" (*nva)
                    : "memory"
                );
    }
    static void logic_xor(void volatile *volatile _v, void * va)
    {
        asVolatile(byte2_t);
        byte2_t *nva = (byte2_t*)va;
        kyInlineASM
                (
                    "lock \n"
                    "xorw %1,%0"
                    : "+m" (*byte)
                    : "ir" (*nva)
                    : "memory"
                );
    }
    static void logic_add(void volatile *volatile _v, void * va)
    {
        asVolatile(byte2_t);
        byte2_t *nva = (byte2_t*)va;
        kyInlineASM
                (
                    "lock \n"
                    "addw %1,%0"
                    : "+m" (*byte)
                    : "ir" (*nva)
                    : "memory"
                );
    }
    static void logic_not(void volatile *volatile _v)
    {
        asVolatile(byte2_t);
        kyInlineASM
                (
                    "lock \n"
                    "notw %0"
                    : "+m" (*byte)
                    :
                    : "memory"
                );
    }
    static void logic_inc(void volatile *volatile _v)
    {
        asVolatile(byte2_t);
        kyInlineASM
                (
                    "lock \n"
                    "incw %0"
                    : "+m" (*byte)
                    :
                    : "memory"
                );
    }
    static void logic_dec(void volatile *volatile _v)
    {
        asVolatile(byte2_t);
        kyInlineASM
                (
                    "lock \n"
                    "decw %0"
                    : "+m" (*byte)
                    :
                    : "memory"
                );
    }

};
static const ky_atomic_arch arch_x86_2 =
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

struct atomic_arch_4 : public arch_x86
{
    static void load(void volatile *volatile _v, void * val, eMemoryFences fence = Fence_Relaxed)
    {
        asVolatile(byte4_t);
        byte4_t *nval = (byte4_t*)val;
        memory_fence(fence);

        kyInlineASM
                (
                    "movl %1, %0"
                    : "=r" (*nval)
                    : "m" (*byte)
                    : "memory"
                );
    }
    static void store(void volatile *volatile _v, void * nv, eMemoryFences  fence = Fence_Relaxed)
    {
        asVolatile(byte4_t);
        byte4_t *nval = (byte4_t*)nv;
        kyInlineASM
                (
                    "movl %1, %0"
                    : "=m" (*byte)
                    : "ir" (*nval)
                    : "memory"
                );
        memory_fence(fence);
    }
    static bool increase(void volatile *volatile _v)
    {
        asVolatile(byte4_t);
        byte1_t r = 0;
        kyInlineASM
                (
                    "lock\n"
                    "addl  $1, %0\n"
                    "setne %1\n"
                    : "=m" (*byte), "=qm" (r)
                    : "m" (*byte)
                    : "memory"
                );
        return r != 0;
    }
    static bool reduction(void volatile *volatile _v)
    {
        asVolatile(byte4_t);
        byte1_t r;
        kyInlineASM
                (
                    "lock\n"
                    "subl $1, %0\n"
                    "setne %1\n"
                    : "=m" (*byte), "=qm" (r)
                    : "m" (*byte)
                    : "memory"
                );
        return r != 0;
    }
    static void fetch_store(void volatile *volatile _v, void * fv, void * ev)
    {
        asVolatile(byte4_t);
        byte4_t *nval = (byte4_t*)ev;
        byte4_t *nfv = (byte4_t*)fv;
        kyInlineASM
                (
                    "xchgl %0,%1"
                    : "=q" (*nval), "+m" (*byte)
                    : "0" (*nval)
                    : "memory"
                );
        *nfv = *nval;
    }
    static void fetch_add(void volatile *volatile _v, void * fv, void * ev)
    {
        asVolatile(byte4_t);
        byte4_t *nval = (byte4_t*)ev;
        byte4_t *nfv = (byte4_t*)fv;
        kyInlineASM
                (
                    "lock\n"
                    "xadd %0,%1"
                    : "=q" (*nval), "+m" (*byte)
                    : "0" (*nval)
                    : "memory"
                );
        *nfv = *nval;
    }
    static bool compare_exchange(void volatile *volatile _v, void * ev, void * nv)
    {
        asVolatile(byte4_t);
        byte4_t *nev = (byte4_t*)ev;
        byte4_t *nnv = (byte4_t*)nv;
        byte1_t r;
        kyInlineASM
                (
                    "lock\n"
                    "cmpxchgl %3,%2\n"
                    "sete %1\n"
                    : "=a" (*nnv), "=qm" (r), "+m" (*byte)
                    : "r" (*nnv), "0" (*nev)
                    : "memory"
                );
        return r != 0;
    }
    static bool compare_exchange(void volatile *volatile _v, void * ev, void * nv, void *cv)
    {
        asVolatile(byte4_t);
        byte4_t *nev = (byte4_t*)ev;
        byte4_t *nnv = (byte4_t*)nv;
        byte4_t *ncv = (byte4_t*)cv;
        byte1_t r;
        kyInlineASM
                (
                    "lock\n"
                    "cmpxchgl %3,%2\n"
                    "sete %1\n"
                    : "=a" (*nnv), "=qm" (r), "+m" (*byte)
                    : "r" (*nnv), "0" (*nev)
                    : "memory"
                );
        *ncv = *nnv;
        return r != 0;
    }
    static void logic_and(void volatile *volatile _v, void *va)
    {
        asVolatile(byte4_t);
        byte4_t *nva = (byte4_t*)va;
        kyInlineASM
                (
                    "lock \n"
                    "and %1,%0"
                    : "+m" (*byte)
                    : "ir" (*nva)
                    : "memory"
                );
    }
    static void logic_or(void volatile *volatile _v, void * va)
    {
        asVolatile(byte4_t);
        byte4_t *nva = (byte4_t*)va;
        kyInlineASM
                (
                    "lock \n"
                    "orl %1,%0"
                    : "+m" (*byte)
                    : "ir" (*nva)
                    : "memory"
                );
    }
    static void logic_xor(void volatile *volatile _v, void * va)
    {
        asVolatile(byte4_t);
        byte4_t *nva = (byte4_t*)va;
        kyInlineASM
                (
                    "lock \n"
                    "xorl %1,%0"
                    : "+m" (*byte)
                    : "ir" (*nva)
                    : "memory"
                );
    }
    static void logic_add(void volatile *volatile _v, void * va)
    {
        asVolatile(byte4_t);
        byte4_t *nva = (byte4_t*)va;
        kyInlineASM
                (
                    "lock \n"
                    "addl %1,%0"
                    : "+m" (*byte)
                    : "ir" (*nva)
                    : "memory"
                );
    }
    static void logic_not(void volatile *volatile _v)
    {
        asVolatile(byte4_t);
        kyInlineASM
                (
                    "lock \n"
                    "notl %0"
                    : "+m" (*byte)
                    :
                    : "memory"
                );
    }
    static void logic_inc(void volatile *volatile _v)
    {
        asVolatile(byte4_t);
        kyInlineASM
                (
                    "lock \n"
                    "incl %0"
                    : "+m" (*byte)
                    :
                    : "memory"
                );
    }
    static void logic_dec(void volatile *volatile _v)
    {
        asVolatile(byte4_t);
        kyInlineASM
                (
                    "lock \n"
                    "decl %0"
                    : "+m" (*byte)
                    :
                    : "memory"
                );
    }

};
static const ky_atomic_arch arch_x86_4 =
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

#  if ((kyArchitecture & kyArchBit_64) == kyArchBit_64) //native support for 64-bit types

struct atomic_arch_8 : public arch_x86
{
    static void load(void volatile *volatile _v, void * val, eMemoryFences fence = Fence_Relaxed)
    {
        asVolatile(byte8_t);
        byte8_t *nval = (byte8_t*)val;
        memory_fence(fence);

        kyInlineASM
                (
                    "movq %1, %0"
                    : "=r" (*nval)
                    : "m" (*byte)
                    : "memory"
                );
    }
    static void store(void volatile *volatile _v, void * nv, eMemoryFences  fence = Fence_Relaxed)
    {
        asVolatile(byte8_t);
        byte8_t *nval = (byte8_t*)nv;
        kyInlineASM
                (
                    "movq %1, %0"
                    : "=m" (*byte)
                    : "ir" (*nval)
                    : "memory"
                );
        memory_fence(fence);
    }
    static bool increase(void volatile *volatile _v)
    {
        asVolatile(byte8_t);
        byte1_t r = 0;
        kyInlineASM
                (
                    "lock\n"
                    "addq  $1, %0\n"
                    "setne %1"
                    : "=m" (*byte), "=qm" (r)
                    : "m" (*byte)
                    : "memory"
                );
        return r != 0;
    }
    static bool reduction(void volatile *volatile _v)
    {
        asVolatile(byte8_t);
        byte1_t r;
        kyInlineASM
                (
                    "lock\n"
                    "subq $1, %0\n"
                    "setne %1"
                    : "=m" (*byte), "=qm" (r)
                    : "m" (*byte)
                    : "memory"
                );
        return r != 0;
    }
    static void fetch_store(void volatile *volatile _v, void * fv, void * ev)
    {
        asVolatile(byte8_t);
        byte8_t *nval = (byte8_t*)ev;
        byte8_t *nfv = (byte8_t*)fv;
        kyInlineASM
                (
                    "xchgq %0,%1"
                    : "=q" (*nval), "+m" (*byte)
                    : "0" (*nval)
                    : "memory"
                );
        *nfv = *nval;
    }
    static void fetch_add(void volatile *volatile _v, void * fv, void * ev)
    {
        asVolatile(byte8_t);
        byte8_t *nval = (byte8_t*)ev;
        byte8_t *nfv = (byte8_t*)fv;
        kyInlineASM
                (
                    "lock\n"
                    "xadd %0,%1"
                    : "=q" (*nval), "+m" (*byte)
                    : "0" (*nval)
                    : "memory"
                );
        *nfv = *nval;
    }
    static bool compare_exchange(void volatile *volatile _v, void * ev, void * nv)
    {
        asVolatile(byte8_t);
        byte8_t *nev = (byte8_t*)ev;
        byte8_t *nnv = (byte8_t*)nv;
        byte1_t r;
        kyInlineASM
                (
                    "lock\n"
                    "cmpxchgq %3,%2\n"
                    "sete %1\n"
                    : "=a" (*nnv), "=qm" (r), "+m" (*byte)
                    : "r" (*nnv), "0" (*nev)
                    : "memory"
                );
        return r != 0;
    }
    static bool compare_exchange(void volatile *volatile _v, void * ev, void * nv, void *cv)
    {
        asVolatile(byte8_t);
        byte8_t *nev = (byte8_t*)ev;
        byte8_t *nnv = (byte8_t*)nv;
        byte8_t *ncv = (byte8_t*)cv;
        byte1_t r;
        kyInlineASM
                (
                    "lock\n"
                    "cmpxchgq %3,%2\n"
                    "sete %1\n"
                    : "=a" (*nnv), "=qm" (r), "+m" (*byte)
                    : "r" (*nnv), "0" (*nev)
                    : "memory"
                );
        *ncv = *nnv;
        return r != 0;
    }
    static void logic_and(void volatile *volatile _v, void *va)
    {
        asVolatile(byte8_t);
        byte8_t *nva = (byte8_t*)va;
        kyInlineASM
                (
                    "lock \n"
                    "and %1,%0"
                    : "+m" (*byte)
                    : "ir" (*nva)
                    : "memory"
                );
    }
    static void logic_or(void volatile *volatile _v, void * va)
    {
        asVolatile(byte8_t);
        byte8_t *nva = (byte8_t*)va;
        kyInlineASM
                (
                    "lock \n"
                    "orq %1,%0"
                    : "+m" (*byte)
                    : "ir" (*nva)
                    : "memory"
                );
    }
    static void logic_xor(void volatile *volatile _v, void * va)
    {
        asVolatile(byte8_t);
        byte8_t *nva = (byte8_t*)va;
        kyInlineASM
                (
                    "lock \n"
                    "xorq %1,%0"
                    : "+m" (*byte)
                    : "ir" (*nva)
                    : "memory"
                );
    }
    static void logic_add(void volatile *volatile _v, void * va)
    {
        asVolatile(byte8_t);
        byte8_t *nva = (byte8_t*)va;
        kyInlineASM
                (
                    "lock \n"
                    "addq %1,%0"
                    : "+m" (*byte)
                    : "ir" (*nva)
                    : "memory"
                );
    }
    static void logic_not(void volatile *volatile _v)
    {
        asVolatile(byte8_t);
        kyInlineASM
                (
                    "lock \n"
                    "notq %0"
                    : "+m" (*byte)
                    :
                    : "memory"
                );
    }
    static void logic_inc(void volatile *volatile _v)
    {
        asVolatile(byte8_t);
        kyInlineASM
                (
                    "lock \n"
                    "incq %0"
                    : "+m" (*byte)
                    :
                    : "memory"
                );
    }
    static void logic_dec(void volatile *volatile _v)
    {
        asVolatile(byte8_t);
        kyInlineASM
                (
                    "lock \n"
                    "decq %0"
                    : "+m" (*byte)
                    :
                    : "memory"
                );
    }

};

static const ky_atomic_arch arch_x86_8 =
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

#  else // i386 architecture, emulate 64-bit support via cmpxchg8b
struct atomic_arch_8 : public arch_x86
{
    static bool compare_exchange(void volatile *volatile _v, void * ev, void * nv)
    {
        asVolatile(byte8_t);
        byte8_t *nev = (byte8_t*)ev;
        byte8_t *nnv = (byte8_t*)nv;
        byte8_t r;
#ifdef __PIC__
# define EBX_reg "r"
# define EBX_load(reg) "xchg " reg ", %%ebx\n"
#else
# define EBX_reg "b"
# define EBX_load(reg)
#endif
        uint highev = uint(*nnv >> 32); // ECX
        kyInlineASM(EBX_load("%3")
                    "lock\n"
                    "cmpxchg8b %0\n"
                    EBX_load("%3")
                    "sete %%cl\n"
                    : "+m" (*byte), "+c" (highev), "+&A" (*nev)
                    : EBX_reg (uint(*nnv & 0xffffffff))
                    : "memory");
        // if the comparison failed, ev here contains the current value
        return uchar(highev) != 0;
#undef EBX_reg
#undef EBX_load
    }
};
static const ky_atomic_arch arch_x86_8 =
{
    .load             = 0,
    .store            = 0,
    .increase         = 0,
    .reduction        = 0,
    .fetch_store      = 0,
    .fetch_add        = 0,
    .compare_exchange = atomic_arch_8::compare_exchange,
    .logic_and        = 0,
    .logic_or         = 0,
    .logic_xor        = 0,
    .logic_add        = 0,
    .logic_not        = 0,
    .logic_inc        = 0,
    .logic_dec        = 0,
    .pause            = atomic_arch_8::pause,
    .memory_fence     = atomic_arch_8::memory_fence
};
#  endif

struct ky_atomic_arch * const kyArchAtomics[9] =
{
    0,
    (ky_atomic_arch *const)&arch_x86_1,
    (ky_atomic_arch *const)&arch_x86_2,
    0,
    (ky_atomic_arch *const)&arch_x86_4,
    0,
    0,
    0,
    (ky_atomic_arch *const)&arch_x86_8
};

#endif
