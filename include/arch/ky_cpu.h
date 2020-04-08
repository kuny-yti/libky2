
/**
 * Basic tool library
 * Copyright (C) 2014 kunyang kunyang.yk@gmail.com
 *
 * @file     ky_cpu.h
 * @brief    关于CPU信息检测类
 *       1.x86, ARM 体系架构的指令集检测.
 *       2.CPU 信息获取
 *       3.CPU 缓存信息
 *       4.CPU 物理信息
 *
 * @author   kunyang
 * @email    kunyang.yk@gmail.com
 * @version  1.1.1.1
 * @date     2016/07/02
 * @license  GNU General Public License (GPL)
 *
 * Change History :
 *    Date    |  Version  |  Author  |   Description
 * 2016/07/02 | 1.0.0.1   | kunyang  | 创建文件，将ky_memory的关于CPU能力部分移到此类
 * 2016/11/10 | 1.0.1.0   | kunyang  | 修改类并加入cpu信息获取
 * 2017/04/04 | 1.1.0.1   | kunyang  | 加入arm体系架构检测
 * 2019/08/12 | 1.1.1.1   | kunyang  | 修改arm CPU信息获取
 */

#ifndef ky_CPU_H
#define ky_CPU_H
#include "ky_define.h"
#include "ky_utils.h"
#include "ky_flags.h"

enum ____
{
#if kyArchIsX86
    CPU_ArchX86 = 1 << 31,
#elif kyArchIsARM
    CPU_ArchARM = 1 << 31,
#elif kyArchIsPowerPC
    CPU_ArchPPC = 1 << 31,
#elif kyArchIsMIPS
    CPU_ArchMips = 1 << 31,
#endif
};

typedef enum
{
    Vendor_Unknown = 0,

#if kyArchIsX86
    ///< x86 Arch
    CPU_VendorIntel     = CPU_ArchX86 | (1 << 5),
    CPU_VendorAMD       = CPU_ArchX86 | (2 << 5),
    CPU_VendorCyrix     = CPU_ArchX86 | (3 << 5),
    CPU_VendorTransmeta = CPU_ArchX86 | (4 << 5),
    CPU_VendorVIA       = CPU_ArchX86 | (5 << 5),
    CPU_VendorUMC       = CPU_ArchX86 | (6 << 5),
    CPU_VendorNexGen    = CPU_ArchX86 | (7 << 5),
    CPU_VendorRise      = CPU_ArchX86 | (8 << 5),
    CPU_VendorSIS       = CPU_ArchX86 | (9 << 5),
    CPU_VendorNSC       = CPU_ArchX86 | (10 << 5),
    CPU_VendorVortex    = CPU_ArchX86 | (11 << 5),
    CPU_VendorRDC       = CPU_ArchX86 | (12 << 5),
    CPU_VendorHygon     = CPU_ArchX86 | (13 << 5),
    CPU_VendorZhaoxin   = CPU_ArchX86 | (14 << 5),

#elif kyArchIsARM
    ///< ARM Arch
    CPU_VendorARM       = CPU_ArchARM | (3 << 5),
    CPU_VendorQualcomm  = CPU_ArchARM | (4 << 5),
    CPU_VendorNvidia    = CPU_ArchARM | (5 << 5),
    CPU_VendorSamsung   = CPU_ArchARM | (6 << 5),
    CPU_VendorApple     = CPU_ArchARM | (7 << 5),
    CPU_VendorCavium    = CPU_ArchARM | (8 << 5),
    CPU_VendorMarvell   = CPU_ArchARM | (9 << 5),
    CPU_VendorBroadcom  = CPU_ArchARM | (10 << 5),
    CPU_VendorMediatek  = CPU_ArchARM | (11 << 5),
    CPU_VendorHisilicon = CPU_ArchARM | (12 << 5),
    CPU_VendorHuawei    = CPU_VendorHisilicon,
    CPU_VendorActions   = CPU_ArchARM | (13 << 5),
    CPU_VendorAllwinner = CPU_ArchARM | (14 << 5),
    CPU_VendorAmlogic   = CPU_ArchARM | (15 << 5),
    CPU_VendorLG        = CPU_ArchARM | (16 << 5),
    CPU_VendorLeadcore  = CPU_ArchARM | (17 << 5),
    CPU_VendorMstar     = CPU_ArchARM | (18 << 5),
    CPU_VendorNovathor  = CPU_ArchARM | (19 << 5),
    CPU_VendorPinecone  = CPU_ArchARM | (20 << 5),
    CPU_VendorRenesas   = CPU_ArchARM | (21 << 5),
    CPU_VendorRockchip  = CPU_ArchARM | (22 << 5),
    CPU_VendorSpreadtrum = CPU_ArchARM | (23 << 5),
    CPU_VendorTelechips = CPU_ArchARM | (24 << 5),
    CPU_VendorWondermedia = CPU_ArchARM | (25 << 5),
    CPU_VendorIBM       = CPU_ArchARM | (26 << 5),
    CPU_VendorIngenic   = CPU_ArchARM | (27 << 5),
    CPU_VendorAPM       = CPU_ArchARM | (28 << 5), // Applied Micro
    CPU_VendorInstruments = CPU_ArchARM | (29 << 5),
    CPU_VendorMotorola  = CPU_ArchARM | (30 << 5),

#elif kyArchIsPowerPC
    CPU_VendorPowerPC   = CPU_ArchPPC | (1 << 5),
#elif kyArchIsMIPS
    CPU_VendorMips      = CPU_ArchMips | (1 << 5),
#endif

}eCPUVendors;

