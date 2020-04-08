#include "arch/ky_memory.h"
#include "ky_utils.h"
#include "ky_cpu.h"
#include "maths/ky_maths.h"

#if kyOSIsWin32
#  include <pthread.h>
#else
#  if kyOSIsApple
#    include <sys/types.h>
#    include <sys/sysctl.h>
#  else
#    include <string.h>
#    include <sched.h>
#    include <unistd.h>

// Linux glibc has posix_memalign and (obsolete) memalign
// Android libc has only memalign
// OS X and BSD probably do not have either.
#    define Have_PosixMemalign (kyOSIsLinux && !kyOSIsAndroid)
#    define Have_Memalign kyOSIsLinux

#  endif
#endif

#ifdef __USE_MISC
# include <alloca.h>
#endif

#if Have_PosixMemalign

void* ky_memory::aligned_alloc (uint64 size, uint64 align)
{
    void *ptr = 0;
    const int ret = ::posix_memalign(&ptr, align, size);
    if (ret)
        return 0;
    return ptr;
}

void ky_memory::aligned_free(void* ptr)
{
    ::free(ptr);
}

#elif Have_Memalign

void* ky_memory::aligned_alloc(uint64 size, uint64 align)
{
    return ::memalign(align, size);
}

void ky_memory::aligned_free(void* ptr)
{
    ::free(ptr);
}

#else

void* ky_memory::aligned_alloc(uint64 size, uint64 align)
{
    // 为了保证我们可以有足够的额外空间来存储原始指针，并产生对齐的缓冲区，假设malloc确保至少与sizeof（void *）对齐。
    if (align < 2*sizeof(void*))
        align = 2*sizeof(void*);

    void* const mptr = ::malloc(size + align);
    if (!mptr)
        return 0;

    // 将 mptr 对齐到下一个地址，在我们将返回的指针之前保留一些未使用的空间。
    // 上面的最小对齐方式确保我们至少有 sizeof（void *）个额外空间。
    void* const aptr =
        (void *)(ky_round_down(uintptr(mptr), uintptr(align)) + align);

    // 确保对上述所有对齐是正确的
    kyENSURE(((void**)aptr) - 1 >= mptr);

    // Store the original pointer which will have to be sent to free().
    ((void **)aptr)[-1] = mptr;

    return aptr;
}

void ky_memory::aligned_free(void* ptr)
{
    if (ptr)
        ::free(((void**)ptr)[-1]);
}

#endif

uint64 ky_memory::query_size()
{
#if kyOSIsLinux
    const uint64 memSize = (uint64)::sysconf(_SC_PHYS_PAGES) * ky_cpu::page_size();
    return uint64(memSize / kyMiB);
#elif

#endif
}

uint64 ky_memory::query_available()
{
#if kyOSIsLinux
    const uint64 MAB = (uint64)::sysconf(_SC_AVPHYS_PAGES) * ky_cpu::page_size();
    const uint64 MA = uint64(MAB / kyMiB);
    return MA;
#elif

#endif
}


