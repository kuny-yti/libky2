#ifndef X86_CPU_ABILITY_H
#define X86_CPU_ABILITY_H

#include "ky_define.h"
#include "arch/ky_cpu.h"


#if kyArchIsX86
typedef enum
{
    EventNotCoreCycle       = 1 << 0, // "core cycle event not available"
    EventNotInstructRetired = 1 << 1, // "instruction retired event not available"
    EventNotReferCycle      = 1 << 2, // "reference cycles event not available"
    EventNotRefCache        = 1 << 3, // "last-level cache ref event not available"
    EventNotMissCache       = 1 << 4, // "last-level cache miss event not avail"
    EventNotRetiredInst     = 1 << 5, // "branch inst retired event not available"
    EventNotRetiredMispred  = 1 << 6, // "branch mispred retired event not avail"
    AnyThreadDeprecat       = 1 << 7  // anythread deprecation
}eCPUPerformanceEvents;
typedef enum
{
    APIC_Invalid = 0,
    APIC_Thread = 1,
    APIC_Core = 2,
    APIC_Module = 3,
    APIC_Tile = 4,
    APIC_Die = 5
}ex2APICLevelTypes;
typedef enum
{
    XCR0_x87              = 1 << 0, // XCR0 supported: x87 state
    XCR0_SSE              = 1 << 1, // XCR0 supported: SSE state
    XCR0_AVX              = 1 << 2, // XCR0 supported: AVX state
    XCR0_MPX_BNDREGS      = 1 << 3, // XCR0 supported: MPX BNDREGS
    XCR0_MPX_BNDCSR       = 1 << 4, // XCR0 supported: MPX BNDCSR
    XCR0_AVX512_Opmask    = 1 << 5, // XCR0 supported: AVX-512 opmask
    XCR0_AVX512_ZMM_Hi256 = 1 << 6, // XCR0 supported: AVX-512 ZMM_Hi256
    XCR0_AVX512_Hi16_ZMM  = 1 << 7, // XCR0 supported: AVX-512 Hi16_ZMM
    XSS_PT                = 1 << 8, // IA32_XSS supported: PT state
    XCR0_PKRU             = 1 << 9, // XCR0 supported: PKRU state
    __unk__               = 1 << 10, // XCR0 supported: PKRU state
    XCR0_CET_U            = 1 << 11, // XCR0 supported: CET_U state
    XCR0_CET_S            = 1 << 12, // XCR0 supported: CET_S state
    XSS_CET_HDC           = 1 << 13, // IA32_XSS supported: HDC state
    LWP                   = 1 << 14,  // AMD only (62)
    XSAVEOPT              = 1 << 15, // XSAVEOPT instruction
    XSAVEC                = 1 << 16, // XSAVEC instruction
    XGETBV                = 1 << 17, // XGETBV instruction
    XSAVES                = 1 << 18, // XSAVES/XRSTORS instructions
    XRSTORS = XSAVES,
}eXSAVEFlags;