typedef enum
{
    uArch_Unknown      = 0,

#if kyArchIsX86
    ///< Intel
    CPU_uArchP5             = 0x100 | CPU_VendorIntel, ///< Pentium and Pentium MMX
    CPU_uArchQuark          = 0x101 | CPU_VendorIntel, ///< Quark
    CPU_uArchP6             = 0x200 | CPU_VendorIntel, ///< Pentium (Pro, II, III)
    CPU_uArchDothan         = 0x201 | CPU_VendorIntel, ///< Pentium M
    CPU_uArchYonah          = 0x202 | CPU_VendorIntel, ///< Core
    CPU_uArchConroe         = 0x203 | CPU_VendorIntel, ///< Core 2 (65 nm)
    CPU_uArchPenryn         = 0x204 | CPU_VendorIntel, ///< Core 2 (45 nm)
    CPU_uArchNehalem        = 0x205 | CPU_VendorIntel, ///< Nehalem (Core i3/i5/i7 1st gen)
    CPU_uArchWestmere       = CPU_uArchNehalem,        ///< Westmere (Core i3/i5/i7 1st gen)
    CPU_uArchSandyBridge    = 0x206 | CPU_VendorIntel, ///< Sandy Bridge (Core i3/i5/i7 2st gen)
    CPU_uArchIvyBridge      = 0x207 | CPU_VendorIntel, ///< Ivy Bridge (Core i3/i5/i7 3st gen)
    CPU_uArchHaswell        = 0x208 | CPU_VendorIntel, ///< Haswell (Core i3/i5/i7 4st gen)
    CPU_uArchBroadwell      = 0x209 | CPU_VendorIntel, ///< Broadwell
    ///< Sky Lake (14 nm)
    ///< include Kaby/Coffee/Whiskey/Amber/Comet/Cascade/Cooper Lake
    CPU_uArchSkyLake        = 0x20A | CPU_VendorIntel,
    CPU_uArchKabyLake       = 0x20A | CPU_VendorIntel, ///< DEPRECATED (Kaby Lake)
    CPU_uArchPalmCove       = 0x20B | CPU_VendorIntel, ///< Palm Cove (10 nm, Cannon Lake)
    CPU_uArchSunnyCove      = 0x20C | CPU_VendorIntel, ///< Sunny Cove (10 nm, Ice Lake)
    CPU_uArchWillamette     = 0x300 | CPU_VendorIntel,  ///< Pentium 4 with Willamette, Northwood, or Foster cores
    CPU_uArchPrescott       = 0x301 | CPU_VendorIntel,  ///< Pentium 4 with Prescott and later cores
    CPU_uArchBonnell        = 0x400 | CPU_VendorIntel, ///< Atom on 45 nm
    CPU_uArchSaltwell       = 0x401 | CPU_VendorIntel, ///< Atom on 32 nm
    CPU_uArchSilvermont     = 0x402 | CPU_VendorIntel, ///< Silvermont (22 nm out-of-order Atom)
    CPU_uArchAirmont        = 0x403 | CPU_VendorIntel, ///< Airmont (14 nm out-of-order Atom)
    CPU_uArchGoldmont       = 0x404 | CPU_VendorIntel, ///< Goldmont (Denverton, Apollo Lake)
    CPU_uArchGoldmontPlus   = 0x405 | CPU_VendorIntel,  ///< Goldmont Plus (Gemini Lake)
    CPU_uArchKnightsFerry   = 0x500 | CPU_VendorIntel, ///< Knights Ferry HPC boards
    CPU_uArchKnightsCorner  = 0x501 | CPU_VendorIntel, ///< Knights Corner HPC boards (aka Xeon Phi)
    CPU_uArchKnightsLanding = 0x502 | CPU_VendorIntel, ///< Knights Landing (2gen MIC)
    CPU_uArchKnightsHill    = 0x503 | CPU_VendorIntel, ///< Knights Hill (3gen MIC)
    CPU_uArchKnightsMill    = 0x504 | CPU_VendorIntel, ///< Knights Mill Xeon Phi
    CPU_uArchXScale         = 0x600 | CPU_VendorIntel, ///< Intel/Marvell XScale series

    ///< AMD
    CPU_uArchK5             = 0x100 | CPU_VendorAMD, ///< K5
    CPU_uArchK6             = 0x101 | CPU_VendorAMD, ///< K6 and alike
    CPU_uArchK7             = 0x102 | CPU_VendorAMD, ///< Athlon and Duron
    CPU_uArchK8             = 0x103 | CPU_VendorAMD, ///< Athlon 64, Opteron 64
    CPU_uArchK10            = 0x104 | CPU_VendorAMD, ///< Family 10h (Barcelona, Istambul, Magny-Cours)
    ///< Bulldozer
    ///< Zambezi FX-series CPUs
    ///< Zurich, Valencia and Interlagos Opteron CPUs
    CPU_uArchBulldozer      = 0x105 | CPU_VendorAMD,
    ///< Piledriver
    ///< Vishera FX-series CPUs
    ///< Trinity and Richland APUs
    ///< Delhi, Seoul, Abu Dhabi Opteron CPUs
    CPU_uArchPiledriver     = 0x106 | CPU_VendorAMD,
    CPU_uArchSteamroller    = 0x107 | CPU_VendorAMD, ///< Steamroller (Kaveri APUs)
    CPU_uArchExcavator      = 0x108 | CPU_VendorAMD, ///< Excavator (Carizzo APUs)
    CPU_uArchZen            = 0x109 | CPU_VendorAMD, ///< Zen (12/14 nm Ryzen and EPYC CPUs)
    CPU_uArchZen2           = 0x10A | CPU_VendorAMD, ///< Zen 2 (7 nm Ryzen and EPYC CPUs)
    CPU_uArchGeode          = 0x200 | CPU_VendorAMD, ///< NSC Geode and Geode GX and LX
    CPU_uArchBobcat         = 0x201 | CPU_VendorAMD, ///< Bobcat mobile
    CPU_uArchJaguar         = 0x202 | CPU_VendorAMD, ///< Jaguar mobile
    CPU_uArchPuma           = 0x203 | CPU_VendorAMD, ///< Puma mobile

#elif kyArchIsARM
    ///< ARM
    CPU_uArchARM7           = 0x100 | CPU_VendorARM, ///< ARM7 series
    CPU_uArchARM9           = 0x101 | CPU_VendorARM, ///< ARM9 series
    CPU_uArchARM11          = 0x102 | CPU_VendorARM, ///< 1136, 1156, 1176, or 11MPCore
    CPU_uArchCortexA5       = 0x205 | CPU_VendorARM, ///< Cortex-A5
    CPU_uArchCortexA7       = 0x207 | CPU_VendorARM, ///< Cortex-A7
    CPU_uArchCortexA8       = 0x208 | CPU_VendorARM, ///< Cortex-A8
    CPU_uArchCortexA9       = 0x209 | CPU_VendorARM, ///< Cortex-A9
    CPU_uArchCortexA12      = 0x212 | CPU_VendorARM, ///< Cortex-A12
    CPU_uArchCortexA15      = 0x215 | CPU_VendorARM, ///< Cortex-A15
    CPU_uArchCortexA17      = 0x217 | CPU_VendorARM, ///< Cortex-A17
    CPU_uArchCortexA32      = 0x332 | CPU_VendorARM, ///< Cortex-A32
    CPU_uArchCortexA35      = 0x335 | CPU_VendorARM, ///< Cortex-A35
    CPU_uArchCortexA53      = 0x353 | CPU_VendorARM, ///< Cortex-A53
    CPU_uArchCortexA55      = 0x355 | CPU_VendorARM, ///< Cortex-A55
    CPU_uArchCortexA57      = 0x357 | CPU_VendorARM, ///< Cortex-A57
    CPU_uArchCortexA65      = 0x365 | CPU_VendorARM, ///< Cortex-A65
    CPU_uArchCortexA72      = 0x372 | CPU_VendorARM, ///< Cortex-A72
    CPU_uArchCortexA73      = 0x373 | CPU_VendorARM, ///< Cortex-A73
    CPU_uArchCortexA75      = 0x375 | CPU_VendorARM, ///< Cortex-A75
    CPU_uArchCortexA76      = 0x376 | CPU_VendorARM, ///< Cortex-A76
    CPU_uArchCortexA76AE    = 0x378 | CPU_VendorARM, ///< Cortex-A76AE
    CPU_uArchCortexA77      = 0x377 | CPU_VendorARM, ///< Cortex-A77
    CPU_uArchNeoverseN1     = 0x400 | CPU_VendorARM, ///< Neoverse N1
    CPU_uArchNeoverseE1     = 0x401 | CPU_VendorARM, ///< Neoverse E1

    ///< Qualcomm
    CPU_uArchScorpion       = 0x100 | CPU_VendorQualcomm, ///< Scorpion
    CPU_uArchKrait          = 0x101 | CPU_VendorQualcomm, ///< Krait
    CPU_uArchKryo           = 0x102 | CPU_VendorQualcomm, ///< Kryo
    CPU_uArchFalkor         = 0x103 | CPU_VendorQualcomm, ///< Falkor
    CPU_uArchSaphira        = 0x104 | CPU_VendorQualcomm, ///< Saphira

    ///< Nvidia
    CPU_uArchDenver         = 0x100 | CPU_VendorNvidia, ///< Denver
    CPU_uArchDenver2        = 0x101 | CPU_VendorNvidia, ///< Denver 2
    CPU_uArchCarmel         = 0x102 | CPU_VendorNvidia, ///< Carmel

    ///< Samsung
    CPU_uArchExynosM1       = 0x100 | CPU_VendorSamsung, ///< Exynos M1 (Exynos 8890 big cores)
    CPU_uArchExynosM2       = 0x101 | CPU_VendorSamsung, ///< Exynos M2 (Exynos 8895 big cores)
    CPU_uArchExynosM3       = 0x102 | CPU_VendorSamsung, ///< Exynos M3 (Exynos 9810 big cores)
    CPU_uArchExynosM4       = 0x103 | CPU_VendorSamsung, ///< Exynos M4 (Exynos 9820 big cores)
    CPU_uArchExynosM5       = 0x104 | CPU_VendorSamsung, ///< Exynos M5 (Exynos 9830 big cores)
    CPU_uArchMongooseM1     = CPU_ExynosM1, //< Old names for Exynos
    CPU_uArchMongooseM2     = CPU_ExynosM2,
    CPU_uArchMeerkatM3      = CPU_ExynosM3,
    CPU_uArchMeerkatM4      = CPU_ExynosM4,

    ///< Apple
    CPU_uArchSwift          = 0x100 | CPU_VendorApple, ///< A6 and A6X
    CPU_uArchCyclone        = 0x101 | CPU_VendorApple, ///< A7
    CPU_uArchTyphoon        = 0x102 | CPU_VendorApple, ///< A8 and A8X
    CPU_uArchTwister        = 0x103 | CPU_VendorApple, ///< A9 and A9X
    CPU_uArchHurricane      = 0x104 | CPU_VendorApple, ///< A10 and A10X
    CPU_uArchMonsoon        = 0x105 | CPU_VendorApple, ///< A11 (big cores)
    CPU_uArchMistral        = 0x106 | CPU_VendorApple, ///< A11 (little cores)
    CPU_uArchVortex         = 0x107 | CPU_VendorApple, ///< A12 (big cores)
    CPU_uArchTempest        = 0x108 | CPU_VendorApple, ///< A12 (little cores)

    ///< Cavium
    CPU_uArchThunderX       = 0x100 | CPU_VendorCavium, ///< ThunderX
    CPU_uArchThunderX2      = 0x200 | CPU_VendorCavium, ///< ThunderX2 (originally Broadcom Vulkan)

    ///< Marvell
    CPU_uArchPJ4            = 0x100 | CPU_VendorMarvell, ///< PJ4

    ///< Broadcom
    CPU_uArchBrahmaB15      = 0x100 | CPU_VendorBroadcom, ///< Brahma B15
    CPU_uArchBrahmaB53      = 0x101 | CPU_VendorBroadcom, ///< Brahma B53

    ///< Applied Micro
    CPU_uArchXGene          = 0x100 | CPU_VendorAPM, ///< X-Gene
#endif
}eCPUuArchs;