namespace cpucapable
{

#if kyArchIsX86
/* memcpy */

#if kyArchIs64Bit
#    define OPSIZE "q"
#    define REG_a "rax"
#    define REG_b "rbx"
#    define REG_c "rcx"
#    define REG_d "rdx"
#    define REG_D "rdi"
#    define REG_S "rsi"
#    define PTR_SIZE "8"

typedef int64 x86_reg;

/* REG_SP is defined in Solaris sys headers, so use REG_sp */
#    define REG_sp "rsp"
#    define REG_BP "rbp"
#    define REGBP   rbp
#    define REGa    rax
#    define REGb    rbx
#    define REGc    rcx
#    define REGd    rdx
#    define REGSP   rsp

#elif kyArchIs32Bit

#    define OPSIZE "l"
#    define REG_a "eax"
#    define REG_b "ebx"
#    define REG_c "ecx"
#    define REG_d "edx"
#    define REG_D "edi"
#    define REG_S "esi"
#    define PTR_SIZE "4"
typedef int32 x86_reg;

#    define REG_sp "esp"
#    define REG_BP "ebp"
#    define REGBP   ebp
#    define REGa    eax
#    define REGb    ebx
#    define REGc    ecx
#    define REGd    edx
#    define REGSP   esp
#else
typedef int x86_reg;
#endif


#if kyHAS_MMX && !kyHAS_MMX2 && !kyHAS_3dNOW && !kyHAS_SSE
#define HAS_ONLY_MMX1
#endif

#if kyHAS_SSE
#define MMREG_SIZE 16
#else
#define MMREG_SIZE 64 //8
#endif

#if kyHAS_MMX
#define PREFETCH "prefetchnta"
#elif kyHAS_3dNOW
#define PREFETCH  "prefetch"
#else
#define PREFETCH " # nop"
#endif

/* On K6 femms is faster of emms. On K7 femms is directly mapped on emms. */
#if kyHAS_3dNOW
#define EMMS     "femms"
#else
#define EMMS     "emms"
#endif

#ifdef HAS_ONLY_MMX1
#define MIN_LEN 0x800  /* 2K blocks */
#else
#define MIN_LEN 0x40  /* 64-byte blocks */
#endif


/* for small memory blocks (<256 bytes) this version is faster */
#   define small_memcpy(to,from,n)\
{\
    register cpucapable::x86_reg dummy;\
    kyInlineASM(\
    "rep; movsb"\
    :"=&D"(to), "=&S"(from), "=&c"(dummy)\
    :"0" (to), "1" (from),"2" (n)\
    : "memory");\
}

/* linux kernel __memcpy (from: /include/asm/string.h) */
static __inline__ void * linux_kernel_memcpy_impl (void * to, const void * from, int64 n)
{
    int d0, d1, d2;

    if (n < 4)
        small_memcpy(to, from, n)
    else
    {
            kyInlineASM
            (
                "rep ; movsl\n\t"
                "testb $2,%b4\n\t"
                "je 1f\n\t"
                "movsw\n"
                "1:\ttestb $1,%b4\n\t"
                "je 2f\n\t"
                "movsb\n"
                "2:"
                : "=&c" (d0), "=&D" (d1), "=&S" (d2)
                :"0" (n/4), "q" (n),"1" ((intptr) to),"2" ((intptr) from)
                : "memory"
            );
    }

    return (to);
}

#endif

}


namespace impl
{
#if kyArchIsX86

    struct x86mem
    {
        eMemcpyModes mode ; //auto

        void * copy(void *dst, const void* src, int64 len)
        {
            if (mode & CPU_SSE)
                return copy_sse(dst, src, len);
            else if (mode & CPU_MMX)
                return copy_mmx(dst, src, len);
            else if (mode & CPU_MMXEXT)
                return copy_mmx2(dst, src, len);
            return memcpy (dst, src, len);
        }
        void *move(void *dst, const void* src, int64 len)
        {
            return memmove (dst, src, len);
        }
        int compare(const void *dst, const void* src, int64 len)
        {
            return memcmp (dst, src, len);
        }
        void zero(void *src, int64 len, int fill = 0)
        {
            memset (src, fill, len);
        }