typedef enum
{
    // RAS Capability (0x80000007/ebx)
    RAS_MCAOR = 1 << 0, // MCA overflow recovery support
    RAS_SUCCOR = 1 << 1, // SUCCOR support
    RAS_HWA  = 1 << 2, // HWA: hardware assert support
    RAS_SMCA  = 1 << 3, // scalable MCA support

    // Advanced Power Management Features (0x80000007/edx)
    APM_TS = 1 << 4, // TS: temperature sensing diode
    APM_FID = 1 << 5, // FID: frequency ID control
    APM_VID = 1 << 6,  // VID: voltage ID control
    APM_TTP = 1 << 7, // TTP: thermal trip
    APM_TM = 1 << 8,  // TM: thermal monitor
    APM_STC = 1 << 9, // STC: software thermal control
    APM_100MhzMulti = 1 << 10, // 100 MHz multiplier control
    APM_HPS = 1 << 11, // hardware P-State control
    APM_TSCI = 1 << 12, // TscInvariant
    APM_CPB = 1 << 13, // CPB: core performance boost
    APM_ROEFI = 1 << 14,  // read-only effective frequency interface
    APM_PFBI = 1 << 15, // processor feedback interface
    APM_PR = 1 << 16, // APM power reporting
    APM_CS = 1 << 17, // connected standby
    APM_RAPL = 1 << 18, // RAPL: running average power limit
}eCPUAdvanceds;
typedef enum // SVM Secure Virtual Machine
{
    SVM_NP = 1 << 0, // nested paging
    SVM_LBR = 1 << 1, // LBR virtualization
    SVM_Lock = 1 << 2, // SVM lock
    SVM_NRIP = 1 << 3, // NRIP save
    SVM_TSCrc = 1 << 4, // MSR based TSC rate control
    SVM_VMCB = 1 << 5, // VMCB clean bits support
    SVM_ASID = 1 << 6, // flush by ASID
    SVM_DA = 1 << 7, // decode assists
    SVM_SSSE5_OSD = 1 << 8, // SSSE3/SSE5 opcode set disable
    SVM_SSSE3_OSD = SVM_SSSE5_OSD,
    SVM_PIF = 1 << 9, // pause intercept filter
    SVM_PFT = 1 << 10, // pause filter threshold
    SVM_AVIC = 1 << 11, // AVIC: AMD virtual interrupt controller
    SVM_VMLOAD = 1 << 12, // virtualized VMLOAD/VMSAVE
    SVM_VMSAVE = SVM_VMLOAD,
    SVM_GIF = 1 << 13, // virtualized global interrupt flag (GIF)
    SVM_GMET = 1 << 14, // GMET: guest mode execute trap
    SVM_GSCS = 1 << 15, // guest Spec_ctl support

    SVM_SSE128 = 1 << 16, // 128-bit SSE executed full-width
    SVM_MOVU = 1 << 17, // MOVU* better than MOVL*/MOVH*
    SVM_SSE256 = 1 << 18 // 256-bit SSE executed full-width
}eSVMFlags;
typedef enum // Instruction Based Sampling Identifiers
{
    IBS_FFV = 1 << 0, // IBS feature flags valid
    IBS_FS = 1 << 1, // IBS fetch sampling
    IBS_ES = 1 << 2, // IBS execution sampling
    IBS_OCM = 1 << 3, // op counting mode
    IBS_BTAR = 1 << 4,  // branch target address reporting
    IBS_OCC = 1 << 5, // IbsOpCurCnt and IbsOpMaxCnt extend 7
    IBS_OMC = IBS_OCC,
    IBS_IRIPI = 1 << 6, // invalid RIP indication support
    IBS_FBMIS = 1 << 7, // fused branch micro-op indication support
    IBS_FCE = 1 << 8, // IBS fetch control extended MSR support
    IBS_OD = 1 << 9, // IBS op data 4 MSR support
    IBS_RWOC = 1 << 10 // read write of op counter
}eIBSFlags;

typedef enum // Lightweight Profiling Capabilities
{
    CPU_LWP = 1 << 0, // lightweight profiling
    CPU_LWPVAL = 1 << 1, // LWPVAL instruction
    CPU_IRE = 1 << 2, // instruction retired event
    CPU_BRE = 1 << 3, // branch retired event
    CPU_DCME = 1 << 4, // DC miss event
    CPU_CCNHE = 1 << 5, // core clocks not halted event
    CPU_CRCNHE = 1 << 6, // core reference clocks not halted event
    CPU_IOTO = 1 << 7, // interrupt on threshold overflow
    CPU_DCMAV = 1 << 8, // data cache miss address valid
    CPU_BPF = 1 << 9, // branch prediction filtering
    CPU_IPFilter = 1 << 10, // IP filtering
    CPU_CacheLevelFilter = 1 << 11, // cache level filtering
    CPU_CacheLatencyFilter = 1 << 12, // cache latency filteing
}eCPUProfilingFlags;