typedef enum
{
    CPU_TypePrimary,        // 主处理器
    CPU_TypeIntelOverDrive,
    CPU_TypeSecondary       // 辅助处理器
}eCPUTypes;

#if kyArchIsARM
typedef enum
{
    CPU_ARMv4    = (1 << 0),
    CPU_ARMv4T   = (1 << 1) | CPU_ARMv4,
    CPU_ARMv5    = (1 << 2) | CPU_ARMVvT,
    CPU_ARMv5T   = (1 << 3) | CPU_ARMv5,
    CPU_ARMv5TE  = (1 << 4) | CPU_ARMv5T,
    CPU_ARMv5TEJ = (1 << 5) | SUBARCH_v5TE,
    CPU_ARMv6    = (1 << 6) | CPU_ARMv5TEJ,
    CPU_ARMv6T2  = (1 << 7) | CPU_ARMv6,
    CPU_ARMv7    = (1 << 8) | CPU_ARMv6T2,
    CPU_ARMv8    = (1 << 9) | CPU_ARMv7,

    CPU_AES      = (1 << 10), // AES instructions
    CPU_SHA1     = (1 << 11), // SHA1 instructions
    CPU_SHA2     = (1 << 12), // SHA2 instructions
    CPU_CRC32    = (1 << 13), // CRC32 instructions
    CPU_EDSP     = (1 << 14), // V5E instructions
    CPU_VFP      = (1 << 15), // vector floating point instructions
    CPU_VFPv3    = (1 << 16)  // VFPv3 instructions
                   | CPU_VFP,
    CPU_VFPv4    = (1 << 17)  // fused multiply-add instructions
                   | CPU_VFPv3,
    CPU_VFPd32   = (1 << 18), // VFP (of any version) with 32 double-precision registers d0-d31
    CPU_IWMMXt   = (1 << 19), // intel/Marvell Wireless MMX instructions

    CPU_NEON     = (1 << 20), // NEON instructions (aka Advanced SIMD)
    CPU_THUMB    = (1 << 21), // support for 16-bit Thumb instruction set
    CPU_THUMB2   = (1 << 22), //
    CPU_THUMBEE  = (1 << 23), // ThumbEE instructions

    CPU_FP       = (1 << 24),
    CPU_SWP      = (1 << 25), // support for SWP instruction
    CPU_FPA      = (1 << 26), // floating point accelerator available
    CPU_TLS      = (1 << 27), // software thread ID registers
    CPU_FPHP     = (1 << 28),
    CPU_FCMA     = (1 << 29),
    CPU_HALF     = (1 << 30), // support for half-word loads and stores
    CPU_JAVA     = (1 << 31), // Jazelle extension

    CPU_Unknown = 0
}eCPUFeatureOnes;
typedef enum
{
    CPU_LPAE    = (1 << 0),  // Large Physical Address Extension (physical address up to 40 bits)
    CPU_TLSI    = (1 << 1),
    CPU_PMULL   = (1 << 2),  // Polinomial Multiplication instructions
    CPU_ASIMD   = (1 << 3),
    CPU_CPUID   = (1 << 4),
    CPU_JSCVT   = (1 << 5),
    CPU_LRCPC   = (1 << 6),
    CPU_26Bit   = (1 << 7),
    CPU_IDIVA   = (1 << 8),  // DIV instructions available in ARM mode
    CPU_IDIVT   = (1 << 9),  // DIV instructions available in Thumb mode
    CPU_CRUNCH  = (1 << 10), // Maverick Crunch instructions
    CPU_EVTSTRM = (1 << 11), // generation of Event Stream by timer
    CPU_ATOMICS = (1 << 12),
    CPU_ASIMDHP = (1 << 13),
    CPU_ASIMDRDM= (1 << 14),
    CPU_FASTMULT= (1 << 15),
    CPU_VFPv3d16= (1 << 16), // VFPv3 instructions with only 16 double-precision registers (d0-d15)
}eCPUFeatureTwos;

