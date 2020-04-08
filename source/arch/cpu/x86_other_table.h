
#ifndef X86_OTHER_TABLE_H
#define X86_OTHER_TABLE_H


/*
** VIA major query
*/
#define v7 (via_is_claim_v7)
#define vM (via_is_claim_vM)
#define vD (via_is_claim_vD)
#define vE (via_is_claim_vE)
#define vZ (via_is_claim_vZ)
static const struct
{
    unsigned int      ands;
    unsigned int      equel;
    bool            (*is_claim)(const struct cpu_ability *ability);
    const char *const str;
}
x86_via_name[] =
{
    _IT_FM  (0, 5,  0, 4,         "VIA WinChip (C6)"),
    _IT_FM  (0, 5,  0, 8,         "VIA WinChip 2 (C6-2)"),
    _IT_FM  (0, 6,  0, 6,         "VIA C3 (Samuel C5A)"),
    _IT_FMS (0, 6,  0, 7,  0,     "VIA C3 (Samuel 2 C5B) / Eden ESP 4000/5000/6000, .15um"),
    _IT_FMS (0, 6,  0, 7,  1,     "VIA C3 (Samuel 2 C5B) / Eden ESP 4000/5000/6000, .15um"),
    _IT_FMS (0, 6,  0, 7,  2,     "VIA C3 (Samuel 2 C5B) / Eden ESP 4000/5000/6000, .15um"),
    _IT_FMS (0, 6,  0, 7,  3,     "VIA C3 (Samuel 2 C5B) / Eden ESP 4000/5000/6000, .15um"),
    _IT_FMS (0, 6,  0, 7,  4,     "VIA C3 (Samuel 2 C5B) / Eden ESP 4000/5000/6000, .15um"),
    _IT_FMS (0, 6,  0, 7,  5,     "VIA C3 (Samuel 2 C5B) / Eden ESP 4000/5000/6000, .15um"),
    _IT_FMS (0, 6,  0, 7,  6,     "VIA C3 (Samuel 2 C5B) / Eden ESP 4000/5000/6000, .15um"),
    _IT_FMS (0, 6,  0, 7,  7,     "VIA C3 (Samuel 2 C5B) / Eden ESP 4000/5000/6000, .15um"),
    _IT_FM  (0, 6,  0, 7,         "VIA C3 (Ezra C5C), .13um"),
    _IT_FM  (0, 6,  0, 8,         "VIA C3 (Ezra-T C5N)"),
    _IT_FMS (0, 6,  0, 9,  0,     "VIA C3 / Eden ESP 7000/8000/10000 (Nehemiah C5XL)"),
    _IT_FMS (0, 6,  0, 9,  1,     "VIA C3 / Eden ESP 7000/8000/10000 (Nehemiah C5XL)"),
    _IT_FMS (0, 6,  0, 9,  2,     "VIA C3 / Eden ESP 7000/8000/10000 (Nehemiah C5XL)"),
    _IT_FMS (0, 6,  0, 9,  3,     "VIA C3 / Eden ESP 7000/8000/10000 (Nehemiah C5XL)"),
    _IT_FMS (0, 6,  0, 9,  4,     "VIA C3 / Eden ESP 7000/8000/10000 (Nehemiah C5XL)"),
    _IT_FMS (0, 6,  0, 9,  5,     "VIA C3 / Eden ESP 7000/8000/10000 (Nehemiah C5XL)"),
    _IT_FMS (0, 6,  0, 9,  6,     "VIA C3 / Eden ESP 7000/8000/10000 (Nehemiah C5XL)"),
    _IT_FMS (0, 6,  0, 9,  7,     "VIA C3 / Eden ESP 7000/8000/10000 (Nehemiah C5XL)"),
    _IT_FM  (0, 6,  0, 9,         "VIA C3 / C3-M / Eden-N (Antaur C5P)"),
    // VIA unpublished BIOS Guide for C7-D.
    _IT_FM  (0, 6,  0,10,         "VIA C7 / C7-M / C7-D / Eden (Esther C5J Model A)"),
    // VIA unpublished BIOS Guide for C7-D.
    // Brand string can be used to differentiate model D CPUs.
    _IT_FMQ (0, 6,  0,13,     vM, "VIA C7-M (Esther C5J Model D)"),
    _IT_FMQ (0, 6,  0,13,     vD, "VIA C7-D (Esther C5J Model D)"),
    _IT_FMQ (0, 6,  0,13,     v7, "VIA C7 (Esther C5J Model D)"),
    _IT_FMQ (0, 6,  0,13,     vE, "VIA Eden (Esther C5J Model D)"),
    _IT_FM  (0, 6,  0,13,         "VIA (unknown type) (Esther C5J Model D)"),
    // VIA unpublished BIOS Guide for Nano, Eden (for steppings 3-14, other than
    // Zhaoxin).
    //
    // Steppings 0-2 for Isaiah come from this post by "redray", circa Apr 2013:
    //    https://forum.osdev.org/viewtopic.php?f=1&t=26351
    // It presents an excerpt from "VIA Nano Processor X2X4 BIOS Guide 2.47".
    //
    // Die size depends on core, but it's unclear which cores are which:
    //    Isaiah    = 65nm, 40nm
    //    Isaiah II = 28nm
    //    ZhangJiang (Zhaoxin) = 28nm ?
    _IT_FMS (0, 6,  0,15,  0,     "VIA Nano 1000/2000 (Isaiah CNA A0)"), // redray; instlatx64 example
    _IT_FMS (0, 6,  0,15,  1,     "VIA Nano 1000/2000 (Isaiah CNA A1)"), // redray; model numbers assumed because of bracketing between 0 & 3
    _IT_FMS (0, 6,  0,15,  2,     "VIA Nano 1000/2000 (Isaiah CNA A2)"), // redray; model numbers assumed because of bracketing between 0 & 3
    _IT_FMS (0, 6,  0,15,  3,     "VIA Nano 1000/2000 (Isaiah CNA A3)"),
    _IT_FMS (0, 6,  0,15,  8,     "VIA Nano 3000 (Isaiah CNB A1)"),
    _IT_FMS (0, 6,  0,15, 10,     "VIA Nano 3000 (Isaiah CNC A2)"),
    _IT_FMS (0, 6,  0,15, 12,     "VIA Nano X2 4000 / QuadCore 4000 (Isaiah CNQ A1)"),
    _IT_FMS (0, 6,  0,15, 13,     "VIA Nano X2 4000 / QuadCore 4000 (Isaiah CNQ A2)"),
    _IT_FMSQ(0, 6,  0,15, 14, vZ, "Zhaoxin KaiXian/Kaisheng ZX-C/ZX-C+"),
    _IT_FMS (0, 6,  0,15, 14,     "VIA Eden X4 4000 (Isaiah CNR)"),
    _IT_FM  (0, 6,  0,15,         "VIA Nano / Eden (unknown type) (Isaiah)"),
    _IT_F   (0, 6,                "VIA C3 / C3-M / C7 / C7-M / Eden / Eden ESP 7000/8000/10000 / Nano (unknown model)"),
    _IT_FM  (0, 7,  0,11,         "Zhaoxin KaiXian KX-5000 / Kaisheng KH-20000 (WuDaoKou)"), // geekbench.com example
};
static const struct
{
    const unsigned int  ands;
    const unsigned int  equel;
    const char *const   uarch;        // process-dependent name
    const unsigned int  core_is_uarch;// for some uarches, the core names are based on the
                                      // uarch names, so the uarch name becomes redundant
    //const char *const   family;       // process-neutral name:
                                      //    sometimes independent (e.g. Core)
                                      //    sometimes based on lead uarch (e.g. Nehalem)
    const char *const   phys;         // physical properties: die process, #pins, etc.
}
x86_via_uarch[] =
{
    _IT_F   (0, 5,            0,  "WinChip",    1,    0),
    _IT_FM  (0, 6,  0, 6,     0,  "C3",         1,  ".18um"),
    _IT_FM  (0, 6,  0, 7,     0,  "C3",         1,    0),     // *p depends on core
    _IT_FM  (0, 6,  0, 8,     0,  "C3",         1,  ".13um"),
    _IT_FM  (0, 6,  0, 9,     0,  "C3",         1,  ".13um"),
    _IT_FM  (0, 6,  0,10,     0,  "C7",         0,  "90nm"),
    _IT_FM  (0, 6,  0,13,     0,  "C7",         0,  "90nm"),
    _IT_FMSQ(0, 6,  0,15, 14, vZ, "ZhangJiang", 0,  "28nm"),
    _IT_FM  (0, 6,  0,15,     0,  "C7",         0,    0,),     // *p depends on core
    _IT_FM  (0, 7,  0,11,     0,  "ZhangJiang", 0,  "28nm")
};