class cpu_ability : ky_singleton<cpu_ability>
{
    friend class ky_cpu;
    friend class ky_singleton<cpu_ability>;
private:
    cpu_ability();
    ~cpu_ability();

    //!
    //! \brief has_cpuid 是否具有 cpuid
    //! \return
    //!
    static bool has_cpuid();
    //!
    //! \brief cpuid 执行x86体系的cpuid
    //! \param ax
    //! \param p
    //! \param count
    //!
    static void cpuid(uint ax, uint *p, uint count = 0);

    //!
    //! \brief rdtsc 执行x86体系rdtsc
    //! \return
    //!
    static u64 rdtsc();
    //!
    //! \brief dbg_break
    //!
    static void dbg_break();

    //!
    //! \brief clock_frequency 通过rdtsc测量CPU时钟频率
    //! \return 准确的测量结果
    //!
    static u64 clock_frequency();

    void query_capable();

    struct cpu_version
    {
        eCPUTypes       type;
        uint            family;
        uint            model;
        uint            stepping;
        uint            family_ext;
        uint            model_ext;
        uint            family_synth;
        uint            model_synth;
    };
    struct cpu_synth_phys
    {
        char *const vendor;
        char *const name;
        char *const uarch;
        char *const family;
        char *const craft;
        bool        core_is_uarch;
        char *const model;
        char *const proc;
        uint        apic_id; // process local APIC physical ID
        uint        count;   // cpu count
        uint        clflush; // CLFLUSH line size
        uint        brand_id; // brand id
        char *const brand;
    };
    struct cpu_mwait
    {
        bool   support;   // enum of Monitor-MWAIT exts supported
        bool   break_event; // supports intrs as break-event for MWAIT
        uint16 smallest;    // smallest monitor-line size (bytes)
        uint16 largest;     // largest monitor-line size (bytes)
        uint8  state_c0;    // number of C0 sub C-states using MWAIT
        uint8  state_c1;
        uint8  state_c2;
        uint8  state_c3;
        uint8  state_c4;
        uint8  state_c5;
        uint8  state_c6;
        uint8  state_c7;
    };
    struct cpu_power
    {
        uint    feature;    // ePowerFeatures
        uint8   thresholds; // digital thermometer thresholds
        uint8   feedback;   // size of feedback struct (4KB pages)
        uint16  row_index; // index of CPU's row in feedback struct
    };
    struct cpu_performance
    {
        uint8  version_id; // version ID
        uint8  counters;   // number of counters per logical processor
        uint8  bit_width;  // bit width of counter
        uint8  bit_length; // length of EBX bit vector
        uint16 event  ;// eCPUPerformanceEvents
        uint8  counters_fixed; // number of fixed counters
        uint8  counters_fixed_bit_width; // bit width of fixed counters
        uint8  anythread_dep; // anythread deprecation
    };
    struct cpu_x2APIC
    {
        uint32 apic_id; // extended APIC ID
        struct
        {
            uint16            number;    // level number
            ex2APICLevelTypes type;      // level type ex2APICLevelTypes
            uint8             bit_width; // bit width of level
            uint16            logical_processors; // number of logical processors at level
        }level[8];
    };
    struct cpu_XSAVE
    {
        uint64 xcr0_mask;
        uint32 flags;             // eXSAVEFlags
        uint32 xcr_required_byte; // bytes required by fields in XCR
        uint32 xsave_required_byte;// bytes required by XSAVE/XRSTOR area
        uint32 area_size; // SAVE area size in bytes
        uint64 xss_mask;