template<typename A, typename B>
struct __CPUOT_
{
    ky_flags<A> a;
    ky_flags<B> b;
    explicit __CPUOT_(const A &v){a = v;}
    explicit __CPUOT_(const B &v){b = v;}
    explicit __CPUOT_(const __CPUOT_ &v){a = v.a;b = v.b;}
    ~__CPUOT_(){}

    inline __CPUOT_ & operator = (const A &v){ a = v; return *this;}
    inline __CPUOT_ & operator = (const B &v){ b = v; return *this;}
    inline __CPUOT_ & operator = (const __CPUOT_ &v)
    {
        a = v.a;b = v.b;
        return *this;
    }

    inline bool operator &(const A &v){return a & v;}
    inline bool operator &(const B &v){return b & v;}
    inline bool operator &(const C &v){return c & v;}
    inline bool operator &(const D &v){return d & v;}
    inline bool operator &(const E &v){return e & v;}
    inline bool operator &(const F &v){return f & v;}

    inline __CPUOT_ &operator &= (const A &v)  {a &= v; return *this;}
    inline __CPUOT_ &operator &= (const B &v)  {b &= v; return *this;}
    inline __CPUOT_ &operator |= (const A &v)  {a |= v; return *this;}
    inline __CPUOT_ &operator |= (const B &v)  {b |= v; return *this;}
    inline __CPUOT_ &operator ^= (const A &v)  {a ^= v; return *this;}
    inline __CPUOT_ &operator ^= (const B &v)  {b ^= v; return *this;}

    inline __CPUOT_ operator & (const A &v)  {return __CPUOT_(a & v);}
    inline __CPUOT_ operator & (const B &v)  {return __CPUOT_(b & v);}
    inline __CPUOT_ operator | (const A &v)  {return __CPUOT_(a | v);}
    inline __CPUOT_ operator | (const B &v)  {return __CPUOT_(b | v);}
    inline __CPUOT_ operator ^ (const A &v)  {return __CPUOT_(a ^ v);}
    inline __CPUOT_ operator ^ (const B &v)  {return __CPUOT_(b ^ v);}
};
typedef __CPUOT_<eCPUFeatureOnes, eCPUFeatureTwos> eCPUFeatures;