        void * copy_sse(void *dst, const void *src, int64 len)
        {
            void *retval;
            uint8 *ufrom=(uint8 *)src;
            uint8 *uto=(uint8 *)dst;
            retval = uto;

#if (kyArchitecture & kyArchBit_64) == kyArchBit_64
            /* PREFETCH has effect even for MOVSB instruction ;) */
            kyInlineASM (
                        PREFETCH" (%0)\n"
                        PREFETCH" 64(%0)\n"
                        PREFETCH" 128(%0)\n"
                        PREFETCH" 192(%0)\n"
                        PREFETCH" 256(%0)\n"
                        : : "r" (ufrom)
                        );
#else
            /* PREFETCH has effect even for MOVSB instruction ;) */
            kyInlineASM (
                        PREFETCH"   (%0)\n"
                        PREFETCH"   32(%0)\n"
                        PREFETCH"   64(%0)\n"
                        PREFETCH"   96(%0)\n"
                        PREFETCH"   128(%0)\n"
                        PREFETCH"   160(%0)\n"
                        PREFETCH"   192(%0)\n"
                        PREFETCH"   224(%0)\n"
                        PREFETCH"   256(%0)\n"
                        PREFETCH"   288(%0)\n"
                        : : "r" (ufrom) );
#endif
            if(len >= MIN_LEN)
            {
                register cpucapable::x86_reg delta;
                /* Align destinition to MMREG_SIZE -boundary */
                delta = ((intptr_t)uto)&(MMREG_SIZE-1);
                if(delta)
                {
                    delta = MMREG_SIZE-delta;
                    len -= delta;
                    small_memcpy(uto, ufrom, delta);
                }
                int64 i = len >> 6; /* len/64 */
                len &= 63;
                if(((intptr_t)ufrom) & 15)
                {
                    /* if SRC is misaligned */
                    for(; i > 0; i--)
                    {
                        kyInlineASM (

            #if (kyArchitecture & kyArchBit_64) == kyArchBit_64
                                    PREFETCH" 320(%0)\n"
            #else
                                    PREFETCH" 320(%0)\n"
                                    PREFETCH" 352(%0)\n"
            #endif
                                    "movups (%0), %%xmm0\n"
                                    "movups 16(%0), %%xmm1\n"
                                    "movups 32(%0), %%xmm2\n"
                                    "movups 48(%0), %%xmm3\n"
                                    "movntps %%xmm0, (%1)\n"
                                    "movntps %%xmm1, 16(%1)\n"
                                    "movntps %%xmm2, 32(%1)\n"
                                    "movntps %%xmm3, 48(%1)\n"
                                    :: "r" (ufrom), "r" (uto) : "memory");
                        ufrom += 64;
                        uto += 64;
                    }
                }
                else
                {
                    for(; i > 0; i--)
                    {
                        kyInlineASM (
            #if (kyArchitecture & kyArchBit_64) == kyArchBit_64
                                    PREFETCH" 320(%0)\n"
            #else
                                    PREFETCH" 320(%0)\n"
                                    PREFETCH" 352(%0)\n"
            #endif
                                    "movaps (%0), %%xmm0\n"
                                    "movaps 16(%0), %%xmm1\n"
                                    "movaps 32(%0), %%xmm2\n"
                                    "movaps 48(%0), %%xmm3\n"
                                    "movntps %%xmm0, (%1)\n"
                                    "movntps %%xmm1, 16(%1)\n"
                                    "movntps %%xmm2, 32(%1)\n"
                                    "movntps %%xmm3, 48(%1)\n"
                                    :: "r" (ufrom), "r" (uto) : "memory");
                        ufrom += 64;
                        uto += 64;
                    }
                }
                /* since movntq is weakly-ordered, a "sfence"
               * is needed to become ordered again. */
                kyInlineASM ("sfence":::"memory");
#if !kyHAS_SSE
                /* enables to use FPU */
                kyInlineASM (EMMS:::"memory");
#endif
            }
            /*
             *	Now do the tail of the block
             */
            if(len)
                cpucapable::linux_kernel_memcpy_impl(uto, ufrom, len);
            return retval;
        }
        void * copy_mmx(void * dst, const void * src, int64 len)
        {
            void *retval;
            uint8 *ufrom=(uint8 *)src;
            uint8 *uto=(uint8 *)dst;
            retval = uto;

            if (len >= MIN_LEN)
            {
                register ulong delta;
                /* Align destinition to MMREG_SIZE -boundary */
                delta = ((intptr)uto)&(MMREG_SIZE-1);
                if (delta)
                {
                    delta = MMREG_SIZE-delta;
                    len -= delta;
                    small_memcpy(uto, ufrom, delta);
                }
                int64 i = len >> 6; /* len/64 */
                len&=63;
                for (; i > 0; i--)
                {
                    kyInlineASM (
                                "movq (%0), %%mm0\n"
                                "movq 8(%0), %%mm1\n"
                                "movq 16(%0), %%mm2\n"
                                "movq 24(%0), %%mm3\n"
                                "movq 32(%0), %%mm4\n"
                                "movq 40(%0), %%mm5\n"
                                "movq 48(%0), %%mm6\n"
                                "movq 56(%0), %%mm7\n"
                                "movq %%mm0, (%1)\n"
                                "movq %%mm1, 8(%1)\n"
                                "movq %%mm2, 16(%1)\n"
                                "movq %%mm3, 24(%1)\n"
                                "movq %%mm4, 32(%1)\n"
                                "movq %%mm5, 40(%1)\n"
                                "movq %%mm6, 48(%1)\n"
                                "movq %%mm7, 56(%1)\n"
                                :: "r" (ufrom), "r" (uto) : "memory");
                    ufrom += 64;
                    uto += 64;
                }
                kyInlineASM ("emms":::"memory");
            }
            /*
           *	Now do the tail of the block
           */
            if(len)
                cpucapable::linux_kernel_memcpy_impl(uto, ufrom, len);
            return retval;
        }
        void * copy_mmx2(void * dst, const void * src, int64 len)
        {
            void *retval;
            uint8 *ufrom=(uint8 *)src;
            uint8 *uto=(uint8 *)dst;
            retval = uto;

#if (kyArchitecture & kyArchBit_64) == kyArchBit_64
            /* PREFETCH has effect even for MOVSB instruction ;) */
            kyInlineASM (
                        PREFETCH" (%0)\n"
                        PREFETCH" 64(%0)\n"
                        PREFETCH" 128(%0)\n"
                        PREFETCH" 192(%0)\n"
                        PREFETCH" 256(%0)\n"
                        : : "r" (ufrom)
                        );
#else
            /* PREFETCH has effect even for MOVSB instruction ;) */
            kyInlineASM (
                        PREFETCH"   (%0)\n"
                        PREFETCH"   32(%0)\n"
                        PREFETCH"   64(%0)\n"
                        PREFETCH"   96(%0)\n"
                        PREFETCH"   128(%0)\n"
                        PREFETCH"   160(%0)\n"
                        PREFETCH"   192(%0)\n"
                        PREFETCH"   224(%0)\n"
                        PREFETCH"   256(%0)\n"
                        PREFETCH"   288(%0)\n"
                        : : "r" (ufrom) );
#endif

            if(len >= MIN_LEN)
            {
                register cpucapable::x86_reg delta;
                /* Align destinition to MMREG_SIZE -boundary */
                delta = ((intptr)uto)&(MMREG_SIZE-1);
                if(delta)
                {
                    delta = MMREG_SIZE-delta;
                    len -= delta;
                    small_memcpy(uto, ufrom, delta);
                }
                int64 i = len >> 6; /* len/64 */
                len&=63;
                for(; i > 0; i--)
                {
                    kyInlineASM (
            #if (kyArchitecture & kyArchBit_64) == kyArchBit_64
                                    PREFETCH" 320(%0)\n"
            #else
                                    PREFETCH" 320(%0)\n"
                                    PREFETCH" 352(%0)\n"
            #endif
                                "movq (%0), %%mm0\n"
                                "movq 8(%0), %%mm1\n"
                                "movq 16(%0), %%mm2\n"
                                "movq 24(%0), %%mm3\n"
                                "movq 32(%0), %%mm4\n"
                                "movq 40(%0), %%mm5\n"
                                "movq 48(%0), %%mm6\n"
                                "movq 56(%0), %%mm7\n"
                                "movntq %%mm0, (%1)\n"
                                "movntq %%mm1, 8(%1)\n"
                                "movntq %%mm2, 16(%1)\n"
                                "movntq %%mm3, 24(%1)\n"
                                "movntq %%mm4, 32(%1)\n"
                                "movntq %%mm5, 40(%1)\n"
                                "movntq %%mm6, 48(%1)\n"
                                "movntq %%mm7, 56(%1)\n"
                                :: "r" (ufrom), "r" (uto) : "memory");
                    ufrom += 64;
                    uto += 64;
                }
                /* since movntq is weakly-ordered, a "sfence"
             * is needed to become ordered again. */
                kyInlineASM ("sfence":::"memory");
#if !kyHAS_SSE
                /* enables to use FPU */
                kyInlineASM (EMMS:::"memory");
#endif
            }
            /*
           *	Now do the tail of the block
           */
            if(len)
                cpucapable::linux_kernel_memcpy_impl(uto, ufrom, len);
            return retval;
        }

