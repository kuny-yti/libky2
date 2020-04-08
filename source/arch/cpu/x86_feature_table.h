
#ifndef X86_FEATURE_TABLE_H
#define X86_FEATURE_TABLE_H

#define FOUR_CHARS(s) \
   ((uint)((s)[0] + ((s)[1] << 8) + ((s)[2] << 16) + ((s)[3] << 24)))
#define CMP_VENDOR_ID(words, s)                        \
   (   (words)[1] == FOUR_CHARS(&(s)[0]) \
    && (words)[3] == FOUR_CHARS(&(s)[4]) \
    && (words)[2] == FOUR_CHARS(&(s)[8]))
#define CMP_HYPERVISOR_ID(words, s)                    \
   (   (words)[1] == FOUR_CHARS(&(s)[0]) \
    && (words)[2] == FOUR_CHARS(&(s)[4]) \
    && (words)[3] == FOUR_CHARS(&(s)[8]))

#define _IT_F(xf, f, ...) \
    {0x0ff00f00, (((xf)<<20) + ((f)<<8)), 0, __VA_ARGS__}
#define _IT_FM(xf, f, xm, m, ...) \
    {0x0fff0ff0, (((xf)<<20) + ((xm)<<16) + ((f)<<8) + ((m)<<4)), 0, __VA_ARGS__}
#define _IT_FMS(xf, f, xm, m, s, ...) \
    {0x0fff0fff, (((xf)<<20) + ((xm)<<16) + ((f)<<8) + ((m)<<4) + (s)), 0, __VA_ARGS__}
#define _IT_TF(t, xf, f, ...) \
    {0x0ff03f00, (((t)<<12) + ((xf)<<20) + ((f)<< 8)), 0, __VA_ARGS__}
#define _IT_TFM(t, xf, f, xm, m, ...) \
    {0x0fff3ff0, (((t)<<12) + ((xf)<<20) + ((xm)<<16) + ((f)<<8) + ((m)<<4)), 0, __VA_ARGS__}
#define _IT_TFMS(t, xf, f, xm, m, s, ...) \
    {0x0fff3fff, (((t)<<12) + ((xf)<<20) + ((xm)<<16) + ((f)<<8) + ((m)<<4) + (s)), 0,__VA_ARGS__}
#define _IT_FQ(xf, f, func, ...) \
    {0x0ff00f00, (((xf)<<20) + ((f)<<8)), func, __VA_ARGS__}
#define _IT_FMQ(xf, f, xm, m, func, ...) \
    {0x0fff0ff0,(((xf)<<20) + ((xm)<<16) + ((f)<<8) + ((m)<<4)), func, __VA_ARGS__}
#define _IT_FMSQ(xf, f, xm, m, s, func, ...) \
    {0x0fff0fff, (((xf)<<20) + ((xm)<<16) + ((f)<<8) + ((m)<<4) + (s)), func, __VA_ARGS__}

static const struct
{
    eCPUVendors       code;
    const char *const str;
}
x86_vendor[] =
{
{CPU_Unknown,   "Unknown"},
{CPU_Intel,     "GenuineIntel"},
{CPU_AMD,       "AuthenticAMD"},
{CPU_Cyrix,     "CyrixInstead"},
{CPU_VIA,       "CentaurHauls"},
{CPU_UMC,       "UMC UMC UMC "},
{CPU_NexGen,    "NexGenDriven"},
{CPU_Rise,      "RiseRiseRise"},
{CPU_Transmeta, "GenuineTMx86"},
{CPU_SIS,       "SiS SiS SiS "},
{CPU_Vortex,    "Vortex86 SoC"},
{CPU_RDC,       "Genuine  RDC"},
{CPU_Hygon,     "HygonGenuine"},
{CPU_Zhaoxin,   "  Shanghai  "}
};