#elif kyArchIsX86
// 2
typedef enum
{
    CPU_FPU       = 1 << 0,    ///</< x87 FPU on chip
    CPU_VME       = 1 << 1,    ///< VME: virtual-8086 mode enhancement
    CPU_DE        = 1 << 2,    ///< DE: debugging extensions
    CPU_PSE       = 1 << 3,    ///< PSE: page size extensions
    CPU_TSC       = 1 << 4,    ///< TSC: time stamp counter
    CPU_RDMSR     = 1 << 5,    ///< RDMSR and WRMSR support
    CPU_WRMSR     = CPU_RDMSR,
    CPU_PAE       = 1 << 6,    ///< PAE: physical address extensions
    CPU_MCE       = 1 << 7,    ///< MCE: machine check exception
    CPU_CMPXCHG8B = 1 << 8,    ///< PSE: page size extensions
    CPU_APIC      = 1 << 9,    ///<APIC on chip
    CPU_SYSENTER  = 1 << 10,   ///< SYSENTER and SYSEXIT
    CPU_SYSEXIT   = CPU_SYSENTER,
    CPU_MTRR      = 1 << 11,   ///< MTRR: memory type range registers
    CPU_PTE       = 1 << 12,   ///< PTE global bit
    CPU_MCA       = 1 << 13,   ///< MCA: machine check architecture
    CPU_CMOV      = 1 << 14,   ///< CMOV: conditional move/compare instr
    CPU_PAT       = 1 << 15,   ///< PAT: page attribute table
    CPU_PSE36     = 1 << 16,   ///< PSE-36: page size extension
    CPU_PSN       = 1 << 17,   ///< PSN: processor serial number
    CPU_CLFLUSH   = 1 << 18,   ///< CLFLUSH instruction
    CPU_DS        = 1 << 19,   ///< DS: debug store
    CPU_ACPI      = 1 << 20,   ///< ACPI: thermal monitor and clock ctrl
    CPU_MMX       = 1 << 21,   ///< MMX Technology
    CPU_FXSAVE    = 1 << 22,   ///< FXSAVE/FXRSTOR
    CPU_FXRSTOR = CPU_FXSAVE,
    CPU_SSE       = 1 << 23,   ///< SSE extensions
    CPU_SSE2      = 1 << 24,   ///< SSE2 extensions
    CPU_SS        = 1 << 25,   ///< SS: self snoop
    CPU_HT        = 1 << 26,   ///< hyper-threading / multi-core supported
    CPU_TM        = 1 << 27,   ///< TM: therm. monitor
    CPU_IA64      = 1 << 28,   ///< IA64
    CPU_PBE       = 1 << 29    ///< pending break event
}eCPUFeatureOnes;
///< 1
typedef enum
{
    CPU_PNI        = 1 << 00,    ///<"PNI/SSE3: Prescott New Instructions"
    CPU_SSE3       = CPU_PNI,
    CPU_PCLMULDQ   = 1 << 01,    ///<"PCLMULDQ instruction"
    CPU_DTES64     = 1 << 02,    ///<"DTES64: 64-bit debug store"
    CPU_MONITOR    = 1 << 03,    ///<"MONITOR/MWAIT"
    CPU_MWAIT      = CPU_MONITOR,
    CPU_QDS        = 1 << 04,    ///<"CPL-qualified debug store"
    CPU_VMX        = 1 << 05,    ///<"VMX: virtual machine extensions"
    CPU_SMX        = 1 << 06,    ///<"SMX: safer mode extensions"
    CPU_EIST       = 1 << 07,    ///<"Enhanced Intel SpeedStep Technology"
    CPU_TM2        = 1 << 08,    ///<"TM2: thermal monitor 2"
    CPU_SSSE3      = 1 << 09,    ///<"SSSE3 extensions"
    CPU_CTXID      = 1 << 10,    ///<"context ID: adaptive or shared L1 data"
    CPU_SDBG       = 1 << 11,    ///<"SDBG: IA32_DEBUG_INTERFACE"
    CPU_FMA        = 1 << 12,    ///<"FMA instruction"
    CPU_CMPXCHG16B = 1 << 13,    ///<"CMPXCHG16B instruction"
    CPU_xTPR       = 1 << 14,    ///<"xTPR disable"
    CPU_PDCM       = 1 << 15,    ///<"PDCM: perfmon and debug"
    CPU_PCID       = 1 << 16,    ///<"PCID: process context identifiers"
    CPU_DCA        = 1 << 17,    ///<"DCA: direct cache access"
    CPU_SSE41      = 1 << 18,    ///<"SSE4.1 extensions"
    CPU_SSE42      = 1 << 19,    ///<"SSE4.2 extensions"
    CPU_x2APIC     = 1 << 20,    ///<"x2APIC: extended xAPIC support"
    CPU_MOVBE      = 1 << 21,    ///<"MOVBE instruction"
    CPU_POPCNT     = 1 << 22,    ///<"POPCNT instruction"
    CPU_TSCD       = 1 << 23,    ///<"time stamp counter deadline"
    CPU_AES        = 1 << 24,    ///<"AES instruction"
    CPU_XSAVE      = 1 << 25,    ///<"XSAVE/XSTOR states"
    CPU_XSTOR      = CPU_XSAVE,
    CPU_OSE        = 1 << 26,    ///<"OS-enabled XSAVE/XSTOR"
    CPU_AVX        = 1 << 27,    ///<"AVX: advanced vector extensions"
    CPU_F16C       = 1 << 28,    ///<"F16C half-precision convert instruction"
    CPU_RDRAND     = 1 << 29,    ///<"RDRAND instruction"
    CPU_HGS        = 1 << 30     ///<"hypervisor guest status"
}eCPUFeatureTwos;
///< 0
typedef enum
{
    CPU_FSGSBASE  = 1 << 0,    ///<FSGSBASE instructions"
    CPU_TSCADJUST = 1 << 1 ,   ///<IA32_TSC_ADJUST MSR supported"
    CPU_SGX       = 1 << 2,    ///<SGX: Software Guard Extensions supported"
    CPU_BMI1      = 1 << 3,    ///<BMI1 instructions"
    CPU_HLE       = 1 << 4,    ///<HLE hardware lock elision"
    CPU_AVX2      = 1 << 5,    ///<AVX2: advanced vector extensions 2"
    CPU_FDP_EXCPTN= 1 << 6,    ///<FDP_EXCPTN_ONLY"
    CPU_SMEP      = 1 << 7,    ///<SMEP supervisor mode exec protection"
    CPU_BMI2      = 1 << 8,    ///<BMI2 instructions"
    CPU_MOVSB     = 1 << 9,    ///<enhanced REP MOVSB/STOSB"
    CPU_STOSB = CPU_MOVSB,
    CPU_INVPCID   = 1 << 10,    ///<INVPCID instruction"
    CPU_RTM       = 1 << 11,    ///<RTM: restricted transactional memory"
    CPU_RDT_CMT   = 1 << 12,    ///<RDT-CMT/PQoS cache monitoring"
    CPU_RDT_PQos = CPU_RDT_CMT,
    CPU_FPU_CS    = 1 << 13,    ///<deprecated FPU CS/DS"
    CPU_FPU_DS = CPU_FPU_CS,
    CPU_MPX       = 1 << 14,    ///<MPX: intel memory protection extensions"
    CPU_RDT_CAT   = 1 << 15,    ///<RDT-CAT/PQE cache allocation"
    CPU_RDT_PQE = CPU_RDT_CAT,
    CPU_AVX512F   = 1 << 16 ,   ///<AVX512F: AVX-512 foundation instructions"
    CPU_AVX512DQ  = 1 << 17,    ///<AVX512DQ: double & quadword instructions"
    CPU_RDSEED    = 1 << 18,    ///<RDSEED instruction"
    CPU_ADX       = 1 << 19,    ///<ADX instructions"
    CPU_SMAP      = 1 << 20,    ///<SMAP: supervisor mode access prevention"
    CPU_AVX512IFMA= 1 << 21,    ///<AVX512IFMA: fused multiply add"
    CPU_PCOMMIT   = 1 << 22,    ///<PCOMMIT instruction"
    CPU_CLFLUSHOPT= 1 << 23,    ///<CLFLUSHOPT instruction"
    CPU_CLWB      = 1 << 24,    ///<CLWB instruction"
    CPU_IPT       = 1 << 25,    ///<Intel processor trace"
    CPU_AVX512PF  = 1 << 26,    ///<AVX512PF: prefetch instructions"
    CPU_AVX512ER  = 1 << 27,    ///<AVX512ER: exponent & reciprocal instrs"
    CPU_AVX512CD  = 1 << 28,    ///<AVX512CD: conflict detection instrs"
    CPU_SHA       = 1 << 29,    ///<SHA instructions"
    CPU_AVX512BW  = 1 << 30,    ///<AVX512BW: byte & word instructions"
    CPU_AVX512VL  = 1 << 31     ///<AVX512VL: vector length"
}eCPUFeatureThrees;
///< 10
typedef enum
{
    CPU_PREFETCHWT1 = 1 << 0,    ///<PREFETCHWT1"
    CPU_AVX512VBMI  = 1 << 1,    ///<AVX512VBMI: vector byte manipulation"
    CPU_UMIP        = 1 << 2,    ///<UMIP: user-mode instruction prevention"
    CPU_PKU         = 1 << 3,    ///<PKU protection keys for user-mode"
    CPU_OSPKE       = 1 << 4,    ///<OSPKE CR4.PKE and RDPKRU/WRPKRU"
    CPU_CR4PKE  = CPU_OSPKE,
    CPU_RDPKRU  = CPU_OSPKE,
    CPU_WRPKRU  = CPU_OSPKE,
    CPU_WAITPKG     = 1 << 5,    ///<WAITPKG instructions"
    CPU_AVX512_VBMI2= 1 << 6,    ///<AVX512_VBMI2: byte VPCOMPRESS, VPEXPAND"
    CPU_CET_SS      = 1 << 7,    ///<CET_SS: CET shadow stack"
    CPU_GFNI        = 1 << 8,    ///<GFNI: Galois Field New Instructions"
    CPU_VAES        = 1 << 9,    ///<VAES instructions"
    CPU_VPCLMULQDQ  = 1 << 10,    ///<VPCLMULQDQ instruction"
    CPU_AVX512_VNNI = 1 << 11,    ///<AVX512_VNNI: neural network instructions"
    CPU_AVX512_BITALG= 1 << 12,    ///<AVX512_BITALG: bit count/shiffle"
    CPU_TME         = 1 << 13,    ///<TME: Total Memory Encryption"
    CPU_AVX512      = 1 << 14,    ///<AVX512: VPOPCNTDQ instruction"
    CPU_VPOPCNTDQ = CPU_AVX512,
    CPU_PAGING5L    = 1 << 15,    ///<5-level paging"
    CPU_RDPID       = 1 << 16,    ///<RDPID: read processor D supported"
    CPU_CLDEMOTE    = 1 << 17,    ///<CLDEMOTE supports cache line demote"
    CPU_MOVDIRI     = 1 << 18,    ///<MOVDIRI instruction"
    CPU_MOVDIR64B   = 1 << 19,    ///<MOVDIR64B instruction"
    CPU_ENQCMD      = 1 << 20,    ///<ENQCMD instruction"
    CPU_SGX_LC      = 1 << 21,    ///<SGX_LC: SGX launch config supported"
}eCPUFeatureFours;
///< 3
typedef enum
{
    CPU_AVX512_4VNNIW = 1 << 0,    ///<AVX512_4VNNIW: neural network instrs"
    CPU_AVX512_4FMAPS = 1 << 1,    ///<AVX512_4FMAPS: multiply acc single prec"
    CPU_FASTMOV       = 1 << 2,    ///<fast short REP MOV"
    CPU_AVX512_VP2INTERSECT = 1 << 3,    ///<AVX512_VP2INTERSECT: intersect mask regs"
    CPU_VERW          = 1 << 4,    ///<VERW md-clear microcode support"
    CPU_HP            = 1 << 5,    ///<hybrid part"
    CPU_PCONFIG       = 1 << 6,    ///<PCONFIG instruction"
    CPU_CET_IBT       = 1 << 7,    ///<CET_IBT: CET indirect branch tracking"
    CPU_IBRS          = 1 << 8,    ///<IBRS/IBPB: indirect branch restrictions"
    CPU_IBPB  = CPU_IBRS,
    CPU_STIBP         = 1 << 9,    ///<STIBP: 1 thr indirect branch predictor"
    CPU_L1D_FLUSH     = 1 << 10,    ///<L1D_FLUSH: IA32_FLUSH_CMD MSR"
    CPU_ARCHCAP       = 1 << 11,    ///<IA32_ARCH_CAPABILITIES MSR"
    CPU_CORECAP       = 1 << 12,    ///<IA32_CORE_CAPABILITIES MSR"
    CPU_SSBD          = 1 << 13,    ///<SSBD: speculative store bypass disable"
    CPU_AVX512_BF16   = 1 << 14,    ///<AVX512_BF16: bfloat16 instructions"

    CPU_SYSCALL = 1 << 15, ///< SYSCALL and SYSRET instructions
    CPU_SYSRET = CPU_SYSCALL,
    CPU_ExecDisable = 1 << 16, ///< execution disable
    CPU_1GBLPS = 1 << 17, ///< 1-GB large page support
    CPU_RDTSCP = 1 << 18, ///< RDTSCP
    CPU_64BitE = 1 << 19, ///< 64-bit extensions technology available
    CPU_GPE = 1 << 20, ///< global paging extension
    CPU_MPC = 1 << 21, ///< multiprocessing capable
    CPU_NEPP = 1 << 22, ///< no-execute page protection
    CPU_MMIE = 1 << 23, ///< AMD multimedia instruction extensions
    CPU_AA64 = 1 << 24, ///< long mode (AA-64)
    CPU_3DNowE = 1 << 25, ///< 3DNow! instruction extensions
    CPU_3DNow = 1 << 26, ///< 3DNow! instructions
    CPU_xMMX =  1 << 27, ///< extended MMX
    CPU_FPUCMOV = 1 << 28 ///< FPU conditional move instruction
}eCPUFeatureFives;

