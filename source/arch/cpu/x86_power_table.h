
#ifndef X86_POWER_TABLE_H
#define X86_POWER_TABLE_H

static const struct
{
    const char *const    str;
    const ePowerFeatures code;
    const uint           bit;
}
x86_power_eax_table[] =
{
{ "digital thermometer"                     , CPU_Thermonmeter,  0},
{ "Intel Turbo Boost Technology"            , CPU_TurboBoost,  1},
{ "ARAT always running APIC timer"          , CPU_ARAT,  2},
{ "PLN power limit notification"            , CPU_PLN,  4},
{ "ECMD extended clock modulation duty"     , CPU_ECMD,  5},
{ "PTM package thermal management"          , CPU_PTM,  6},
{ "HWP base registers"                      , CPU_HWPBR,  7},
{ "HWP notification"                        , CPU_HWPN,  8},
{ "HWP activity window"                     , CPU_HWPAW,  9},
{ "HWP energy performance preference"       , CPU_HWPEPP, 10},
{ "HWP package level request"               , CPU_HWPPLR, 11},
{ "HDC base registers"                      , CPU_HDCBR, 13},
{ "Intel Turbo Boost Max Technology 3.0"    , CPU_TurboBoostMax3, 14},
{ "HWP capabilities"                        , CPU_HWPCAP, 15},
{ "HWP PECI override"                       , CPU_HWPPECI, 16},
{ "flexible HWP"                            , CPU_HWPF, 17},
{ "IA32_HWP_REQUEST MSR fast access mode"   , CPU_HWP_REQUEST, 18},
{ "HW_FEEDBACK"                             , CPU_HW_FEEDBACK, 19},
{ "ignoring idle logical processor HWP req" , CPU_IILPHWPR, 20}
};
static const struct
{
    const char *const    str;
    const ePowerFeatures code;
    const uint           bit;
}
x86_power_ecx_table[] =
{
{ "hardware coordination feedback"          , CPU_hWC_FEEDBACK,  0},
{ "ACNT2 available"                         , CPU_ACNT2,  1},
{ "performance-energy bias capability"      , CPU_PEBC,  3},
};
static const struct
{
    const char *const    str;
    const ePowerFeatures code;
    const uint           bit;
}
x86_power_edx_table[] =
{
{ "performance capability reporting"        , CPU_PCR,  0},
{ "energy efficiency capability reporting"  , CPU_EECR,  1},
};

static const struct
{
    const char *const str;
    const eXSAVEFlags code;
    const uint        bit;
}
x86_xsave_xcr0_table[] =
{
{ "   XCR0 supported: x87 state"            , XCR0_x87,  0},
{ "   XCR0 supported: SSE state"            , XCR0_SSE,  1},
{ "   XCR0 supported: AVX state"            , XCR0_AVX,  2},
{ "   XCR0 supported: MPX BNDREGS"          , XCR0_MPX_BNDREGS,  3},
{ "   XCR0 supported: MPX BNDCSR"           , XCR0_MPX_BNDCSR,  4},
{ "   XCR0 supported: AVX-512 opmask"       , XCR0_AVX512_Opmask,  5},
{ "   XCR0 supported: AVX-512 ZMM_Hi256"    , XCR0_AVX512_ZMM_Hi256,  6},
{ "   XCR0 supported: AVX-512 Hi16_ZMM"     , XCR0_AVX512_Hi16_ZMM,  7},
{ "   IA32_XSS supported: PT state"         , XSS_PT,  8},
{ "   XCR0 supported: PKRU state"           , XCR0_PKRU,  9},
{ "   XCR0 supported: CET_U state"          , XCR0_CET_U, 11},
{ "   XCR0 supported: CET_S state"          , XCR0_CET_S, 12},
{ "   IA32_XSS supported: HDC state"        , XSS_CET_HDC, 13}
};
static const struct
{
    const char *const str;
    const eXSAVEFlags code;
    const uint        bit;
}
x86_xsave_instruct_table[] =
{
{ "XSAVEOPT instruction"                    ,  XSAVEOPT,  0},
{ "XSAVEC instruction"                      ,  XSAVEC,  1},
{ "XGETBV instruction"                      ,  XGETBV,  2},
{ "XSAVES/XRSTORS instructions"             ,  XSAVES,  3}
};
#endif