        int64 microsec()
        {
            struct timeval tv;
            gettimeofday(&tv, 0);
            return static_cast<int64>(tv.tv_sec) * 1000 * 1000 + tv.tv_usec;
        }

        eMemcpyModes probe(int64 size = 1024*1024, int64 *time = 0)
        {
            void *src,*dst;
            int64 start, stop, length;
            eMemcpyModes rets = Memcpy_Default;
            ky_cpu cpu;

            src = kyMalloc (size);
            dst = kyMalloc (size);


            // default
            start = microsec();
            memcpy (dst, src, size);
            stop = microsec();
            length = stop - start;

            // sse
            if (cpu.has (CPU_SSE))
            {
                start = microsec();
                copy_sse (dst, src, size);
                stop = microsec();
                if ((stop - start) < length)
                {
                    rets = Memcpy_SSE;
                    length = stop - start;
                }
            }
            // mmx
            if (cpu.has(CPU_MMX))
            {
                start = microsec();
                copy_mmx (dst, src, size);
                stop = microsec();
                if ((stop - start) < length)
                {
                    rets = Memcpy_MMX;
                    length = stop - start;
                }
            }
            // mmx2
            if (cpu.has(CPU_MMXEXT))
            {
                start = microsec();
                copy_mmx2 (dst, src, size);
                stop = microsec();
                if ((stop - start) < length)
                {
                    rets = Memcpy_MMX2;
                    length = stop - start;
                }
            }
            kyFree(src);
            kyFree(dst);

            if (time)
                *time = length;
            return rets;
        }

    };

#elif kyArchIsARM
    struct armmem
    {
        void *copy(void *dst, const void* src, int64 len)
        {
            return memcpy (dst, src, len);
        }
        void *move(void *dst, const void* src, int64 len)
        {
            return memmove (dst, src, len);
        }
        int compare(const void *dst, const void* src, int64 len)
        {
            return memcmp (dst, src, len);
        }
        void zero(void *src, int64 len, int fill = 0)
        {
            memset (src, fill, len);
        }
    };

#endif

}


