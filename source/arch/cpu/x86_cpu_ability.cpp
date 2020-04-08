
#include "x86_cpu_ability.h"
#include "ky_timer.h"
#include "ky_bitset.h"

#if kyArchIsX86
#  if kyArchIs64Bit
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

#  elif kyArchIs32Bit

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
#  else
typedef int x86_reg;
#  endif

#  define xgetbv(index, eax, edx)                                 \
    kyInlineASM (".byte 0x0f, 0x01, 0xd0" : "=a"(eax), "=d"(edx) : "c" (index))


cpu_ability::cpu_ability()
{

}
cpu_ability::~cpu_ability()
{

}

bool cpu_ability::has_cpuid()
{
#if kyArchIs64Bit
    return true;
#else

    long a, c;
    kyInlineASM
            (
                "pushf\n\t"
                "pop %0\n\t"
                "mov %0, %1\n\t"

                "xor $0x200000, %0\n\t"
                "push %0\n\t"
                "popf\n\t"

                "pushf\n\t"
                "pop %0\n\t"
                : "=a" (a), "=c" (c)
                :
                : "cc"
            );

    if (a == c)
        return false;
    return true;
#endif
}
void cpu_ability::cpuid(uint ax, uint *p, uint count)
{
    kyInlineASM
            ("mov %%" REG_b ", %%" REG_S "\n\t"
            "cpuid\n\t"
            "xchg %%" REG_b ", %%" REG_S
             : "=a" (p[0]), "=S" (p[1]),
            "=c" (p[2]), "=d" (p[3])
        : "0" (ax), "2"(count));
}

#  if !kyCompilerIsMSVC
u64 cpu_ability::rdtsc()
{
    volatile u32 lo, hi;
    kyInlineASM ("rdtsc" : "=a" (lo), "=d" (hi));
    return u64(hi << 32) | u64(lo);
}
#  else
uint64 cpu_ability::rdtsc() { return __rdtsc(); }
#  endif

void cpu_ability::dbg_break()
{
#if kyCompilerIsMSVC
    __debugbreak();
#elif kyCompilerIsGNUC || kyCompilerIsCLANG
    kyInlineASM ("int $3");
#endif
}

// 设置计划优先级并在超出范围时恢复
class ScopedSetPriority
{
public:
    ScopedSetPriority(int newPriority)
    {
        // 获取当前的调度策略和优先级
        ::pthread_getschedparam(pthread_self(), &m_oldPolicy, &m_oldParam);

        // 新的优先级
        sched_param newParam = {0};
        newParam.sched_priority = newPriority;
        pthread_setschedparam(pthread_self(), SCHED_FIFO, &newParam);
    }

    ~ScopedSetPriority()
    {
        pthread_setschedparam(pthread_self(), m_oldPolicy, &m_oldParam);
    }

private:
    int m_oldPolicy;
    sched_param m_oldParam;
};

u64 cpu_ability::clock_frequency()
{
    if(!(feature & CPU_TSC))
        return -1.0;

    const int priority = ::sched_get_priority_max(SCHED_FIFO)-1;
    ScopedSetPriority ssp(priority);kyUnused2(ssp);

    f64 samples[10];
    for(size_t i = 0; i < kyArraySizeOf(samples); ++i)
    {
        f64 dt;
        i64 dc;

        const f64 t0 = (f64)ky_timer::nanosec() * 1e-9;
        u64 c1; f64 t1;
        do
        {
            t1 = (f64)ky_timer::nanosec() * 1e-9;
            c1 = rdtsc();
        }
        while(t1 == t0);
        do
        {
            const f64 t2 = (f64)ky_timer::nanosec()* 1e-9;
            const i64 c2 = rdtsc();
            dc = (i64)(c2 - c1);
            dt = t2 - t1;
        }
        while(dt < 1e-3);
        samples[i] = dc / dt;// freq
    }

    f64 sum = 0.0;
    for(uint i = 0; i < kyArraySizeOf(samples); i++)
        sum += samples[i];
    return (u64)(sum / kyArraySizeOf(samples)); // clockFrequency
}

#include "x86_cache_table.h"
#include "x86_feature_table.h"
#include "x86_power_table.h"