static const struct
{
    eCPUTypes         code;
    const char *const str;
}
x86_type[] =
{
{CPU_Primary, "Primary Processor"},
{CPU_IntelOverDrive, "Intel OverDrive"},
{CPU_Secondary, "Secondary Processor"}
};
static const struct
{
    eCPUVendors       code;
    const char *const str;
}
x86_vendor_brief[] =
{
{CPU_Unknown,  "Unknown"},
{CPU_Intel,     "Intel"},
{CPU_AMD,       "AMD"},
{CPU_Cyrix,     "Cyrix"},
{CPU_VIA,       "VIA"}, // "Zhaoxin"
{CPU_Transmeta, "Transmeta"},
{CPU_UMC,       "UMC"},
{CPU_NexGen,    "NexGen"},
{CPU_Rise,      "Rise"},
{CPU_SIS,       "SiS"},
{CPU_NSC,       "NSC"},
{CPU_Vortex,    "Vortex"},
{CPU_RDC,       "RDC"},
{CPU_Hygon,     "Hygon"},
{CPU_Zhaoxin,   "Zhaoxin"}
};

static const struct
{
    const char *const  str;
    const eCPUFeatureOnes code;
    const unsigned int bit;
}
x86_feature_edx[] =
{
{ "x87 FPU on chip"                         , CPU_FPU,  0},
{ "VME: virtual-8086 mode enhancement"      , CPU_VME,  1},
{ "DE: debugging extensions"                , CPU_DE,   2},
{ "PSE: page size extensions"               , CPU_PSE,  3},
{ "TSC: time stamp counter"                 , CPU_TSC,  4},
{ "RDMSR and WRMSR support"                 , CPU_RDMSR,5},
{ "PAE: physical address extensions"        , CPU_PAE,  6},
{ "MCE: machine check exception"            , CPU_MCE,  7},
{ "CMPXCHG8B inst."                         , CPU_CMPXCHG8B,8},
{ "APIC on chip"                            , CPU_APIC,  9},
{ "SYSENTER and SYSEXIT"                    , CPU_SYSENTER, 11},
{ "MTRR: memory type range registers"       , CPU_MTRR, 12},
{ "PTE global bit"                          , CPU_PTE, 13},
{ "MCA: machine check architecture"         , CPU_MCA, 14},
{ "CMOV: conditional move/compare instr"    , CPU_CMOV, 15},
{ "PAT: page attribute table"               , CPU_PAT, 16},
{ "PSE-36: page size extension"             , CPU_PSE36, 17},
{ "PSN: processor serial number"            , CPU_PSN, 18},
{ "CLFLUSH instruction"                     , CPU_CLFLUSH, 19},
{ "DS: debug store"                         , CPU_DS, 21},
{ "ACPI: thermal monitor and clock ctrl"    , CPU_ACPI, 22},
{ "MMX Technology"                          , CPU_MMX, 23},
{ "FXSAVE/FXRSTOR"                          , CPU_FXSAVE, 24},
{ "SSE extensions"                          , CPU_SSE, 25},
{ "SSE2 extensions"                         , CPU_SSE2, 26},
{ "SS: self snoop"                          , CPU_SS, 27},
{ "hyper-threading / multi-core supported"  , CPU_HT, 28},
{ "TM: therm. monitor"                      , CPU_TM, 29},
{ "IA64"                                    , CPU_IA64, 30},
{ "PBE: pending break event"                , CPU_PBE, 31},
};


