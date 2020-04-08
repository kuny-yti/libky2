
#ifndef ARM_CPU_ABILIRY_H
#define ARM_CPU_ABILIRY_H

#include "ky_define.h"
#include "arch/ky_cpu.h"

#define __midr_implementerBitWide (8)
#define __midr_variantBitWide     (4)
#define __midr_archBitWide        (4)
#define __midr_partBitWide        (12)
#define __midr_revisionBitWide    (4)

#define __midr_implementerFlags   (24)
#define __midr_variantFlags       (20)
#define __midr_archFlags          (16)
#define __midr_partFlags          (4)
#define __midr_revisionFlags      (0)

#define __midrARMFlags       (0x41 << 24)
#define __midrKryoFlags      (0x51 << 24)
#define __midrExynosFlags    (0x53 << 24)
#define __midrDenver2Flags   (0x4e << 24)
#define __midrHiSiliconFlags (0x48 << 24)
#define __midrRockFlags       __midrARMFlags

#define midrFlags(B) (__midr_##B##Flags)
#define midrMaskField(B) (((1 << __midr_##B##BitWide) -1) << midrFlags(B))

#define midrARM1156   (__midrARMFlags | 0xfb560)
#define midrCortexA7  (__midrARMFlags | 0xfc070)
#define midrCortexA9  (__midrARMFlags | 0xfc090)
#define midrCortexA15 (__midrARMFlags | 0xfc0f0)
#define midrCortexA17 (__midrARMFlags | 0xfc0e0)
#define midrCortexA35 (__midrARMFlags | 0xfd040)
#define midrCortexA53 (__midrARMFlags | 0xfd030)
#define midrCortexA55 (__midrARMFlags | 0xfd050)
#define midrCortexA57 (__midrARMFlags | 0xfd070)
#define midrCortexA72 (__midrARMFlags | 0xfd080)
#define midrCortexA73 (__midrARMFlags | 0xfd090)
#define midrCortexA75 (__midrARMFlags | 0xfd0a0)

#define midrKryo280Gold   (__midrKryoFlags | 0xaf8001)
#define midrKryo280Silver (__midrKryoFlags | 0xaf8014)
#define midrKryo385Gold   (__midrKryoFlags | 0x8f802d)
#define midrKryo385Silver (__midrKryoFlags | 0x8f803c)
#define midrKryoSilver821 (__midrKryoFlags | 0x0f2010)
#define midrKryoGold      (__midrKryoFlags | 0x0f2050)
#define midrKryoSilver820 (__midrKryoFlags | 0x0f2110)

#define midrExynosM1_2    (__midrExynosFlags | 0xf0010)
#define midrDenver2       (__midrDenver2Flags | 0xf0030)

struct midr_t
{
    u32 value;
    operator u32()()const{return value;}
    operator u32()(){return value;}

    inline void set_implementer(u32 v)
    {
        value = (value & ~midrMaskField(implementer)) |
                ((v << midrFlags(implementer)) & midrMaskField(implementer));
    }
    inline void set_variant(u32 v)
    {
        value = (value & ~midrMaskField(variant)) |
                ((v << midrFlags(variant)) & midrMaskField(variant));
    }
    inline void set_arch(u32 v)
    {
        value = (value & ~midrMaskField(arch)) |
                ((v << midrFlags(arch)) & midrMaskField(arch));
    }
    inline void set_part(u32 v)
    {
        value = (value & ~midrMaskField(part)) |
                ((v << midrFlags(part)) & midrMaskField(part));
    }
    inline void set_revision(u32 v)
    {
        value = (value & ~midrMaskField(revision)) |
                ((v << midrFlags(revision)) & midrMaskField(revision));
    }

    inline u32 implementer()const
    {
        return (value & midrMaskField(implementer)) >> midrFlags(implementer);
    }
    inline u32 variant()const
    {
        return (value & midrMaskField(variant)) >> midrFlags(variant);
    }
    inline u32 arch()const
    {
        return (value & midrMaskField(arch)) >> midrFlags(arch);
    }
    inline u32 part()const
    {
        return (value & midrMaskField(part)) >> midrFlags(part);
    }
    inline u32 revision()const
    {
        return (value & midrMaskField(revision)) >> midrFlags(revision);
    }

    inline void cpy_implementer(u32 other)
    {
        value = (value & ~midrMaskField(implementer)) |
                (other & midrMaskField(implementer));
    }
    inline void cpy_variant(u32 other)
    {
        value = (value & ~midrMaskField(variant)) |
                (other & midrMaskField(variant));
    }
    inline void cpy_arch(u32 other)
    {
        value = (value & ~midrMaskField(arch)) |
                (other & midrMaskField(arch));
    }
    inline void cpy_part(u32 other)
    {
        value = (value & ~midrMaskField(part)) |
                (other & midrMaskField(part));
    }
    inline void cpy_revision(u32 other)
    {
        value = (value & ~midrMaskField(revision)) |
                (other & midrMaskField(revision));
    }