// applies to L2, L3 and TLB2
static const struct
{
    const char *const str;
    uint8             associate;
}
assoc_table[] =
{
{"L2 off",        0},
{"direct mapped", 1},
{"2-way",         2},
{"3-way",         3},
{"4-way",         4},
{"6-way",         6},
{"8-way",         8},
{0,               0},
{"16-way",        16},
{0,               0},
{"32-way",        32},
{"48-way",        48},
{"64-way",        64},
{"96-way",        96},
{"128-way",       128},
{"full",         0xff}
};
namespace Intel
{
static bool intel_is_claim_dG(const struct cpu_ability *ability)
{
    return !ability->br.mobile && ability->br.generic;
}
static bool intel_is_claim_dP(const struct cpu_ability *ability)
{
    return ability->br.pentium || ability->bri.desktop_pentium;
}
static bool intel_is_claim_dC(const struct cpu_ability *ability)
{
    return (!ability->br.mobile && ability->br.celeron) ||
            ability->bri.desktop_celeron;
}
static bool intel_is_claim_da(const struct cpu_ability *ability)
{
    return ability->br.atom;
}
static bool intel_is_claim_dd(const struct cpu_ability *ability)
{
    return ability->br.pentium_d;
}

#define TODD_WAS_HERE
#ifdef TODD_WAS_HERE
static bool intel_is_claim_dc(const struct cpu_ability *ability)
{
    return !ability->br.mobile && ability->br.core;
}
#else
static bool intel_is_claim_dc(const struct cpu_ability *ability)
{
    return !ability->br.mobile && (ability->br.core || ability->br.generic);
}
#endif
static bool intel_is_claim_sX(const struct cpu_ability *ability)
{
    return ability->br.xeon || ability->bri.xeon;
}
static bool intel_is_claim_sM(const struct cpu_ability *ability)
{
    return ability->br.xeon_mp || ability->bri.xeon_mp;
}
static bool intel_is_claim_sS(const struct cpu_ability *ability)
{
    return ability->br.xeon && ability->br.scalable;
}
static bool intel_is_claim_MP(const struct cpu_ability *ability)
{
    return (ability->br.mobile && ability->br.pentium) ||
            ability->bri.mobile_pentium;
}
static bool intel_is_claim_MC(const struct cpu_ability *ability)
{
    return (ability->br.mobile && ability->br.celeron) ||
            ability->bri.mobile_celeron;
}
static bool intel_is_claim_MM(const struct cpu_ability *ability)
{
    return ability->br.pentium_m || ability->bri.mobile_pentium_m;
}
static bool intel_is_claim_Mc(const struct cpu_ability *ability)
{
    return ability->br.mobile && ability->br.core;
}
static bool intel_is_claim_Xc(const struct cpu_ability *ability)
{
    return ability->br.extreme;
}
static bool intel_is_claim_UC(const struct cpu_ability *ability)
{
    return ability->br.u_line;
}
static bool intel_is_claim_LY(const struct cpu_ability *ability)
{
    return ability->br.y_line;
}
static bool intel_is_claim_LG(const struct cpu_ability *ability)
{
    return ability->br.g_line;
}
static bool intel_is_claim_UC(const struct cpu_ability *ability)
{
    return intel_is_claim_dC(ability) && ability->br.u_line;
}
static bool intel_is_claim_UP(const struct cpu_ability *ability)
{
    return intel_is_claim_dP(ability) && ability->br.u_line;
}
static bool intel_is_claim_YC(const struct cpu_ability *ability)
{
    return intel_is_claim_dC(ability) && ability->br.y_line;
}
static bool intel_is_claim_YP(const struct cpu_ability *ability)
{
    return intel_is_claim_dP(ability) && ability->br.y_line;
}

/*
** Intel special cases
*/
/* Pentium II Xeon (Deschutes), distinguished from Pentium II (Deschutes) */
static bool intel_is_claim_xD(const struct cpu_ability *ability)
{
    return ability->L2_4w_1Mor2M;
}
/* Mobile Pentium II (Deschutes), distinguished from Pentium II (Deschutes) */
static bool intel_is_claim_mD(const struct cpu_ability *ability)
{
    return ability->L2_4w_256K;
}
/* Intel Celeron (Deschutes), distinguished from  Pentium II (Deschutes) */
static bool intel_is_claim_cD(const struct cpu_ability *ability)
{
    return !ability->L2_4w_1Mor2M && !ability->L2_4w_512K && !ability->L2_4w_256K ;
}
/* Pentium III Xeon (Katmai), distinguished from Pentium III (Katmai) */
static bool intel_is_claim_xK(const struct cpu_ability *ability)
{
    return ability->L2_4w_1Mor2M || ability->L2_8w_1Mor2M;
}
/* Pentium II (Katmai), verified, so distinguished from fallback case */
static bool intel_is_claim_pK(const struct cpu_ability *ability)
{
    return (ability->L2_4w_512K || ability->L2_8w_256K || ability->L2_8w_512K) &&
            !ability->L2_4w_1Mor2M && !ability->L2_8w_1Mor2M;
}
/* Irwindale, distinguished from Nocona */
static bool intel_is_claim_sI(const struct cpu_ability *ability)
{
    return intel_is_claim_sX(ability) && ability->L2_2M;
}
/* Potomac, distinguished from Cranford */
static bool intel_is_claim_sP(const struct cpu_ability *ability)
{
    return intel_is_claim_sM(ability) && ability->L3;
}
/* Allendale, distinguished from Conroe */
static bool intel_is_claim_dL(const struct cpu_ability *ability)
{
    return intel_is_claim_dc(ability) && ability->L2_2M;
}
/* Dual-Core Xeon Processor 5100 (Woodcrest B1) pre-production,
   distinguished from Core 2 Duo (Conroe B1) */
static bool intel_is_claim_QW(const struct cpu_ability *ability)
{
    return intel_is_claim_dG(ability) && ability->br.generic  &&
            (ability->mp.cores == 4 || (ability->mp.cores == 2 &&
                                        ability->mp.hyperthreads == 2));
}
/* Core Duo (Yonah), distinguished from Core Solo (Yonah) */
static bool intel_is_claim_DG(const struct cpu_ability *ability)
{
    return intel_is_claim_dG(ability) && ability->mp.cores == 2;
}
/* Core 2 Quad, distinguished from Core 2 Duo */
static bool intel_is_claim_Qc(const struct cpu_ability *ability)
{
    return intel_is_claim_dc(ability) && ability->mp.cores == 4;
}
/* Core 2 Extreme (Conroe B1), distinguished from Core 2 Duo (Conroe B1) */
static bool intel_is_claim_XE(const struct cpu_ability *ability)
{
    return intel_is_claim_dc(ability) && strstr(ability->brand, " E6800") != 0;
}
/* Quad-Core Xeon, distinguished from Xeon; and
   Xeon Processor 3300, distinguished from Xeon Processor 3100 */
static bool intel_is_claim_sQ(const struct cpu_ability *ability)
{
    return intel_is_claim_sX(ability) && ability->mp.cores == 4;
}
/* Xeon Processor 7000, distinguished from Xeon */
#define IS_VMX(val_1_ecx)  (BIT_EXTRACT_LE((val_1_ecx), 5, 6))
static bool intel_is_claim_s7(const struct cpu_ability *ability)
{
    ky_bitset<32, uint> bit_ecx(ability->val_1_ecx);
    return intel_is_claim_sX(ability) && IS_VMX(ability->val_1_ecx);
}
/* Wolfdale C0/E0, distinguished from Wolfdale M0/R0 */
static bool intel_is_claim_de(const struct cpu_ability *ability)
{
    return intel_is_claim_dc(ability) && ability->L2_6M;
}
/* Penryn C0/E0, distinguished from Penryn M0/R0 */
static bool intel_is_claim_Me(const struct cpu_ability *ability)
{
    return intel_is_claim_Mc(ability) && ability->L2_6M;
}
/* Yorkfield C1/E0, distinguished from Yorkfield M1/E0 */
static bool intel_is_claim_Qe(const struct cpu_ability *ability)
{
    return intel_is_claim_Qc(ability) && ability->L2_6M;
}
/* Yorkfield E0, distinguished from Yorkfield R0 */
static bool intel_is_claim_se(const struct cpu_ability *ability)
{
    return intel_is_claim_sQ(ability) && ability->L2_6M;
}
/* Amber Lake-Y, distinguished from Kaby Lake-Y */
static bool intel_is_claim_Y8(const struct cpu_ability *ability)
{
    return intel_is_claim_LY(ability) && ability->br.i_8000;
}
/* Comet Lake V1, distinguished from Whiskey Lake V0 */
static bool intel_is_claim_UX(const struct cpu_ability *ability)
{
    return intel_is_claim_LU(ability) && ability->br.i_10000;
}

#include "x86_intel_table.h"
}
namespace AMD
{
static bool amd_is_claim_dA(const struct cpu_ability *ability)
{
    return !ability->br.mobile && ability->br.athlon;
}
static bool amd_is_claim_dX(const struct cpu_ability *ability)
{
    return !ability->br.mobile && ability->br.athlon_xp;
}
static bool amd_is_claim_dF(const struct cpu_ability *ability)
{
    return !ability->br.mobile && ability->br.athlon_fx;
}
static bool amd_is_claim_df(const struct cpu_ability *ability)
{
    return !ability->br.mobile && ability->br.fx;
}
static bool amd_is_claim_dD(const struct cpu_ability *ability)
{
    return !ability->br.mobile && ability->br.duron;
}
static bool amd_is_claim_dS(const struct cpu_ability *ability)
{
    return !ability->br.mobile && ability->br.sempron;
}
static bool amd_is_claim_dp(const struct cpu_ability *ability)
{
    return !ability->br.mobile && ability->br.phenom;
}
static bool amd_is_claim_dI(const struct cpu_ability *ability)
{
    return !ability->br.mobile && ability->br.firepro;
}
static bool amd_is_claim_dR(const struct cpu_ability *ability)
{
    return !ability->br.mobile && ability->br.ryzen;
}
static bool amd_is_claim_sO(const struct cpu_ability *ability)
{
    return !ability->br.mobile && ability->br.opteron;
}
static bool amd_is_claim_sA(const struct cpu_ability *ability)
{
    return !ability->br.mobile && ability->br.athlon_mp;
}
static bool amd_is_claim_sD(const struct cpu_ability *ability)
{
    return !ability->br.mobile && ability->br.duron_mp;
}
static bool amd_is_claim_sE(const struct cpu_ability *ability)
{
    return !ability->br.mobile && ability->br.epyc;
}
static bool amd_is_claim_MA(const struct cpu_ability *ability)
{
    return ability->br.mobile && ability->br.athlon;
}
static bool amd_is_claim_MX(const struct cpu_ability *ability)
{
    return ability->br.mobile && ability->br.athlon_xp;
}
static bool amd_is_claim_ML(const struct cpu_ability *ability)
{
    return ability->br.mobile && ability->br.athlon_lv;
}
static bool amd_is_claim_MD(const struct cpu_ability *ability)
{
    return ability->br.mobile && ability->br.duron;
}
static bool amd_is_claim_MS(const struct cpu_ability *ability)
{
    return ability->br.mobile && ability->br.sempron;
}
static bool amd_is_claim_Mp(const struct cpu_ability *ability)
{
    return ability->br.mobile && ability->br.phenom;
}
static bool amd_is_claim_Ms(const struct cpu_ability *ability)
{
    return ability->br.mobile && ability->br.series;
}
static bool amd_is_claim_Mr(const struct cpu_ability *ability)
{
    return ability->br.mobile && ability->br.r_series;
}
static bool amd_is_claim_MG(const struct cpu_ability *ability)
{
    return ability->br.geode;
}
static bool amd_is_claim_MT(const struct cpu_ability *ability)
{
    return ability->br.turion;
}
static bool amd_is_claim_MU(const struct cpu_ability *ability)
{
    return ability->br.ultra;
}
static bool amd_is_claim_Mn(const struct cpu_ability *ability)
{
    return ability->br.turion && ability->br.neo;
}
static bool amd_is_claim_MN(const struct cpu_ability *ability)
{
    return ability->br.neo;
}
static bool amd_is_claim_Sa(const struct cpu_ability *ability)
{
    return !ability->br.mobile && ability->br.a_series;
}
static bool amd_is_claim_Sc(const struct cpu_ability *ability)
{
    return !ability->br.mobile && ability->br.athlon;
}
static bool amd_is_claim_Se(const struct cpu_ability *ability)
{
    return !ability->br.mobile && ability->br.e_series;
}
static bool amd_is_claim_Sg(const struct cpu_ability *ability)
{
    return !ability->br.mobile && ability->br.g_series;
}
static bool amd_is_claim_Sr(const struct cpu_ability *ability)
{
    return !ability->br.mobile && ability->br.r_series;
}
static bool amd_is_claim_Sz(const struct cpu_ability *ability)
{
    return !ability->br.mobile && ability->br.z_series;
}
static bool amd_is_claim_Ta(const struct cpu_ability *ability)
{
    return ability->br.t_suffix && ability->br.a_series;
}
static bool amd_is_claim_Te(const struct cpu_ability *ability)
{
    return ability->br.t_suffix && ability->br.e_series;
}
static bool amd_is_claim_AR(const struct cpu_ability *ability)
{
    return ability->br.ryzen;
}
static bool amd_is_claim_ER(const struct cpu_ability *ability)
{
    return ability->br.ryzen && ability->br.embedded;
}
static bool amd_is_claim_EE(const struct cpu_ability *ability)
{
    return ability->br.epyc_3000;
}

/*
** AMD special cases
*/
static bool is_amd_egypt_athens_8xx(const cpu_ability* ability)
{
   if (ability->br.opteron)
   {
      switch (ability->val_1_eax & 0x0fff0ff0)
      {
      case (((2)<<16) + ((15)<<8) + ((1)<<4)): /* Italy/Egypt */
      case (((2)<<16) + ((15)<<8) + ((5)<<4)): /* Troy/Athens */
         {
            uint  bti;
            ky_bitset<8> bit1_ebx(ability->val_1_ebx & 0xff);
            ky_bitset<32> bit81_ebx(ability->val_80000001_ebx);
            if (bit1_ebx.any())
               bti = bit1_ebx.extract(5, 8) << 2;
            else if (bit81_ebx.extract(0, 12) != 0)
               bti = bit81_ebx.extract(6, 12);
            else
               return false;

            switch (bti)
            {
            case 0x10:
            case 0x11:
            case 0x12:
            case 0x13:
            case 0x2a:
            case 0x30:
            case 0x31:
            case 0x39:
            case 0x3c:
            case 0x32:
            case 0x33:
               /* It's a 2xx */
               return false;
            case 0x14:
            case 0x15:
            case 0x16:
            case 0x17:
            case 0x2b:
            case 0x34:
            case 0x35:
            case 0x3a:
            case 0x3d:
            case 0x36:
            case 0x37:
               /* It's an 8xx */
               return true;
            }
         }
      }
   }

   return false;
}

/* Embedded Opteron, distinguished from Opteron (Barcelona & Shanghai) */
static bool amd_is_claim_EO(const struct cpu_ability *ability)
{
    return amd_is_claim_sO(ability)  && ability->br.embedded;
}
/* Opterons, distinguished by number of processors */
static bool amd_is_claim_DO(const struct cpu_ability *ability)
{
    return amd_is_claim_sO(ability)  && ability->br.cores == 2;
}
static bool amd_is_claim_SO(const struct cpu_ability *ability)
{
    return amd_is_claim_sO(ability) && ability->br.cores == 6;
}
/* Athlons, distinguished by number of processors */
static bool amd_is_claim_DA(const struct cpu_ability *ability)
{
    return amd_is_claim_dA(ability) && ability->br.cores == 2;
}
static bool amd_is_claim_TA(const struct cpu_ability *ability)
{
    return amd_is_claim_dA(ability) && ability->br.cores == 3;
}
static bool amd_is_claim_QA(const struct cpu_ability *ability)
{
    return amd_is_claim_dA(ability) && ability->br.cores == 4;
}
/* Phenoms distinguished by number of processors */
static bool amd_is_claim_Dp(const struct cpu_ability *ability)
{
    return amd_is_claim_dp(ability) && ability->br.cores == 2;
}
static bool amd_is_claim_Tp(const struct cpu_ability *ability)
{
    return amd_is_claim_dp(ability) && ability->br.cores == 3;
}
static bool amd_is_claim_Qp(const struct cpu_ability *ability)
{
    return amd_is_claim_dp(ability) && ability->br.cores == 4;
}
static bool amd_is_claim_Sp(const struct cpu_ability *ability)
{
    return amd_is_claim_dp(ability) && ability->br.cores == 6;
}
/* Semprons, distinguished by number of processors */
static bool amd_is_claim_DS(const struct cpu_ability *ability)
{
    return amd_is_claim_dS(ability) && ability->br.cores == 2;
}
/* Egypt, distinguished from Italy; and
   Athens, distingushed from Troy */
static bool amd_is_claim_s8(const struct cpu_ability *ability)
{
    return amd_is_claim_sO(ability) && is_amd_egypt_athens_8xx(ability);
}
/* Thorton A2, distinguished from Barton A2 */
static bool amd_is_claim_dt(const struct cpu_ability *ability)
{
    return amd_is_claim_dX(ability) && ability->L2_256K;
}
/* Manchester E6, distinguished from from Toledo E6 */
static bool amd_is_claim_dm(const struct cpu_ability *ability)
{
    return amd_is_claim_dA(ability) && ability->L2_512K;
}
/* Propus, distinguished from Regor */
static bool amd_is_claim_dr(const struct cpu_ability *ability)
{
    return amd_is_claim_dA(ability) && ability->L2_512K;
}
/* Trinidad, distinguished from Taylor */
static bool amd_is_claim_Mt(const struct cpu_ability *ability)
{
    return amd_is_claim_MT(ability) && ability->L2_512K;
}

static void decode_amd_model(const cpu_ability*  ability,
                 const char**         brand_pre,
                 const char**         brand_post,
                 char*                proc)
{
   *brand_pre  = 0;
   *brand_post = 0;
   *proc       = '\0';

   if (ability == 0) return;

   if ((ability->val_1_eax & 0x0ff00f00) == (15 << 8) &&
       (ability->val_1_eax & 0x000f00f0) < (4 << 16))
   {
      /*
      ** Algorithm from:
      **    Revision Guide for AMD Athlon 64 and AMD Opteron Processors
      **    (25759 Rev 3.79), Constructing the Processor Name String.
      ** But using only the Processor numbers.
      */
      unsigned int  bti;
      unsigned int  NN;
      ky_bitset<8> bits_ebx(ability->val_1_ebx & 0xff);
      ky_bitset<32> bits81_ebx(ability->val_80000001_ebx);
      if (bits_ebx.any())
      {
         bti = bits_ebx.extract(5, 8) << 2;
         NN  = bits_ebx.extract(0, 5);
      }
      else if (bits81_ebx.extract(0, 12) != 0)
      {
         bti = bits81_ebx.extract(6, 12);
         NN  = bits81_ebx.extract(0,  6);
      }
      else
         return;

#define XX  (22 + NN)
#define YY  (38 + 2*NN)
#define ZZ  (24 + NN)
#define TT  (24 + NN)
#define RR  (45 + 5*NN)
#define EE  ( 9 + NN)

      switch (bti)
      {
      case 0x04:
         *brand_pre = "AMD Athlon(tm) 64";
         sprintf(proc, "Processor %02d00+", XX);
         break;
      case 0x05:
         *brand_pre = "AMD Athlon(tm) 64 X2 Dual Core";
         sprintf(proc, "Processor %02d00+", XX);
         break;
      case 0x06:
         *brand_pre  = "AMD Athlon(tm) 64";
         sprintf(proc, "FX-%02d", ZZ);
         *brand_post = "Dual Core";
         break;
      case 0x08:
         *brand_pre = "AMD Athlon(tm) 64";
         sprintf(proc, "Processor %02d00+", XX);
         break;
      case 0x09:
         *brand_pre = "AMD Athlon(tm) 64";
         sprintf(proc, "Processor %02d00+", XX);
         break;
      case 0x0a:
         *brand_pre = "AMD Turion(tm) Mobile Technology";
         sprintf(proc, "ML-%02d", XX);
         break;
      case 0x0b:
         *brand_pre = "AMD Turion(tm) Mobile Technology";
         sprintf(proc, "MT-%02d", XX);
         break;
      case 0x0c:
      case 0x0d:
         *brand_pre = "AMD Opteron(tm)";
         sprintf(proc, "Processor 1%02d", YY);
         break;
      case 0x0e:
         *brand_pre = "AMD Opteron(tm)";
         sprintf(proc, "Processor 1%02d HE", YY);
         break;
      case 0x0f:
         *brand_pre = "AMD Opteron(tm)";
         sprintf(proc, "Processor 1%02d EE", YY);
         break;
      case 0x10:
      case 0x11:
         *brand_pre = "AMD Opteron(tm)";
         sprintf(proc, "Processor 2%02d", YY);
         break;
      case 0x12:
         *brand_pre = "AMD Opteron(tm)";
         sprintf(proc, "Processor 2%02d HE", YY);
         break;
      case 0x13:
         *brand_pre = "AMD Opteron(tm)";
         sprintf(proc, "Processor 2%02d EE", YY);
         break;
      case 0x14:
      case 0x15:
         *brand_pre = "AMD Opteron(tm)";
         sprintf(proc, "Processor 8%02d", YY);
         break;
      case 0x16:
         *brand_pre = "AMD Opteron(tm)";
         sprintf(proc, "Processor 8%02d HE", YY);
         break;
      case 0x17:
         *brand_pre = "AMD Opteron(tm)";
         sprintf(proc, "Processor 8%02d EE", YY);
         break;
      case 0x18:
         *brand_pre = "AMD Athlon(tm) 64";
         sprintf(proc, "Processor %02d00+", EE);
         break;
      case 0x1d:
         *brand_pre = "Mobile AMD Athlon(tm) XP-M";
         sprintf(proc, "Processor %02d00+", XX);
         break;
      case 0x1e:
         *brand_pre = "Mobile AMD Athlon(tm) XP-M";
         sprintf(proc, "Processor %02d00+", XX);
         break;
      case 0x20:
         *brand_pre = "AMD Athlon(tm) XP";
         sprintf(proc, "Processor %02d00+", XX);
         break;
      case 0x21:
      case 0x23:
         *brand_pre = "Mobile AMD Sempron(tm)";
         sprintf(proc, "Processor %02d00+", TT);
         break;
      case 0x22:
      case 0x26:
         *brand_pre = "AMD Sempron(tm)";
         sprintf(proc, "Processor %02d00+", TT);
         break;
      case 0x24:
         *brand_pre = "AMD Athlon(tm) 64";
         sprintf(proc, "FX-%02d", ZZ);
         break;
      case 0x29:
      case 0x2c:
      case 0x2d:
      case 0x38:
      case 0x3b:
         *brand_pre = "Dual Core AMD Opteron(tm)";
         sprintf(proc, "Processor 1%02d", RR);
         break;
      case 0x2a:
      case 0x30:
      case 0x31:
      case 0x39:
      case 0x3c:
         *brand_pre = "Dual Core AMD Opteron(tm)";
         sprintf(proc, "Processor 2%02d", RR);
         break;
      case 0x2b:
      case 0x34:
      case 0x35:
      case 0x3a:
      case 0x3d:
         *brand_pre = "Dual Core AMD Opteron(tm)";
         sprintf(proc, "Processor 8%02d", RR);
         break;
      case 0x2e:
         *brand_pre = "Dual Core AMD Opteron(tm)";
         sprintf(proc, "Processor 1%02d HE", RR);
         break;
      case 0x2f:
         *brand_pre = "Dual Core AMD Opteron(tm)";
         sprintf(proc, "Processor 1%02d EE", RR);
         break;
      case 0x32:
         *brand_pre = "Dual Core AMD Opteron(tm)";
         sprintf(proc, "Processor 2%02d HE", RR);
         break;
      case 0x33:
         *brand_pre = "Dual Core AMD Opteron(tm)";
         sprintf(proc, "Processor 2%02d EE", RR);
         break;
      case 0x36:
         *brand_pre = "Dual Core AMD Opteron(tm)";
         sprintf(proc, "Processor 8%02d HE", RR);
         break;
      case 0x37:
         *brand_pre = "Dual Core AMD Opteron(tm)";
         sprintf(proc, "Processor 8%02d EE", RR);
         break;
      }

#undef XX
#undef YY
#undef ZZ
#undef TT
#undef RR
#undef EE
   }

   else if ((ability->val_1_eax & 0x0ff00f00) == (15 << 8) &&
            (ability->val_1_eax & 0x000f00f0) >= (4 << 16))
   {
      /*
      ** Algorithm from:
      **    Revision Guide for AMD NPT Family 0Fh Processors (33610 Rev 3.46),
      **    Constructing the Processor Name String.
      ** But using only the Processor numbers.
      */
      unsigned int  bti;
      unsigned int  pwrlmt;
      unsigned int  NN;
      unsigned int  pkgtype;
      unsigned int  cmpcap;

      ky_bitset<32> bits81_eax(ability->val_80000001_eax);
      ky_bitset<32> bits81_ebx(ability->val_80000001_ebx);
      ky_bitset<32> bits88_ecx(ability->val_80000008_ecx);
      pwrlmt  = ((bits81_ebx.extract(6, 9) << 1) + bits81_ebx(14, 15));
      bti     = bits81_ebx.extract(9, 14);
      NN      = ((bits81_ebx.extract(15, 16) << 5) + bits81_ebx.extract(0, 5));
      pkgtype = bits81_eax.extract(4, 6);
      cmpcap  = ((bits88_ecx.extract(0, 8) > 0) ? 0x1 : 0x0);

#define RR  (NN - 1)
#define PP  (26 + NN)
#define TT  (15 + cmpcap*10 + NN)
#define ZZ  (57 + NN)
#define YY  (29 + NN)

#define PKGTYPE(pkgtype)  ((pkgtype) << 11)
#define CMPCAP(cmpcap)    ((cmpcap)  <<  9)
#define BTI(bti)          ((bti)     <<  4)
#define PWRLMT(pwrlmt)    (pwrlmt)

      switch (PKGTYPE(pkgtype) + CMPCAP(cmpcap) + BTI(bti) + PWRLMT(pwrlmt))
      {
      /* Table 7: Name String Table for F (1207) and Fr3 (1207) Processors */
      case PKGTYPE(1) + CMPCAP(0) + BTI(1) + PWRLMT(2):
         *brand_pre = "AMD Opteron(tm)";
         sprintf(proc, "Processor 22%02d EE", RR);
         break;
      case PKGTYPE(1) + CMPCAP(1) + BTI(1) + PWRLMT(2):
         *brand_pre = "Dual-Core AMD Opteron(tm)";
         sprintf(proc, "Processor 22%02d EE", RR);
         break;
      case PKGTYPE(1) + CMPCAP(1) + BTI(0) + PWRLMT(2) :
         *brand_pre = "Dual-Core AMD Opteron(tm)";
         sprintf(proc, "Processor 12%02d EE", RR);
         break;
      case PKGTYPE(1) + CMPCAP(1) + BTI(0) + PWRLMT(6):
         *brand_pre = "Dual-Core AMD Opteron(tm)";
         sprintf(proc, "Processor 12%02d HE", RR);
         break;
      case PKGTYPE(1) + CMPCAP(1) + BTI(1) + PWRLMT(6):
         *brand_pre = "Dual-Core AMD Opteron(tm)";
         sprintf(proc, "Processor 22%02d HE", RR);
         break;
      case PKGTYPE(1) + CMPCAP(1) + BTI(1) + PWRLMT(10):
         *brand_pre = "Dual-Core AMD Opteron(tm)";
         sprintf(proc, "Processor 22%02d", RR);
         break;
      case PKGTYPE(1) + CMPCAP(1) + BTI(1) + PWRLMT(12):
         *brand_pre = "Dual-Core AMD Opteron(tm)";
         sprintf(proc, "Processor 22%02d SE", RR);
         break;
      case PKGTYPE(1) + CMPCAP(1) + BTI(4) + PWRLMT(2):
         *brand_pre = "Dual-Core AMD Opteron(tm)";
         sprintf(proc, "Processor 82%02d EE", RR);
         break;
      case PKGTYPE(1) + CMPCAP(1) + BTI(4) + PWRLMT(6):
         *brand_pre = "Dual-Core AMD Opteron(tm)";
         sprintf(proc, "Processor 82%02d HE", RR);
         break;
      case PKGTYPE(1) + CMPCAP(1) + BTI(4) + PWRLMT(10):
         *brand_pre = "Dual-Core AMD Opteron(tm)";
         sprintf(proc, "Processor 82%02d", RR);
         break;
      case PKGTYPE(1) + CMPCAP(1) + BTI(4) + PWRLMT(12):
         *brand_pre = "Dual-Core AMD Opteron(tm)";
         sprintf(proc, "Processor 82%02d SE", RR);
         break;
      case PKGTYPE(1) + CMPCAP(1) + BTI(6) + PWRLMT(14):
         *brand_pre = "AMD Athlon(tm) 64";
         sprintf(proc, "FX-%02d", ZZ);
         break;
      /* Table 8: Name String Table for AM2 and ASB1 Processors */
      case PKGTYPE(3) + CMPCAP(0) + BTI(1) + PWRLMT(5):
         *brand_pre = "AMD Sempron(tm)";
         sprintf(proc, "Processor LE-1%02d0", RR);
         break;
      case PKGTYPE(3) + CMPCAP(0) + BTI(2) + PWRLMT(6):
         *brand_pre = "AMD Athlon(tm)";
         sprintf(proc, "Processor LE-1%02d0", ZZ);
         break;
      case PKGTYPE(3) + CMPCAP(0) + BTI(3) + PWRLMT(6):
         *brand_pre = "AMD Athlon(tm)";
         sprintf(proc, "Processor 1%02d0B", ZZ);
         break;
      case PKGTYPE(3) + CMPCAP(0) + BTI(4) + PWRLMT(1):
      case PKGTYPE(3) + CMPCAP(0) + BTI(4) + PWRLMT(2):
      case PKGTYPE(3) + CMPCAP(0) + BTI(4) + PWRLMT(3):
      case PKGTYPE(3) + CMPCAP(0) + BTI(4) + PWRLMT(4):
      case PKGTYPE(3) + CMPCAP(0) + BTI(4) + PWRLMT(5):
      case PKGTYPE(3) + CMPCAP(0) + BTI(4) + PWRLMT(8):
         *brand_pre = "AMD Athlon(tm) 64";
         sprintf(proc, "Processor %02d00+", TT);
         break;
      case PKGTYPE(3) + CMPCAP(0) + BTI(5) + PWRLMT(2):
         *brand_pre = "AMD Sempron(tm)";
         sprintf(proc, "Processor %02d50p", RR);
         break;
      case PKGTYPE(3) + CMPCAP(0) + BTI(6) + PWRLMT(4):
      case PKGTYPE(3) + CMPCAP(0) + BTI(6) + PWRLMT(8):
         *brand_pre = "AMD Sempron(tm)";
         sprintf(proc, "Processor %02d00+", TT);
         break;
      case PKGTYPE(3) + CMPCAP(0) + BTI(7) + PWRLMT(1):
      case PKGTYPE(3) + CMPCAP(0) + BTI(7) + PWRLMT(2):
         *brand_pre = "AMD Sempron(tm)";
         sprintf(proc, "Processor %02d0U", TT);
         break;
      case PKGTYPE(3) + CMPCAP(0) + BTI(8) + PWRLMT(2):
      case PKGTYPE(3) + CMPCAP(0) + BTI(8) + PWRLMT(3):
         *brand_pre = "AMD Athlon(tm)";
         sprintf(proc, "Processor %02d50e", TT);
         break;
      case PKGTYPE(3) + CMPCAP(0) + BTI(9) + PWRLMT(2):
         *brand_pre = "AMD Athlon(tm) Neo";
         sprintf(proc, "Processor MV-%02d", TT);
         break;
      case PKGTYPE(3) + CMPCAP(0) + BTI(12) + PWRLMT(2):
         *brand_pre = "AMD Sempron(tm)";
         sprintf(proc, "Processor 2%02dU", RR);
         break;
      case PKGTYPE(3) + CMPCAP(1) + BTI(1) + PWRLMT(6):
         *brand_pre = "Dual-Core AMD Opteron(tm)";
         sprintf(proc, "Processor 12%02d HE", RR);
         break;
      case PKGTYPE(3) + CMPCAP(1) + BTI(1) + PWRLMT(10):
         *brand_pre = "Dual-Core AMD Opteron(tm)";
         sprintf(proc, "Processor 12%02d", RR);
         break;
      case PKGTYPE(3) + CMPCAP(1) + BTI(1) + PWRLMT(12):
         *brand_pre = "Dual-Core AMD Opteron(tm)";
         sprintf(proc, "Processor 12%02d SE", RR);
         break;
      case PKGTYPE(3) + CMPCAP(1) + BTI(3) + PWRLMT(3):
         *brand_pre = "AMD Athlon(tm) X2 Dual Core";
         sprintf(proc, "Processor BE-2%02d0", TT);
         break;
      case PKGTYPE(3) + CMPCAP(1) + BTI(4) + PWRLMT(1):
      case PKGTYPE(3) + CMPCAP(1) + BTI(4) + PWRLMT(2):
      case PKGTYPE(3) + CMPCAP(1) + BTI(4) + PWRLMT(6):
      case PKGTYPE(3) + CMPCAP(1) + BTI(4) + PWRLMT(8):
      case PKGTYPE(3) + CMPCAP(1) + BTI(4) + PWRLMT(12):
         *brand_pre = "AMD Athlon(tm) 64 X2 Dual Core";
         sprintf(proc, "Processor %02d00+", TT);
         break;
      case PKGTYPE(3) + CMPCAP(1) + BTI(5) + PWRLMT(12):
         *brand_pre  = "AMD Athlon(tm) 64";
         sprintf(proc, "FX-%02d", ZZ);
         *brand_post = "Dual Core";
         break;
      case PKGTYPE(3) + CMPCAP(1) + BTI(6) + PWRLMT(6):
         *brand_pre = "AMD Sempron(tm) Dual Core";
         sprintf(proc, "Processor %02d00", RR);
         break;
      case PKGTYPE(3) + CMPCAP(1) + BTI(7) + PWRLMT(3):
         *brand_pre = "AMD Athlon(tm) Dual Core";
         sprintf(proc, "Processor %02d50e", TT);
         break;
      case PKGTYPE(3) + CMPCAP(1) + BTI(7) + PWRLMT(6):
      case PKGTYPE(3) + CMPCAP(1) + BTI(7) + PWRLMT(7):
         *brand_pre = "AMD Athlon(tm) Dual Core";
         sprintf(proc, "Processor %02d00B", TT);
         break;
      case PKGTYPE(3) + CMPCAP(1) + BTI(8) + PWRLMT(3):
         *brand_pre = "AMD Athlon(tm) Dual Core";
         sprintf(proc, "Processor %02d50B", TT);
         break;
      case PKGTYPE(3) + CMPCAP(1) + BTI(9) + PWRLMT(1):
         *brand_pre = "AMD Athlon(tm) X2 Dual Core";
         sprintf(proc, "Processor %02d50e", TT);
         break;
      case PKGTYPE(3) + CMPCAP(1) + BTI(10) + PWRLMT(1):
      case PKGTYPE(3) + CMPCAP(1) + BTI(10) + PWRLMT(2):
         *brand_pre = "AMD Athlon(tm) Neo X2 Dual Core";
         sprintf(proc, "Processor %02d50e", TT);
         break;
      case PKGTYPE(3) + CMPCAP(1) + BTI(11) + PWRLMT(0):
         *brand_pre = "AMD Turion(tm) Neo X2 Dual Core";
         sprintf(proc, "Processor L6%02d", RR);
         break;
      case PKGTYPE(3) + CMPCAP(1) + BTI(12) + PWRLMT(0):
         *brand_pre = "AMD Turion(tm) Neo X2 Dual Core";
         sprintf(proc, "Processor L3%02d", RR);
         break;
      /* Table 9: Name String Table for S1g1 Processors */
      case PKGTYPE(0) + CMPCAP(0) + BTI(1) + PWRLMT(2):
         *brand_pre = "AMD Athlon(tm) 64";
         sprintf(proc, "Processor %02d00+", TT);
         break;
      case PKGTYPE(0) + CMPCAP(0) + BTI(2) + PWRLMT(12):
         *brand_pre = "AMD Turion(tm) 64 Mobile Technology";
         sprintf(proc, "MK-%02d", YY);
         break;
      case PKGTYPE(0) + CMPCAP(0) + BTI(3) + PWRLMT(1):
         *brand_pre = "Mobile AMD Sempron(tm)";
         sprintf(proc, "Processor %02d00+", TT);
         break;
      case PKGTYPE(0) + CMPCAP(0) + BTI(3) + PWRLMT(6):
      case PKGTYPE(0) + CMPCAP(0) + BTI(3) + PWRLMT(12):
         *brand_pre = "Mobile AMD Sempron(tm)";
         sprintf(proc, "Processor %02d00+", PP);
         break;
      case PKGTYPE(0) + CMPCAP(0) + BTI(4) + PWRLMT(2):
         *brand_pre = "AMD Sempron(tm)";
         sprintf(proc, "Processor %02d00+", TT);
         break;
      case PKGTYPE(0) + CMPCAP(0) + BTI(6) + PWRLMT(4):
      case PKGTYPE(0) + CMPCAP(0) + BTI(6) + PWRLMT(6):
      case PKGTYPE(0) + CMPCAP(0) + BTI(6) + PWRLMT(12):
         *brand_pre = "AMD Athlon(tm)";
         sprintf(proc, "Processor TF-%02d", TT);
         break;
      case PKGTYPE(0) + CMPCAP(0) + BTI(7) + PWRLMT(3):
         *brand_pre = "AMD Athlon(tm)";
         sprintf(proc, "Processor L1%02d", RR);
         break;
      case PKGTYPE(0) + CMPCAP(1) + BTI(1) + PWRLMT(12):
         *brand_pre = "AMD Sempron(tm)";
         sprintf(proc, "Processor TJ-%02d", YY);
         break;
      case PKGTYPE(0) + CMPCAP(1) + BTI(2) + PWRLMT(12):
         *brand_pre = "AMD Turion(tm) 64 X2 Mobile Technology";
         sprintf(proc, "Processor TL-%02d", YY);
         break;
      case PKGTYPE(0) + CMPCAP(1) + BTI(3) + PWRLMT(4):
      case PKGTYPE(0) + CMPCAP(1) + BTI(3) + PWRLMT(12):
         *brand_pre = "AMD Turion(tm) 64 X2 Dual-Core";
         sprintf(proc, "Processor TK-%02d", YY);
         break;
      case PKGTYPE(0) + CMPCAP(1) + BTI(5) + PWRLMT(4):
         *brand_pre = "AMD Turion(tm) 64 X2 Dual Core";
         sprintf(proc, "Processor %02d00+", TT);
         break;
      case PKGTYPE(0) + CMPCAP(1) + BTI(6) + PWRLMT(2):
         *brand_pre = "AMD Turion(tm) X2 Dual Core";
         sprintf(proc, "Processor L3%02d", RR);
         break;
      case PKGTYPE(0) + CMPCAP(1) + BTI(7) + PWRLMT(4):
         *brand_pre = "AMD Turion(tm) X2 Dual Core";
         sprintf(proc, "Processor L5%02d", RR);
         break;
      }

#undef RR
#undef PP
#undef TT
#undef ZZ
#undef YY
   }
   else if ((ability->val_1_eax & 0x0ff00f00) == (1 << 20) + (15 << 8) ||
            (ability->val_1_eax & 0x0ff00f00) == (2 << 20) + (15 << 8) ||
            (ability->val_1_eax & 0x0ff00f00) == (3 << 20) + (15 << 8) ||
            (ability->val_1_eax & 0x0ff00f00) == (5 << 20) + (15 << 8))
   {
      /*
      ** Algorithm from:
      **    AMD Revision Guide for AMD Family 10h Processors (41322 Rev 3.74)
      **    AMD Revision Guide for AMD Family 11h Processors (41788 Rev 3.08)
      **    AMD Revision Guide for AMD Family 12h Processors (44739 Rev 3.10)
      **    AMD Revision Guide for AMD Family 14h Models 00h-0Fh Processors
      **    (47534 Rev 3.00)
      ** But using only the Processor numbers.
      */
      unsigned int  str1;
      unsigned int  str2;
      unsigned int  pg;
      unsigned int  partialmodel;
      unsigned int  pkgtype;
      unsigned int  nc;
      const char*   s1;
      const char*   s2;

      ky_bitset<32> bits81_ebx(ability->val_80000001_ebx);
      ky_bitset<32> bits88_ecx(ability->val_80000008_ecx);
      str2         = bits81_ebx.extract( 0,  4);
      partialmodel = bits81_ebx.extract( 4, 11);
      str1         = bits81_ebx.extract(11, 15);
      pg           = bits81_ebx.extract(15, 16);
      pkgtype      = bits81_ebx.extract(28, 32);
      nc           = bits88_ecx.extract( 0,  8);

#define NC(nc)            ((nc)   << 9)
#define PG(pg)            ((pg)   << 8)
#define STR1(str1)        ((str1) << 4)
#define STR2(str2)        (str2)

      /*
      ** In every String2 Values table, there were special cases for
      ** pg == 0 && str2 == 15 which defined them as the empty string.
      ** But that produces the same result as an undefined string, so
      ** don't bother trying to handle them.
      */
      if ((ability->val_1_eax & 0x0ff00f00) == (1 << 20) + (15 << 8))
      {
         if (pkgtype >= 2)
            partialmodel--;

         /* Family 10h tables */
         switch (pkgtype)
         {
         case 0:
            /* 41322 3.74: table 14: String1 Values for Fr2, Fr5, and Fr6 (1207) Processors */
            switch (PG(pg) + NC(nc) + STR1(str1))
            {
            case PG(0) + NC(3) + STR1(0): *brand_pre = "Quad-Core AMD Opteron(tm)"; s1 = "Processor 83"; break;
            case PG(0) + NC(3) + STR1(1): *brand_pre = "Quad-Core AMD Opteron(tm)"; s1 = "Processor 23"; break;
            case PG(0) + NC(5) + STR1(0): *brand_pre = "Six-Core AMD Opteron(tm)";  s1 = "Processor 84"; break;
            case PG(0) + NC(5) + STR1(1): *brand_pre = "Six-Core AMD Opteron(tm)";  s1 = "Processor 24"; break;
            case PG(1) + NC(3) + STR1(1): *brand_pre = "Embedded AMD Opteron(tm)";  s1 = "Processor ";   break;
            case PG(1) + NC(5) + STR1(1): *brand_pre = "Embedded AMD Opteron(tm)";  s1 = "Processor ";   break;
            default:                                                                s1 = NULL;           break;
            }
            /* 41322 3.74: table 15: String2 Values for Fr2, Fr5, and Fr6 (1207) Processors */
            switch (PG(pg) + NC(nc) + STR2(str2))
            {
            case PG(0) + NC(3) + STR2(10): s2 = " SE";   break;
            case PG(0) + NC(3) + STR2(11): s2 = " HE";   break;
            case PG(0) + NC(3) + STR2(12): s2 = " EE";   break;
            case PG(0) + NC(5) + STR2(0):  s2 = " SE";   break;
            case PG(0) + NC(5) + STR2(1):  s2 = " HE";   break;
            case PG(0) + NC(5) + STR2(2):  s2 = " EE";   break;
            case PG(1) + NC(3) + STR2(1):  s2 = "GF HE"; break;
            case PG(1) + NC(3) + STR2(2):  s2 = "HF HE"; break;
            case PG(1) + NC(3) + STR2(3):  s2 = "VS";    break;
            case PG(1) + NC(3) + STR2(4):  s2 = "QS HE"; break;
            case PG(1) + NC(3) + STR2(5):  s2 = "NP HE"; break;
            case PG(1) + NC(3) + STR2(6):  s2 = "KH HE"; break;
            case PG(1) + NC(3) + STR2(7):  s2 = "KS HE"; break;
            case PG(1) + NC(5) + STR2(1):  s2 = "QS";    break;
            case PG(1) + NC(5) + STR2(2):  s2 = "KS HE"; break;
            default:                       s2 = NULL;    break;
            }
            break;
         case 1:
            /* 41322 3.74: table 16: String1 Values for AM2r2 and AM3 Processors */
            switch (PG(pg) + NC(nc) + STR1(str1))
            {
            case PG(0) + NC(0) + STR1(2):  *brand_pre = "AMD Sempron(tm)";           s1 = "1";            break;
            /* This case obviously collides with one later */
            /* case PG(0) + NC(0) + STR1(3): *brand_pre = "AMD Athlon(tm) II";         s1 = "AMD Athlon(tm) II 1"; */
            case PG(0) + NC(0) + STR1(1):  *brand_pre = "AMD Athlon(tm)";            s1 = "";             break;
            case PG(0) + NC(0) + STR1(3):  *brand_pre = "AMD Athlon(tm) II X2";      s1 = "2";            break;
            case PG(0) + NC(0) + STR1(4):  *brand_pre = "AMD Athlon(tm) II X2";      s1 = "B";            break;
            case PG(0) + NC(0) + STR1(5):  *brand_pre = "AMD Athlon(tm) II X2";      s1 = "";             break;
            case PG(0) + NC(0) + STR1(7):  *brand_pre = "AMD Phenom(tm) II X2";      s1 = "5";            break;
            case PG(0) + NC(0) + STR1(10): *brand_pre = "AMD Phenom(tm) II X2";      s1 = "";             break;
            case PG(0) + NC(0) + STR1(11): *brand_pre = "AMD Phenom(tm) II X2";      s1 = "B";            break;
            case PG(0) + NC(0) + STR1(12): *brand_pre = "AMD Sempron(tm) X2";        s1 = "1";            break;
            case PG(0) + NC(2) + STR1(0):  *brand_pre = "AMD Phenom(tm)";            s1 = "";             break;
            case PG(0) + NC(2) + STR1(3):  *brand_pre = "AMD Phenom(tm) II X3";      s1 = "B";            break;
            case PG(0) + NC(2) + STR1(4):  *brand_pre = "AMD Phenom(tm) II X3";      s1 = "";             break;
            case PG(0) + NC(2) + STR1(7):  *brand_pre = "AMD Phenom(tm) II X3";      s1 = "4";            break;
            case PG(0) + NC(2) + STR1(8):  *brand_pre = "AMD Phenom(tm) II X3";      s1 = "7";            break;
            case PG(0) + NC(2) + STR1(10): *brand_pre = "AMD Phenom(tm) II X3";      s1 = "";             break;
            case PG(0) + NC(3) + STR1(0):  *brand_pre = "Quad-Core AMD Opteron(tm)"; s1 = "Processor 13"; break;
            case PG(0) + NC(3) + STR1(2):  *brand_pre = "AMD Phenom(tm)";            s1 = "";             break;
            case PG(0) + NC(3) + STR1(3):  *brand_pre = "AMD Phenom(tm) II X4";      s1 = "9";            break;
            case PG(0) + NC(3) + STR1(4):  *brand_pre = "AMD Phenom(tm) II X4";      s1 = "8";            break;
            case PG(0) + NC(3) + STR1(7):  *brand_pre = "AMD Phenom(tm) II X4";      s1 = "B";            break;
            case PG(0) + NC(3) + STR1(8):  *brand_pre = "AMD Phenom(tm) II X4";      s1 = "";             break;
            case PG(0) + NC(3) + STR1(10): *brand_pre = "AMD Athlon(tm) II X4";      s1 = "6";            break;
            case PG(0) + NC(3) + STR1(15): *brand_pre = "AMD Athlon(tm) II X4";      s1 = "";             break;
            case PG(0) + NC(5) + STR1(0):  *brand_pre = "AMD Phenom(tm) II X6";      s1 = "1";            break;
            case PG(1) + NC(1) + STR1(1):  *brand_pre = "AMD Athlon(tm) II XLT V";   s1 = "";             break;
            case PG(1) + NC(1) + STR1(2):  *brand_pre = "AMD Athlon(tm) II XL V";    s1 = "";             break;
            case PG(1) + NC(3) + STR1(1):  *brand_pre = "AMD Phenom(tm) II XLT Q";   s1 = "";             break;
            case PG(1) + NC(3) + STR1(2):  *brand_pre = "AMD Phenom(tm) II X4";      s1 = "9";            break;
            case PG(1) + NC(3) + STR1(3):  *brand_pre = "AMD Phenom(tm) II X4";      s1 = "8";            break;
            case PG(1) + NC(3) + STR1(4):  *brand_pre = "AMD Phenom(tm) II X4";      s1 = "6";            break;
            default:                                                                 s1 = NULL;           break;
            }
            /* 41322 3.74: table 17: String2 Values for AM2r2 and AM3 Processors */
            switch (PG(pg) + NC(nc) + STR2(str2))
            {
            case PG(0) + NC(0) + STR2(10): s2 = " Processor";                break;
            case PG(0) + NC(0) + STR2(11): s2 = "u Processor";               break;
            case PG(0) + NC(1) + STR2(3):  s2 = "50 Dual-Core Processor";    break;
            case PG(0) + NC(1) + STR2(6):  s2 = " Processor";                break;
            case PG(0) + NC(1) + STR2(7):  s2 = "e Processor";               break;
            case PG(0) + NC(1) + STR2(9):  s2 = "0 Processor";               break;
            case PG(0) + NC(1) + STR2(10): s2 = "0e Processor";              break;
            case PG(0) + NC(1) + STR2(11): s2 = "u Processor";               break;
            case PG(0) + NC(2) + STR2(0):  s2 = "00 Triple-Core Processor";  break;
            case PG(0) + NC(2) + STR2(1):  s2 = "00e Triple-Core Processor"; break;
            case PG(0) + NC(2) + STR2(2):  s2 = "00B Triple-Core Processor"; break;
            case PG(0) + NC(2) + STR2(3):  s2 = "50 Triple-Core Processor";  break;
            case PG(0) + NC(2) + STR2(4):  s2 = "50e Triple-Core Processor"; break;
            case PG(0) + NC(2) + STR2(5):  s2 = "50B Triple-Core Processor"; break;
            case PG(0) + NC(2) + STR2(6):  s2 = " Processor";                break;
            case PG(0) + NC(2) + STR2(7):  s2 = "e Processor";               break;
            case PG(0) + NC(2) + STR2(9):  s2 = "0e Processor";              break;
            case PG(0) + NC(2) + STR2(10): s2 = "0 Processor";               break;
            case PG(0) + NC(3) + STR2(0):  s2 = "00 Quad-Core Processor";    break;
            case PG(0) + NC(3) + STR2(1):  s2 = "00e Quad-Core Processor";   break;
            case PG(0) + NC(3) + STR2(2):  s2 = "00B Quad-Core Processor";   break;
            case PG(0) + NC(3) + STR2(3):  s2 = "50 Quad-Core Processor";    break;
            case PG(0) + NC(3) + STR2(4):  s2 = "50e Quad-Core Processor";   break;
            case PG(0) + NC(3) + STR2(5):  s2 = "50B Quad-Core Processor";   break;
            case PG(0) + NC(3) + STR2(6):  s2 = " Processor";                break;
            case PG(0) + NC(3) + STR2(7):  s2 = "e Processor";               break;
            case PG(0) + NC(3) + STR2(9):  s2 = "0e Processor";              break;
            case PG(0) + NC(3) + STR2(14): s2 = "0 Processor";               break;
            case PG(0) + NC(5) + STR2(0):  s2 = "5T Processor";              break;
            case PG(0) + NC(5) + STR2(1):  s2 = "0T Processor";              break;
            case PG(1) + NC(1) + STR2(1):  s2 = "L Processor";               break;
            case PG(1) + NC(1) + STR2(2):  s2 = "C Processor";               break;
            case PG(1) + NC(3) + STR2(1):  s2 = "L Processor";               break;
            case PG(1) + NC(3) + STR2(4):  s2 = "T Processor";               break;
            default:                       s2 = NULL;                        break;
            }
            break;
         case 2:
            /* 41322 3.74: table 18: String1 Values for S1g3 and S1g4 Processors */
            switch (PG(pg) + NC(nc) + STR1(str1))
            {
            case PG(0) + NC(0) + STR1(0): *brand_pre = "AMD Sempron(tm)";                          s1 = "M1"; break;
            case PG(0) + NC(0) + STR1(1): *brand_pre = "AMD";                                      s1 = "V";  break;
            case PG(0) + NC(1) + STR1(0): *brand_pre = "AMD Turion(tm) II Ultra Dual-Core Mobile"; s1 = "M6"; break;
            case PG(0) + NC(1) + STR1(1): *brand_pre = "AMD Turion(tm) II Dual-Core Mobile";       s1 = "M5"; break;
            case PG(0) + NC(1) + STR1(2): *brand_pre = "AMD Athlon(tm) II Dual-Core";              s1 = "M3"; break;
            case PG(0) + NC(1) + STR1(3): *brand_pre = "AMD Turion(tm) II";                        s1 = "P";  break;
            case PG(0) + NC(1) + STR1(4): *brand_pre = "AMD Athlon(tm) II";                        s1 = "P";  break;
            case PG(0) + NC(1) + STR1(5): *brand_pre = "AMD Phenom(tm) II";                        s1 = "X";  break;
            case PG(0) + NC(1) + STR1(6): *brand_pre = "AMD Phenom(tm) II";                        s1 = "N";  break;
            case PG(0) + NC(1) + STR1(7): *brand_pre = "AMD Turion(tm) II";                        s1 = "N";  break;
            case PG(0) + NC(1) + STR1(8): *brand_pre = "AMD Athlon(tm) II";                        s1 = "N";  break;
            case PG(0) + NC(2) + STR1(2): *brand_pre = "AMD Phenom(tm) II";                        s1 = "P";  break;
            case PG(0) + NC(2) + STR1(3): *brand_pre = "AMD Phenom(tm) II";                        s1 = "N";  break;
            case PG(0) + NC(3) + STR1(1): *brand_pre = "AMD Phenom(tm) II";                        s1 = "P";  break;
            case PG(0) + NC(3) + STR1(2): *brand_pre = "AMD Phenom(tm) II";                        s1 = "X";  break;
            case PG(0) + NC(3) + STR1(3): *brand_pre = "AMD Phenom(tm) II";                        s1 = "N";  break;
            default:                                                                               s1 = NULL; break;
            }
            /* 41322 3.74: table 19: String1 Values for S1g3 and S1g4 Processors */
            switch (PG(pg) + NC(nc) + STR2(str2))
            {
            case PG(0) + NC(0) + STR2(1): s2 = "0 Processor";             break;
            case PG(0) + NC(1) + STR2(2): s2 = "0 Dual-Core Processor";   break;
            case PG(0) + NC(2) + STR2(2): s2 = "0 Triple-Core Processor"; break;
            case PG(0) + NC(3) + STR2(1): s2 = "0 Quad-Core Processor";   break;
            default:                      s2 = NULL;                      break;
            }
            break;
         case 3:
            /* 41322 3.74: table 20: String1 Values for G34r1 Processors */
            switch (PG(pg) + NC(nc) + STR1(str1))
            {
            case PG(0) + NC(7)  + STR1(0): *brand_pre = "AMD Opteron(tm)";          s1 = "Processor 61"; break;
            case PG(0) + NC(11) + STR1(0): *brand_pre = "AMD Opteron(tm)";          s1 = "Processor 61"; break;
            case PG(1) + NC(7)  + STR1(1): *brand_pre = "Embedded AMD Opteron(tm)"; s1 = "Processor ";   break;
            /* It sure is odd that there are no 0/7/1 or 0/11/1 cases here. */
            default:                                                                s1 = NULL;           break;
            }
            /* 41322 3.74: table 21: String2 Values for G34r1 Processors */
            switch (PG(pg) + NC(nc) + STR2(str2))
            {
            case PG(0) + NC(7)  + STR1(0): s2 = " HE"; break;
            case PG(0) + NC(7)  + STR1(1): s2 = " SE"; break;
            case PG(0) + NC(11) + STR1(0): s2 = " HE"; break;
            case PG(0) + NC(11) + STR1(1): s2 = " SE"; break;
            case PG(1) + NC(7)  + STR1(1): s2 = "QS";  break;
            case PG(1) + NC(7)  + STR1(2): s2 = "KS";  break;
            default:                       s2 = NULL;  break;
            }
            break;
         case 4:
            /* 41322 3.74: table 22: String1 Values for ASB2 Processors */
            switch (PG(pg) + NC(nc) + STR1(str1))
            {
            case PG(0) + NC(0) + STR1(1): *brand_pre = "AMD Athlon(tm) II Neo"; s1 = "K";  break;
            case PG(0) + NC(0) + STR1(2): *brand_pre = "AMD";                   s1 = "V";  break;
            case PG(0) + NC(0) + STR1(3): *brand_pre = "AMD Athlon(tm) II Neo"; s1 = "R";  break;
            case PG(0) + NC(1) + STR1(1): *brand_pre = "AMD Turion(tm) II Neo"; s1 = "K";  break;
            case PG(0) + NC(1) + STR1(2): *brand_pre = "AMD Athlon(tm) II Neo"; s1 = "K";  break;
            case PG(0) + NC(1) + STR1(3): *brand_pre = "AMD";                   s1 = "V";  break;
            case PG(0) + NC(1) + STR1(4): *brand_pre = "AMD Turion(tm) II Neo"; s1 = "N";  break;
            case PG(0) + NC(1) + STR1(5): *brand_pre = "AMD Athlon(tm) II Neo"; s1 = "N";  break;
            default:                                                            s1 = NULL; break;
            }
            /* 41322 3.74: table 23: String2 Values for ASB2 Processors */
            switch (PG(pg) + NC(nc) + STR2(str2))
            {
            case PG(0) + NC(0)  + STR1(1): s2 = "5 Processor";           break;
            case PG(0) + NC(0)  + STR1(2): s2 = "L Processor";           break;
            case PG(0) + NC(1)  + STR1(1): s2 = "5 Dual-Core Processor"; break;
            case PG(0) + NC(1)  + STR1(2): s2 = "L Dual-Core Processor"; break;
            default:                       s2 = NULL;                    break;
            }
            break;
         case 5:
            /* 41322 3.74: table 24: String1 Values for C32r1 Processors */
            switch (PG(pg) + NC(nc) + STR1(str1))
            {
            case PG(0) + NC(3) + STR1(0): *brand_pre = "AMD Opteron(tm)";          s1 = "41"; break;
            case PG(0) + NC(5) + STR1(0): *brand_pre = "AMD Opteron(tm)";          s1 = "41"; break;
            case PG(1) + NC(3) + STR1(1): *brand_pre = "Embedded AMD Opteron(tm)"; s1 = " ";  break;
            case PG(1) + NC(5) + STR1(1): *brand_pre = "Embedded AMD Opteron(tm)"; s1 = " ";  break;
            /* It sure is odd that there are no 0/3/1 or 0/5/1 cases here. */
            default:                                                               s1 = NULL; break;
            }
            /* 41322 3.74: table 25: String2 Values for C32r1 Processors */
            /* 41322 3.74: table 25 */
            switch (PG(pg) + NC(nc) + STR2(str2))
            {
            case PG(0) + NC(3) + STR1(0): s2 = " HE";   break;
            case PG(0) + NC(3) + STR1(1): s2 = " EE";   break;
            case PG(0) + NC(5) + STR1(0): s2 = " HE";   break;
            case PG(0) + NC(5) + STR1(1): s2 = " EE";   break;
            case PG(1) + NC(3) + STR1(1): s2 = "QS HE"; break;
            case PG(1) + NC(3) + STR1(2): s2 = "LE HE"; break;
            case PG(1) + NC(3) + STR1(3): s2 = "CL EE"; break;
            case PG(1) + NC(5) + STR1(1): s2 = "KX HE"; break;
            case PG(1) + NC(5) + STR1(2): s2 = "GL EE"; break;
            default:                      s2 = NULL;    break;
            }
            break;
         default:
            s1 = NULL;
            s2 = NULL;
            break;
         }
      }
      else if ((ability->val_1_eax & 0x0ff00f00) == (2 << 20) + (15 << 8))
      {
         /* Family 11h tables */
         switch (pkgtype)
         {
         case 2:
            /* 41788 3.08: table 3: String1 Values for S1g2 Processors */
            switch (PG(pg) + NC(nc) + STR1(str1))
            {
            case PG(0) + NC(0) + STR1(0): *brand_pre = "AMD Sempron(tm)";                          s1 = "SI-"; break;
            case PG(0) + NC(0) + STR1(1): *brand_pre = "AMD Athlon(tm)";                           s1 = "QI-"; break;
            case PG(0) + NC(1) + STR1(0): *brand_pre = "AMD Turion(tm) X2 Ultra Dual-Core Mobile"; s1 = "ZM-"; break;
            case PG(0) + NC(1) + STR1(1): *brand_pre = "AMD Turion(tm) X2 Dual-Core Mobile";       s1 = "RM-"; break;
            case PG(0) + NC(1) + STR1(2): *brand_pre = "AMD Athlon(tm) X2 Dual-Core";              s1 = "QL-"; break;
            case PG(0) + NC(1) + STR1(3): *brand_pre = "AMD Sempron(tm) X2 Dual-Core";             s1 = "NI-"; break;
            default:                                                                               s1 = NULL;  break;
            }
            /* 41788 3.08: table 4: String2 Values for S1g2 Processors */
            switch (PG(pg) + NC(nc) + STR2(str2))
            {
            case PG(0) + NC(0) + STR2(0): s2 = "";   break;
            case PG(0) + NC(1) + STR2(0): s2 = "";   break;
            default:                      s2 = NULL; break;
            }
            break;
         default:
            s1 = NULL;
            s2 = NULL;
            break;
         }
      }
      else if ((ability->val_1_eax & 0x0ff00f00) == (3 << 20) + (15 << 8))
      {
         partialmodel--;

         /* Family 12h tables */
         switch (pkgtype)
         {
         case 1:
            /* 44739 3.10: table 6: String1 Values for FS1 Processors */
            switch (PG(pg) + NC(nc) + STR1(str1))
            {
            case PG(0) + NC(1) + STR1(3): *brand_pre = "AMD"; s1 = "A4-33"; break;
            case PG(0) + NC(1) + STR1(5): *brand_pre = "AMD"; s1 = "E2-30"; break;
            case PG(0) + NC(3) + STR1(1): *brand_pre = "AMD"; s1 = "A8-35"; break;
            case PG(0) + NC(3) + STR1(3): *brand_pre = "AMD"; s1 = "A6-34"; break;
            default:                                          s1 = NULL;    break;
            }
            /* 44739 3.10: table 7: String2 Values for FS1 Processors */
            switch (PG(pg) + NC(nc) + STR2(str2))
            {
            case PG(0) + NC(1) + STR2(1):  s2 = "M";  break;
            case PG(0) + NC(1) + STR2(2):  s2 = "MX"; break;
            case PG(0) + NC(3) + STR2(1):  s2 = "M";  break;
            case PG(0) + NC(3) + STR2(2):  s2 = "MX"; break;
            default:                       s2 = NULL; break;
            }
            break;
         case 2:
            /* 44739 3.10: table 8: String1 Values for FM1 Processors */
            switch (PG(pg) + NC(nc) + STR1(str1))
            {
            case PG(0) + NC(1) + STR1(1):  *brand_pre = "AMD";                   s1 = "A4-33"; break;
            case PG(0) + NC(1) + STR1(2):  *brand_pre = "AMD";                   s1 = "E2-32"; break;
            case PG(0) + NC(1) + STR1(4):  *brand_pre = "AMD Athlon(tm) II X2";  s1 = "2";     break;
            case PG(0) + NC(1) + STR1(5):  *brand_pre = "AMD";                   s1 = "A4-34"; break;
            case PG(0) + NC(1) + STR1(12): *brand_pre = "AMD Sempron(tm) X2";    s1 = "1";     break;
            case PG(0) + NC(2) + STR1(5):  *brand_pre = "AMD";                   s1 = "A6-35"; break;
            case PG(0) + NC(3) + STR1(5):  *brand_pre = "AMD";                   s1 = "A8-38"; break;
            case PG(0) + NC(3) + STR1(6):  *brand_pre = "AMD";                   s1 = "A6-36"; break;
            case PG(0) + NC(3) + STR1(13): *brand_pre = "AMD Athlon(tm) II X4";  s1 = "6";     break;
            default:                                                             s1 = NULL;    break;
            }
            /* 44739 3.10: table 9: String2 Values for FM1 Processors */
            switch (PG(pg) + NC(nc) + STR2(str2))
            {
            case PG(0) + NC(1) + STR2(1):  s2 = " APU with Radeon(tm) HD Graphics"; break;
            case PG(0) + NC(1) + STR2(2):  s2 = " Dual-Core Processor";             break;
            case PG(0) + NC(2) + STR2(1):  s2 = " APU with Radeon(tm) HD Graphics"; break;
            case PG(0) + NC(3) + STR2(1):  s2 = " APU with Radeon(tm) HD Graphics"; break;
            case PG(0) + NC(3) + STR2(3):  s2 = " Quad-Core Processor";             break;
            default:                       s2 = NULL;                               break;
            }
            break;
         default:
            s1 = NULL;
            s2 = NULL;
            break;
         }
      }
      else if ((ability->val_1_eax & 0x0ff00f00) == (5 << 20) + (15 << 8))
      {
         partialmodel--;

         /* Family 14h Models 00h-0Fh tables */
         switch (pkgtype)
         {
         case 0:
            /* 47534 3.00: table 4: String1 Values for FT1 Processors */
            switch (PG(pg) + NC(nc) + STR1(str1))
            {
            case PG(0) + NC(0) + STR1(1): *brand_pre = "AMD"; s1 = "C-";   break;
            case PG(0) + NC(0) + STR1(2): *brand_pre = "AMD"; s1 = "E-";   break;
            case PG(0) + NC(0) + STR1(4): *brand_pre = "AMD"; s1 = "G-T";  break;
            case PG(0) + NC(1) + STR1(1): *brand_pre = "AMD"; s1 = "C-";   break;
            case PG(0) + NC(1) + STR1(2): *brand_pre = "AMD"; s1 = "E-";   break;
            case PG(0) + NC(1) + STR1(3): *brand_pre = "AMD"; s1 = "Z-";   break;
            case PG(0) + NC(1) + STR1(4): *brand_pre = "AMD"; s1 = "G-T";  break;
            case PG(0) + NC(1) + STR1(5): *brand_pre = "AMD"; s1 = "E1-1"; break;
            case PG(0) + NC(1) + STR1(6): *brand_pre = "AMD"; s1 = "E2-1"; break;
            case PG(0) + NC(1) + STR1(7): *brand_pre = "AMD"; s1 = "E2-2"; break;
            default:                                          s1 = NULL;   break;
            }
            /* 47534 3.00: table 5: String2 Values for FT1 Processors */
            switch (PG(pg) + NC(nc) + STR2(str2))
            {
            case PG(0) + NC(0) + STR2(1):  s2 = "";   break;
            case PG(0) + NC(0) + STR2(2):  s2 = "0";  break;
            case PG(0) + NC(0) + STR2(3):  s2 = "5";  break;
            case PG(0) + NC(0) + STR2(4):  s2 = "0x"; break;
            case PG(0) + NC(0) + STR2(5):  s2 = "5x"; break;
            case PG(0) + NC(0) + STR2(6):  s2 = "x";  break;
            case PG(0) + NC(0) + STR2(7):  s2 = "L";  break;
            case PG(0) + NC(0) + STR2(8):  s2 = "N";  break;
            case PG(0) + NC(0) + STR2(9):  s2 = "R";  break;
            case PG(0) + NC(0) + STR2(10): s2 = "0";  break;
            case PG(0) + NC(0) + STR2(11): s2 = "5";  break;
            case PG(0) + NC(0) + STR2(12): s2 = "";   break;
            case PG(0) + NC(0) + STR2(13): s2 = "0D"; break;
            case PG(0) + NC(1) + STR2(1):  s2 = "";   break;
            case PG(0) + NC(1) + STR2(2):  s2 = "0";  break;
            case PG(0) + NC(1) + STR2(3):  s2 = "5";  break;
            case PG(0) + NC(1) + STR2(4):  s2 = "0x"; break;
            case PG(0) + NC(1) + STR2(5):  s2 = "5x"; break;
            case PG(0) + NC(1) + STR2(6):  s2 = "x";  break;
            case PG(0) + NC(1) + STR2(7):  s2 = "L";  break;
            case PG(0) + NC(1) + STR2(8):  s2 = "N";  break;
            case PG(0) + NC(1) + STR2(9):  s2 = "0";  break;
            case PG(0) + NC(1) + STR2(10): s2 = "5";  break;
            case PG(0) + NC(1) + STR2(11): s2 = "";   break;
            case PG(0) + NC(1) + STR2(12): s2 = "E";  break;
            case PG(0) + NC(1) + STR2(13): s2 = "0D"; break;
            default:                       s2 = NULL; break;
            }
            break;
         default:
            s1 = NULL;
            s2 = NULL;
            break;
         }
      }
      else
      {
         s1 = 0;
         s2 = 0;
      }

#undef NC
#undef PG
#undef STR1
#undef STR2

      if (s1 != 0)
      {
         char*  p = proc;
         p += sprintf(p, "%s%02d", s1, partialmodel);
         if (s2)
             sprintf(p, "%s", s2);
      }
   }
}

#include "x86_amd_table.h"
}
namespace Other
{
/*
** VIA major query
*/
static bool via_is_claim_v7(const struct cpu_ability *ability)
{
    return ability->br.c7;
}
static bool via_is_claim_vM(const struct cpu_ability *ability)
{
    return ability->br.c7m;
}
static bool via_is_claim_vD(const struct cpu_ability *ability)
{
    return ability->br.c7d;
}
static bool via_is_claim_vE(const struct cpu_ability *ability)
{
    return ability->br.eden;
}
static bool via_is_claim_vZ(const struct cpu_ability *ability)
{
    return ability->br.zhaoxin;
}
// Cyrix
static bool cyrix_is_claim_cm(const struct cpu_ability *ability)
{
    return ability->br.mediagx;
}
// Transmeta
static bool transmeta_is_claim_t2(const struct cpu_ability *ability)
{
    return (ability->transmeta_proc_rev & 0xffff0000) == 0x01010000;
}
static bool transmeta_is_claim_t4(const struct cpu_ability *ability)
{
    return ((ability->transmeta_proc_rev & 0xffff0000) == 0x01020000) ||
            (((ability->transmeta_proc_rev & 0xffff0000) == 0x01030000) &&
             ability->L2_4w_256K);
}
static bool transmeta_is_claim_t5(const struct cpu_ability *ability)
{
    return ((ability->transmeta_proc_rev & 0xffff0000) == 0x01040000) ||
            (((ability->transmeta_proc_rev & 0xffff0000) == 0x01040000) &&
             ability->L2_4w_256K);
}
static bool transmeta_is_claim_t6(const struct cpu_ability *ability)
{
    return ((ability->transmeta_proc_rev & 0xffff0000) == 0x01030000) &&
            ability->L2_4w_256K;
}
static bool transmeta_is_claim_t8(const struct cpu_ability *ability)
{
    return ((ability->transmeta_proc_rev & 0xffff0000) == 0x01040000) ||
            (((ability->transmeta_proc_rev & 0xffff0000) == 0x01040000) &&
             ability->L2_4w_512K);
}
#include "x86_other_table.h"
}