static const struct
{
    const char *const  str;
    const eCPUFeatureOnes code;
    const unsigned int bit;
}
x86_amd_feature_edx[] =
{
{ "x87 FPU on chip"                         , CPU_FPU,  0},
{ "VME: virtual-8086 mode enhancement"      , CPU_VME,  1},
{ "DE: debugging extensions"                , CPU_DE,   2},
{ "PSE: page size extensions"               , CPU_PSE,  3},
{ "TSC: time stamp counter"                 , CPU_TSC,  4},
{ "RDMSR and WRMSR support"                 , CPU_RDMSR,5},
{ "PAE: physical address extensions"        , CPU_PAE,  6},
{ "MCE: machine check exception"            , CPU_MCE,  7},
{ "CMPXCHG8B inst."                         , CPU_CMPXCHG8B,8},
{ "APIC on chip"                            , CPU_APIC,  9},
{ "SYSCALL and SYSRET instructions"         , CPU_SYSCALL, 11},
{ "MTRR: memory type range registers"       , CPU_MTRR, 12},
{ "global paging extension"                 , CPU_GPE, 13},
{ "MCA: machine check architecture"         , CPU_MCA, 14},
{ "CMOV: conditional move/compare instr"    , CPU_CMOV, 15},
{ "PAT: page attribute table"               , CPU_PAT, 16},
{ "PSE-36: page size extension"             , CPU_PSE36, 17},
{ "multiprocessing capable"                 , CPU_MPC, 19},
{ "no-execute page protection"              , CPU_NEPP, 20},
{ "AMD multimedia instruction extensions"   , CPU_MMIE, 22},
{ "MMX Technology"                          , CPU_MMX, 23},
{ "FXSAVE/FXRSTOR"                          , CPU_FXSAVE, 24},
{ "SSE extensions"                          , CPU_SSE, 25},
{ "1-GB large page support"                 , CPU_1GBLPS, 26},
{ "RDTSCP"                                  , CPU_RDTSCP, 27},
{ "long mode (AA-64)"                       , CPU_AA64, 29},
{ "3DNow! instruction extensions"           , CPU_3DNowE, 30},
{ "3DNow! instructions"                     , CPU_3DNow, 31},
};
static const struct
{
    const char *const  str;
    const eCPUFeatureOnes code;
    const unsigned int bit;
}
x86_cyrix_feature_edx[] =
{
{ "x87 FPU on chip"                         , CPU_FPU,  0},
{ "virtual-8086 mode enhancement"           , CPU_VME,  1},
{ "debugging extensions"                    , CPU_DE,  2},
{ "page size extensions"                    , CPU_PSE,  3},
{ "time stamp counter"                      , CPU_TSC,  4},
{ "RDMSR and WRMSR support"                 , CPU_RDMSR,  5},
{ "physical address extensions"             , CPU_PAE,  6},
{ "machine check exception"                 , CPU_MCE,  7},
{ "CMPXCHG8B inst."                         , CPU_CMPXCHG8B,  8},
{ "APIC on chip"                            , CPU_APIC,  9},
{ "SYSCALL and SYSRET instructions"         , CPU_SYSCALL, 11},
{ "memory type range registers"             , CPU_MTRR, 12},
{ "global paging extension"                 , CPU_GPE, 13},
{ "machine check architecture"              , CPU_MCA, 14},
{ "conditional move/compare instruction"    , CPU_CMOV, 15},
{ "page attribute table"                    , CPU_PAT, 16},
{ "page size extension"                     , CPU_PSE, 17},
{ "multiprocessing capable"                 , CPU_MPC, 19},
{ "AMD multimedia instruction extensions"   , CPU_MMIE, 22},
{ "MMX Technology"                          , CPU_MMX, 23},
{ "extended MMX"                            , CPU_xMMX, 24},
{ "SSE extensions"                          , CPU_SSE, 25},
{ "AA-64"                                   , CPU_AA64, 29},
{ "3DNow! instruction extensions"           , CPU_3DNowE, 30},
{ "3DNow! instructions"                     , CPU_3DNow, 31},
};
static const struct
{
    const char *const  str;
    const eCPUFeatureOnes code;
    const unsigned int bit;
}
x86_transmeta_feature_edx[] =
{
    { "x87 FPU on chip"                         , CPU_FPU,  0},
    { "virtual-8086 mode enhancement"           , CPU_VME,  1},
    { "debugging extensions"                    , CPU_DE,  2},
    { "page size extensions"                    , CPU_PSE,  3},
    { "time stamp counter"                      , CPU_TSC,  4},
    { "RDMSR and WRMSR support"                 , CPU_RDMSR,  5},
    { "CMPXCHG8B inst."                         , CPU_CMPXCHG8B,  8},
    { "APIC on chip"                            , CPU_APIC,  9},
    { "memory type range registers"             , CPU_MTRR, 12},
    { "global paging extension"                 , CPU_GPE, 13},
    { "machine check architecture"              , CPU_MCA, 14},
    { "conditional move/compare instruction"    , CPU_CMOV, 15},
    { "FPU conditional move instructions"       , CPU_FPCMOV, 16},
    { "page size extension"                     , CPU_PSE, 17},
    { "AMD multimedia instruction extensions"   , CPU_MMIE, 22},
    { "MMX Technology"                          , CPU_MMX, 23},
    { "FXSAVE/FXRSTOR"                          , CPU_FXSAVE, 24},
};
static const struct
{
    const char *const  str;
    const eCPUFeatureOnes code;
    const unsigned int bit;
}
x86_nsc_feature_edx[] =
{
{ "x87 FPU on chip"                         , CPU_FPU,  0},
{ "virtual-8086 mode enhancement"           , CPU_VME,  1},
{ "debugging extensions"                    , CPU_DE,  2},
{ "page size extensions"                    , CPU_PSE,  3},
{ "time stamp counter"                      , CPU_TSC,  4},
{ "RDMSR and WRMSR support"                 , CPU_RDMSR,  5},
{ "machine check exception"                 , CPU_MCE,  7},
{ "CMPXCHG8B inst."                         , CPU_CMPXCHG8B,  8},
{ "SYSCALL and SYSRET instructions"         , CPU_SYSCALL, 11},
{ "global paging extension"                 , CPU_GPE, 13},
{ "conditional move/compare instruction"    , CPU_CMOV, 15},
{ "FPU conditional move instruction"        , CPU_FPUCMOV, 16},
{ "MMX Technology"                          , CPU_MMX, 23},
{ "6x86MX multimedia extensions"            , CPU_xMMX, 24}
};
static const struct
{
    const char *const  str;
    const eCPUFeatureTwos code;
    const unsigned int bit;
}
x86_feature_ecx[] =
{
{ "PNI/SSE3: Prescott New Instructions"     , CPU_PNI,  0},
{ "PCLMULDQ instruction"                    , CPU_PCLMULDQ,  1},
{ "DTES64: 64-bit debug store"              , CPU_DTES64,  2},
{ "MONITOR/MWAIT"                           , CPU_MONITOR,  3},
{ "CPL-qualified debug store"               , CPU_QDS,  4},
{ "VMX: virtual machine extensions"         , CPU_VMX,  5},
{ "SMX: safer mode extensions"              , CPU_SMX,  6},
{ "Enhanced Intel SpeedStep Technology"     , CPU_EIST,  7},
{ "TM2: thermal monitor 2"                  , CPU_TM2,  8},
{ "SSSE3 extensions"                        , CPU_SSSE3,  9},
{ "context ID: adaptive or shared L1 data"  , CPU_CTXID, 10},
{ "SDBG: IA32_DEBUG_INTERFACE"              , CPU_SDBG, 11},
{ "FMA instruction"                         , CPU_FMA, 12},
{ "CMPXCHG16B instruction"                  , CPU_CMPXCHG16B, 13},
{ "xTPR disable"                            , CPU_xTPR, 14},
{ "PDCM: perfmon and debug"                 , CPU_PDCM, 15},
{ "PCID: process context identifiers"       , CPU_PCID, 17},
{ "DCA: direct cache access"                , CPU_DCA, 18},
{ "SSE4.1 extensions"                       , CPU_SSE41, 19},
{ "SSE4.2 extensions"                       , CPU_SSE42, 20},
{ "x2APIC: extended xAPIC support"          , CPU_x2APIC, 21},
{ "MOVBE instruction"                       , CPU_MOVBE, 22},
{ "POPCNT instruction"                      , CPU_POPCNT, 23},
{ "time stamp counter deadline"             , CPU_TSCD, 24},
{ "AES instruction"                         , CPU_AES, 25},
{ "XSAVE/XSTOR states"                      , CPU_XSAVE, 26},
{ "OS-enabled XSAVE/XSTOR"                  , CPU_OSE, 27},
{ "AVX: advanced vector extensions"         , CPU_AVX, 28},
{ "F16C half-precision convert instruction" , CPU_F16C, 29},
{ "RDRAND instruction"                      , CPU_RDRAND, 30},
{ "hypervisor guest status"                 , CPU_HGS, 31},
};
static const struct
{
    const char *const  str;
    const eCPUFeatureTrees code;
    const unsigned int bit;
}
x86_feature_ext_ebx[] =
{
{ "FSGSBASE instructions"                   , CPU_FSGSBASE,  0 },
{ "IA32_TSC_ADJUST MSR supported"           , CPU_TSCADJUST,  1 },
{ "SGX: Software Guard Extensions supported", CPU_SGX,  2 },
{ "BMI1 instructions"                       , CPU_BMI1,  3 },
{ "HLE hardware lock elision"               , CPU_HLE,  4 },
{ "AVX2: advanced vector extensions 2"      , CPU_AVX2,  5 },
{ "FDP_EXCPTN_ONLY"                         , CPU_FDP_EXCPTN,  6 },
{ "SMEP supervisor mode exec protection"    , CPU_SMEP,  7 },
{ "BMI2 instructions"                       , CPU_BMI2,  8 },
{ "enhanced REP MOVSB/STOSB"                , CPU_MOVSB,  9 },
{ "INVPCID instruction"                     , CPU_INVPCID, 10 },
{ "RTM: restricted transactional memory"    , CPU_RTM, 11 },
{ "RDT-CMT/PQoS cache monitoring"           , CPU_RDT_CMT, 12 },
{ "deprecated FPU CS/DS"                    , CPU_FPU_CS, 13 },
{ "MPX: intel memory protection extensions" , CPU_MPX, 14 },
{ "RDT-CAT/PQE cache allocation"            , CPU_RDT_CAT, 15 },
{ "AVX512F: AVX-512 foundation instructions", CPU_AVX512F, 16 },
{ "AVX512DQ: double & quadword instructions", CPU_AVX512DQ, 17 },
{ "RDSEED instruction"                      , CPU_RDSEED, 18 },
{ "ADX instructions"                        , CPU_ADX, 19 },
{ "SMAP: supervisor mode access prevention" , CPU_SMAP, 20 },
{ "AVX512IFMA: fused multiply add"          , CPU_AVX512IFMA, 21 },
{ "PCOMMIT instruction"                     , CPU_PCOMMIT, 22 },
{ "CLFLUSHOPT instruction"                  , CPU_CLFLUSHOPT, 23 },
{ "CLWB instruction"                        , CPU_CLWB, 24 },
{ "Intel processor trace"                   , CPU_IPT, 25 },
{ "AVX512PF: prefetch instructions"         , CPU_AVX512PF, 26 },
{ "AVX512ER: exponent & reciprocal instrs"  , CPU_AVX512ER, 27 },
{ "AVX512CD: conflict detection instrs"     , CPU_AVX512CD, 28 },
{ "SHA instructions"                        , CPU_SHA, 29 },
{ "AVX512BW: byte & word instructions"      , CPU_AVX512BW, 30 },
{ "AVX512VL: vector length"                 , CPU_AVX512VL, 31 }
};

