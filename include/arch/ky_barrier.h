
/**
 * Basic tool library
 * Copyright (C) 2014 kunyang kunyang.yk@gmail.com
 *
 * @file     ky_barrier.h
 * @brief    系统内存屏障操作，支持x86/64,arm/aarch64
 *       1.32/64位屏障实现
 *       2.部分参考linux内核
 *
 * @author   kunyang
 * @email    kunyang.yk@gmail.com
 * @version  1.0.1.1
 * @date     2017/10/09
 * @license  GNU General Public License (GPL)
 *
 * Change History :
 *    Date    |  Version  |  Author  |   Description
 * 2017/10/09 | 1.0.0.1   | kunyang  | 创建文件
 * 2018/01/16 | 1.0.1.0   | kunyang  | 修改部分汇编参考linux内核
 */

#ifndef KY_BARRIER_H
#define KY_BARRIER_H

#include "ky_main.h"
#include "ky_macro.h"

/** 屏障的主要功能介绍:
  * kyHasMemoryBarrier             支持CPU的内存屏障,未定义时采用编译屏障
  * kyHasMemoryBarrierSMP          内存屏障为SMP实现,SMP定义是根据CPU的架构而定
  * kyCompilerBarrier()            编译器屏障函数,不要让编译器重新排序
  * kyMemoryBarrierReadWrite()     CPU 实现的读写内存屏障
  * kyMemoryBarrierReadOnly()      CPU 实现的读内存屏障
  * kyMemoryBarrierWriteOnly()     CPU 实现的写内存屏障
  * kyMemoryBarrierReadOnlyDMA()   CPU 实现的DMA总线的读内存屏障
  * kyMemoryBarrierWriteOnlyDMA()  CPU 实现的DMA总线的写内存屏障
  * kyMemoryBarrierReadOnlySMP()   CPU 实现的SMP读写内存屏障
  * kyMemoryBarrierWriteOnlySMP()  CPU 实现的SMP读内存屏障
  * kyMemoryBarrierReadWriteSMP()  CPU 实现的SMP写内存屏障
***/

///< 编译器屏障
#define kyCompilerBarrier()	kyInlineASM("": : :"memory")

///< 当前为x86架构
#if kyArchIsX86

///< 支持x86_64,intel已经实现了内存屏障功能
#  if kyArchIs64Bit

#    define mfence() kyInlineASM("mfence" ::: "memory")
#    define lfence() kyInlineASM("lfence" ::: "memory")
#    define sfence() kyInlineASM("sfence" ::: "memory")

#  else

#    define mfence() kyCompilerBarrier()
#    define lfence() kyCompilerBarrier()
#    define sfence() kyCompilerBarrier()

#  endif

///< x86架构未有DMA模式
#define kyMemoryBarrierReadOnlyDMA()	kyCompilerBarrier()
#define kyMemoryBarrierWriteOnlyDMA()	kyCompilerBarrier()

///< 内存屏障功能定义
#  ifndef kyHasMemoryBarrier
#    define kyMemoryBarrierReadWrite()		kyCompilerBarrier()
#    define kyMemoryBarrierReadOnly()		kyCompilerBarrier()
#    define kyMemoryBarrierWriteOnly()		kyCompilerBarrier()
#  else
#    define kyMemoryBarrierReadWrite()      mfence()
#    define kyMemoryBarrierReadOnly()		lfence()
#    define kyMemoryBarrierWriteOnly()		sfence()
#  endif

///< 当定义kyHasMemoryBarrierSMP宏时，系统暂时用编译屏障
#  ifndef kyHasMemoryBarrierSMP
#    define kyMemoryBarrierReadOnlySMP() kyCompilerBarrier()
#    define kyMemoryBarrierWriteOnlySMP() kyCompilerBarrier()
#    define kyMemoryBarrierReadWriteSMP()  kyCompilerBarrier()

#  else
#    define kyMemoryBarrierReadWriteSMP()	kyMemoryBarrierReadWrite()
#    define kyMemoryBarrierReadOnlySMP()	kyMemoryBarrierReadOnly()
#    define kyMemoryBarrierWriteOnlySMP()	kyCompilerBarrier()

#  endif

///< 当前为ARM架构
#elif kyArchIsARM

