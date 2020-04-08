#ifndef ARM_FEATURE_TABLE_H
#define ARM_FEATURE_TABLE_H


static const struct
{
    const char *const match;
    const eCPUFeatures code;
    const char *const str;
}
arm_feature_table[] =
{
{"fp", CPU_FP, "fp"},
{"wp", CPU_SWP, "wp"},

{"swp", CPU_SWP, "support for SWP instruction"},
{"aes", CPU_AES, "AES instructions"},
{"fpa", CPU_FPA, "floating point accelerator available"},
{"vfp", CPU_VFP, "vector floating point instructions"},
{"tls", CPU_TLS, "software thread ID registers"},

{"sha1", CPU_SHA1, "SHA1 instructions"},
{"sha2", CPU_SHA2, "SHA2 instructions"},
{"fphp", CPU_FPHP, "fphp"},
{"fcma", CPU_FCAM, "fcma"},
{"half", CPU_HALF, "support for half-word loads and stores"},
{"edsp", CPU_EDSP, "V5E instructions"},
{"java", CPU_JAVA, "Jazelle extension"},
{"neon", CPU_NEON, "NEON instructions (aka Advanced SIMD)"},
{"lpae", CPU_LPAE, "Large Physical Address Extension (physical address up to 40 bits)"},
{"tlsi", CPU_TLSI, "tlsi"},

{"pmull", CPU_PMULL, "Polinomial Multiplication instructions"},
{"crc32", CPU_CRC32, "CRC32 instructions"},
{"asimd", CPU_ASIMD, "asimd"},
{"cpuid", CPU_CPUID, "cpuid"},
{"jscvt", CPU_JSCVT, "jscvt"},
{"lrcpc", CPU_LRCPC, "lrcpc"},
{"thumb", CPU_Thumb, "support for 16-bit Thumb instruction set"},
{"26bit", CPU_26Bit, "26bit"},
{"vfpv3", CPU_VFPv3, "VFPv3 instructions"},
{"vfpv4", CPU_VFPv4, "fused multiply-add instructions"},
{"idiva", CPU_IDIVA, "DIV instructions available in ARM mode"},
{"idivt", CPU_IDIVT, "DIV instructions available in Thumb mode"},

{"iwmmxt", CPU_IWMMXT, "intel/Marvell Wireless MMX instructions"},
{"crunch", CPU_CRUNCH, "Maverick Crunch instructions"},
{"vfpd32", CPU_VFPd32, "VFP (of any version) with 32 double-precision registers d0-d31"},

{"evtstrm", CPU_EVTSTRM, "generation of Event Stream by timer"},
{"atomics", CPU_ATOMICS, "atomics"},
{"asimdhp", CPU_ASIMDHP, "asimdhp"},
{"thumbee", CPU_ThumbEE, "ThumbEE instructions"},

{"asimdrdm", CPU_ASIMDRDM, "asimdrdm"},
{"fastmult", CPU_FASTMULT, "fastmult"},
{"vfpv3d16", CPU_VFPv3d16, "VFPv3 instructions with only 16 double-precision registers (d0-d15)"}
};
static const uint arm_feature_table_index[] =
{
    0, // 0
    1, // 0

    0, // 2 lenth
    2, // 3 lenth
    7, // 4 lenth
    17, // 5 lenth
    29, // 6 lenth
    32, // 7 lenth
    36, // 7 lenth
    kyArraySizeOf(arm_feature_table)
};