static const struct
{
    const unsigned int ands;
    const unsigned int equel;
    const unsigned int ext1;
    const char *const  str;
}
x86_via_brand[] =
{
   _IT_FM (0,6,  0,6,     "VIA C3 (WinChip C5A)"),
   _IT_FM (0,6,  0,6,     "VIA C3 (WinChip C5A)"),
   _IT_FMS(0,6,  0,7,  0, "VIA C3 (WinChip C5B)"),
   _IT_FMS(0,6,  0,7,  1, "VIA C3 (WinChip C5B)"),
   _IT_FMS(0,6,  0,7,  2, "VIA C3 (WinChip C5B)"),
   _IT_FMS(0,6,  0,7,  3, "VIA C3 (WinChip C5B)"),
   _IT_FMS(0,6,  0,7,  4, "VIA C3 (WinChip C5B)"),
   _IT_FMS(0,6,  0,7,  5, "VIA C3 (WinChip C5B)"),
   _IT_FMS(0,6,  0,7,  6, "VIA C3 (WinChip C5B)"),
   _IT_FMS(0,6,  0,7,  7, "VIA C3 (WinChip C5B)"),
   _IT_FM (0,6,  0,7,     "VIA C3 (WinChip C5C)"),
   _IT_FM (0,6,  0,8,     "VIA C3 (WinChip C5N)"),
   _IT_FM (0,6,  0,9,     "VIA C3 (WinChip C5XL)"),
   _IT_F  (0,6,           "VIA C3 (unknown model)")

};