///< ARMv7架构内存屏障
#  if ((kyArchitecture & kyArch_ARMv7) == kyArch_ARMv7)
#    define sev()	kyInlineASM ("sev" : : : "memory")
#    define wfe()	kyInlineASM ("wfe" : : : "memory")
#    define wfi()	kyInlineASM ("wfi" : : : "memory")
#    define isb(option) kyInlineASM ("isb " #option : : : "memory")
#    define dsb(option) kyInlineASM ("dsb " #option : : : "memory")
#    define dmb(option) kyInlineASM ("dmb " #option : : : "memory")

///< ARMv6架构内存屏障
#   elif ((kyArchitecture & kyArch_ARMv6) == kyArch_ARMv6)
#    define isb(x) kyInlineASM ("mcr p15, 0, %0, c7, c5, 4" \
                    : : "r" (0) : "memory")
#    define dsb(x) kyInlineASM ("mcr p15, 0, %0, c7, c10, 4" \
                    : : "r" (0) : "memory")
#    define dmb(x) kyInlineASM ("mcr p15, 0, %0, c7, c10, 5" \
                    : : "r" (0) : "memory")

///< ARMv6以下架构内存屏障
#  else
#    define isb(x) kyCompilerBarrier()
#    define dsb(x) kyInlineASM ("mcr p15, 0, %0, c7, c10, 4" \
                    : : "r" (0) : "memory")
#    define dmb(x) kyCompilerBarrier()
#  endif

///< 内存屏障功能定义
#  ifndef kyHasMemoryBarrier
#    define kyMemoryBarrierReadWrite()		kyCompilerBarrier()
#    define kyMemoryBarrierReadOnly()		kyCompilerBarrier()
#    define kyMemoryBarrierWriteOnly()		kyCompilerBarrier()
#    define kyMemoryBarrierReadOnlyDMA()	kyCompilerBarrier()
#    define kyMemoryBarrierWriteOnlyDMA()	kyCompilerBarrier()
#  else
#    define kyMemoryBarrierReadWrite()      dsb()
#    define kyMemoryBarrierReadOnly()		dsb()
#    define kyMemoryBarrierWriteOnly()		dsb(st)
#    define kyMemoryBarrierReadOnlyDMA()	dmb(osh)
#    define kyMemoryBarrierWriteOnlyDMA()	dmb(oshst)
#  endif

///< 当定义kyHasMemoryBarrierSMP宏时，系统暂时用编译屏障
#  ifndef kyHasMemoryBarrierSMP
#    define kyMemoryBarrierReadWriteSMP()	kyCompilerBarrier()
#    define kyMemoryBarrierReadOnlySMP()	kyCompilerBarrier()
#    define kyMemoryBarrierWriteOnlySMP()	kyCompilerBarrier()
#  else
#    define kyMemoryBarrierReadWriteSMP()	dmb(ish)
#    define kyMemoryBarrierReadOnlySMP()	dmb(ish)
#    define kyMemoryBarrierWriteOnlySMP()	dmb(ishst)
#  endif

///< 其他架构时若编译器为GCC或Clang时采用GCC的方式
#elif kyCompilerIsGNUC || kyCompilerIsCLANG

#  define kyMemoryBarrierReadWrite()    __sync_synchronize()
#  define kyMemoryBarrierReadOnly()		__sync_synchronize()
#  define kyMemoryBarrierWriteOnly()    __sync_synchronize()
#  define kyMemoryBarrierReadOnlyDMA()	__sync_synchronize()
#  define kyMemoryBarrierWriteOnlyDMA()	__sync_synchronize()
#  define kyMemoryBarrierReadOnlySMP()	__sync_synchronize()
#  define kyMemoryBarrierWriteOnlySMP()	__sync_synchronize()
#  define kyMemoryBarrierReadWriteSMP()	__sync_synchronize()

///< 其他架构时内存屏障功能采用编译屏障
#else
#  pragma message ("Unknown architecture, memory barrier feature using compiler barrier.")
#  define kyMemoryBarrierReadWrite()    kyCompilerBarrier()
#  define kyMemoryBarrierReadOnly()		kyCompilerBarrier()
#  define kyMemoryBarrierWriteOnly()    kyCompilerBarrier()
#  define kyMemoryBarrierReadOnlyDMA()	kyCompilerBarrier()
#  define kyMemoryBarrierWriteOnlyDMA()	kyCompilerBarrier()
#  define kyMemoryBarrierReadOnlySMP()	kyCompilerBarrier()
#  define kyMemoryBarrierWriteOnlySMP()	kyCompilerBarrier()
#  define kyMemoryBarrierReadWriteSMP()	kyCompilerBarrier()
#endif

#endif // KY_BARRIER_H