static const struct
{
    const uint        code;
    const uint        core;
    const char *const str;
}
midr_model_table[] =
{
{__midrExynosFlags    | 0x40,     6, "Exynos M5"},
{__midrExynosFlags    | 0x30,     6, "Exynos M4"},
{__midrDenver2Flags   | 0x30,     5, "Denver 2"},
{__midrExynosFlags    | 0x10,     5, "Exynos M1 and Exynos M2"},
{__midrExynosFlags    | 0x20,     5, "Exynos M3"},
{__midrExynosFlags    | 0x8040,   5, "Kryo 485 Gold / Gold Prime"},
{__midrExynosFlags    | 0x8020,   5, "Kryo 385 Gold"},
{__midrExynosFlags    | 0x8000,   5, "Kryo 260 / 280 Gold"},
{__midrExynosFlags    | 0x2050,   5, "Kryo Gold"},
{__midrHiSiliconFlags | 0xd400,   5, "Cortex-A76 (HiSilicon)"},
{__midrARMFlags       | 0xd0d0,   5, "Cortex-A77"},
{__midrARMFlags       | 0xd0e0,   5, "Cortex-A76AE"},
{__midrARMFlags       | 0xd0b0,   5, "Cortex-A76"},
{__midrARMFlags       | 0xd0a0,   5, "Cortex-A75"},
{__midrARMFlags       | 0xd090,   5, "Cortex-A73"},
{__midrARMFlags       | 0xd080,   5, "Cortex-A72"},
{__midrARMFlags       | 0xc0f0,   5, "Cortex-A15"},
{__midrARMFlags       | 0xc0e0,   5, "Cortex-A17"},

{__midrRockFlags      | 0xc0d0,   5, "Rockchip RK3288 cores"},
{__midrARMFlags       | 0xc0c0,   5, "Cortex-A12"},
{__midrARMFlags       | 0xd070,   4, "Cortex-A57"},

{__midrARMFlags       | 0xd060,   2, "Cortex-A65"},
{__midrARMFlags       | 0xd050,   2, "Cortex-A55"},
{__midrARMFlags       | 0xd030,   2, "Cortex-A53"},

{__midrARMFlags       | 0xd040,   1, "Cortex-A35"},
{__midrARMFlags       | 0xc070,   1, "Cortex-A7"},
{__midrKryoFlags      | 0x8050,   1, "Kryo 485 Silver"},
{__midrKryoFlags      | 0x8030,   1, "Kryo 385 Silver"},
{__midrKryoFlags      | 0x8010,   1, "Kryo 260 / 280 Silver"},
{__midrKryoFlags      | 0x2110,   1, "Kryo Silver (Snapdragon 820)"},
{__midrKryoFlags      | 0x2010,   1, "Kryo Silver (Snapdragon 821)"},

};


//!
//! \brief The cluster config 芯片组的核心群配置描述(由系列和型号标识)
//!
static const struct {
    /* 处理器逻辑核心数 */
    u8             cores;
    /* ARM 芯片组系列 */
    eChipsetSeries series;
    /* 芯片组型号 */
    u16            model;
    /* 异构群数 */
    u8             clusters;
    /*
     * Number of cores in each cluster:
     # - Symmetric configurations: [0] = # cores
     * - big.LITTLE configurations: [0] = # LITTLE cores, [1] = # big cores
     * - Max.Med.Min configurations: [0] = # Min cores, [1] = # Med cores, [2] = # Max cores
     */
    uint8_t cluster_cores[3];
    /*
     * MIDR of cores in each cluster:
     * - Symmetric configurations: [0] = core MIDR
     * - big.LITTLE configurations: [0] = LITTLE core MIDR, [1] = big core MIDR
     * - Max.Med.Min configurations: [0] = Min core MIDR, [1] = Med core MIDR, [2] = Max core MIDR
     */
    uint32_t cluster_midr[3];
}
/*
 * The list of chipsets where MIDR may not be unambigiously decoded at least on some devices.
 * The typical reasons for impossibility to decoded MIDRs are buggy kernels, which either do not report all MIDR
 * information (e.g. on ATM7029 kernel doesn't report CPU Part), or chipsets have more than one type of cores
 * (i.e. 4x Cortex-A53 + 4x Cortex-A53 is out) and buggy kernels report MIDR information only about some cores
 * in /proc/cpuinfo (either only online cores, or only the core that reads /proc/cpuinfo). On these kernels/chipsets,
 * it is not possible to detect all core types by just parsing /proc/cpuinfo, so we use chipset name and this table to
 * find their MIDR (and thus microarchitecture, cache, etc).
 *
 * Note: not all chipsets with heterogeneous multiprocessing need an entry in this table. The following HMP
 * chipsets always list information about all cores in /proc/cpuinfo:
 *
 * - Snapdragon 660
 * - Snapdragon 820 (MSM8996)
 * - Snapdragon 821 (MSM8996PRO)
 * - Snapdragon 835 (MSM8998)
 * - Exynos 8895
 * - Kirin 960
 *
 * As these are all new processors, there is hope that this table won't uncontrollably grow over time.
 */