typedef enum
{
    CPU_LAHF64 = 1 << 0, ///< LAHF/SAHF supported in 64-bit mode
    CPU_SAHF64 = CPU_LAHF64,
    CPU_CMPL = 1 << 1, ///< CMP Legacy
    CPU_SVM = 1 << 2,  ///< SVM: secure virtual machine
    CPU_xAPICs = 1 << 3, ///< extended APIC space
    CPU_AltMovCr8 = 1 << 4, ///< AltMovCr8
    CPU_LZCNT = 1 << 5, ///< LZCNT advanced bit manipulation
    CPU_SSE4a = 1 << 6, ///< SSE4A support
    CPU_mSSE = 1 << 7, ///< misaligned SSE mode
    CPU_3DNowPREFETCH = 1 << 8, ///< 3DNow! PREFETCH/PREFETCHW instructions
    CPU_3DNowPREFETCHW = CPU_3DNowPREFETCH,
    CPU_OSVW = 1 << 9, ///< OS visible workaround
    CPU_IBS = 1 << 10,  ///< instruction based sampling
    CPU_XOP = 1 << 11, ///< XOP support
    CPU_SKINIT = 1 << 12, ///< SKINIT/STGI support
    CPU_STGI = CPU_SKINIT,
    CPU_WTS = 1 << 13, ///< watchdog timer support"
    CPU_LPS = 1 << 14, ///< lightweight profiling support
    CPU_FMA4 = 1 << 15, ///< 4-operand FMA instruction
    CPU_TCE = 1 << 16, ///< TCE: translation cache extension
    CPU_NID = 1 << 17, ///< NodeId MSR C001100C
    CPU_TBM = 1 << 18, ///< TBM support
    CPU_TPYX = 1 << 19, ///< topology extensions
    CPU_CPCE = 1 << 20, ///< core performance counter extensions
    CPU_NBPCE = 1 << 21, ///< NB/DF performance counter extensions
    CPU_SFPCE = CPU_NBPCE,
    CPU_DBE = 1 << 22, ///< data breakpoint extension
    CPU_PTSC = 1 << 23, ///< performance time-stamp counter support
    CPU_LLCPCE = 1 << 24, ///< LLC performance counter extensions
    CPU_MWAITX = 1 << 25, ///< MWAITX/MONITORX supported
    CPU_MONITORX = CPU_MWAITX,
    CPU_AME = 1 << 26, ///< Address mask extension support
}eCPUFeatureAMDs;
typedef enum ///< Extended Feature Extensions ID
{
    CPU_CLZERO = 1 << 0, ///< CLZERO instruction
    CPU_IRCS = 1 << 1, ///< instructions retired count support
    CPU_ASEP = 1 << 2, ///< always save/restore error pointers
    CPU_AREP = CPU_ASEP,
    CPU_MBE = 1 << 3, ///< memory bandwidth enforcement
    CPU_WBNOINVD = 1 << 4, ///< WBNOINVD instruction
    CPU_IBPB = 1 << 5, ///< IBPB: indirect branch prediction barrier
    CPU_IBRS = 1 << 6, ///< IBRS: indirect branch restr speculation
    CPU_STIBP = 1 << 7, ///< STIBP: 1 thr indirect branch predictor
    CPU_STIBP_AOPM = 1 << 8, ///< STIBP always on preferred mode
    CPU_PPIN = 1 << 9, ///< ppin processor id number supported
    CPU_SSBD = 1 << 10, ///<SSBD: speculative store bypass disable
    CPU_vSSBD = 1 << 11, ///< virtualized SSBD
    CPU_SSBD_FIHW = 1 << 12, ///< SSBD fixed in hardware
    CPU_RDPRU = 1 << 13 ///< DPRU instruction
}eCPUExtIDs;