/*
** Transmeta major queries
**
** t2 = TMx200
** t4 = TMx400
** t5 = TMx500
** t6 = TMx600
** t8 = TMx800
*/
/* TODO: Add cases for Transmeta Crusoe TM5700/TM5900 */
/* TODO: Add cases for Transmeta Efficeon */
#define t2 (transmeta_is_claim_t2)
#define t4 (transmeta_is_claim_t4)
#define t5 (transmeta_is_claim_t5)
#define t6 (transmeta_is_claim_t6)
#define t8 (transmeta_is_claim_t8)

static const struct
{
    unsigned int      ands;
    unsigned int      equel;
    bool            (*is_claim)(const struct cpu_ability *ability);
    const char *const str;
}
x86_transmeta_name[] =
{
_IT_FMSQ(0, 5,  0,4,  2, t2, "Transmeta Crusoe TM3200"),
_IT_FMS (0, 5,  0,4,  2,     "Transmeta Crusoe TM3x00 (unknown model)"),
_IT_FMSQ(0, 5,  0,4,  3, t4, "Transmeta Crusoe TM5400"),
_IT_FMSQ(0, 5,  0,4,  3, t5, "Transmeta Crusoe TM5500 / Crusoe SE TM55E"),
_IT_FMSQ(0, 5,  0,4,  3, t6, "Transmeta Crusoe TM5600"),
_IT_FMSQ(0, 5,  0,4,  3, t8, "Transmeta Crusoe TM5800 / Crusoe SE TM58E"),
_IT_FMS (0, 5,  0,4,  3,     "Transmeta Crusoe TM5x00 (unknown model)"),
_IT_FM  (0, 5,  0,4,         "Transmeta Crusoe"),
_IT_F   (0, 5,               "Transmeta Crusoe (unknown model)"),
_IT_FM  (0,15,  0,2,         "Transmeta Efficeon TM8x00"),
_IT_FM  (0,15,  0,3,         "Transmeta Efficeon TM8x00"),
_IT_F   (0,15,               "Transmeta Efficeon (unknown model)"),
};
static const struct
{
    const unsigned int  ands;
    const unsigned int  equel;
    const char *const   uarch;        // process-dependent name
    const unsigned int  core_is_uarch;// for some uarches, the core names are based on the
                                      // uarch names, so the uarch name becomes redundant
    //const char *const   family;       // process-neutral name:
                                      //    sometimes independent (e.g. Core)
                                      //    sometimes based on lead uarch (e.g. Nehalem)
    //const char *const   phys;         // physical properties: die process, #pins, etc.
}
x86_transmeta_uarch[] =
{
    _IT_F   (0, 5,                "Crusoe",   1),
    _IT_F   (0,15,                "Efficeon", 1)
};