        struct SupportedFeatures
        {
            uint32      size;   // save state byte size
            uint32      offset; // save state byte offset
            // supported in IA32_XSS or XCR0
            // 0 = XCR0 (user state)
            // 1 = IA32_XSS (supervisor state)
            uint8       is_xss;
            // 64-byte alignment in compacted XSAVE
            uint8       is_align64byte;
        } feature[64];
    };
    struct cpu_Qos // Quality of Service Monitoring Resource Type (0xf/0)
    {
        uint32 RMID; // Maximum range of RMID =
        bool   has_L3Qos; // supports L3 cache QoS monitoring
        struct // L3 Cache Quality of Service Monitoring (0xf/1)
        {
            uint32 factor;  // Conversion factor from IA32_QM_CTR to bytes =
            uint32 RMID;  // Maximum range of RMID =
            bool   has_Occupancy;  // supports L3 occupancy monitoring
            bool   has_TotalBandwidth; // supports L3 total bandwidth monitoring
            bool   has_LocalBandwidth; // supports L3 local bandwidth monitoring
        }L3Qos;
    };
    struct cpu_DirectorAllocate // Resource Director Technology Allocation (0x10/0)
    {
        bool has_L3Allocate; // L3 cache allocation technology supported
        bool has_L2Allocate; // L2 cache allocation technology supported
        bool has_BandwidthAllocate; // memory bandwidth allocation supported

        struct CacheAllocate
        {
            uint8 mask_length;       // length of capacity bit mask
            uint32 bit_granular;     // Bit-granular map of isolation/contention = 0x%08x
            bool cos_update;         // infrequent updates of COS
            bool has_prioritization; // code and data prioritization supported
            uint16 cos_number;    // highest COS number supported
        };
        CacheAllocate l3_cache; // L3 Cache Allocation Technology (0x10/1)
        CacheAllocate l2_cache; // L2 Cache Allocation Technology (0x10/2)

        struct BandwidthAllocate
        {
            uint16 max_throttling;  // maximum throttling value
            bool   has_LinearDelay; // delay values are linear
            uint16 cos_number;   // highest COS number supported
        };
        BandwidthAllocate bandwidth;// Memory Bandwidth Allocation (0x10/3)
    };
    struct cpu_SGX // Software Guard Extensions (SGX) capability (0x12/0)
    {
        bool has_SGX1; // SGX1 supported
        bool has_SGX2; // SGX2 supported
        bool has_SGX_ENCLV; // SGX ENCLV E*VIRTCHILD, ESETCONTEXT
        bool has_SGX_ENCLS; // SGX ENCLS ETRACKC, ERDINFO, ELDBC, ELDUC
        bool has_EXINFO; // MISCSELECT.EXINFO supported: #PF & #GP
        bool has_CPINFO; // MISCSELECT.CPINFO supported: #CP
        uint8 max_enclave64not; // MaxEnclaveSize_Not64 (log2)
        uint8 max_enclave64; // MaxEnclaveSize_64 (log2)

        struct Attribute // SGX attributes (0x12/1)
        {
            uint32 mask[4]; // ECREATE SECS.ATTRIBUTES valid bit mask = 0x%08x%08x%08x%08x
        } attribute;
        struct Enumeration //SGX EPC enumeration (0x12/n)
        {
            uint32 phy_address[2]; // section physical address = 0x%08x%08x
            uint32 size[2];
            uint8  props; // section property (0:enumerated as 0, 1:confidentiality & integrity protection)
        } enumeration[32];
    };
    struct IntelTrace  // Intel Processor Trace (0x14)
    {
        bool is_access_rtit_cr3; // IA32_RTIT_CR3_MATCH is accessible
        bool has_config_psb; // configurable PSB & cycle-accurate
        bool has_filter_ip; // IP & TraceStop filtering; PT preserve
        bool has_mtc_timing; // MTC timing packet; suppress COFI-based
        bool has_ptwrite; // PTWRITE support
        bool has_power_trace; // power event trace support
        bool has_ToPA_output; // ToPA output scheme support
        bool has_ToPA_entrie; // ToPA can hold many output entrie
        bool has_single_range; // single-range output scheme support
        bool ts_output_trace; // output to trace transport
        bool have_ip_lip_cs; // IP payloads have LIP values & CS