template<typename A, typename B, typename C, typename D, typename E, typename F>
struct __CPUOT_
{
    ky_flags<A> a;
    ky_flags<B> b;
    ky_flags<C> c;
    ky_flags<D> d;
    ky_flags<E> e;
    ky_flags<F> f;
    explicit __CPUOT_(const A &v){a = v;}
    explicit __CPUOT_(const B &v){b = v;}
    explicit __CPUOT_(const C &v){c = v;}
    explicit __CPUOT_(const D &v){d = v;}
    explicit __CPUOT_(const E &v){e = v;}
    explicit __CPUOT_(const F &v){f = v;}
    explicit __CPUOT_(const __CPUOT_ &v){a = v.a;b = v.b;c = v.c;d = v.d;e = v.e;f = v.f;}
    ~__CPUOT_(){}

    inline __CPUOT_ & operator = (const A &v){ a = v; return *this;}
    inline __CPUOT_ & operator = (const B &v){ b = v; return *this;}
    inline __CPUOT_ & operator = (const C &v){ c = v; return *this;}
    inline __CPUOT_ & operator = (const D &v){ d = v; return *this;}
    inline __CPUOT_ & operator = (const E &v){ e = v; return *this;}
    inline __CPUOT_ & operator = (const F &v){ f = v; return *this;}
    inline __CPUOT_ & operator = (const __CPUOT_ &v)
    {
        a = v.a;b = v.b;c = v.c;
        d = v.d;e = v.e;f = v.f;
        return *this;
    }

    inline bool operator &(const A &v){return a & v;}
    inline bool operator &(const B &v){return b & v;}
    inline bool operator &(const C &v){return c & v;}
    inline bool operator &(const D &v){return d & v;}
    inline bool operator &(const E &v){return e & v;}
    inline bool operator &(const F &v){return f & v;}

    inline __CPUOT_ &operator &= (const A &v)  {a &= v; return *this;}
    inline __CPUOT_ &operator &= (const B &v)  {b &= v; return *this;}
    inline __CPUOT_ &operator &= (const C &v)  {c &= v; return *this;}
    inline __CPUOT_ &operator &= (const D &v)  {d &= v; return *this;}
    inline __CPUOT_ &operator &= (const E &v)  {e &= v; return *this;}
    inline __CPUOT_ &operator &= (const F &v)  {f &= v; return *this;}
    inline __CPUOT_ &operator |= (const A &v)  {a |= v; return *this;}
    inline __CPUOT_ &operator |= (const B &v)  {b |= v; return *this;}
    inline __CPUOT_ &operator |= (const C &v)  {c |= v; return *this;}
    inline __CPUOT_ &operator |= (const D &v)  {d |= v; return *this;}
    inline __CPUOT_ &operator |= (const E &v)  {e |= v; return *this;}
    inline __CPUOT_ &operator |= (const F &v)  {f |= v; return *this;}
    inline __CPUOT_ &operator ^= (const A &v)  {a ^= v; return *this;}
    inline __CPUOT_ &operator ^= (const B &v)  {b ^= v; return *this;}
    inline __CPUOT_ &operator ^= (const C &v)  {c ^= v; return *this;}
    inline __CPUOT_ &operator ^= (const D &v)  {d ^= v; return *this;}
    inline __CPUOT_ &operator ^= (const E &v)  {e ^= v; return *this;}
    inline __CPUOT_ &operator ^= (const F &v)  {f ^= v; return *this;}