    inline bool is_arm1156()const
    {
        const u32 ua_mask = midrMaskField(implementer) | midrMaskField(part);
        return (value & ua_mask) == (midrARM1156 & ua_mask);
    }
    inline bool is_arm11()const
    {
        return (value & (midrMaskField(implementer) | 0xf000)) == (__midrARMFlags | 0xb000);
    }
    inline bool is_cortex_a9()const
    {
        const u32 ua_mask = midrMaskField(implementer) | midrMaskField(part);
        return (value & ua_mask) == (midrCortexA9 & ua_mask);
    }
    inline bool is_cortex_a53()const
    {
        const u32 ua_mask = midrMaskField(implementer) | midrMaskField(part);
        return (value & ua_mask) == (midrCortexA53 & ua_mask);
    }
    inline bool is_cortex_a53_qualcomm_silver()const
    {
        const u32 ua_mask = midrMaskField(implementer) | midrMaskField(part);
        return (value & ua_mask) == (midrKryo280Silver & ua_mask);
    }
    inline bool is_cortex_a55_qualcomm_silver()const
    {
        const u32 ua_mask = midrMaskField(implementer) | midrMaskField(part);
        return (value & ua_mask) == (midrKryo385Silver & ua_mask);
    }
    inline bool is_kryo280_gold()const
    {
        const u32 ua_mask = midrMaskField(implementer) | midrMaskField(part);
        return (value & ua_mask) == (midrKryo280Gold & ua_mask);
    }
    inline bool is_kryo_silver()const
    {
        const u32 ua_mask = midrMaskField(implementer) | midrMaskField(part);
        switch (value & ua_mask)
        {
            case midrKryoSilver820:
            case midrKryoSilver821:
                return true;
        }
        return false;
    }
    inline bool is_kryo_gold()const
    {
        const u32 ua_mask = midrMaskField(implementer) | midrMaskField(part);
        return (value & ua_mask) == (midrKryoGold & ua_mask);
    }

    inline bool is_scorpion()const
    {
        const u32 ua_mask = midrMaskField(implementer) | midrMaskField(part);
        switch (value & ua_mask)
        {
            case __midrKryoFlags | (0xf0):
            case __midrKryoFlags | (0x02d0):
                return true;
        }
        return false;
    }
    inline bool is_krait()const
    {
        const u32 ua_mask = midrMaskField(implementer) | midrMaskField(part);
        switch (value & ua_mask)
        {
            case __midrKryoFlags | (0x04d0):
            case __midrKryoFlags | (0x06f0):
                return true;
        }
        return false;
    }

    inline u32 score_core()
    {
        const u32 core_mask = midrMaskField(implementer) | midrMaskField(part);
        for (uint i = 0; i < kyArraySizeOf(midr_model_table); ++i)
        {
            if ((value & core_mask) == midr_model_table[i].code)
                return midr_model_table[i].core;
        }
        /*
         * Unknown cores, or cores which do not have big/LITTLE roles.
         * To be future-proof w.r.t. cores not yet recognized in cpuinfo, assume position between
         * Cortex-A57/A72/A73/A75 and Cortex-A53/A55. Then at least future cores paired with
         * one of these known cores will be properly scored.
         */
        return 3;
    }

    inline static u32 little_core_for_big()
    {
        const u32 core_mask = midrMaskField(implementer) | midrMaskField(part) | midrMaskField(arch);
        switch (value & core_mask)
        {
            case midrCortexA75:
                return midrCortexA55;
            case midrCortexA73:
            case midrCortexA72:
            case midrCortexA57:
            case midrExynosM1_2:
                return midrCortexA53;
            case midrCortexA17:
            case midrCortexA15:
                return midrCortexA7;
            case midrKryo280Gold:
                return midrKryo280Silver;
            case midrKryoGold:
                return midrKryoSilver820;
            case midrDenver2:
                return midrCortexA57;
        }
        return value;
    }
};

typedef enum
{
    Chipset_unknown = 0,
    // Qualcomm
    Chipset_QSD ,
    Chipset_MSM,
    Chipset_APQ,
    Chipset_Snapdragon,
    // Mediatek
    Chipset_MT,
    // Samsung
    Chipset_Exynos,
    // Hisilicon
    Chipset_K3V,
    Chipset_HI,
    Chipset_Kirin,
    // Actions
    Chipset_ATM,
    // Allwinner
    Chipset_A,
    // Amlogic
    Chipset_AML,
    Chipset_S,
    // Broadcom
    Chipset_BCM,
    // LG
    Chipset_Nuclun,
    // Leadcore
    Chipset_LC,
    // Marvell
    Chipset_PXA,
    // Mstar
    Chipset_6A,
    // Novathor
    Chipset_U,
    // Nvidia
    Chipset_TegraT,
    Chipset_TegraAP,
    Chipset_TegraSL,
    // Pinecone
    Chipset_SurgeS,
    // Renesas
    Chipset_MP,
    // Rockchip
    Chipset_RK,
    // Spreadtrum
    Chipset_SC,
    // Telechips
    Chipset_TCC,
    // TexasInstruments
    Chipset_OMAP,
    // Wondermedia
    Chipset_WM,

    Chipset_Count
}eChipsetSeries;