        uint8 address_range; // configurable address ranges
        uint16 mtc_bitmask; // supported MTC periods bitmask
        uint16 cycle_bitmask; // supported cycle threshold bitmask
        uint16 psb_bitmask; // supported config PSB freq bitmask
    };
    struct cpu_TSC // Time Stamp Counter/Core Crystal Clock Information (0x15)
    {
        // TSC/clock ratio = %u/%u
        uint32 timestamp ; // Time Stamp Counter
        uint32 clock_ratio; // clock ratio

        // nominal core crystal clock = %u Hz
        uint32 core_clock;
    };
    struct cpu_Frequency // Processor Frequency Information (0x16)
    {
        uint16 base; // Core Base Frequency (MHz)
        uint16 max;  // Core Maximum Frequency (MHz)
        uint16 bus;  // Bus (Reference) Frequency (MHz)
    };
    struct cpu_VendorAttribut // System-On-Chip Vendor Attribute (0x17/0)
    {
        uint16 id; // vendor id
        bool is_industry; // vendor scheme (0:assigned by intel, 1:industry standard)
        uint32 project; // project id  = 0x%08x
        uint32 stepping; // stepping id = 0x%08x
        char soc_brand[48]; // SoC brand   =
    };
    struct cpu_DATP // Deterministic Address Translation Parameters (0x18/%d)
    {
        bool has_page_4KB; // 4KB page size entries supported
        bool has_page_2MB; // 2MB page size entries supported
        bool has_page_4MB; // 4MB page size entries supported
        bool has_page_1GB; // 1GB page size entries supported
        uint8 partition;   // partitioning (0:soft between logical processors)
        uint16 way_asso;   // ways of associativity
        uint32 sets;       // number of sets = 0x%08x

        // translation cache type
        // 0:invalid
        // 1:data TLB
        // 2:instruction TLB
        // 3:unified TLB
        uint8 translation_type;
        uint8 translation_level; // translation cache level
        bool  has_fullyassoc; // fully associative
        uint16 max_ids; // maximum number of addressible IDs

        cpu_DATP *next;

        ~cpu_DATP()
        {
            kyDelete(next);
        }
    };
    struct cpu_hybrid // Hybrid Information (0x1a/0)
    {
        uint32 model_id; // native model ID of core
        uint8  type; // core type (0x20:Intel Atom, 0x40:Intel Core)
    };
    struct cpu_PCONFIG// PCONFIG information (0x1b/n)
    {
        uint16 subleaf_type; // sub-leaf type (0:invalid, 1:target identifier)
        uint32 identifier[3]; // identifier of target %d = 0x%08x
    };
    struct cpu_BrandExt // extended brand id (0x80000001/ebx)
    {
        uint16 id;       // BrandId
        uint8  index;    // BrandTableIndex
        uint8  nn;       // NN or [str1,str2]
        uint8  pwrlmt;   // PwrLmt [PartialModel,PG]
        const char *pkt; // PkgType
    };
    struct cpu_Advanced
    {
        uint   flags; // eCPUAdvanceds
        // Advanced Power Management Features (0x80000007/ecx)
        uint32 CUPSTR; // CmpUnitPwrSampleTimeRatio
    };
    struct cpu_PhyAddr
    {
        // Physical Address and Linear Address Size (0x80000008/eax)
        uint8 max_phy_bit; // maximum physical address bits
        uint8 max_linear_bit; // maximum linear (virtual) address bits
        uint8 max_guest_bit; // maximum guest physical address bits