ky_memory::ky_memory(){}

ky_memory::~ky_memory(){}

void *ky_memory::copy(void *dst, const void* src, int64 len)
{
    return memcpy (dst, src, len);
}
void *ky_memory::move(void *dst, const void* src, int64 len)
{
    return memmove (dst, src, len);
}
void ky_memory::zero(void *dst, int64 len, int fill)
{
    memset (dst, fill, len);
}

int ky_memory::compare(const void *dst, const void* src, int64 len)
{
    return memcmp (dst, src, len);
}


#if kyArchIsX86
static bool mul_overflow(int v1, int v2, int *r)
{
    uint8 overflow = 0;
    int res = v1;

    kyInlineASM
            (
                "imul %2, %1\n"
                "seto %0"
                : "=q" (overflow), "=r" (res)
                : "r" (v2), "1" (res)
                : "cc"
            );
    *r = res;
    return overflow;
}
static bool sub_overflow(int v1, int v2, int *r)
{
    uint8 overflow = 0;
    int res = v1;

    kyInlineASM
            (
                "subl %2, %1\n"
                "seto %0"
                : "=q" (overflow), "=r" (res)
                : "r" (v2), "1" (res)
                : "cc"
            );
    *r = res;
    return overflow;
}
static bool add_overflow(int v1, int v2, int *r)
{
    uint8 overflow = 0;
    int res = v1;

    kyInlineASM
            (
                "addl %2, %1\n"
                "seto %0"
                : "=q" (overflow), "=r" (res)
                : "r" (v2), "1" (res)
                : "cc"
            );
    *r = res;
    return overflow;
}
#else
static bool mul_overflow(int v1, int v2, int *r)
{
    const int64 tmp = (int64)v1 * (int64)v2;
    *r = (int)tmp;
    return tmp > INT32_MAX || tmp < INT32_MIN;
}
static bool sub_overflow(int v1, int v2, int *r)
{
    const int64 tmp = (int64)v1 - (int64)v2;
    *r = static_cast<int>(tmp);
    return tmp > INT32_MAX || tmp < INT32_MIN;
}
static bool add_overflow(int v1, int v2, int *r)
{
    const int64 tmp = (int64)v1 + (int64)v2;
    *r = static_cast<int>(tmp);
    return tmp > INT32_MAX || tmp < INT32_MIN;
}
#endif

int64 ky_memory::block_size(int64 size, int align, int64 header)
{
    const uint count = (uint)size;

    // 大于最大内存字节数
    if (count != size)
        return INT32_MAX; // 2GB

    int bytes =0;
    if (mul_overflow(align, count, &bytes) ||
            add_overflow(bytes, header, &bytes))
        return INT32_MAX;
    if ((int)bytes < 0)
        return INT32_MAX;

    return bytes;
}

static uint32 NextPowerOfTwo(uint32 v)
{
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    ++v;
    return v;
}
static uint64 NextPowerOfTwo(uint64 v)
{
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v |= v >> 32;
    ++v;
    return v;
}

int64 ky_memory::block_growing(int64 size, int align, int64 header, int64 *count)
{
    const uint sz = (uint)size;

    int result = 0;
    // 大于最大内存字节数
    if ((sz != size) || mul_overflow(align, sz, &result) ||
            add_overflow(result, header, &result))
        result = INT32_MAX; // 2GB
    if ((int)result < 0)
        result = INT32_MAX;

    const uint more_byte = NextPowerOfTwo((uint)result);
    if ((int)more_byte < 0)
        result += (more_byte - result) / 2;
    else
        result = more_byte;
    if (count)
        *count = (result - (uint)header) / align;
    return result;
}