static const struct
{
    const char *const  str;
    const eCPUFeatureFours code;
    const unsigned int bit;
}
x86_feature_ext_ecx[] =
{
{ "PREFETCHWT1"                             , CPU_PREFETCHWT1,  0},
{ "AVX512VBMI: vector byte manipulation"    , CPU_AVX512VBMI,  1},
{ "UMIP: user-mode instruction prevention"  , CPU_UMIP,  2},
{ "PKU protection keys for user-mode"       , CPU_PKU,  3},
{ "OSPKE CR4.PKE and RDPKRU/WRPKRU"         , CPU_OSPKE,  4},
{ "WAITPKG instructions"                    , CPU_WAITPKG,  5},
{ "AVX512_VBMI2: byte VPCOMPRESS, VPEXPAND" , CPU_AVX512_VBMI2,  6},
{ "CET_SS: CET shadow stack"                , CPU_CET_SS,  7},
{ "GFNI: Galois Field New Instructions"     , CPU_GFNI,  8},
{ "VAES instructions"                       , CPU_VAES,  9},
{ "VPCLMULQDQ instruction"                  , CPU_VPCLMULQDQ, 10},
{ "AVX512_VNNI: neural network instructions", CPU_AVX512_VNNI, 11},
{ "AVX512_BITALG: bit count/shiffle"        , CPU_AVX512_BITALG, 12},
{ "TME: Total Memory Encryption"            , CPU_TME, 13},
{ "AVX512: VPOPCNTDQ instruction"           , CPU_AVX512, 14},
{ "5-level paging"                          , CPU_PAGING5L, 16},
{ "RDPID: read processor D supported"       , CPU_RDPID, 22},
{ "CLDEMOTE supports cache line demote"     , CPU_CLDEMOTE, 25},
{ "MOVDIRI instruction"                     , CPU_MOVDIRI, 27},
{ "MOVDIR64B instruction"                   , CPU_MOVDIR64B, 28},
{ "ENQCMD instruction"                      , CPU_ENQCMD, 29},
{ "SGX_LC: SGX launch config supported"     , CPU_ESGX_LC, 30}
};