static const struct
{
    const unsigned int ands;
    const unsigned int equel;
    bool            (*is_claim)(const struct cpu_ability *ability);
    const char *const  str;
}
x86_transmeta_brand[] =
{
    _IT_FMSQ(0, 5,  0,4,  2, t2, "Transmeta Crusoe TM3200"),
    _IT_FMS (0, 5,  0,4,  2,     "Transmeta Crusoe TM3x00 (unknown model)"),
    _IT_FMSQ(0, 5,  0,4,  3, t4, "Transmeta Crusoe TM5400"),
    _IT_FMSQ(0, 5,  0,4,  3, t5, "Transmeta Crusoe TM5500 / Crusoe SE TM55E"),
    _IT_FMSQ(0, 5,  0,4,  3, t6, "Transmeta Crusoe TM5600"),
    _IT_FMSQ(0, 5,  0,4,  3, t8, "Transmeta Crusoe TM5800 / Crusoe SE TM58E"),
    _IT_FMS (0, 5,  0,4,  3,     "Transmeta Crusoe TM5x00 (unknown model)"),
    _IT_FM  (0, 5,  0,4,         "Transmeta Crusoe"),
    _IT_F   (0, 5,               "Transmeta Crusoe (unknown model)"),
    _IT_FM  (0,15,  0,2,         "Transmeta Efficeon TM8x00"),
    _IT_FM  (0,15,  0,3,         "Transmeta Efficeon TM8x00"),
    _IT_F   (0,15,               "Transmeta Efficeon (unknown model)")
};

/*
** Cyrix major query
*/
#define cm (cyrix_is_claim_cm)

static const struct
{
    unsigned int      ands;
    unsigned int      equel;
    bool            (*is_claim)(const struct cpu_ability *ability);
    const char *const str;
}
x86_cyrix_name[] =
{
_IT_FM  (0,4,  0,4,     "Cyrix Media GX / GXm"),
_IT_FM  (0,4,  0,9,     "Cyrix 5x86"),
_IT_F   (0,4,           "Cyrix 5x86 (unknown model)"),
_IT_FM  (0,5,  0,2,     "Cyrix M1 6x86"),
_IT_FMQ (0,5,  0,4, cm, "Cyrix MediaGX (C6)"),
_IT_FM  (0,5,  0,4,     "Cyrix M1 WinChip (C6)"),
_IT_FM  (0,5,  0,8,     "Cyrix M1 WinChip 2 (C6-2)"),
_IT_FM  (0,5,  0,9,     "Cyrix M1 WinChip 3 (C6-2)"),
_IT_F   (0,5,           "Cyrix M1 (unknown model)"),
_IT_FM  (0,6,  0,0,     "Cyrix M2 6x86MX"),
_IT_FM  (0,6,  0,5,     "Cyrix M2"),
_IT_F   (0,6,           "Cyrix M2 (unknown model)"),
};
static const struct
{
    const unsigned int  ands;
    const unsigned int  equel;
    const char *const   uarch;        // process-dependent name
    const unsigned int  core_is_uarch;// for some uarches, the core names are based on the
                                      // uarch names, so the uarch name becomes redundant
    //const char *const   family;       // process-neutral name:
                                      //    sometimes independent (e.g. Core)
                                      //    sometimes based on lead uarch (e.g. Nehalem)
    //const char *const   phys;         // physical properties: die process, #pins, etc.
}
x86_cyrix_uarch[] =
{
_IT_F   (0, 5,                "M1", 1),
_IT_F   (0, 6,                "M2", 1)
};