#if 1|| kyArchIsARM

class cpu_ability : ky_singleton<cpu_ability>
{
    friend class ky_cpu;
    friend class ky_singleton<cpu_ability>;
private:
    cpu_ability();
    ~cpu_ability();

    struct processor_x{u32 first; u32 count;};

    struct arm_cache
    {
        /** Cache size in bytes */
        u32 size;
        /** Number of ways of associativity */
        u32 associativity;
        /** Number of sets */
        u32 sets;
        /** Number of partitions */
        u32 partitions;
        /** Line size in bytes */
        u32 line_size;
        /**
         * Binary characteristics of the cache (unified cache, inclusive cache, cache with complex indexing).
         *
         * @see CPUINFO_CACHE_UNIFIED, CPUINFO_CACHE_INCLUSIVE, CPUINFO_CACHE_COMPLEX_INDEXING
         */
        u32 flags;
        processor_x shareBy;
    };

    struct arm_tlb
    {
        u32 entries;
        u32 associativity;
        u64 pages;
    };

    //!
    //! \brief The package_t struct 处理器物理封装
    //!
    struct arm_package
    {

        char       *name;    ///< 芯片名称
        processor_x logical; ///< 处理器逻辑核
        processor_x cores;   ///< 处理器物理核
        processor_x cluster; ///< 处理器集群
    };
    ///!
    //! \brief The cluster_t struct 处理器集群
    //!
    struct arm_cluster
    {
        u32         id;      ///< 在封装中的集群id
        eCPUVendors vendor;  ///< 供应商
        eCPUuArchs  uarch;   ///< 体系架构
        processor_x logical; ///< 处理器逻辑核信息
        processor_x cores;   ///< 处理器物理核信息

        u32         midr;
        u64         frequency; ///< 核心时钟频率 (non-Turbo, in Hz)
        const arm_package* package; ///< 物理封装
    };
    struct arm_cores
    {
        u32         id;     ///< 在封装中的核心id
        eCPUVendors vendor; ///< 供应商
        eCPUuArchs  uarch;  ///< 体系架构
        processor_x logical; ///< 处理器逻辑核信息
        u32         midr;
        u64         frequency; ///< 核心时钟频率 (non-Turbo, in Hz)
        const arm_cluster* cluster; ///< 集群信息
        const arm_package* package; ///< 物理封装
    };
    struct arm_processor
    {
        i32        id;
        u32        smt_id; ///< 内核中的 SMT (hyperthread) ID

        const arm_cores   *core;    ///< 物理核信息
        const arm_cluster *cluster; ///< 集群信息
        const arm_package *package; ///< 物理封装
        struct
        {
            /** Level 1 instruction cache */
            const struct arm_cache* l1i;
            /** Level 1 data cache */
            const struct arm_cache* l1d;
            /** Level 2 unified or data cache */
            const struct arm_cache* l2;
            /** Level 3 unified or data cache */
            const struct arm_cache* l3;
            /** Level 4 unified or data cache */
            const struct arm_cache* l4;
        } cache; ///< 缓存信息

        arm_processor *next;
    };

    struct cache_t
    {
        u32 i_size;
        u32 i_assoc;
        u32 i_line_length;
        u32 i_sets;
        u32 d_size;
        u32 d_assoc;
        u32 d_line_length;
        u32 d_sets;
    };
    struct processor_t
    {
        u32            version;
        u32            arch_flags;
        struct cache_t cache;

        u32      features;
        u32      features2;
        u32      midr;   ///< Main ID Register value.
        eCPUVendors vendor;
        eCPUuArchs  uarch;

        struct
        {
            u32 id;           ///< 封装的ID
            u32 leader_id;    ///< 封装的最小处理器ID
            u32 logical_count; ///< 封装中逻辑处理器的数量
        } package; ///< 逻辑处理器的物理封装

        struct
        {
            u32 max; ///< 最大频率 (kHZ)
            u32 min; ///< 最小频率 (kHZ)
        } freauency;///< 处理器的频率

        u32      sys_id; ///< system processor ID
        u32      flags;
    };

    u32          max_count;
    midr_t       midr;
    arm_processor proc;
};
#endif

#endif