static const struct
{
    const char *const  str;
    const eCPUFeatureFives code;
    const unsigned int bit;
}
x86_feature_ext_edx[] =
{
{ "AVX512_4VNNIW: neural network instrs"    , CPU_AVX512_4VNNIW,  2},
{ "AVX512_4FMAPS: multiply acc single prec" , CPU_AVX512_4FMAPS,  3},
{ "fast short REP MOV"                      , CPU_FASTMOV,  4},
{ "AVX512_VP2INTERSECT: intersect mask regs", CPU_AVX512_VP2INTERSECT,  8},
{ "VERW md-clear microcode support"         , CPU_VERW, 10}, // Xen*/Qemu*
{ "hybrid part"                             , CPU_HP, 15},
{ "PCONFIG instruction"                     , CPU_PCONFIG, 18},
{ "CET_IBT: CET indirect branch tracking"   , CPU_CET_IBT, 20},
{ "IBRS/IBPB: indirect branch restrictions" , CPU_IBRS, 26},
{ "STIBP: 1 thr indirect branch predictor"  , CPU_STIBP, 27},
{ "L1D_FLUSH: IA32_FLUSH_CMD MSR"           , CPU_L1D_FLUSH, 28},
{ "IA32_ARCH_CAPABILITIES MSR"              , CPU_ARCHCAP, 29},
{ "IA32_CORE_CAPABILITIES MSR"              , CPU_CORECAP, 30},
{ "SSBD: speculative store bypass disable"  , CPU_SSBD, 31}
};