// Hygon major query
static const struct
{
    unsigned int      ands;
    unsigned int      equel;
    bool            (*is_claim)(const struct cpu_ability *ability);
    const char *const str;
}
x86_hygon_name[] =
{
_IT_FMS (9,15,  0, 0,  1,     "Hygon Dhyana (A1)"),
_IT_FMS (9,15,  0, 0,  2,     "Hygon Dhyana (A2)")
};
static const struct
{
    const unsigned int  ands;
    const unsigned int  equel;
    const char *const   uarch;        // process-dependent name
    //const unsigned int  core_is_uarch;// for some uarches, the core names are based on the
                                      // uarch names, so the uarch name becomes redundant
    //const char *const   family;       // process-neutral name:
                                      //    sometimes independent (e.g. Core)
                                      //    sometimes based on lead uarch (e.g. Nehalem)
    const char *const   phys;         // physical properties: die process, #pins, etc.
}
x86_hygon_uarch[] =
{
_IT_FM  (9,15,  0, 0,         "Moksha", "14nm")
};

// NexGen
static const struct
{
    unsigned int      ands;
    unsigned int      equel;
    bool            (*is_claim)(const struct cpu_ability *ability);
    const char *const str;
}
x86_nexgen_name[] =
{
_IT_FMS(0,5,  0,0,  4, "NexGen P100"),
_IT_FMS(0,5,  0,0,  6, "NexGen P120 (E2/C0)")
};
static const struct
{
    const unsigned int  ands;
    const unsigned int  equel;
    const char *const   uarch;        // process-dependent name
    //const unsigned int  core_is_uarch;// for some uarches, the core names are based on the
                                      // uarch names, so the uarch name becomes redundant
    //const char *const   family;       // process-neutral name:
                                      //    sometimes independent (e.g. Core)
                                      //    sometimes based on lead uarch (e.g. Nehalem)
    //const char *const   phys;         // physical properties: die process, #pins, etc.
}
x86_nexgen_uarch[] =
{
_IT_IF   (0, 5,                "Nx586")
};

// NSC
static const struct
{
    unsigned int      ands;
    unsigned int      equel;
    bool            (*is_claim)(const struct cpu_ability *ability);
    const char *const str;
}
x86_nsc_name[] =
{
_IT_FM (0, 5,  0, 4,     "NSC Geode GX1/GXLV/GXm"),
_IT_FM (0, 5,  0, 5,     "NSC Geode GX2"),
_IT_FM (0, 5,  0,10,     "NSC Geode LX"), // sandpile.org
_IT_F  (0, 5,            "NSC Geode (unknown model)")
};

// RDC
static const struct
{
    unsigned int      ands;
    unsigned int      equel;
    bool            (*is_claim)(const struct cpu_ability *ability);
    const char *const str;
}
x86_rdc_name[] =
{
_IT_FM (0, 5,  0, 8,     "RDC IAD 100")
};

// RISE
static const struct
{
    unsigned int      ands;
    unsigned int      equel;
    bool            (*is_claim)(const struct cpu_ability *ability);
    const char *const str;
}
x86_rise_name[] =
{
_IT_FM (0,5,  0,0,     "Rise mP6 iDragon, .25u"),
_IT_FM (0,5,  0,2,     "Rise mP6 iDragon, .18u"),
_IT_FM (0,5,  0,8,     "Rise mP6 iDragon II, .25u"),
_IT_FM (0,5,  0,9,     "Rise mP6 iDragon II, .18u")
};
static const struct
{
    const unsigned int  ands;
    const unsigned int  equel;
    const char *const   uarch;        // process-dependent name
    const unsigned int  core_is_uarch;// for some uarches, the core names are based on the
                                      // uarch names, so the uarch name becomes redundant
    //const char *const   family;       // process-neutral name:
                                      //    sometimes independent (e.g. Core)
                                      //    sometimes based on lead uarch (e.g. Nehalem)
    //const char *const   phys;         // physical properties: die process, #pins, etc.
}
x86_rise_uarch[] =
{
    _IT_F   (0, 5,                "mP6", 1)
};