    inline __CPUOT_ operator & (const A &v)  {return __CPUOT_(a & v);}
    inline __CPUOT_ operator & (const B &v)  {return __CPUOT_(b & v);}
    inline __CPUOT_ operator & (const C &v)  {return __CPUOT_(c & v);}
    inline __CPUOT_ operator & (const D &v)  {return __CPUOT_(d & v);}
    inline __CPUOT_ operator & (const E &v)  {return __CPUOT_(e & v);}
    inline __CPUOT_ operator & (const F &v)  {return __CPUOT_(f & v);}
    inline __CPUOT_ operator | (const A &v)  {return __CPUOT_(a | v);}
    inline __CPUOT_ operator | (const B &v)  {return __CPUOT_(b | v);}
    inline __CPUOT_ operator | (const C &v)  {return __CPUOT_(c | v);}
    inline __CPUOT_ operator | (const D &v)  {return __CPUOT_(d | v);}
    inline __CPUOT_ operator | (const E &v)  {return __CPUOT_(e | v);}
    inline __CPUOT_ operator | (const F &v)  {return __CPUOT_(f | v);}
    inline __CPUOT_ operator ^ (const A &v)  {return __CPUOT_(a ^ v);}
    inline __CPUOT_ operator ^ (const B &v)  {return __CPUOT_(b ^ v);}
    inline __CPUOT_ operator ^ (const C &v)  {return __CPUOT_(c ^ v);}
    inline __CPUOT_ operator ^ (const D &v)  {return __CPUOT_(d ^ v);}
    inline __CPUOT_ operator ^ (const E &v)  {return __CPUOT_(e ^ v);}
    inline __CPUOT_ operator ^ (const F &v)  {return __CPUOT_(f ^ v);}
};
typedef __CPUOT_<eCPUFeatureOnes, eCPUFeatureTwos, eCPUFeatureThrees,
                eCPUFeatureFours, eCPUFeatureFives, eCPUFeatureAMDs>
eCPUFeatures;

typedef enum
{
    CPU_Thermonmeter = 1 << 0, ///< "digital thermometer"
    CPU_TurboBoost   = 1 << 1, ///< "Intel Turbo Boost Technology"
    CPU_ARAT         = 1 << 2, ///< "ARAT always running APIC timer"
    CPU_PLN          = 1 << 3, ///< "PLN power limit notification"
    CPU_ECMD         = 1 << 4, ///< "ECMD extended clock modulation duty"
    CPU_PTM          = 1 << 5, ///< "PTM package thermal management"
    CPU_HWPBR        = 1 << 6, ///< "HWP base registers"
    CPU_HWPN         = 1 << 7, ///< "HWP notification"
    CPU_HWPAW        = 1 << 8, ///< "HWP activity window"
    CPU_HWPEPP       = 1 << 9, ///< "HWP energy performance preference"
    CPU_HWPPLR       = 1 << 10,///< "HWP package level request"
    CPU_HDCBR        = 1 << 11,///< "HDC base registers"
    CPU_TurboBoostMax3 = 1 << 12, ///< "Intel Turbo Boost Max Technology 3.0"
    CPU_HWPCAP       = 1 << 13, ///< "HWP capabilities"
    CPU_HWPPECI      = 1 << 14, ///< "HWP PECI override"
    CPU_HWPF         = 1 << 15, ///< "flexible HWP"
    CPU_HWP_REQUEST  = 1 << 16, ///< "IA32_HWP_REQUEST MSR fast access mode"
    CPU_HW_FEEDBACK  = 1 << 17, ///< "HW_FEEDBACK"
    CPU_IILPHWPR     = 1 << 18, ///< "ignoring idle logical processor HWP req"
    CPU_hWC_FEEDBACK = 1 << 19, ///< "hardware coordination feedback"
    CPU_ACNT2        = 1 << 20, ///< "ACNT2 available"
    CPU_PEBC         = 1 << 21, ///< "performance-energy bias capability"
    CPU_PCR          = 1 << 22, ///< "performance capability reporting"
    CPU_EECR         = 1 << 23  ///< "energy efficiency capability reporting"
}ePowerFeatures;

#elif kyArchIsPowerPC

#elif kyArchIsMIPS

#endif


class ky_cpu
{
public:
    struct cache_t
    {
        enum Flag
        {
            Null     = 0,
            L1       = 1,
            L2       = 2,
            L3       = 3,
            L4       = 4,

            TLB         = 1 << 5, //< TLB缓存
            Data        = 1 << 6, //< 数据缓存
            Instruction = 1 << 7, //< 指令缓存
            Unified =             //< 通用缓存
                      Data | Instruction,
            Static      = 1 << 8, //< 静态结构

#if kyArchIsX86
            Prefetch         = 9,  //< 预取能力
            Range            = 10, //< 缓存是范围的
            CPUID            = 11, //< 缓存位置在CPUID 0x18 or 0x04
            Trace            = 1 << 9, //< 跟踪缓存
            SelfInitial      = 1 << 10, // "self-initializing cache level"
            FullyAssociative = 1 << 11, // "fully associative cache"
            WBINVD           = 1 << 12, // "WBINVD/INVD acts on lower caches"
            WriteBackInvalid = WBINVD,  // "write-back invalidate"
            InclusiveLower   = 1 << 13, // "inclusive to lower caches"
            ComplexIndex     = 1 << 14, // "complex cache indexing"
#endif

        };

        const uint8  code;
        // 缓存大小（以字节为单位）
        const uint   size;     // TLB:pages, cache:line size
        // 关联方式路数
        const uint8  associate;// -way
        // 如果为0，则禁用高速缓存，并且所有其他值为零
        const uint16 entries;  // entries
        const uint16 flags;    // level, type
        const uint8  sharedBy; // 共享此缓存的线程数
        const uint8  core;     // 该芯片上的处理器内核数量
        cache_t     *next;

        explicit cache_t(const struct arch_cache* ac);
        ~cache_t();

        //!
        //! \brief is_valid 是否有效
        //! \return
        //!
        bool is_valid() const;

        //!
        //! \brief total 总数
        //! \return
        //!
        uint total() const;

    private:
        static cache_t *query(const uint &idx);
        friend class ky_cpu;
    };

private:
    ky_cpu(){}
    ~ky_cpu(){}

public:
    //!
    //! \brief has cpu支持的能力
    //! \param cap
    //! \return
    //!
    static bool has(eCPUFeatures cap);

    //!
    //! \brief cache CPU缓存
    //! \param index
    //! \return
    //!
    static cache_t *cache(const uint &index);
    //!
    //! \brief vendor CPU厂商
    //! \return
    //!
    static eCPUVendors vendor();

    //!
    //! \brief count cpu核心数
    //! \return
    //!
    static int count();

    //!
    //! \brief page_size cpu 页尺寸
    //! \return
    //!
    static int page_size();
    //!
    //! \brief large_page_size cpu 大页尺寸
    //! \return
    //!
    static int large_page_size();
};


#endif // ky_CPU_H