static const struct
{
    const char *const  str;
    const eCPUFeatureFives code;
    const unsigned int bit;
}
x86_feature_ext1_eax[] =
{
{ "AVX512_BF16: bfloat16 instructions"      ,  CPU_AVX512_BF16,  5}
};


static const struct
{
    const char *const  str;
    const eCPUFeatureAMDs code;
    const unsigned int bit;
}
x86_feature_amd_ecx[] =
{
{ "LAHF/SAHF supported in 64-bit mode"      , CPU_LAHF64,  0},
{ "CMP Legacy"                              , CPU_CMPL,  1},
{ "SVM: secure virtual machine"             , CPU_SVM,  2},
{ "extended APIC space"                     , CPU_xAPICs,  3},
{ "AltMovCr8"                               , CPU_AltMovCr8,  4},
{ "LZCNT advanced bit manipulation"         , CPU_LZCNT,  5},
{ "SSE4A support"                           , CPU_SSE4a,  6},
{ "misaligned SSE mode"                     , CPU_mSSE,  7},
{ "3DNow! PREFETCH/PREFETCHW instructions"  , CPU_3DNowPREFETCH,  8},
{ "OS visible workaround"                   , CPU_OSVW,  9},
{ "instruction based sampling"              , CPU_IBS, 10},
{ "XOP support"                             , CPU_XOP, 11},
{ "SKINIT/STGI support"                     , CPU_SKINIT, 12},
{ "watchdog timer support"                  , CPU_WTS, 13},
{ "lightweight profiling support"           , CPU_LPS, 15},
{ "4-operand FMA instruction"               , CPU_FMA4, 16},
{ "TCE: translation cache extension"        , CPU_TCE, 17},
{ "NodeId MSR C001100C"                     , CPU_NID, 19},
{ "TBM support"                             , CPU_TBM, 21},
{ "topology extensions"                     , CPU_TPYX, 22},
{ "core performance counter extensions"     , CPU_CPCE, 23},
{ "NB/DF performance counter extensions"    , CPU_NBPCE, 24},
{ "data breakpoint extension"               , CPU_DBE, 26},
{ "performance time-stamp counter support"  , CPU_PTSC, 27},
{ "LLC performance counter extensions"      , CPU_LLCPCE, 28},
{ "MWAITX/MONITORX supported"               , CPU_MWAITX, 29},
{ "Address mask extension support"          , CPU_AME, 30}
};
static const struct
{
    const char *const str;
    const eCPUExtIDs code;
    const uint       bit;
}
x86_extid_feature_table[] =
{
{ "CLZERO instruction"                      , CPU_CLZERO,  0 },
{ "instructions retired count support"      , CPU_IRCS,  1 },
{ "always save/restore error pointers"      , CPU_ASEP,  2 },
{ "RDPRU instruction"                       , CPU_RDPRU,  4 },
{ "memory bandwidth enforcement"            , CPU_MBE,  6 },
{ "WBNOINVD instruction"                    , CPU_WBNOINVD,  9 },
{ "IBPB: indirect branch prediction barrier", CPU_IBPB, 12 },
{ "IBRS: indirect branch restr speculation" , CPU_IBRS, 14 },
{ "STIBP: 1 thr indirect branch predictor"  , CPU_STIBP, 15 },
{ "STIBP always on preferred mode"          , CPU_STIBP_AOPM, 17 },
{ "ppin processor id number supported"      , CPU_PPIN, 23 },
{ "SSBD: speculative store bypass disable"  , CPU_SSBD, 24 },
{ "virtualized SSBD"                        , CPU_vSSBD, 25 },
{ "SSBD fixed in hardware"                  , CPU_SSBD_FIHW, 26 }
};
static const struct
{
    const char *const str;
    const eSVMFlags code;
    const uint      bit;
}
x86_svm_table[] =
{
{ "nested paging"                           , SVM_NP,  0 },
{ "LBR virtualization"                      , SVM_LBR,  1 },
{ "SVM lock"                                , SVM_Lock,  2 },
{ "NRIP save"                               , SVM_NRIP,  3 },
{ "MSR based TSC rate control"              , SVM_TSCrc,  4 },
{ "VMCB clean bits support"                 , SVM_VMCB,  5 },
{ "flush by ASID"                           , SVM_ASID,  6 },
{ "decode assists"                          , SVM_DA,  7 },
{ "SSSE3/SSE5 opcode set disable"           , SVM_SSSE3_OSD,  9 },
{ "pause intercept filter"                  , SVM_PIF, 10 },
{ "pause filter threshold"                  , SVM_PFT, 12 },
{ "AVIC: AMD virtual interrupt controller"  , SVM_AVIC, 13 },
{ "virtualized VMLOAD/VMSAVE"               , SVM_VMLOAD, 15 },
{ "virtualized global interrupt flag (GIF)" , SVM_GIF, 16 },
{ "GMET: guest mode execute trap"           , SVM_GMET, 17 },
{ "guest Spec_ctl support"                  , SVM_GSCS, 20 }
};
#endif