// SIS
static const struct
{
    unsigned int      ands;
    unsigned int      equel;
    bool            (*is_claim)(const struct cpu_ability *ability);
    const char *const str;
}
x86_sis_name[] =
{
_IT_FM (0, 5,  0, 0,     "SiS 55x")
};
static const struct
{
    const unsigned int  ands;
    const unsigned int  equel;
    const char *const   uarch;        // process-dependent name
    const unsigned int  core_is_uarch;// for some uarches, the core names are based on the
                                      // uarch names, so the uarch name becomes redundant
    //const char *const   family;       // process-neutral name:
                                      //    sometimes independent (e.g. Core)
                                      //    sometimes based on lead uarch (e.g. Nehalem)
    //const char *const   phys;         // physical properties: die process, #pins, etc.
}
x86_sis_uarch[] =
{
_IT_F   (0, 5,                "mP6", 1)
};

// UMC
static const struct
{
    unsigned int      ands;
    unsigned int      equel;
    bool            (*is_claim)(const struct cpu_ability *ability);
    const char *const str;
}
x86_umc_name[] =
{
_IT_FM (0,4,  0,1,     "UMC U5D (486DX)"),
_IT_FMS(0,4,  0,2,  3, "UMC U5S (486SX)"),
_IT_FM (0,4,  0,2,     "UMC U5S (486SX) (unknown stepping)")
};
static const struct
{
    const unsigned int  ands;
    const unsigned int  equel;
    const char *const   uarch;        // process-dependent name
    const unsigned int  core_is_uarch;// for some uarches, the core names are based on the
                                      // uarch names, so the uarch name becomes redundant
    //const char *const   family;       // process-neutral name:
                                      //    sometimes independent (e.g. Core)
                                      //    sometimes based on lead uarch (e.g. Nehalem)
    //const char *const   phys;         // physical properties: die process, #pins, etc.
}
x86_umc_uarch[] =
{
_IT_F   (0, 4,                "486 U5", 1)
};

// Vortex
static const struct
{
    unsigned int      ands;
    unsigned int      equel;
    bool            (*is_claim)(const struct cpu_ability *ability);
    const char *const str;
}
x86_vortex_name[] =
{
_IT_FM (0, 5,  0, 2,     "Vortex86DX"),
_IT_FM (0, 5,  0, 8,     "Vortex86MX")
};

// Zhaoxin
static const struct
{
    unsigned int      ands;
    unsigned int      equel;
    bool            (*is_claim)(const struct cpu_ability *ability);
    const char *const str;
}
x86_zhaoxin_name[] =
{
_IT_FM  (0, 7,  1,11,         "Zhaoxin KaiXian KX-5000 / Kaisheng KH-20000 (WuDaoKou)"),
_IT_FM  (0, 7,  1,15,         "Zhaoxin KaiXian ZX-D (WuDaoKou)"),
_IT_FM  (0, 7,  3,11,         "Zhaoxin KaiXian KX-6000 / Kaisheng KH-30000 (LuJiaZui)")
};
static const struct
{
    const unsigned int  ands;
    const unsigned int  equel;
    const char *const   uarch;        // process-dependent name
    const unsigned int  core_is_uarch;// for some uarches, the core names are based on the
                                      // uarch names, so the uarch name becomes redundant
    //const char *const   family;       // process-neutral name:
                                      //    sometimes independent (e.g. Core)
                                      //    sometimes based on lead uarch (e.g. Nehalem)
    const char *const   phys;         // physical properties: die process, #pins, etc.
}
x86_zhaoxin_uarch[] =
{
_IT_FM  (0, 7,  1,11,         "WuDaoKou", 1, "28nm"),
_IT_FM  (0, 7,  3,11,         "LuJiaZui", 1, "16nm")
};
#endif