cluster_configs[] =
{
#if kyArchIsARM
/* MSM8916 (Snapdragon 410): 4x Cortex-A53 */
{ 4, Chipset_MSM, 8916, 1, {4,}, {0x410fd030,} },
/* MSM8939 (Snapdragon 615): 4x Cortex-A53 + 4x Cortex-A53 */
{ 8, Chipset_MSM, 8939, 2, {4,4,}, {0x410fd034, 0x410fd034,} },
#endif
/* MSM8956 (Snapdragon 650): 2x Cortex-A72 + 4x Cortex-A53 */
{ 6, Chipset_MSM, 8956, 2, {4, 2,}, {0x410fd034, 0x410fd080,} },
/* MSM8976/MSM8976PRO (Snapdragon 652/653): 4x Cortex-A72 + 4x Cortex-A53 */
{ 8, Chipset_MSM, 8976, 2, {4, 4,}, {0x410fd034, 0x410fd080,} },
/* MSM8992 (Snapdragon 808): 2x Cortex-A57 + 4x Cortex-A53 */
{ 6, Chipset_MSM, 8992, 2, {4, 2,}, {0x410fd033, 0x411fd072,} },
/* MSM8994/MSM8994V (Snapdragon 810): 4x Cortex-A57 + 4x Cortex-A53 */
{ 8, Chipset_MSM, 8994, 2, {4, 4,}, {0x410FD032, 0x411FD071,} },
#if kyArchIsARM
/* Exynos 5422: 4x Cortex-A15 + 4x Cortex-A7 */
{ 8, Chipset_Exynos, 5422, 2, {4, 4,}, {0x410fc073, 0x412fc0f3,} },
/* Exynos 5430: 4x Cortex-A15 + 4x Cortex-A7 */
{ 8, Chipset_Exynos, 5430, 2, {4, 4,}, {0x410fc074, 0x413fc0f3,} },
#endif
/* Exynos 5433: 4x Cortex-A57 + 4x Cortex-A53 */
{ 8, Chipset_Exynos, 5433, 2, {4, 4,}, {0x410fd031, 0x411fd070,} },
/* Exynos 7420: 4x Cortex-A57 + 4x Cortex-A53 */
{ 8, Chipset_Exynos, 7420, 2, {4, 4,}, {0x410fd032, 0x411fd070,} },
/* Exynos 8890: 4x Exynos M1 + 4x Cortex-A53 */
{ 8, Chipset_Exynos, 8890, 2, {4, 4,}, {0x410fd034, 0x531f0011,} },

#if kyArchIsARM
/* Kirin 920: 4x Cortex-A15 + 4x Cortex-A7 */
{ 8, Chipset_Kirin, 920, 2, {4, 4,}, {0x410fc075, 0x413fc0f3,} },
/* Kirin 925: 4x Cortex-A15 + 4x Cortex-A7 */
{ 8, Chipset_Kirin, 925, 2, {4, 4,}, {0x410fc075, 0x413fc0f3,} },
/* Kirin 928: 4x Cortex-A15 + 4x Cortex-A7 */
{ 8, Chipset_Kirin, 928, 2, {4, 4,}, {0x410fc075, 0x413fc0f3,} },
#endif
/* Kirin 950: 4x Cortex-A72 + 4x Cortex-A53 */
{ 8, Chipset_Kirin, 950, 2, {4, 4,}, {0x410fd034, 0x410fd080,} },
/* Kirin 955: 4x Cortex-A72 + 4x Cortex-A53 */
{ 8, Chipset_Kirin, 955, 2, {4, 4,}, {0x410fd034, 0x410fd080,} },

#if kyArchIsARM
/* MediaTek MT8135: 2x Cortex-A7 + 2x Cortex-A15 */
{ 4, Chipset_MT, 8135, 2, {2, 2,}, {0x410fc073, 0x413fc0f2,} },
#endif
/* MediaTek MT8173: 2x Cortex-A72 + 2x Cortex-A53 */
{ 4, Chipset_MT, 8173, 2, {2, 2,}, {0x410fd032, 0x410fd080,} },
/* MediaTek MT8176: 2x Cortex-A72 + 4x Cortex-A53 */
{ 6, Chipset_MT, 8176, 2, {2, 4,}, {0x410fd032, 0x410fd080,} },
#if kyArchIs64Bit
/* MediaTek MT8735: 4x Cortex-A53 */
{ 4, Chipset_MT, 8735, 1, {4,}, {0x410fd034,} },
#endif

#if kyArchIsARM
/* MediaTek MT6592: 4x Cortex-A7 + 4x Cortex-A7 */
{ 8, Chipset_MT, 6592, 2, {4, 4,}, {0x410fc074, 0x410fc074,} },
/* MediaTek MT6595: 4x Cortex-A17 + 4x Cortex-A7 */
{ 8, Chipset_MT, 6595, 2, {4, 4,}, {0x410fc075, 0x410fc0e0,} },
#endif
/* MediaTek MT6797: 2x Cortex-A72 + 4x Cortex-A53 + 4x Cortex-A53 */
{10, Chipset_MT, 6797, 3, {4, 4, 2}, {0x410fd034, 0x410fd034, 0x410fd081} },
/* MediaTek MT6799: 2x Cortex-A73 + 4x Cortex-A53 + 4x Cortex-A35 */
{10, Chipset_MT, 6799, 3, {4, 4, 2}, {0x410fd041, 0x410fd034, 0x410fd092} },
/* Rockchip RK3399: 2x Cortex-A72 + 4x Cortex-A53 */
{ 6, Chipset_RK, 3399, 2, {4, 2,}, {0x410fd034, 0x410fd082,} },
#if kyArchIsARM
/* Actions ATM8029: 4x Cortex-A5 */
{ 4, Chipset_ATM, 7029, 1, {4,}, {0x410fc051,} },
#endif
};


#endif