static eCPUVendors query_vendor(uint words[4])
{
    for (int i = 1; i < kyArraySizeOf(x86_vendor); ++i)
        if (CMP_VENDOR_ID(words, x86_vendor[i].str))
            return x86_vendor[i].code;
    return CPU_Unknown;
}
static cpu_ability::cpu_version query_version(const uint &eax)
{
    cpu_ability::cpu_version version;
    version.stepping     = eax & 0x0f;
    version.model        = (eax >> 4) & 0x0f;
    version.family       = (eax >> 8) & 0x0f;
    uint tmp_type        = (eax >> 13) & 0x03;
    version.model_ext    = (eax >> 16) & 0x0f;
    version.family_ext   = (eax >> 20) & 0xff;
    version.type         = x86_type[tmp_type].code;

    version.family_synth = version.family + version.family_ext;
    version.model_synth  = version.model + (version.model_ext << 4);
    return version;
}
static cpu_ability::cpu_synth_phys query_phys(const cpu_ability *ability, uint eax, uint ebx)
{
#define __ARG_A(V) 0
#define __ARG_B(V) V
#define SYNTH_PHYS(var, Vendor, seg, A, B, C, D)\
{ \
    var.brand = 0;\
    var.proc = 0;\
    var.vendor = x86_vendor_brief[Vendor].str;\
    var.name = 0;\
    var.uarch = 0; \
    var.family = 0; \
    var.craft = 0; \
    var.core_is_uarch = 0; \
    for (uint i = 0; i < kyArraySizeOf(x86_##seg##_name); ++i)\
    {\
        bool and_equel = (eax & x86_##seg##_name[i].ands) == x86_##seg##_name[i].equel;\
        if (!and_equel || (x86_##seg##_name[i].is_claim && !x86_##seg##_name[i].is_claim(ability)))\
            continue;\
        var.name = x86_##seg##_name[i].str;\
        break;\
    }\
    if (var.name == 0) var.name = x86_vendor[CPU_Unknown].str;\
    for (uint i = 0; i < kyArraySizeOf(x86_##seg##_uarch); ++i)\
    {\
        if ((eax & x86_##seg##_uarch[i].ands) == x86_##seg##_uarch[i].equel)\
        {\
            var.uarch = A(x86_##seg##_uarch[i].uarch);\
            var.family = B(x86_##seg##_uarch[i].family);\
            var.craft = C(x86_##seg##_uarch[i].phys);\
            var.core_is_uarch = D(x86_##seg##_uarch[i].core_is_uarch);\
            break;\
        }\
    }\
}
    cpu_ability::cpu_synth_phys phys;

    switch (vendor)
    {
    case CPU_Intel:
    {
        SYNTH_PHYS(phys, CPU_Intel, intel, __ARG_B, __ARG_B, __ARG_B, __ARG_B);
        break;
    }
    case CPU_AMD:
    {
        SYNTH_PHYS(phys, CPU_AMD, amd, __ARG_B, __ARG_A, __ARG_B, __ARG_A);

        const  char*  brand_pre = 0;
        const  char*  brand_post = 0;
        static char   proc[96] = {'\0'};
        AMD::decode_amd_model(this, &brand_pre, &brand_post, proc);
        if (proc[0] != '\0')
        {
            phys.model = brand_pre;
            phys.proc = proc;
        }
        break;
    }
    case CPU_Cyrix:
    {
        SYNTH_PHYS(phys, CPU_Cyrix, cyrix, __ARG_B, __ARG_A, __ARG_A, __ARG_B);
        break;
    }
    case CPU_VIA:
    {
        SYNTH_PHYS(phys, CPU_VIA, via, __ARG_B, __ARG_A, __ARG_B, __ARG_B);
        break;
    }
    case CPU_Transmeta:
    {
        SYNTH_PHYS(phys, CPU_Transmeta, transmeta, __ARG_B, __ARG_A, __ARG_A, __ARG_B);
        break;
    }
    case CPU_UMC:
    {
        SYNTH_PHYS(phys, CPU_UMC, umc, __ARG_B, __ARG_A, __ARG_A, __ARG_B);
        break;
    }
    case CPU_NexGen:
    {
       SYNTH_PHYS(phys, CPU_NexGen, nexgen, __ARG_B, __ARG_A, __ARG_A, __ARG_A);
       break;
    }
    case CPU_Rise:
    {
       SYNTH_PHYS(phys, CPU_Rise, rise, __ARG_B, __ARG_A, __ARG_A, __ARG_B);
       break;
    }
    case CPU_SIS:
    {
       SYNTH_PHYS(phys, CPU_SIS, sis, __ARG_B, __ARG_A, __ARG_A, __ARG_B);
       break;
    }
    case CPU_NSC:
    {
       SYNTH_PHYS(phys, CPU_NSC, nsc, __ARG_A, __ARG_A, __ARG_A, __ARG_A);
       break;
    }
    case CPU_Vortex:
    {
       SYNTH_PHYS(phys, CPU_Vortex, vortex, __ARG_A, __ARG_A, __ARG_A, __ARG_A);
       break;
    }
    case CPU_RDC:
    {
       SYNTH_PHYS(phys, CPU_RDC, rdc, __ARG_A, __ARG_A, __ARG_A, __ARG_A);
       break;
    }
    case CPU_Hygon:
    {
       SYNTH_PHYS(phys, CPU_Hygon, hygon, __ARG_B, __ARG_A, __ARG_B, __ARG_A);
       break;
    }
    case CPU_Zhaoxin:
    {
        SYNTH_PHYS(phys, CPU_Zhaoxin, zhaoxin, __ARG_B, __ARG_A, __ARG_B, __ARG_B);
        break;
    }
    }

#undef __ARG_A
#undef __ARG_B
#undef SYNTH_PHYS

    // 其他杂项
    ky_bitset<32> bit_ebx(ebx);
    phys.apic_id   = bit_ebx.extract(24, 31);
    phys.count     = bit_ebx.extract(16, 23); // LogicalProcessorCount
    phys.clflush   = bit_ebx.extract(8, 15);
    phys.brand_id = bit_ebx.extract(0, 7);
    return phys;
}
static eCPUFeatures query_feature(const uint &ecx, const uint &edx)
{
    eCPUFeatures feature;
    for (uint i = 0; i < kyArraySizeOf(x86_feature_edx); ++i)
    {
        if (edx & (1 << x86_feature_edx[i].bit))
            feature |= x86_feature_edx[i].code;
    }
    for (uint i = 0; i < kyArraySizeOf(x86_feature_ecx); ++i)
    {
        if (ecx & (1 << x86_feature_ecx[i].bit))
            feature |= x86_feature_ecx[i].code;
    }
    return feature;
}
static ky_cpu::cache_t *cache_append(ky_cpu::cache_t *root, ky_cpu::cache_t  *n)
{
    if (root != 0)
    {
        n->next = root->next;
        root->next = n;
    }
    return root ? root : n;
}
static ky_cpu::cache_t *query_cache(const uint8 &byte, const uint &eax1)
{
    ky_cpu::cache_t *cache = 0;
    if (byte == 0x40)
    {
        if ((0x0ff00f00 & eax1) <= (6 << 8))
            return 0;// "No L2 cache"
        else
            return 0;// "No L3 cache"
    }
    else if (byte == 0x49)
    {
        if ((0x0fff0ff0 & eax1) == ((15 << 8)+(6 << 4)))
        {
            // "L3 cache: 4M, 16-way, 64 byte lines"
            static ky_cpu::cache_t l3c =
                    X86C( 0x49, 04*kyMiB, 16, 64, CACHE::Unified | CACHE::L3);
            return l3c;
        }
        else
        {
            // "L2 cache: 4M, 16-way, 64 byte lines"
            static ky_cpu::cache_t l2c =
                    X86C( 0x49, 04*kyMiB, 16, 64, CACHE::Unified | CACHE::L2);
            return l2c;
        }
    }

    for (uint i = 0; i < kyArraySizeOf(x86_cache_table); ++i)
    {
        if (x86_cache_table[i].code != byte)
            continue;

        do
            cache = cache_append(cache, &x86_cache_table[i]);
        while ((i < kyArraySizeOf(x86_cache_table)) &&
               x86_cache_table[++i].code == byte);
        break;
    }
    return cache;
}

static cpu_ability::cpu_BrandExt query_81ebx_amd(uint  value, uint  val_1_eax)
{
    cpu_ability::cpu_BrandExt brand_ext;
   if ((val_1_eax & 0x0ff00f00) == (15 << 8) &&
       (val_1_eax & 0x000f00f0) < (4 << 16))
   {
      brand_ext.id = value & 0x1ffff;
      brand_ext.index = (value >> 6) & 0x7f;
      brand_ext.nn = value & 0x7f;
   }
   else if ((val_1_eax & 0x0ff00f00) == (15 << 8) &&
            (val_1_eax & 0x000f00f0) >= (4 << 16))
   {
       brand_ext.id = value & 0x1ffff;
       brand_ext.index = (value >> 9) & 0x1f;
       brand_ext.nn = (((value >> 15) & 0x01) << 6) | (value & 0x3f);
       brand_ext.pwrlmt = (((value >> 6) & 0x07) << 1) | ((value >> 14) & 0x01);
   }
   else if ((val_1_eax & 0x0ff00f00) == (1 << 20) + (15 << 8) ||
            (val_1_eax & 0x0ff00f00) == (2 << 20) + (15 << 8))
   {
       brand_ext.id = value & 0xffff;
       brand_ext.nn = (((value >> 11) & 0x0f) << 4) | value & 0x0f;
       brand_ext.pwrlmt = (((value >> 4) & 0x7f) << 1) | (value >> 15 & 0x01);
   }
   else
   {
      brand_ext.id = value & 0xffff;
   }

   // PkgType values come from these two guides (depending on family):
   //    AMD BIOS and Kernel Developer's Guide (BKDG) for ...
   //    Processor Programming Reference (PPR) for ...
   //
   // NOTE: AMD Family = XF + F,         e.g. 0x17 (17h) = 0xf + 0x8
   //       AMD Model  = (XM << 4) + M,  e.g. 0x18 (18h) = (0x1 << 4) + 0x8

   if ((val_1_eax & 0x0ff00f00) >= (1 << 20) + (15 << 8))
   {
       const char *const * pkt = 0;
       uint max_len = 0;
      if ((val_1_eax & 0x0ff00f00) == (1 << 20) + (15 << 8))
      {
          // Family 10h
          static const char *const pkg_type[] =
          {
              "Fr2/Fr5/Fr6 (0)",
              "AM2r2/AM3 (1)",
              "S1g3/S1g4 (2)",
              "G34 (3)",
              "ASB2 (4)",
              "C32 (5)"
          };
          pkt = pkg_type;
          max_len = kyArraySizeOf(pkg_type);
      }
      else if ((val_1_eax & 0x0ff00f00) == (6 << 20) + (15 << 8))
      {
         // Family 15h
         if ((val_1_eax & 0x000f00f0) <= (15 << 4))
         {
             static const char *const pkg_type[] =
             {
                 0,
                 "AM3r2 (1)",
                 0,
                 "G34r1 (3)",
                 0,
                 "C32r1 (5)"
             };
             pkt = pkg_type;
             max_len = kyArraySizeOf(pkg_type);
         }
         else if ((val_1_eax & 0x000f00f0) >= (1 << 16) &&
                  (val_1_eax & 0x000f00f0) <= (1 << 16) + (15 << 4))
         {
            static const char *const pkg_type[] =
            {
                "FP2 (BGA) (0)",
                "FS1r2 (uPGA) (1)",
                "FM2 (PGA) (2)"
            };

            pkt = pkg_type;
            max_len = kyArraySizeOf(pkg_type);
         }
         else if ((val_1_eax & 0x000f00f0) >= (3 << 16) &&
                  (val_1_eax & 0x000f00f0) <= (3 << 16) + (15 << 4))
         {
             static const char *const pkg_type[] =
             {
                 "FP3 (BGA) (0)",
                 "FM2r2 (uPGA) (1)"
             };

             pkt = pkg_type;
             max_len = kyArraySizeOf(pkg_type);
         }
         else if ((val_1_eax & 0x000f00f0) >= (6 << 16) &&
                  (val_1_eax & 0x000f00f0) <= (6 << 16) + (15 << 4))
         {
            static const char *const pkg_type[] =
            {
                "FP4 (BGA) (0)",
                0,
                "AM4 (uPGA) (2)",
                "FM2r2 (uPGA) (3)"
            };

            pkt = pkg_type;
            max_len = kyArraySizeOf(pkg_type);
         }
         else if ((val_1_eax & 0x000f00f0) >= (7 << 16) &&
                    (val_1_eax & 0x000f00f0) <= (7 << 16) + + (15 << 4))
         {
            static const char *const pkg_type[] =
            {
                "FP4 (BGA) (0)",
                0,
                "AM4 (uPGA) (2)",
                0,
                "FT4 (BGA) (4)"
            };
            pkt = pkg_type;
            max_len = kyArraySizeOf(pkg_type);
         }

      }
      else if ((val_1_eax & 0x0ff00f00) == (7 << 20) + (15 << 8))
      {
         // Family 16h
         if ((val_1_eax & 0x000f00f0) <= (15 << 4))
         {
             static const char *const pkg_type[]  =
             {
                 "FT3 (BGA) (0)",
                 "FS1b (1)"
             };
             pkt = pkg_type;
             max_len = kyArraySizeOf(pkg_type);
         }
         else if ((val_1_eax & 0x000f00f0) >= (3 << 16) &&
                  (val_1_eax & 0x000f00f0) <= (3 << 16) + (15 << 4))
         {
             static const char *const pkg_type[] =
             {
                 "FT3b (BGA) (0)",
                 0,
                 0,
                 "FP4 (3)"
             };

             pkt = pkg_type;
             max_len = kyArraySizeOf(pkg_type);
         }
      }
      else if ((val_1_eax & 0x0ff00f00) == (8 << 20) + (15 << 8))
      {
         // Family 17h
         if ((val_1_eax & 0x000f00f0) == (1 << 4)  ||
             (val_1_eax & 0x000f00f0) == (8 << 4))
         {
             static const char *const pkg_type[] =
             {
                 0,
                 0,
                 "AM4 (2)"
             };
             pkt = pkg_type;
             max_len = kyArraySizeOf(pkg_type);
         }
         else if ((val_1_eax & 0x000f00f0) == (1 << 16) + (8 << 4))
         {
             static const char *const pkg_type[] =
             {
                 "FP5 (0)",
                 0,
                 "AM4 (2)"
             };
             use_pkg_type = pkg_type;
         }
         else if ((val_1_eax & 0x000f00f0) == (7 << 16) + (1<< 4))
         {
             static const char *const pkg_type[] =
             {
                 0,
                 0,
                 "AM4 (2)"
             };
             use_pkg_type = pkg_type;
         }
      }

      const uint inx = (value >> 28) & 0x0f;
      if (inx < max_len)
          brand_ext.pkt = pkt[inx];
   }
   return brand_ext;
}

void cpu_ability::query_capable()
{
    // 0x00
    uint max_extension = 0;
    for (uint reg = 0; reg <= max_extension; ++reg)
    {
        x86_reg words[4] = {0, 0, 0, 0};
        cpuid(reg, words);

        if (reg == 0)// CPU厂商
        {
           max_extension = words[0];
           val_0_eax = words[0];
           vendor = query_vendor(words);
        }
        else if (reg == 1) // CPU物理信息
        {
            val_1_eax = words[0];
            val_1_ebx = words[1];
            val_1_ecx = words[2];
            val_1_edx = words[3];

            // 版本信息获取
            version = query_version(words[0]);
            // 物理信息获取
            synth_phys = query_phys(this, words[0], words[1]);

            // 品牌信息
            synth_phys.brand = 0;
            for (uint i = 0; i < kyArraySizeOf(x86_brand_id); ++i)
            {
                bool and_equel = (words[0] & x86_intel_brand[i].ands) == x86_intel_brand[i].equel;
                bool and1_equel = (words[1] & x86_intel_brand[i].ands1) == x86_intel_brand[i].equel1;
                if (!and_equel || (x86_intel_brand[i].ext1 && !and1_equel))
                    continue;
                synth_phys.brand = x86_intel_brand[i].str;
                break;
            }
            if (synth_phys.brand == 0)
                synth_phys.brand = x86_vendor[CPU_Unknown].str;

            // 基本指令集
            feature = query_feature( words[2],  words[3]);
        }
        else if (reg == 2) // 缓存和TLB信息
        {
            uint  max_tries = words[0] & 0xff;
            uint  _try       = 0;

            while (true)
            {
                for (uint word = 0; word < 4; ++word)
                {
                    if ((words[word] & 0x80000000) == 0)
                    {
                        const uchar*  bytes = (const uchar*)&words[word];
                        uint          byte  = (_try == 0 && word == 0 ? 1 : 0);
                        for (; byte < 4; ++byte)
                            cache = query_cache(cache, bytes[byte]);
                    }
                }

               if (++_try >= max_tries)
                   break;

               cpuid(reg, words, _try);
            }
        }
        else if (reg == 3) // CPU SN
        {
            sn[0] = val_1_eax >> 16;
            sn[1] = val_1_eax & 0xffff;
            sn[2] = words[3] >> 16;
            sn[3] = words[3] & 0xffff;
            sn[4] = words[2] >> 16;
            sn[5] = words[2] & 0xffff;
        }
        else if (reg == 4) // 缓存参数
        {
            uint  _try = 0;
            while ((words[0] & 0x1f) != 0)
            {
                {
                    saw_4 = true;
                    if (_try == 0)
                        val_4_eax = words[0];


                    ky_bitset<32> bit_eax(regs[0]);
                    uint16 type     = bit_eax.extract( 0,  4); //  "cache type"
                    uint8  level    = bit_eax.extract( 5,  7); //  "cache level"
                    bool   is_self  = bit_eax.extract( 8,  8); //  "self-initializing cache level"
                    bool   is_fully = bit_eax.extract( 9, 9); //  "fully associative cache"
                    uint8  shareBy  = bit_eax.extract(14, 25); //  "extra threads sharing this cache"
                    uint8  core_die = bit_eax.extract(26, 31); //  "extra processor cores on this die"

                    if (type == 0)
                        type = ky_cpu::cache_t::Null; // "no more caches"
                    else if (type == 1)
                        type = ky_cpu::cache_t::Data; // "data cache"
                    else if (type == 2)
                        type = ky_cpu::cache_t::Instruction; // "instruction cache"
                    else if (type == 3)
                        type = ky_cpu::cache_t::Unified; // "unified cache"
                    type |= level;
                    type |= is_self ? ky_cpu::cache_t::SelfInitial : 0;
                    type |= is_fully ? ky_cpu::cache_t::FullyAssociative : 0;

                    ky_bitset<32> bit_ebx(regs[1]);
                    uint  line_size  = bit_ebx.extract( 0, 11)+ 1; // system coherency line size;
                    uint  parts      = bit_ebx.extract(12, 21)+ 1; // "physical line partitions"
                    uint  ways_assoc = bit_ebx.extract(22, 31)+ 1; // "ways of associativity"

                    uint  sets = regs[2] +1; // "number of sets"
                    uint  size = (ways_assoc * parts * line_size * sets);

                    type |= (regs[3] & 1) ? ky_cpu::cache_t::WBINVD : 0;
                    type |= (regs[3] & 2) ? ky_cpu::cache_t::InclusiveLower : 0;
                    type |= (regs[3] & 4) ? ky_cpu::cache_t::ComplexIndex : 0;

                    arch_cache ac = {0x00, size, ways_assoc, line_size, type, shareBy, core_die, 0};
                    cache = cache_append(cache, kyNew(ky_cpu::cache_t(&ac))) ;
                }
                ++_try;
                cpuid(reg, words, _try);
            }
        }
        else if (reg == 5) // MONITOR/MWAIT (5)
        {
            mwait.smallest    = words[0] & 0xffff; // "smallest monitor-line size (bytes)" 最小监视器行大小（字节）
            mwait.largest     = words[1] & 0xffff; // "largest monitor-line size (bytes)" 最大监视器行大小（字节）
            mwait.support     = words[2] & 1; // "enum of Monitor-MWAIT exts supported"
            mwait.break_event = words[2] & 2; // "supports intrs as break-event for MWAIT"
            ky_bitset<32> bit_edx(words[3]);
            mwait.state_c0 = bit_edx.extract(0, 3); // "number of C0 sub C-states using MWAIT"
            mwait.state_c1 = bit_edx.extract(4, 7); // "number of C1 sub C-states using MWAIT"
            mwait.state_c2 = bit_edx.extract(8, 11); // "number of C2 sub C-states using MWAIT"
            mwait.state_c3 = bit_edx.extract(12, 15); // "number of C3 sub C-states using MWAIT"
            mwait.state_c4 = bit_edx.extract(16, 19); // "number of C4 sub C-states using MWAIT"
            mwait.state_c5 = bit_edx.extract(20, 23); // "number of C5 sub C-states using MWAIT"
            mwait.state_c6 = bit_edx.extract(24, 27); // "number of C6 sub C-states using MWAIT"
            mwait.state_c7 = bit_edx.extract(28, 31); // "number of C7 sub C-states using MWAIT"
        }
        else if (reg == 6) // Thermal and Power Management Features
        {
            power.feature = 0;
            for (uint i = 0; i < kyArraySizeOf(x86_power_eax_table); ++i)
            {
                if ((1 << x86_power_eax_table[i].bit) & words[0])
                    power.feature |= x86_power_eax_table[i].code;
            }
            for (uint i = 0; i < kyArraySizeOf(x86_power_ecx_table); ++i)
            {
                if ((1 << x86_power_ecx_table[i].bit) & words[2])
                    power.feature |= x86_power_ecx_table[i].code;
            }
            for (uint i = 0; i < kyArraySizeOf(x86_power_edx_table); ++i)
            {
                if ((1 << x86_power_edx_table[i].bit) & words[3])
                    power.feature |= x86_power_edx_table[i].code;
            }
            power.thresholds = words[1] & 0x0f; // "digital thermometer thresholds"

            power.feedback = (words[3] >> 8) & 0x0f; // "size of feedback struct (4KB pages)"
            power.row_index = (words[3] >> 16) & 0xffff; // "index of CPU's row in feedback struct"
        }
        else if (reg == 7) // extended feature flags (7)
        {
            uint  _try = 0;
            uint  max_tries;
            while (true)
            {
                if (_try == 0)
                {
                    max_tries = words[0];
                    for (uint i = 0; i < kyArraySizeOf(x86_feature_ext_ebx); ++i)
                    {
                        if ((1 << x86_feature_ext_ebx[i].bit) & words[1])
                            feature |= x86_feature_ext_ebx[i].code;
                    }
                    for (uint i = 0; i < kyArraySizeOf(x86_feature_ext_ecx); ++i)
                    {
                        if ((1 << x86_feature_ext_ecx[i].bit) & words[2])
                            feature |= x86_feature_ext_ecx[i].code;
                    }
                    for (uint i = 0; i < kyArraySizeOf(x86_feature_ext_edx); ++i)
                    {
                        if ((1 << x86_feature_ext_edx[i].bit) & words[2])
                            feature |= x86_feature_ext_edx[i].code;
                    }
                }
                else if (_try == 1)
                {
                    for (uint i = 0; i < kyArraySizeOf(x86_feature_ext1_eax); ++i)
                    {
                        if ((1 << x86_feature_ext1_eax[i].bit) & words[1])
                            feature |= x86_feature_ext1_eax[i].code;
                    }
                }

               if (++_try > max_tries)
                   break;

               cpuid(reg, words, _try);
            }
        }
        else if (reg == 9) // Direct Cache Access Parameters (9)
        {
            PLATFORM_DCA_CAP = words[0]; // PLATFORM_DCA_CAP MSR bits =
        }
        else if (reg == 0x0a) // Architecture Performance Monitoring Features (0x0a)
        {
            // eax
            performance.version_id = words[0] & 0xff; // "version ID"
            performance.counters = (words[0] >> 8) & 0xff; // "number of counters per logical processor"
            performance.bit_width = (words[0] >> 16) & 0xff; // "bit width of counter"
            performance.bit_length = (words[0] >> 24) & 0xff; // "length of EBX bit vector"

            // ebx
            performance.event = 0;
            performance.event |= (words[1] & (1 << 0)) ? EventNotCoreCycle : 0;
            performance.event |= (words[1] & (1 << 1)) ? EventNotInstructRetired : 0;
            performance.event |= (words[1] & (1 << 2)) ? EventNotReferCycle : 0;
            performance.event |= (words[1] & (1 << 3)) ? EventNotRefCache : 0;
            performance.event |= (words[1] & (1 << 4)) ? EventNotMissCache : 0;
            performance.event |= (words[1] & (1 << 5)) ? EventNotRetiredInst : 0;
            performance.event |= (words[1] & (1 << 6)) ? EventNotRetiredMispred : 0;

            // edx
            performance.counters_fixed = words[3] & 0x0f; // "number of fixed counters"
            performance.counters_fixed_bit_width = (words[3] >> 5)& 0x0f;// "bit width of fixed counters"
            performance.anythread_dep = (words[3] & (1 << 15)) ? AnyThreadDeprecat : 0;
        }
        else if (reg == 0x0b) // x2APIC features / processor topology (0xb)
        {
            uint  _try = 0;
            while (words[0] != 0 || words[1] != 0)
            {
                saw_b = true;
                if (_try < kyArraySizeOf(val_b_eax))
                    val_b_eax[_try] = words[0];
                if (_try < kyArraySizeOf(val_b_ebx))
                    val_b_ebx[_try] = words[1];

                if (_try == 0)  // extended APIC ID
                    x2apic.apic_id = words[3];
                //fprintf(stderr,"      --- level %d ---\n", try);
                x2apic.level[_try].number = words[2] & 0xff; // "level number"
                x2apic.level[_try].type = (ex2APICLevelTypes)(words[2] >> 8)& 0xff; // "level type"
                x2apic.level[_try].bit_width = words[0] & 0x0f; // "bit width of level"
                x2apic.level[_try].logical_processors = words[1] & 0xff; // "number of logical processors at level"

               _try++;
               cpuid(reg, words, _try);
            }
        }
        else if (reg == 0x0d) // XSAVE features (0xd/0)(0xd/1)
        {
            // try = 0
            // fprintf(stderr,"   XSAVE features (0xd/0):\n");
            // XCR0 lower 32 bits valid bit field mask = 0x%08x
            // XCR0 upper 32 bits valid bit field mask = 0x%08x
            xsave.xcr0_mask = words[0] | uint64(words[3] << 32);

            for (uint i = 0; i < kyArraySizeOf(x86_xsave_xcr0_table); ++i)
            {
                if ((1 << x86_xsave_xcr0_table[i].bit) & words[0])
                    xsave.flags |= x86_xsave_xcr0_table[i].code;
            }

            //  bytes required by fields in XCR0        = 0x%08x (%u)
            xsave.xcr_required_byte = words[1];
            // bytes required by XSAVE/XRSTOR area     = 0x%08x (%u)
            xsave.xsave_required_byte = words[2];

            // try = 1
            uint64  valid_xcr0 = ((uint64)words[3] << 32) | words[0];
            cpuid (reg, words, 1);
            // fprintf(stderr,"   XSAVE features (0xd/1):\n");
            for (uint i = 0; i < kyArraySizeOf(x86_xsave_instruct_table); ++i)
            {
                if ((1 << x86_xsave_instruct_table[i].bit) & words[0])
                    xsave.flags |= x86_xsave_instruct_table[i].code;
            }
            // SAVE area size in bytes  = 0x%08x (%u)
            xsave.area_size = words[1];
            // IA32_XSS lower 32 bits valid bit field mask
            // IA32_XSS upper 32 bits valid bit field mask
            xsave.xss_mask = words[2] | (words[3] << 32);

            uint64  valid_xss = ((uint64)words[3] << 32) | words[2];
            uint64  valid_tries = valid_xcr0 | valid_xss;
            for (uint _try = 2; _try < 63; ++_try)
            {
               if (valid_tries & (1ull << _try))
               {
                   cpuid (reg, words, _try);

                   xsave.feature[_try].size = words[0];
                   xsave.feature[_try].offset = words[1];
                   xsave.feature[_try].is_xss = words[2] & 1;
                   xsave.feature[_try].is_align64byte = words[2] & 2;
               }
            }
        }
        else if (reg == 0xf) // Quality of Service Monitoring Resource Type (0xf/0)
        {
          qos.RMID = words[1];
          qos.has_L3Qos = words[3] & 2;
           if ((words[3] >> 1) & 3)// L3 Cache Quality of Service Monitoring (0xf/1)
           {
               cpuid(reg, words, 1);
               qos.L3Qos.factor = words[1];
               qos.L3Qos.RMID = words[2];
               qos.L3Qos.has_Occupancy = words[3] & 1;
               qos.L3Qos.has_TotalBandwidth = words[3] & 2;
               qos.L3Qos.has_LocalBandwidth = words[3] & 4;
           }
        }
        else if (reg == 0x10) // Resource Director Technology Allocation (0x10/0)
        {
           uint  mask = words[1];
           allocate.has_L3Allocate = words[1] & 2;
           allocate.has_L2Allocate = words[1] & 4;
           allocate.has_BandwidthAllocate = words[1] & 8;

           for (uint _try = 1; _try < 32; _try++)
           {
              if (mask & (1 << _try))
              {
                 cpuid(reg, words, _try);

                 cpu_DirectorAllocate::CacheAllocate * cache =
                         _try == 1 ? &allocate.l3_cache :
                         _try == 2 ? &allocate.l2_cache : 0;
                 if (cache)
                 {
                     cache->mask_length = words[0] & 0x0f;
                     cache->bit_granular = words[1];
                     cache->cos_update = words[2] & 2;
                     cache->has_prioritization = words[2] & 4;
                     cache->cos_number = words[3] & 0xffff;
                 }
                 else if (_try == 3)
                 {
                     allocate.bandwidth.max_throttling = words[0] & 0x0fff;
                     allocate.bandwidth.has_LinearDelay = words[2] & 4;
                     allocate.bandwidth.cos_number = words[3] & 0xffff;
                 }
              }
           }
        }
        else if (reg == 0x12) // Software Guard Extensions (SGX) capability (0x12/0)
        {
           uint  mask = words[0];

           sgx.has_SGX1 = words[0] & 1;
           sgx.has_SGX2 = words[0] & 2;
           sgx.has_SGX_ENCLV = words[0] & (1 << 5);
           sgx.has_SGX_ENCLS = words[0] & (1 << 6);
           sgx.has_EXINFO = words[1] & 1;
           sgx.has_CPINFO = words[1] & 2;
           sgx.max_enclave64not = words[2] & 0xff;
           sgx.max_enclave64 = (words[2] >> 8) & 0xff;

           for (uint _try = 1; _try < 33; _try++)
           {
              if (mask & (1 << (_try-1)))
              {
                 cpuid(reg, words, _try);

                 if (_try == 1)
                 {
                     sgx.attribute.mask[0] = words[0];
                     sgx.attribute.mask[1] = words[1];
                     sgx.attribute.mask[2] = words[2];
                     sgx.attribute.mask[3] = words[3];
                 }
                 else if (words[0] & 0xf)
                 {
                     sgx.enumeration[_try-2].phy_address[0] = words[0] & 0xfffff000;
                     sgx.enumeration[_try-2].phy_address[1] = words[1];
                     sgx.enumeration[_try-2].size[0] = words[2] & 0xfffff000;
                     sgx.enumeration[_try-2].size[1] = words[3] ;
                     sgx.enumeration[_try-2].props = (words[2] & 0xf); // section property
                     if ((words[2] & 0xf) == 0) // enumerated as 0
                         ;
                     else if ((words[2] & 0xf) == 1) // confidentiality & integrity protection
                         ;
                 }
              }
           }
        }
        else if (reg == 0x14) // Intel Processor Trace (0x14)
        {
           uint  _try = 0;
           uint  max_tries = 0;
           for (;;)
           {
              if (_try == 0)
              {
                 intel_trace.is_access_rtit_cr3 = words[1] & 1;
                 intel_trace.has_config_psb = words[1] & 2;
                 intel_trace.has_filter_ip = words[1] & 4;
                 intel_trace.has_mtc_timing = words[1] & 8;
                 intel_trace.has_ptwrite = words[1] & 0x10;
                 intel_trace.has_power_trace = words[1] & 0x20;

                 intel_trace.has_ToPA_output = words[2] & 1;
                 intel_trace.has_ToPA_entrie = words[2] & 2;
                 intel_trace.has_single_range = words[2] & 4;
                 intel_trace.ts_output_trace = words[2] & 8;
                 intel_trace.have_ip_lip_cs = words[2] & 0x80000000;
              }
              else if (_try == 1)
              {
                 intel_trace.address_range = words[0] & 0x7;
                 intel_trace.mtc_bitmask = (words[0] >> 16)& 0xffff;

                 intel_trace.cycle_bitmask = words[1] & 0xff;
                 intel_trace.psb_bitmask = (words[1] >> 16)& 0xffff;
              }

              if (_try == 0)
                 max_tries = words[0];
              if (++_try > max_tries)
                  break;
              cpuid(reg, words, _try);
           }
        }
        else if (reg == 0x15) // Time Stamp Counter/Core Crystal Clock Information (0x15)
        {
           tsc.timestamp = words[1];
           tsc.clock_ratio = words[0];
           tsc.core_clock = words[2];
        }
        else if (reg == 0x16) // Processor Frequency Information (0x16)
        {
            frequency.base = words[0] & 0xffff;
            frequency.max =  words[1] & 0xffff;
            frequency.bus =  words[2] & 0xffff;
        }
        else if (reg == 0x17) // System-On-Chip Vendor Attribute (0x17/0)
        {
           uint  _try = 0;
           uint  max_tries;
           for (;;)
           {
               if (_try == 0)
               {
                   vendor_att.id = words[1] & 0xffff;
                   vendor_att.is_industry = (words[1]>>16) & 1;
                   vendor_att.project = words[2];
                   vendor_att.stepping = words[3];
               }
               else if (_try == 1)
                   memcpy(&vendor_att.soc_brand[0], words, sizeof(words));
               else if (_try == 2)
                   memcpy(&vendor_att.soc_brand[16], words, sizeof(words));
               else if (_try == 3)
                   memcpy(&vendor_att.soc_brand[32], words, sizeof(words));

              if (_try == 0)
                 max_tries = words[0];

              if (++_try > max_tries)
                  break;
              cpuid(reg, words, _try);
           }
        }
        else if (reg == 0x18) // Deterministic Address Translation Parameters (0x18/%d)
        {
           uint  _try = 0;
           uint  max_tries = 0;
           for (;;)
           {
               if (_try == 0)
               {
                   datp.has_page_4KB = words[1] & 1;
                   datp.has_page_2MB = words[1] & 2;
                   datp.has_page_4MB = words[1] & 4;
                   datp.has_page_1GB = words[1] & 8;
                   datp.partition = (words[1] >> 8) & 0x07;
                   datp.way_asso = (words[1] >> 16) & 0xffff;
                   datp.sets = words[2];

                   datp.translation_type = words[3] & 0x1f;
                   datp.translation_level = (words[3] >> 5) & 0x07;
                   datp.has_fullyassoc = (words[3] & (1 << 8));
                   datp.max_ids = words[3] >> 14 & 0x0fff;
                   datp.next = 0;
               }
               else
               {
                   cpu_DATP *cdp = kyNew(cpu_DATP());
                   cdp->has_page_4KB = words[1] & 1;
                   cdp->has_page_2MB = words[1] & 2;
                   cdp->has_page_4MB = words[1] & 4;
                   cdp->has_page_1GB = words[1] & 8;
                   cdp->partition = (words[1] >> 8) & 0x07;
                   cdp->way_asso = (words[1] >> 16) & 0xffff;
                   cdp->sets = words[2];

                   cdp->translation_type = words[3] & 0x1f;
                   cdp->translation_level = (words[3] >> 5) & 0x07;
                   cdp->has_fullyassoc = (words[3] & (1 << 8));
                   cdp->max_ids = words[3] >> 14 & 0x0fff;
                   cdp->next = 0;

                   cpu_DATP *last_dp = datp.next;
                   datp.next = cdp;
                   cdp->next = last_dp;
               }

              if (_try == 0)
                 max_tries = words[0];
              if (++_try > max_tries) break;
              cpuid(reg, words, _try);
           }
        }
        else if (reg == 0x1a) // Hybrid Information (0x1a/0)
        {
            hybrid.model_id = words[0] & 0x00ffffff;
            hybrid.type = (words[0] >> 24) & 0xff;
        }
        else if (reg == 0x1b) // PCONFIG information (0x1b/n)
        {
            pconfig.subleaf_type = words[0] & 0x0fff;
            pconfig.identifier[0] = words[1];
            pconfig.identifier[1] = words[2];
            pconfig.identifier[2] = words[3];
        }
        else if (reg == 0x1f) // V2 extended topology (0x1f)
        {
            saw_1f = true;
            if (_try < kyArraySizeOf(val_1f_eax))
                val_1f_eax[_try] = words[0];
            if (_try < kyArraySizeOf(val_1f_ebx))
                val_1f_ebx[_try] = words[1];
            if (_try < kyArraySizeOf(val_1f_ecx))
                val_1f_ecx[_try] = words[2];

            if (_try == 0) // x2APIC ID of logical processor = 0x%x
                x2apic_v2.apic_id = words[3];

            //fprintf(stderr,"      --- level %d ---\n", try);
            x2apic_v2.level[_try].number = words[2] & 0xff;
            x2apic_v2.level[_try].type = (ex2APICLevelTypes)(words[2] >> 8)& 0xff;
            x2apic_v2.level[_try].bit_width = words[0] & 0x0f;
            x2apic_v2.level[_try].logical_processors = words[1] & 0xff;
        }

        else if (reg == 0x80860003)
            ky_memory::copy(&transmeta_info[0], words, sizeof(words));
        else if (reg == 0x80860004)
            ky_memory::copy(&transmeta_info[16], words, sizeof(words));
        else if (reg == 0x80860005)
            ky_memory::copy(&transmeta_info[32], words, sizeof(words));
        else if (reg == 0x80860006)
            ky_memory::copy(&transmeta_info[48], words, sizeof(words));
    }

    // 0x40000000 暂时不需要
    if (val_1_ecx & 0x80000000){}

    // 0x20000000 暂时不需要
    max_extension = 0x20000000;
    for (uint reg = 0x20000000; reg <= max_extension; ++reg)
    {
        x86_reg words[4] = {0, 0, 0, 0};
        cpuid(reg, words);

       if (reg == 0x20000000)
          max_extension = words[0];
       //
    }

    // 0x80000000
    max_extension = 0x80000000;
    for (uint reg = 0x80000000; reg <= max_extension; ++reg)
    {
       x86_reg words[4] = {0, 0, 0, 0};
       cpuid(reg, words);

       if (reg == 0x80000000)
          max_extension = words[0];
       else if (reg == 0x80000001)
       {
           if ((vendor == CPU_AMD) || (vendor == CPU_VIA) ||
                   (vendor == CPU_Transmeta) || (vendor == CPU_Hygon))
           {
               version = query_version(words[0]);
               // 物理信息获取
               synth_phys = query_phys(this, words[0], words[1]);
           }

           if (vendor == CPU_Intel)
           {
               if (words[3] & (1 << 11))
                   feature |= CPU_SYSCALL;
               if (words[3] & (1 << 20))
                   feature |= CPU_ExecDisable;
               if (words[3] & (1 << 26))
                   feature |= CPU_1GBLPS;
               if (words[3] & (1 << 27))
                   feature |= CPU_RDTSCP;
               if (words[3] & (1 << 29))
                   feature |= CPU_64BitE;

               if (words[2] & 0x01)
                   feature |= CPU_LAHF64;
               if (words[2] & (1 << 5))
                   feature |= CPU_LZCNT;
               if (words[2] & (1 << 8))
                   feature |= CPU_3DNowPREFETCH;
           }
           else if (vendor == CPU_AMD) // extended processor signature (0x80000001/eax)
           {
               // 品牌信息
               synth_phys.brand = 0;
               for (uint i = 0; i < kyArraySizeOf(x86_amd_brand); ++i)
               {
                   bool and_equel = (words[0] & x86_amd_brand[i].ands) == x86_amd_brand[i].equel;
                   if (!and_equel)
                       continue;

                   synth_phys.brand = x86_amd_brand[i].str;
                   break;
               }

               if (synth_phys.brand == 0)
                   synth_phys.brand = x86_vendor[CPU_Unknown].str;

               for (uint i = 0; i < kyArraySizeOf(x86_amd_feature_edx); ++i)
               {
                   if ((1 << x86_amd_feature_edx[i].bit) & words[3])
                       feature |= x86_amd_feature_edx[i].code;
               }

               brand_ext = query_81ebx_amd(words[1], val_1_eax);

               for (uint i = 0; i < kyArraySizeOf(x86_feature_amd_ecx); ++i)
               {
                   if ((1 << x86_feature_amd_ecx[i].bit) & words[2])
                       feature |= x86_feature_amd_ecx[i].code;
               }
           }
           else if (vendor == CPU_Hygon)
           {
               brand_ext = query_81ebx_amd(words[1], val_1_eax);
               for (uint i = 0; i < kyArraySizeOf(x86_feature_amd_ecx); ++i)
               {
                   if ((1 << x86_feature_amd_ecx[i].bit) & words[2])
                       feature |= x86_feature_amd_ecx[i].code;
               }
           }
           else if (vendor == CPU_VIA) // extended processor signature (0x80000001/eax)
           {
               // 品牌信息
               synth_phys.brand = 0;
               for (uint i = 0; i < kyArraySizeOf(x86_via_brand); ++i)
               {
                   bool and_equel = (words[0] & x86_via_brand[i].ands) == x86_via_brand[i].equel;
                   if (!and_equel)
                       continue;

                   synth_phys.brand = x86_via_brand[i].str;
                   break;
               }
               if (synth_phys.brand == 0)
                   synth_phys.brand = x86_vendor[CPU_Unknown].str;
           }
           else if (vendor == CPU_Transmeta) // extended processor signature (0x80000001/eax)
           {
               // 品牌信息
               synth_phys.brand = 0;
               for (uint i = 0; i < kyArraySizeOf(x86_transmeta_brand); ++i)
               {
                   bool and_equel = (words[0] & x86_transmeta_brand[i].ands) == x86_transmeta_brand[i].equel;
                   if (!and_equel || (x86_transmeta_brand[i].is_claim && !x86_transmeta_brand[i].is_claim(this)))
                       continue;

                   synth_phys.brand = x86_transmeta_brand[i].str;
                   break;
               }
               if (synth_phys.brand == 0)
                   synth_phys.brand = x86_vendor[CPU_Unknown].str;

               for (uint i = 0; i < kyArraySizeOf(x86_transmeta_feature_edx); ++i)
               {
                   if ((1 << x86_transmeta_feature_edx[i].bit) & words[3])
                       feature |= x86_transmeta_feature_edx[i].code;
               }
           }
           else if (vendor == CPU_Cyrix)
           {
               for (uint i = 0; i < kyArraySizeOf(x86_cyrix_feature_edx); ++i)
               {
                   if ((1 << x86_cyrix_feature_edx[i].bit) & words[3])
                       feature |= x86_cyrix_feature_edx[i].code;
               }
           }
           else if (vendor == CPU_NSC)
           {
               for (uint i = 0; i < kyArraySizeOf(x86_nsc_feature_edx); ++i)
               {
                   if ((1 << x86_nsc_feature_edx[i].bit) & words[3])
                       feature |= x86_nsc_feature_edx[i].code;
               }
           }

           val_80000001_eax = words[0];
           val_80000001_ebx = words[1];
           val_80000001_ecx = words[2];
           val_80000001_edx = words[3];
       }
       else if (reg == 0x80000002)
           ky_memory::copy(&brand[0], words, sizeof(words));
       else if (reg == 0x80000003)
           ky_memory::copy(&brand[16], words, sizeof(words));
       else if (reg == 0x80000004)
           ky_memory::copy(&brand[32], words, sizeof(words));
       else if (reg == 0x80000005)
       {
           // L1 TLB/cache information: 2M/4M pages & L1 TLB (0x80000005/eax)
           uint entries = words[0] & 0xff;
           uint associate = (words[0] >> 8) & 0xff;
           if (entries != 0 && associate != 0)
           {
               arch_cache ac = {0x00, 2*kyMiB, associate, entries,
                                ky_cpu::cache_t::L1 | ky_cpu::cache_t::TLB | ky_cpu::cache_t::Instruction,
                                1, 1, 0};
               cache = cache_append(cache, kyNew(ky_cpu::cache_t(&ac))) ;
           }
           entries = (words[0] >> 16) & 0xff;
           associate = (words[0] >> 24) & 0xff;
           if (entries != 0 && associate != 0)
           {
               arch_cache ac = {0x00, 2*kyMiB, associate, entries,
                                ky_cpu::cache_t::L1 | ky_cpu::cache_t::TLB | ky_cpu::cache_t::Data,
                                1, 1, 0};
               cache = cache_append(cache, kyNew(ky_cpu::cache_t(&ac))) ;
           }

           //  L1 TLB/cache information: 4K pages & L1 TLB (0x80000005/ebx
           entries = (words[1] >> 0) & 0xff;
           associate = (words[1] >> 8) & 0xff;
           if (entries != 0 && associate != 0)
           {
               arch_cache ac = {0x00, 4*kyKiB, associate, entries,
                                ky_cpu::cache_t::L1 | ky_cpu::cache_t::TLB | ky_cpu::cache_t::Instruction,
                                1, 1, 0};
               cache = cache_append(cache, kyNew(ky_cpu::cache_t(&ac))) ;
           }
           entries = (words[1] >> 16) & 0xff;
           associate = (words[1] >> 24) & 0xff;
           if (entries != 0 && associate != 0)
           {
               arch_cache ac = {0x00, 4*kyKiB, associate, entries,
                                ky_cpu::cache_t::L1 | ky_cpu::cache_t::TLB | ky_cpu::cache_t::Data,
                                1, 1, 0};
               cache = cache_append(cache, kyNew(ky_cpu::cache_t(&ac))) ;
           }

           // L1 data cache information (0x80000005/ecx)
           uint lineSize  = words[2] & 0xff;
           uint linePreTag = (words[2] >> 8) & 0xff;
           associate = (words[2] >> 16) & 0xff;
           uint totalSize = (words[2] >> 24) & 0xff * kyKiB;
           if(lineSize != 0 && associate != 0 && totalSize != 0)
           {
               arch_cache ac = {0x00, totalSize, associate, totalSize / lineSize,
                                ky_cpu::cache_t::L1 | ky_cpu::cache_t::Data,
                                1, 1, 0};
               cache = cache_append(cache, kyNew(ky_cpu::cache_t(&ac))) ;
           }

           // L1 instruction cache information (0x80000005/edx)
           lineSize  = words[3] & 0xff;
           linePreTag = (words[3] >> 8) & 0xff;
           associate = (words[3] >> 16) & 0xff;
           totalSize = (words[3] >> 24) & 0xff * kyKiB;
           if(lineSize != 0 && associate != 0 && totalSize != 0)
           {
               arch_cache ac = {0x00, totalSize, associate, totalSize / lineSize,
                                ky_cpu::cache_t::L1 | ky_cpu::cache_t::Instruction,
                                1, 1, 0};
               cache = cache_append(cache, kyNew(ky_cpu::cache_t(&ac))) ;
           }
       }
       else if (reg == 0x80000006)
       {
           // L2 TLB/cache information: 2M/4M pages & L2 TLB (0x80000006/eax)
           if(((words[0] >> 16) & 0xffff) != 0)
           {
               uint entries = words[0] & 0x0fff;
               uint associnx = (words[0] >> 12) & 0x0f;
               if (entries != 0 && associnx != 0)
               {
                   arch_cache ac = {0x00, 2*kyMiB, assoc_table[associnx].associate, entries,
                                    ky_cpu::cache_t::L2 | ky_cpu::cache_t::TLB | ky_cpu::cache_t::Instruction,
                                    1, 1, 0};
                   cache = cache_append(cache, kyNew(ky_cpu::cache_t(&ac))) ;
               }

               entries = (words[0] >> 16) & 0x0fff;
               associnx = (words[0] >> 28) & 0x0f;
               if (entries != 0 && associnx != 0)
               {
                   arch_cache ac = {0x00, 2*kyMiB, assoc_table[associnx].associate, entries,
                                    ky_cpu::cache_t::L2 | ky_cpu::cache_t::TLB | ky_cpu::cache_t::Data,
                                    1, 1, 0};
                   cache = cache_append(cache, kyNew(ky_cpu::cache_t(&ac))) ;
               }
           }
           else
           {
               uint entries = words[0] & 0x0fff;
               uint associnx = (words[0] >> 12) & 0x0f;
               if (entries != 0 && associnx != 0)
               {
                   arch_cache ac = {0x00, 2*kyMiB, assoc_table[associnx].associate, entries,
                                    ky_cpu::cache_t::L2 | ky_cpu::cache_t::TLB | ky_cpu::cache_t::Unified,
                                    1, 1, 0};
                   cache = cache_append(cache, kyNew(ky_cpu::cache_t(&ac))) ;
               }
           }

           // L2 TLB/cache information: 4K pages & L2 TLB (0x80000006/ebx)
           if(((words[1] >> 16) & 0xffff) != 0)
           {
               uint entries = words[1] & 0x0fff;
               uint associnx = (words[1] >> 12) & 0x0f;
               if (entries != 0 && associnx != 0)
               {
                   arch_cache ac = {0x00, 4*kyKiB, assoc_table[associnx].associate, entries,
                                    ky_cpu::cache_t::L2 | ky_cpu::cache_t::TLB | ky_cpu::cache_t::Instruction,
                                    1, 1, 0};
                   cache = cache_append(cache, kyNew(ky_cpu::cache_t(&ac))) ;
               }

               entries = (words[1] >> 16) & 0x0fff;
               associnx = (words[1] >> 28) & 0x0f;
               if (entries != 0 && associnx != 0)
               {
                   arch_cache ac = {0x00, 4*kyKiB, assoc_table[associnx].associate, entries,
                                    ky_cpu::cache_t::L2 | ky_cpu::cache_t::TLB | ky_cpu::cache_t::Data,
                                    1, 1, 0};
                   cache = cache_append(cache, kyNew(ky_cpu::cache_t(&ac))) ;
               }
           }
           else
           {
               uint entries = words[1] & 0x0fff;
               uint associnx = (words[1] >> 12) & 0x0f;
               if (entries != 0 && associnx != 0)
               {
                   arch_cache ac = {0x00, 4*kyKiB, assoc_table[associnx].associate, entries,
                                    ky_cpu::cache_t::L2 | ky_cpu::cache_t::TLB | ky_cpu::cache_t::Unified,
                                    1, 1, 0};
                   cache = cache_append(cache, kyNew(ky_cpu::cache_t(&ac))) ;
               }
           }

           //  L2 unified cache information (0x80000006/ecx)
           uint lineSize     = words[2] & 0xff;
           uint assoc_idx    = (words[2] >> 12) & 0x0f;
           uint totalSize    = ((words[2] >> 16) & 0xffff)* kyKiB;
           if(lineSize != 0 && assoc_idx != 0 && totalSize != 0)
           {
               arch_cache ac = {0x00, totalSize, assoc_table[assoc_idx], totalSize / lineSize,
                                ky_cpu::cache_t::L2 | ky_cpu::cache_t::Unified,
                                1, 1, 0};
               cache = cache_append(cache, kyNew(ky_cpu::cache_t(&ac))) ;
           }
           if (((words[2] >> 12) & 0xf) == 4 && (words[2] >> 16) == 256)\
               L2_4w_256K = 1;
           else if (((words[2] >> 12) & 0xf) == 4 && (words[2] >> 16) == 512)
               L2_4w_512K = 1;

           // L3 cache information (0x80000006/edx)
           lineSize  = words[3] & 0xff;
           uint linePerTag = (words[3] >> 8) & 0x0f;
           assoc_idx = (words[3] >> 12) & 0x0f;
           totalSize = ((words[3] >> 18) & 0x3fff)* 512 * kyKiB;
           if(lineSize != 0 && assoc_idx != 0 && totalSize != 0)
           {
               arch_cache ac = {0x00, totalSize, assoc_table[assoc_idx], totalSize / lineSize,
                                ky_cpu::cache_t::L3 | ky_cpu::cache_t::Unified,
                                1, 1, 0};
               cache = cache_append(cache, kyNew(ky_cpu::cache_t(&ac))) ;
           }
       }
       else if (reg == 0x80000007)
       {
           static const struct
           {
               const char *const str;
               const eCPUAdvanceds code;
               const uint        bit;
           }
           x86_ras_table[] =
           {
           {"MCA overflow recovery support", RAS_MCAOR, 0},
           {"SUCCOR support",                RAS_SUCCOR, 1},
           {"hardware assert support",       RAS_HWA, 1},
           {"scalable MCA support",          RAS_SMCA, 1},
           };
           static const struct
           {
               const char *const str;
               const eCPUAdvanceds code;
               const uint        bit;
           }
           x86_adv_power_table[] =
           {
           { "TS: temperature sensing diode"           , APM_TS,  0},
           { "FID: frequency ID control"               , APM_FID,  1},
           { "VID: voltage ID control"                 , APM_VID,  2},
           { "TTP: thermal trip"                       , APM_TTP,  3},
           { "TM: thermal monitor"                     , APM_TM,  4},
           { "STC: software thermal control"           , APM_STC,  5},
           { "100 MHz multiplier control"              , APM_100MhzMulti,  6},
           { "hardware P-State control"                , APM_HPS,  7},
           { "TscInvariant"                            , APM_TSCI,  8},
           { "CPB: core performance boost"             , APM_CPB,  9},
           { "read-only effective frequency interface" , APM_ROEFI, 10},
           { "processor feedback interface"            , APM_PFBI, 11},
           { "APM power reporting"                     , APM_PR, 12},
           { "connected standby"                       , APM_CS, 13},
           { "RAPL: running average power limit"       , APM_RAPL, 14},
           };

           // RAS Capability (0x80000007/ebx)
           for (uint i = 0; i < kyArraySizeOf(x86_ras_table); ++i)
           {
               if ((1 << x86_ras_table[i].bit) & words[1])
                   advanced.flags |= x86_ras_table[i].code;
           }
           // Advanced Power Management Features (0x80000007/ecx)
           advanced.CUPSTR = words[2];
           // Advanced Power Management Features (0x80000007/edx)
           for (uint i = 0; i < kyArraySizeOf(x86_adv_power_table); ++i)
           {
               if ((1 << x86_adv_power_table[i].bit) & words[3])
                   advanced.flags |= x86_adv_power_table[i].code;
           }

       }
       else if (reg == 0x80000008)
       {
           val_80000008_ecx = words[2];

           phy_addr.max_phy_bit = words[0] & 0xff;
           phy_addr.max_linear_bit = (words[0] >> 8) & 0xff;
           phy_addr.max_guest_bit = (words[0] >> 16) & 0xff;

           // Extended Feature Extensions ID (0x80000008/ebx)
           for (uint i = 0; i < kyArraySizeOf(x86_extid_feature_table); ++i)
           {
               if ((1 << x86_extid_feature_table[i].bit) & words[1])
                   feature |= x86_extid_feature_table[i].code;
           }

           // Size Identifiers (0x80000008/ecx)
           if (version.family_synth > 0x16)
           {
               phy_addr.threads = words[2] & 0xff;
           }
           else
           {
               phy_addr.cores = words[2] & 0xff;
           }

           phy_addr.acid_size = (words[2] >> 12) & 0x0f;
           phy_addr.ptsc_size = (words[2] >> 16) & 0x03;
           phy_addr.RDPRU_maxin = (words[3] >> 16) & 0xff;
       }
       else if (reg == 0x8000000a)
       {
           // SVM Secure Virtual Machine
           svm.revision = words[0] & 0xff;
           for (uint i = 0; i < kyArraySizeOf(x86_svm_table); ++i)
           {
               if ((1 << x86_svm_table[i].bit) & words[3])
                   svm.flags |= x86_svm_table[i].code;
           }
           svm.NASID = words[1];
       }
       else if (reg == 0x80000019)
       {
           // L1 TLB information: 1G pages (0x80000019/eax)
           uint entries = words[0] & 0xfff;
           uint associnx = (words[0] >> 12) & 0x0f;
           if (entries != 0 && associnx != 0)
           {
               arch_cache ac = {0x00, 1*kyGiB, assoc_table[associnx].associate, entries,
                                ky_cpu::cache_t::L1 | ky_cpu::cache_t::TLB | ky_cpu::cache_t::Instruction,
                                1, 1, 0};
               cache = cache_append(cache, kyNew(ky_cpu::cache_t(&ac))) ;
           }
           entries = (words[0] >> 16) & 0xfff;
           associnx = (words[0] >> 28) & 0x0f;
           if (entries != 0 && associnx != 0)
           {
               arch_cache ac = {0x00, 1*kyGiB, assoc_table[associnx].associate, entries,
                                ky_cpu::cache_t::L1 | ky_cpu::cache_t::TLB | ky_cpu::cache_t::Data,
                                1, 1, 0};
               cache = cache_append(cache, kyNew(ky_cpu::cache_t(&ac))) ;
           }

           // L2 TLB information: 1G pages (0x80000019/ebx)
           entries = words[1] & 0xfff;
           associnx = (words[1] >> 12) & 0x0f;
           if (entries != 0 && associnx != 0)
           {
               arch_cache ac = {0x00, 1*kyGiB, assoc_table[associnx].associate, entries,
                                ky_cpu::cache_t::L2 | ky_cpu::cache_t::TLB | ky_cpu::cache_t::Instruction,
                                1, 1, 0};
               cache = cache_append(cache, kyNew(ky_cpu::cache_t(&ac))) ;
           }
           entries = (words[1] >> 16) & 0xfff;
           associnx = (words[1] >> 28) & 0x0f;
           if (entries != 0 && associnx != 0)
           {
               arch_cache ac = {0x00, 1*kyGiB, assoc_table[associnx].associate, entries,
                                ky_cpu::cache_t::L2 | ky_cpu::cache_t::TLB | ky_cpu::cache_t::Data,
                                1, 1, 0};
               cache = cache_append(cache, kyNew(ky_cpu::cache_t(&ac))) ;
           }

       }
       else if (reg == 0x8000001a)
       {
           //  SVM Secure Virtual Machine (0x8000001a/eax)
           if (words[0] & 1)
               svm.flags |= SVM_SSE128;
           if (words[0] & 2)
               svm.flags |= SVM_MOVU;
           if (words[0] & 3)
               svm.flags |= SVM_SSE256;
       }
       else if (reg == 0x8000001b)
       {
           static const struct
           {
               const char *const str;
               const eIBSFlags   code;
               const uint        bit;
           }
           x86_ibs_table[]=
           {
               { "IBS feature flags valid"                 , IBS_FFV,  0 },
               { "IBS fetch sampling"                      , IBS_FS,  1 },
               { "IBS execution sampling"                  , IBS_ES,  2 },
               { "read write of op counter"                , IBS_OCM,  3 },
               { "op counting mode"                        , IBS_BTAR,  4 },
               { "branch target address reporting"         , IBS_OCC,  5 },
               { "IbsOpCurCnt and IbsOpMaxCnt extend 7"    , IBS_IRIPI,  6 },
               { "invalid RIP indication support"          , IBS_FBMIS,  7 },
               { "fused branch micro-op indication support", IBS_FCE,  8 },
               { "IBS fetch control extended MSR support"  , IBS_OD,  9 },
               { "IBS op data 4 MSR support"               , IBS_RWOC, 10 },
           };
           // Instruction Based Sampling Identifiers (0x8000001b/eax)
           for (uint i = 0; i < kyArraySizeOf(x86_ibs_table); ++i)
           {
               if ((1 << x86_ibs_table[i].bit) & words[0])
                   ibs_flags |= x86_ibs_table[i].code;
           }
       }
       else if (reg == 0x8000001c)
       {
           static const struct
           {
               const char *const        str;
               const eCPUProfilingFlags code;
               const uint               bit;
           }
           x86_profiling_table[] =
           {
           { "lightweight profiling"                   , CPU_LWP,  0 },
           { "LWPVAL instruction"                      , CPU_LWPVAL,  1 },
           { "instruction retired event"               , CPU_IRE,  2 },
           { "branch retired event"                    , CPU_BRE,  3 },
           { "DC miss event"                           , CPU_DCME,  4 },
           { "core clocks not halted event"            , CPU_CCNHE,  5 },
           { "core reference clocks not halted event"  , CPU_CRCNHE,  6 },
           { "interrupt on threshold overflow"         , CPU_IOTO, 31 },
           };
           // Lightweight Profiling Capabilities: Availability (0x8000001c/eax)
           for (uint i = 0; i < kyArraySizeOf(x86_profiling_table); ++i)
           {
               if ((1 << x86_profiling_table[i].bit) & words[0])
                   profiling.flags |= x86_profiling_table[i].code;
           }
           // Lightweight Profiling Capabilities: Supported (0x8000001c/edx)
           for (uint i = 0; i < kyArraySizeOf(x86_profiling_table); ++i)
           {
               if ((1 << x86_profiling_table[i].bit) & words[3])
                   profiling.flags |= x86_profiling_table[i].code;
           }
           // Lightweight Profiling Capabilities (0x8000001c/ebx)
           profiling.LWPCB_byte = words[1] & 0xff; // "LWPCB byte size"
           profiling.evt_record_byte = (words[1] >> 8) & 0xff; // "event record byte size"
           profiling.max_evtid = (words[1] >> 16) & 0xff; // "maximum EventId"
           profiling.evt_offset = (words[1] >> 24) & 0xff; // "EventInterval1 field offset"

           if ((1 << 5) & words[2]) // "data cache miss address valid"
               profiling.flags |= CPU_DCMAV;
           if ((1 << 28) & words[2]) // "branch prediction filtering"
               profiling.flags |= CPU_BPF;
           if ((1 << 29) & words[2]) // "IP filtering"
               profiling.flags |= CPU_IPFilter;
           if ((1 << 30) & words[2]) // "cache level filtering"
               profiling.flags |= CPU_CacheLevelFilter;
           if ((1 << 31) & words[2]) // "cache latency filtering"
               profiling.flags |= CPU_CacheLatencyFilter;

           // Lightweight Profiling Capabilities (0x8000001c/ecx)
           profiling.latency_cnt_bit = (words[2] >> 0) & 0x0f; // "latency counter bit size"
           profiling.latency_rounded = (words[2] >> 6) & 0x07; // "amount cache latency is rounded"
           profiling.LWP_version = (words[2] >> 9) & 0x7f; // "LWP implementation version"
           profiling.evt_record_buffer = (words[2] >> 16) & 0xff; // "event ring buffer size in records"

       }
       else if (reg == 0x8000001d) // Cache Properties (0x8000001d)
       {
          uint  _try = 0;
          while ((words[0] & 0x1f) != 0)
          {
              //fprintf(stderr,"      --- cache %d ---\n", try);
              ky_bitset<32> bit_eax(words[0]);
              uint16 type     = bit_eax.extract( 0,  4); //  "cache type"
              uint8  level    = bit_eax.extract( 5,  7); //  "cache level"
              bool   is_self  = bit_eax.extract( 8,  8); //  "self-initializing cache level"
              bool   is_fully = bit_eax.extract( 9, 9); //  "fully associative cache"
              uint8  shareBy  = bit_eax.extract(14, 25); //  "extra cores sharing this cache"

              if (type == 0)
                  type = ky_cpu::cache_t::Null; // "no more caches"
              else if (type == 1)
                  type = ky_cpu::cache_t::Data; // "data cache"
              else if (type == 2)
                  type = ky_cpu::cache_t::Instruction; // "instruction cache"
              else if (type == 3)
                  type = ky_cpu::cache_t::Unified; // "unified cache"
              type |= level;
              type |= is_self ? ky_cpu::cache_t::SelfInitial : 0;
              type |= is_fully ? ky_cpu::cache_t::FullyAssociative : 0;

              ky_bitset<32> bit_ebx(words[1]);
              uint  line_size  = bit_ebx.extract( 0, 11)+ 1; // line size in bytes
              uint  parts      = bit_ebx.extract(12, 21)+ 1; // "physical line partitions"
              uint  ways_assoc = bit_ebx.extract(22, 31)+ 1; // "number of ways"

              uint  sets = words[2] +1; // "number of sets"
              uint  size = (ways_assoc * parts * line_size * sets);

              type |= (words[3] & 1) ? ky_cpu::cache_t::WriteBackInvalid : 0; // write-back invalidate
              type |= (words[3] & 2) ? ky_cpu::cache_t::InclusiveLower : 0;

              arch_cache ac = {0x00, size, ways_assoc, line_size, type, shareBy, 1, 0};
              cache = cache_append(cache, kyNew(ky_cpu::cache_t(&ac))) ;

              cpuid(reg, words, ++_try);
          }
       }
       else if (reg == 0x8000001e) // extended APIC
       {
           val_8000001e_ebx = words[1];
           apic_ext.apic_id = words[0];

           if (version.family_synth > 0x16) // Core Identifiers (0x8000001e/ebx)
           {
               apic_ext.core_id = words[1] & 0xff;
               apic_ext.threads = (words[1] >> 8) & 0xff;
           }
           else // Compute Unit Identifiers (0x8000001e/ebx)
           {
               apic_ext.unit_id = words[1] & 0xff;
               apic_ext.units = (words[1] >> 8) & 0x03;
           }
           apic_ext.node_id = words[2] & 0xff;
           apic_ext.node_pp = (words[2] >> 8) & 0x07;
       }
       else if (reg == 0x8000001f)
       {
           amd_secure.has_SME = words[0] & 1;
           amd_secure.has_SEV = words[0] & 2;
           amd_secure.has_VMPF = words[0] & 4;
           amd_secure.has_SEV_ES = words[0] & 8;
           amd_secure.PTE_enc = words[1] & 0x1f;
           amd_secure.phy_ASWR = (words[1] >> 6) & 0x3f;
           amd_secure.SEV_guest = words[2];
           amd_secure.SEV_ASID_min = words[3];
       }
       else if (reg == 0x80000020)
       {
           struct cpu_MemBwPQoS // PQoS Enforcement for Memory Bandwidth
           {
               bool has_MemBw; // memory bandwidth enforcement support
               uint32 bitmask_length; // capacity bitmask length = 0x%0llx
               uint32 nocos; // number of classes of service = 0x%0x
           }membw_PQoS;
          uint  _try = 0;
          while (words[0] != 0 || words[1] != 0 ||
                 words[2] != 0 || words[3] != 0)
          {
              // PQoS Enforcement for Memory Bandwidth (0x80000020)
              if (_try == 0)
              {
                  membw_PQoS.has_MemBw = words[1] & 2;
              }
              else if (_try == 1)
              {
                  membw_PQoS.bitmask_length = words[0]+1;
                  membw_PQoS.nocos = words[3];
              }

              cpuid(reg, words, ++_try);
          }
       }
    }

}

#endif