        // Size Identifiers (0x80000008/ecx)
        union
        {
            uint8 cores; // number of CPU cores
            uint8 threads; // number of threads
        };
        uint8 acid_size; // ApicIdCoreIdSize
        uint8 ptsc_size; // performance time-stamp counter size
        // Feature Extended Size (0x80000008/edx)
        uint8 RDPRU_maxin; // RDPRU instruction max input support
    };
    struct cpu_SVM // SVM Secure Virtual Machine
    {
        uint8 revision; // SvmRev: SVM revision
        uint32 flags; // eSVMFlags
        uint32 NASID; //  NASID: number of address space identifiers = 0x%x
    };
    struct cpu_Profiling
    {
        uint16 flags;
        uint8 LWPCB_byte; // LWPCB byte size
        uint8 evt_record_byte; // event record byte size
        uint8 max_evtid; // maximum EventId
        uint8 evt_offset; // EventInterval1 field offset

        uint8 latency_cnt_bit; // latency counter bit size
        uint8 latency_rounded; // amount cache latency is rounded
        uint8 LWP_version; // LWP implementation version
        uint8 evt_record_buffer; // event ring buffer size in records
    };
    struct cpu_APICext
    {
        uint32 apic_id; // extended APIC ID = %u
        uint8  node_id; // node ID
        uint8  node_pp; // nodes per processor

        union
        {
            uint8 core_id; // core ID
            uint8 unit_id; // compute unit ID
        };
        union
        {
            uint8 threads; // threads per core
            uint8 units;   // cores per compute unit
        };
    };
    struct cpu_AMDSecureEncrypt
    {
        // AMD Secure Encryption (0x8000001f)
        bool has_SME; // SME: secure memory encryption support
        bool has_SEV; // SEV: secure encrypted virtualize support
        bool has_VMPF; // VM page flush MSR support
        bool has_SEV_ES; // SEV-ES: SEV encrypted state support
        uint8 PTE_enc; // encryption bit position in PTE
        uint8 phy_ASWR; // physical address space width reduction
        uint32 SEV_guest; // number of SEV-enabled guests supported = 0x%0x
        uint32 SEV_ASID_min; // minimum SEV guest ASID = 0x%0x
    };

    eCPUVendors       vendor;
    cpu_version       version;
    cpu_synth_phys    synth_phys;
    cpu_power         power;
    cpu_mwait         mwait;
    cpu_performance   performance;
    cpu_x2APIC        x2apic;
    cpu_XSAVE         xsave;
    cpu_Qos           qos;
    cpu_DirectorAllocate allocate;
    cpu_SGX           sgx;
    IntelTrace        intel_trace;
    cpu_TSC           tsc;
    cpu_Frequency     frequency;
    cpu_VendorAttribut vendor_att;
    cpu_DATP          datp;
    cpu_hybrid        hybrid;
    cpu_PCONFIG       pconfig;
    cpu_x2APIC        x2apic_v2;
    cpu_BrandExt      brand_ext;
    cpu_Advanced      advanced;
    cpu_PhyAddr       phy_addr;
    cpu_SVM           svm;
    uint16            ibs_flags;
    cpu_Profiling     profiling;
    cpu_APICext       apic_ext;
    cpu_AMDSecureEncrypt amd_secure;
    eCPUFeatures      feature;
    ky_cpu::cache_t  *cache;
    uint16            sn[6];
    uint              PLATFORM_DCA_CAP;

    uint   val_0_eax;
    uint   val_1_eax;
    uint   val_1_ebx;
    uint   val_1_ecx;
    uint   val_1_edx;
    uint   val_4_eax;
    uint   val_b_eax[2];
    uint   val_b_ebx[2];
    uint   val_1f_eax[6];
    uint   val_1f_ebx[6];
    uint   val_1f_ecx[6];
    uint   val_80000001_eax;
    uint   val_80000001_ebx;
    uint   val_80000001_ecx;
    uint   val_80000001_edx;
    uint   val_80000008_ecx;
    uint   val_8000001e_ebx;
    uint   transmeta_proc_rev;
    char   brand[48+1];
    char   transmeta_info[64+1];
    char   override_brand[48*2+1];
    char   soc_brand[48+1];

    bool   saw_4;
    bool   saw_b;
    bool   saw_1f;
};

#endif

#endif
