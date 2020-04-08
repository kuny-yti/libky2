
#ifndef X86_AMD_TABLE_H
#define X86_AMD_TABLE_H


/*
** AMD major queries:
**
** d? = think "desktop"
** s? = think "server" (MP)
** M? = think "mobile"
** S? = think "Series"
** T? = think "Tablet"
** A? = think "any"
** E? = think "Embedded"
**
** ?A = think Athlon
** ?X = think Athlon XP
** ?L = think Athlon XP (LV)
** ?E = think EPYC
** ?f = think FX
** ?F = think Athlon FX
** ?D = think Duron
** ?S = think Sempron
** ?O = think Opteron
** ?T = think Turion
** ?U = think Turion Ultra
** ?p = think Phenom
** ?R = think Ryzen
** ?s = think ?-Series
** ?n = think Turion Neo
** ?N = think Neo
*/

#define dA (amd_is_claim_dA)
#define dX (amd_is_claim_dX)
#define dF (amd_is_claim_dF)
#define df (amd_is_claim_df)
#define dD (amd_is_claim_dD)
#define dS (amd_is_claim_dS)
#define dp (amd_is_claim_dp)
#define dI (amd_is_claim_dI)
#define dR (amd_is_claim_dR)
#define sO (amd_is_claim_sO)
#define sA (amd_is_claim_sA)
#define sD (amd_is_claim_sD)
#define sE (amd_is_claim_sE)
#define MA (amd_is_claim_MA)
#define MX (amd_is_claim_MX)
#define ML (amd_is_claim_ML)
#define MD (amd_is_claim_MD)
#define MS (amd_is_claim_MS)
#define Mp (amd_is_claim_Mp)
#define Ms (amd_is_claim_Ms)
#define Mr (amd_is_claim_Mr)
#define MG (amd_is_claim_MG)
#define MT (amd_is_claim_MT)
#define MU (amd_is_claim_MU)
#define Mn (amd_is_claim_Mn)
#define MN (amd_is_claim_MN)
#define Sa (amd_is_claim_Sa)
#define Sc (amd_is_claim_Sc)
#define Se (amd_is_claim_Se)
#define Sg (amd_is_claim_Sg)
#define Sr (amd_is_claim_Sr)
#define Sz (amd_is_claim_Sz)
#define Ta (amd_is_claim_Ta)
#define Te (amd_is_claim_Te)
#define AR (amd_is_claim_AR)
#define ER (amd_is_claim_ER)
#define EE (iamd_is_claim_EE)


/*
** AMD special cases
*/

/* Embedded Opteron, distinguished from Opteron (Barcelona & Shanghai) */
#define EO (amd_is_claim_EO)
/* Opterons, distinguished by number of processors */
#define DO (amd_is_claim_DO)
#define SO (amd_is_claim_SO)
/* Athlons, distinguished by number of processors */
#define DA (amd_is_claim_DA)
#define TA (amd_is_claim_TA)
#define QA (amd_is_claim_QA)
/* Phenoms distinguished by number of processors */
#define Dp (amd_is_claim_Dp)
#define Tp (amd_is_claim_Tp)
#define Qp (amd_is_claim_Qp)
#define Sp (amd_is_claim_Sp)
/* Semprons, distinguished by number of processors */
#define DS (amd_is_claim_DS)
/* Egypt, distinguished from Italy; and
   Athens, distingushed from Troy */
#define s8 (amd_is_claim_s8)
/* Thorton A2, distinguished from Barton A2 */
#define dt (amd_is_claim_dt)
/* Manchester E6, distinguished from from Toledo E6 */
#define dm (amd_is_claim_dm)
/* Propus, distinguished from Regor */
#define dr (amd_is_claim_dr)
/* Trinidad, distinguished from Taylor */
#define Mt (amd_is_claim_Mt)

static const struct
{
    unsigned int      ands;
    unsigned int      equel;
    bool            (*is_claim)(const struct cpu_ability *ability);
    const char *const str;
}
x86_amd_name[] =
{
   _IT_FM  ( 0, 4,  0, 3,         "AMD 80486DX2"),
   _IT_FM  ( 0, 4,  0, 7,         "AMD 80486DX2WB"),
   _IT_FM  ( 0, 4,  0, 8,         "AMD 80486DX4"),
   _IT_FM  ( 0, 4,  0, 9,         "AMD 80486DX4WB"),
   _IT_FM  ( 0, 4,  0,10,         "AMD Elan SC400"), // sandpile.org
   _IT_FM  ( 0, 4,  0,14,         "AMD 5x86"),
   _IT_FM  ( 0, 4,  0,15,         "AMD 5xWB"),
   _IT_F   ( 0, 4,                "AMD 80486 / 5x (unknown model)"),
   _IT_FM  ( 0, 5,  0, 0,         "AMD SSA5 (PR75, PR90, PR100)"),
   _IT_FM  ( 0, 5,  0, 1,         "AMD 5k86 (PR120, PR133)"),
   _IT_FM  ( 0, 5,  0, 2,         "AMD 5k86 (PR166)"),
   _IT_FM  ( 0, 5,  0, 3,         "AMD 5k86 (PR200)"),
   _IT_FM  ( 0, 5,  0, 5,         "AMD Geode GX"),
   _IT_FM  ( 0, 5,  0, 6,         "AMD K6"),
   _IT_FM  ( 0, 5,  0, 7,         "AMD K6 (Little Foot)"),
   _IT_FMS ( 0, 5,  0, 8,  0,     "AMD K6-2 (Chomper A)"),
   _IT_FMS ( 0, 5,  0, 8, 12,     "AMD K6-2 (Chomper A)"),
   _IT_FM  ( 0, 5,  0, 8,         "AMD K6-2 (Chomper)"),
   _IT_FMS ( 0, 5,  0, 9,  1,     "AMD K6-III (Sharptooth B)"),
   _IT_FM  ( 0, 5,  0, 9,         "AMD K6-III (Sharptooth)"),
   _IT_FM  ( 0, 5,  0,10,         "AMD Geode LX"),
   _IT_FM  ( 0, 5,  0,13,         "AMD K6-2+, K6-III+"),
   _IT_F   ( 0, 5,                "AMD 5k86 / K6 / Geode (unknown model)"),
   _IT_FM  ( 0, 6,  0, 1,         "AMD Athlon (Argon)"),
   _IT_FM  ( 0, 6,  0, 2,         "AMD Athlon (K75 / Pluto / Orion)"),
   _IT_FMS ( 0, 6,  0, 3,  0,     "AMD Duron / mobile Duron (Spitfire A0)"),
   _IT_FMS ( 0, 6,  0, 3,  1,     "AMD Duron / mobile Duron (Spitfire A2)"),
   _IT_FM  ( 0, 6,  0, 3,         "AMD Duron / mobile Duron (Spitfire)"),
   _IT_FMS ( 0, 6,  0, 4,  2,     "AMD Athlon (Thunderbird A4-A7)"),
   _IT_FMS ( 0, 6,  0, 4,  4,     "AMD Athlon (Thunderbird A9)"),
   _IT_FM  ( 0, 6,  0, 4,         "AMD Athlon (Thunderbird)"),
   _IT_FMSQ( 0, 6,  0, 6,  0, sA, "AMD Athlon MP (Palomino A0)"),
   _IT_FMSQ( 0, 6,  0, 6,  0, dA, "AMD Athlon (Palomino A0)"),
   _IT_FMSQ( 0, 6,  0, 6,  0, MA, "AMD mobile Athlon 4 (Palomino A0)"),
   _IT_FMSQ( 0, 6,  0, 6,  0, sD, "AMD Duron MP (Palomino A0)"),
   _IT_FMSQ( 0, 6,  0, 6,  0, MD, "AMD mobile Duron (Palomino A0)"),
   _IT_FMS ( 0, 6,  0, 6,  0,     "AMD Athlon (unknown type)  (Palomino A0)"),
   _IT_FMSQ( 0, 6,  0, 6,  1, sA, "AMD Athlon MP (Palomino A2)"),
   _IT_FMSQ( 0, 6,  0, 6,  1, dA, "AMD Athlon (Palomino A2)"),
   _IT_FMSQ( 0, 6,  0, 6,  1, MA, "AMD mobile Athlon 4 (Palomino A2)"),
   _IT_FMSQ( 0, 6,  0, 6,  1, sD, "AMD Duron MP (Palomino A2)"),
   _IT_FMSQ( 0, 6,  0, 6,  1, MD, "AMD mobile Duron (Palomino A2)"),
   _IT_FMSQ( 0, 6,  0, 6,  1, dD, "AMD Duron (Palomino A2)"),
   _IT_FMS ( 0, 6,  0, 6,  1,     "AMD Athlon (unknown type) (Palomino A2)"),
   _IT_FMSQ( 0, 6,  0, 6,  2, sA, "AMD Athlon MP (Palomino A5)"),
   _IT_FMSQ( 0, 6,  0, 6,  2, dX, "AMD Athlon XP (Palomino A5)"),
   _IT_FMSQ( 0, 6,  0, 6,  2, MA, "AMD mobile Athlon 4 (Palomino A5)"),
   _IT_FMSQ( 0, 6,  0, 6,  2, sD, "AMD Duron MP (Palomino A5)"),
   _IT_FMSQ( 0, 6,  0, 6,  2, MD, "AMD mobile Duron (Palomino A5)"),
   _IT_FMSQ( 0, 6,  0, 6,  2, dD, "AMD Duron (Palomino A5)"),
   _IT_FMS ( 0, 6,  0, 6,  2,     "AMD Athlon (unknown type) (Palomino A5)"),
   _IT_FMQ ( 0, 6,  0, 6,     MD, "AMD mobile Duron (Palomino)"),
   _IT_FMQ ( 0, 6,  0, 6,     dD, "AMD Duron (Palomino)"),
   _IT_FMQ ( 0, 6,  0, 6,     MA, "AMD mobile Athlon (Palomino)"),
   _IT_FMQ ( 0, 6,  0, 6,     dX, "AMD Athlon XP (Palomino)"),
   _IT_FMQ ( 0, 6,  0, 6,     dA, "AMD Athlon (Palomino)"),
   _IT_FM  ( 0, 6,  0, 6,         "AMD Athlon (unknown type) (Palomino)"),
   _IT_FMSQ( 0, 6,  0, 7,  0, sD, "AMD Duron MP (Morgan A0)"),
   _IT_FMSQ( 0, 6,  0, 7,  0, MD, "AMD mobile Duron (Morgan A0)"),
   _IT_FMSQ( 0, 6,  0, 7,  0, dD, "AMD Duron (Morgan A0)"),
   _IT_FMS ( 0, 6,  0, 7,  0,     "AMD Duron (unknown type)  (Morgan A0)"),
   _IT_FMSQ( 0, 6,  0, 7,  1, sD, "AMD Duron MP (Morgan A1)"),
   _IT_FMSQ( 0, 6,  0, 7,  1, MD, "AMD mobile Duron (Morgan A1)"),
   _IT_FMSQ( 0, 6,  0, 7,  1, dD, "AMD Duron (Morgan A1)"),
   _IT_FMS ( 0, 6,  0, 7,  1,     "AMD Duron (unknown type)  (Morgan A1)"),
   _IT_FMQ ( 0, 6,  0, 7,     sD, "AMD Duron MP (Morgan)"),
   _IT_FMQ ( 0, 6,  0, 7,     MD, "AMD mobile Duron (Morgan)"),
   _IT_FMQ ( 0, 6,  0, 7,     dD, "AMD Duron (Morgan)"),
   _IT_FM  ( 0, 6,  0, 7,         "AMD Duron (unknown type)  (Morgan)"),
   _IT_FMSQ( 0, 6,  0, 8,  0, dS, "AMD Sempron (Thoroughbred A0)"),
   _IT_FMSQ( 0, 6,  0, 8,  0, sD, "AMD Duron MP (Applebred A0)"),
   _IT_FMSQ( 0, 6,  0, 8,  0, dD, "AMD Duron (Applebred A0)"),
   _IT_FMSQ( 0, 6,  0, 8,  0, MX, "AMD mobile Athlon XP (Thoroughbred A0)"),
   _IT_FMSQ( 0, 6,  0, 8,  0, sA, "AMD Athlon MP (Thoroughbred A0)"),
   _IT_FMSQ( 0, 6,  0, 8,  0, dX, "AMD Athlon XP (Thoroughbred A0)"),
   _IT_FMSQ( 0, 6,  0, 8,  0, dA, "AMD Athlon (Thoroughbred A0)"),
   _IT_FMS ( 0, 6,  0, 8,  0,     "AMD Athlon (unknown type) (Thoroughbred A0)"),
   _IT_FMSQ( 0, 6,  0, 8,  1, MG, "AMD Geode NX (Thoroughbred B0)"),
   _IT_FMSQ( 0, 6,  0, 8,  1, dS, "AMD Sempron (Thoroughbred B0)"),
   _IT_FMSQ( 0, 6,  0, 8,  1, sD, "AMD Duron MP (Applebred B0)"),
   _IT_FMSQ( 0, 6,  0, 8,  1, dD, "AMD Duron (Applebred B0)"),
   _IT_FMSQ( 0, 6,  0, 8,  1, sA, "AMD Athlon MP (Thoroughbred B0)"),
   _IT_FMSQ( 0, 6,  0, 8,  1, dX, "AMD Athlon XP (Thoroughbred B0)"),
   _IT_FMSQ( 0, 6,  0, 8,  1, dA, "AMD Athlon (Thoroughbred B0)"),
   _IT_FMS ( 0, 6,  0, 8,  1,     "AMD Athlon (unknown type) (Thoroughbred B0)"),
   _IT_FMQ ( 0, 6,  0, 8,     MG, "AMD Geode NX (Thoroughbred)"),
   _IT_FMQ ( 0, 6,  0, 8,     dS, "AMD Sempron (Thoroughbred)"),
   _IT_FMQ ( 0, 6,  0, 8,     sD, "AMD Duron MP (Thoroughbred)"),
   _IT_FMQ ( 0, 6,  0, 8,     dD, "AMD Duron (Thoroughbred)"),
   _IT_FMQ ( 0, 6,  0, 8,     MX, "AMD mobile Athlon XP (Thoroughbred)"),
   _IT_FMQ ( 0, 6,  0, 8,     sA, "AMD Athlon MP (Thoroughbred)"),
   _IT_FMQ ( 0, 6,  0, 8,     dX, "AMD Athlon XP (Thoroughbred)"),
   _IT_FMQ ( 0, 6,  0, 8,     dA, "AMD Athlon XP (Thoroughbred)"),
   _IT_FM  ( 0, 6,  0, 8,         "AMD Athlon (unknown type) (Thoroughbred)"),
   _IT_FMSQ( 0, 6,  0,10,  0, dS, "AMD Sempron (Barton A2)"),
   _IT_FMSQ( 0, 6,  0,10,  0, ML, "AMD mobile Athlon XP-M (LV) (Barton A2)"),
   _IT_FMSQ( 0, 6,  0,10,  0, MX, "AMD mobile Athlon XP-M (Barton A2)"),
   _IT_FMSQ( 0, 6,  0,10,  0, dt, "AMD Athlon XP (Thorton A2)"),
   _IT_FMSQ( 0, 6,  0,10,  0, sA, "AMD Athlon MP (Barton A2)"),
   _IT_FMSQ( 0, 6,  0,10,  0, dX, "AMD Athlon XP (Barton A2)"),
   _IT_FMS ( 0, 6,  0,10,  0,     "AMD Athlon (unknown type) (Barton A2)"),
   _IT_FMQ ( 0, 6,  0,10,     dS, "AMD Sempron (Barton)"),
   _IT_FMQ ( 0, 6,  0,10,     ML, "AMD mobile Athlon XP-M (LV) (Barton)"),
   _IT_FMQ ( 0, 6,  0,10,     MX, "AMD mobile Athlon XP-M (Barton)"),
   _IT_FMQ ( 0, 6,  0,10,     sA, "AMD Athlon MP (Barton)"),
   _IT_FMQ ( 0, 6,  0,10,     dX, "AMD Athlon XP (Barton)"),
   _IT_FM  ( 0, 6,  0,10,         "AMD Athlon (unknown type) (Barton)"),
   _IT_F   ( 0, 6,                "AMD Athlon (unknown model)"),
   _IT_F   ( 0, 7,                "AMD Opteron (unknown model)"),
   _IT_FMS ( 0,15,  0, 4,  0,     "AMD Athlon 64 (SledgeHammer SH7-B0)"),
   _IT_FMSQ( 0,15,  0, 4,  8, MX, "AMD mobile Athlon XP-M (SledgeHammer SH7-C0)"),
   _IT_FMSQ( 0,15,  0, 4,  8, MA, "AMD mobile Athlon 64 (SledgeHammer SH7-C0)"),
   _IT_FMSQ( 0,15,  0, 4,  8, dA, "AMD Athlon 64 (SledgeHammer SH7-C0)"),
   _IT_FMS ( 0,15,  0, 4,  8,     "AMD Athlon 64 (unknown type) (SledgeHammer SH7-C0)"),
   _IT_FMSQ( 0,15,  0, 4, 10, MX, "AMD mobile Athlon XP-M (SledgeHammer SH7-CG)"),
   _IT_FMSQ( 0,15,  0, 4, 10, MA, "AMD mobile Athlon 64 (SledgeHammer SH7-CG)"),
   _IT_FMSQ( 0,15,  0, 4, 10, dA, "AMD Athlon 64 (SledgeHammer SH7-CG)"),
   _IT_FMS ( 0,15,  0, 4, 10,     "AMD Athlon 64 (unknown type) (SledgeHammer SH7-CG)"),
   _IT_FMQ ( 0,15,  0, 4,     MX, "AMD mobile Athlon XP-M (SledgeHammer SH7)"),
   _IT_FMQ ( 0,15,  0, 4,     MA, "AMD mobile Athlon 64 (SledgeHammer SH7)"),
   _IT_FMQ ( 0,15,  0, 4,     dA, "AMD Athlon 64 (SledgeHammer SH7)"),
   _IT_FM  ( 0,15,  0, 4,         "AMD Athlon 64 (unknown type) (SledgeHammer SH7)"),
   _IT_FMS ( 0,15,  0, 5,  0,     "AMD Opteron (DP SledgeHammer SH7-B0)"),
   _IT_FMS ( 0,15,  0, 5,  1,     "AMD Opteron (DP SledgeHammer SH7-B3)"),
   _IT_FMSQ( 0,15,  0, 5,  8, sO, "AMD Opteron (DP SledgeHammer SH7-C0)"),
   _IT_FMSQ( 0,15,  0, 5,  8, dF, "AMD Athlon 64 FX (DP SledgeHammer SH7-C0)"),
   _IT_FMS ( 0,15,  0, 5,  8,     "AMD Athlon 64 (unknown type) (DP SledgeHammer SH7-C0)"),
   _IT_FMSQ( 0,15,  0, 5, 10, sO, "AMD Opteron (DP SledgeHammer SH7-CG)"),
   _IT_FMSQ( 0,15,  0, 5, 10, dF, "AMD Athlon 64 FX (DP SledgeHammer SH7-CG)"),
   _IT_FMS ( 0,15,  0, 5, 10,     "AMD Athlon 64 (unknown type) (DP SledgeHammer SH7-CG)"),
   _IT_FMQ ( 0,15,  0, 5,     sO, "AMD Opteron (SledgeHammer SH7)"),
   _IT_FMQ ( 0,15,  0, 5,     dF, "AMD Athlon 64 FX (SledgeHammer SH7)"),
   _IT_FM  ( 0,15,  0, 5,         "AMD Athlon 64 (unknown type) (SledgeHammer SH7) FX"),
   _IT_FMSQ( 0,15,  0, 7, 10, dF, "AMD Athlon 64 FX (DP SledgeHammer SH7-CG)"),
   _IT_FMSQ( 0,15,  0, 7, 10, dA, "AMD Athlon 64 (DP SledgeHammer SH7-CG)"),
   _IT_FMS ( 0,15,  0, 7, 10,     "AMD Athlon 64 (unknown type) (DP SledgeHammer SH7-CG)"),
   _IT_FMQ ( 0,15,  0, 7,     dF, "AMD Athlon 64 FX (DP SledgeHammer SH7)"),
   _IT_FMQ ( 0,15,  0, 7,     dA, "AMD Athlon 64 (DP SledgeHammer SH7)"),
   _IT_FM  ( 0,15,  0, 7,         "AMD Athlon 64 (unknown type) (DP SledgeHammer SH7)"),
   _IT_FMSQ( 0,15,  0, 8,  2, MS, "AMD mobile Sempron (ClawHammer CH7-CG)"),
   _IT_FMSQ( 0,15,  0, 8,  2, MX, "AMD mobile Athlon XP-M (ClawHammer CH7-CG)"),
   _IT_FMSQ( 0,15,  0, 8,  2, MA, "AMD mobile Athlon 64 (Odessa CH7-CG)"),
   _IT_FMSQ( 0,15,  0, 8,  2, dA, "AMD Athlon 64 (ClawHammer CH7-CG)"),
   _IT_FMS ( 0,15,  0, 8,  2,     "AMD Athlon 64 (unknown type) (ClawHammer/Odessa CH7-CG)"),
   _IT_FMQ ( 0,15,  0, 8,     MS, "AMD mobile Sempron (Odessa CH7)"),
   _IT_FMQ ( 0,15,  0, 8,     MX, "AMD mobile Athlon XP-M (Odessa CH7)"),
   _IT_FMQ ( 0,15,  0, 8,     MA, "AMD mobile Athlon 64 (Odessa CH7)"),
   _IT_FMQ ( 0,15,  0, 8,     dA, "AMD Athlon 64 (ClawHammer CH7)"),
   _IT_FM  ( 0,15,  0, 8,         "AMD Athlon 64 (unknown type) (ClawHammer/Odessa CH7)"),
   _IT_FMS ( 0,15,  0,11,  2,     "AMD Athlon 64 (ClawHammer CH7-CG)"),
   _IT_FM  ( 0,15,  0,11,         "AMD Athlon 64 (ClawHammer CH7)"),
   _IT_FMSQ( 0,15,  0,12,  0, MS, "AMD mobile Sempron (Dublin DH7-CG)"),
   _IT_FMSQ( 0,15,  0,12,  0, dS, "AMD Sempron (Paris DH7-CG)"),
   _IT_FMSQ( 0,15,  0,12,  0, MX, "AMD mobile Athlon XP-M (ClawHammer/Odessa DH7-CG)"),
   _IT_FMSQ( 0,15,  0,12,  0, MA, "AMD mobile Athlon 64 (ClawHammer/Odessa DH7-CG)"),
   _IT_FMSQ( 0,15,  0,12,  0, dA, "AMD Athlon 64 (NewCastle DH7-CG)"),
   _IT_FMS ( 0,15,  0,12,  0,     "AMD Athlon 64 (unknown type) (ClawHammer/Odessa/NewCastle/Paris/Dublin DH7-CG)"),
   _IT_FMQ ( 0,15,  0,12,     MS, "AMD mobile Sempron (Dublin DH7)"),
   _IT_FMQ ( 0,15,  0,12,     dS, "AMD Sempron (Paris DH7)"),
   _IT_FMQ ( 0,15,  0,12,     MX, "AMD mobile Athlon XP-M (NewCastle DH7)"),
   _IT_FMQ ( 0,15,  0,12,     MA, "AMD mobile Athlon 64 (ClawHammer/Odessa DH7)"),
   _IT_FMQ ( 0,15,  0,12,     dA, "AMD Athlon 64 (NewCastle DH7)"),
   _IT_FM  ( 0,15,  0,12,         "AMD Athlon 64 (unknown type) (ClawHammer/Odessa/NewCastle/Paris/Dublin DH7)"),
   _IT_FMSQ( 0,15,  0,14,  0, MS, "AMD mobile Sempron (Dublin DH7-CG)"),
   _IT_FMSQ( 0,15,  0,14,  0, dS, "AMD Sempron (Paris DH7-CG)"),
   _IT_FMSQ( 0,15,  0,14,  0, MX, "AMD mobile Athlon XP-M (ClawHammer/Odessa DH7-CG)"),
   _IT_FMSQ( 0,15,  0,14,  0, MA, "AMD mobile Athlon 64 (ClawHammer/Odessa DH7-CG)"),
   _IT_FMSQ( 0,15,  0,14,  0, dA, "AMD Athlon 64 (NewCastle DH7-CG)"),
   _IT_FMS ( 0,15,  0,14,  0,     "AMD Athlon 64 (unknown type) (ClawHammer/Odessa/NewCastle/Paris/Dublin DH7-CG)"),
   _IT_FMQ ( 0,15,  0,14,     dS, "AMD Sempron (Paris DH7)"),
   _IT_FMQ ( 0,15,  0,14,     MS, "AMD mobile Sempron (Dublin DH7)"),
   _IT_FMQ ( 0,15,  0,14,     MX, "AMD mobile Athlon XP-M (ClawHammer/Odessa DH7)"),
   _IT_FMQ ( 0,15,  0,14,     MA, "AMD mobile Athlon 64 (ClawHammer/Odessa DH7)"),
   _IT_FMQ ( 0,15,  0,14,     dA, "AMD Athlon 64 (NewCastle DH7)"),
   _IT_FM  ( 0,15,  0,14,         "AMD Athlon 64 (unknown type) (ClawHammer/Odessa/NewCastle/Paris/Dublin DH7)"),
   _IT_FMSQ( 0,15,  0,15,  0, dS, "AMD Sempron (Paris DH7-CG)"),
   _IT_FMSQ( 0,15,  0,15,  0, MA, "AMD mobile Athlon 64 (ClawHammer/Odessa DH7-CG)"),
   _IT_FMSQ( 0,15,  0,15,  0, dA, "AMD Athlon 64 (NewCastle DH7-CG)"),
   _IT_FMS ( 0,15,  0,15,  0,     "AMD Athlon 64 (unknown type) (ClawHammer/Odessa/NewCastle/Paris DH7-CG)"),
   _IT_FMQ ( 0,15,  0,15,     dS, "AMD Sempron (Paris DH7)"),
   _IT_FMQ ( 0,15,  0,15,     MA, "AMD mobile Athlon 64 (ClawHammer/Odessa DH7)"),
   _IT_FMQ ( 0,15,  0,15,     dA, "AMD Athlon 64 (NewCastle DH7)"),
   _IT_FM  ( 0,15,  0,15,         "AMD Athlon 64 (unknown type) (ClawHammer/Odessa/NewCastle/Paris DH7)"),
   _IT_FMSQ( 0,15,  1, 4,  0, MX, "AMD mobile Athlon XP-M (Oakville SH7-D0)"),
   _IT_FMSQ( 0,15,  1, 4,  0, MA, "AMD mobile Athlon 64 (Oakville SH7-D0)"),
   _IT_FMSQ( 0,15,  1, 4,  0, dA, "AMD Athlon 64 (Winchester SH7-D0)"),
   _IT_FMS ( 0,15,  1, 4,  0,     "AMD Athlon 64 (unknown type) (Winchester/Oakville SH7-D0)"),
   _IT_FMQ ( 0,15,  1, 4,     MX, "AMD mobile Athlon XP-M (Oakville SH7)"),
   _IT_FMQ ( 0,15,  1, 4,     MA, "AMD mobile Athlon 64 (Oakville SH7)"),
   _IT_FMQ ( 0,15,  1, 4,     dA, "AMD Athlon 64 (Winchester SH7)"),
   _IT_FM  ( 0,15,  1, 4,         "AMD Athlon 64 (unknown type) (Winchester/Oakville SH7)"),
   _IT_FMSQ( 0,15,  1, 5,  0, sO, "AMD Opteron (Winchester SH7-D0)"),
   _IT_FMSQ( 0,15,  1, 5,  0, dF, "AMD Athlon 64 FX (Winchester SH7-D0)"),
   _IT_FMS ( 0,15,  1, 5,  0,     "AMD Athlon 64 (unknown type) (Winchester SH7-D0)"),
   _IT_FMQ ( 0,15,  1, 5,     sO, "AMD Opteron (Winchester SH7)"),
   _IT_FMQ ( 0,15,  1, 5,     dF, "AMD Athlon 64 FX (Winchester SH7)"),
   _IT_FM  ( 0,15,  1, 5,         "AMD Athlon 64 (unknown type) (Winchester SH7)"),
   _IT_FMSQ( 0,15,  1, 7,  0, dF, "AMD Athlon 64 FX (Winchester SH7-D0)"),
   _IT_FMSQ( 0,15,  1, 7,  0, dA, "AMD Athlon 64 (Winchester SH7-D0)"),
   _IT_FMS ( 0,15,  1, 7,  0,     "AMD Athlon 64 (unknown type) (Winchester SH7-D0)"),
   _IT_FMQ ( 0,15,  1, 7,     dF, "AMD Athlon 64 FX (Winchester SH7)"),
   _IT_FMQ ( 0,15,  1, 7,     dA, "AMD Athlon 64 (Winchester SH7)"),
   _IT_FM  ( 0,15,  1, 7,         "AMD Athlon 64 (unknown type) (Winchester SH7)"),
   _IT_FMSQ( 0,15,  1, 8,  0, MS, "AMD mobile Sempron (Georgetown/Sonora CH-D0)"),
   _IT_FMSQ( 0,15,  1, 8,  0, MX, "AMD mobile Athlon XP-M (Oakville CH-D0)"),
   _IT_FMSQ( 0,15,  1, 8,  0, MA, "AMD mobile Athlon 64 (Oakville CH-D0)"),
   _IT_FMSQ( 0,15,  1, 8,  0, dA, "AMD Athlon 64 (Winchester CH-D0)"),
   _IT_FMS ( 0,15,  1, 8,  0,     "AMD Athlon 64 (unknown type) (Winchester/Oakville/Georgetown/Sonora CH-D0)"),
   _IT_FMQ ( 0,15,  1, 8,     MS, "AMD mobile Sempron (Georgetown/Sonora CH)"),
   _IT_FMQ ( 0,15,  1, 8,     MX, "AMD mobile Athlon XP-M (Oakville CH)"),
   _IT_FMQ ( 0,15,  1, 8,     MA, "AMD mobile Athlon 64 (Oakville CH)"),
   _IT_FMQ ( 0,15,  1, 8,     dA, "AMD Athlon 64 (Winchester CH)"),
   _IT_FM  ( 0,15,  1, 8,         "AMD Athlon 64 (unknown type) (Winchester/Oakville/Georgetown/Sonora CH)"),
   _IT_FMS ( 0,15,  1,11,  0,     "AMD Athlon 64 (Winchester CH-D0)"),
   _IT_FM  ( 0,15,  1,11,         "AMD Athlon 64 (Winchester CH)"),
   _IT_FMSQ( 0,15,  1,12,  0, MS, "AMD mobile Sempron (Georgetown/Sonora DH8-D0)"),
   _IT_FMSQ( 0,15,  1,12,  0, dS, "AMD Sempron (Palermo DH8-D0)"),
   _IT_FMSQ( 0,15,  1,12,  0, MX, "AMD Athlon XP-M (Winchester DH8-D0)"),
   _IT_FMSQ( 0,15,  1,12,  0, MA, "AMD mobile Athlon 64 (Oakville DH8-D0)"),
   _IT_FMSQ( 0,15,  1,12,  0, dA, "AMD Athlon 64 (Winchester DH8-D0)"),
   _IT_FMS ( 0,15,  1,12,  0,     "AMD Athlon 64 (unknown type) (Winchester/Oakville/Georgetown/Sonora/Palermo DH8-D0)"),
   _IT_FMQ ( 0,15,  1,12,     MS, "AMD mobile Sempron (Georgetown/Sonora DH8)"),
   _IT_FMQ ( 0,15,  1,12,     dS, "AMD Sempron (Palermo DH8)"),
   _IT_FMQ ( 0,15,  1,12,     MX, "AMD Athlon XP-M (Winchester DH8)"),
   _IT_FMQ ( 0,15,  1,12,     MA, "AMD mobile Athlon 64 (Oakville DH8)"),
   _IT_FMQ ( 0,15,  1,12,     dA, "AMD Athlon 64 (Winchester DH8)"),
   _IT_FM  ( 0,15,  1,12,         "AMD Athlon 64 (Winchester/Oakville/Georgetown/Sonora/Palermo DH8)"),
   _IT_FMSQ( 0,15,  1,15,  0, dS, "AMD Sempron (Palermo DH8-D0)"),
   _IT_FMSQ( 0,15,  1,15,  0, dA, "AMD Athlon 64 (Winchester DH8-D0)"),
   _IT_FMS ( 0,15,  1,15,  0,     "AMD Athlon 64 (Winchester DH8-D0) / Sempron (Palermo DH8-D0)"),
   _IT_FMQ ( 0,15,  1,15,     dS, "AMD Sempron (Palermo DH8)"),
   _IT_FMQ ( 0,15,  1,15,     dA, "AMD Athlon 64 (Winchester DH8)"),
   _IT_FM  ( 0,15,  1,15,         "AMD Athlon 64 (unknown type) (Winchester/Palermo DH8)"),
   _IT_FMSQ( 0,15,  2, 1,  0, s8, "AMD Dual Core Opteron (Egypt JH-E1)"),
   _IT_FMSQ( 0,15,  2, 1,  0, sO, "AMD Dual Core Opteron (Italy JH-E1)"),
   _IT_FMS ( 0,15,  2, 1,  0,     "AMD Dual Core Opteron (Italy/Egypt JH-E1)"),
   _IT_FMSQ( 0,15,  2, 1,  2, s8, "AMD Dual Core Opteron (Egypt JH-E6)"),
   _IT_FMSQ( 0,15,  2, 1,  2, sO, "AMD Dual Core Opteron (Italy JH-E6)"),
   _IT_FMS ( 0,15,  2, 1,  2,     "AMD Dual Core Opteron (Italy/Egypt JH-E6)"),
   _IT_FMQ ( 0,15,  2, 1,     s8, "AMD Dual Core Opteron (Egypt JH)"),
   _IT_FMQ ( 0,15,  2, 1,     sO, "AMD Dual Core Opteron (Italy JH)"),
   _IT_FM  ( 0,15,  2, 1,         "AMD Dual Core Opteron (Italy/Egypt JH)"),
   _IT_FMSQ( 0,15,  2, 3,  2, DO, "AMD Dual Core Opteron (Denmark JH-E6)"),
   _IT_FMSQ( 0,15,  2, 3,  2, dF, "AMD Athlon 64 FX (Toledo JH-E6)"),
   _IT_FMSQ( 0,15,  2, 3,  2, dm, "AMD Athlon 64 X2 (Manchester JH-E6)"),
   _IT_FMSQ( 0,15,  2, 3,  2, dA, "AMD Athlon 64 X2 (Toledo JH-E6)"),
   _IT_FMS ( 0,15,  2, 3,  2,     "AMD Athlon 64 (unknown type) (Toledo/Manchester/Denmark JH-E6)"),
   _IT_FMQ ( 0,15,  2, 3,     sO, "AMD Dual Core Opteron (Denmark JH)"),
   _IT_FMQ ( 0,15,  2, 3,     dF, "AMD Athlon 64 FX (Toledo JH)"),
   _IT_FMQ ( 0,15,  2, 3,     dm, "AMD Athlon 64 X2 (Manchester JH)"),
   _IT_FMQ ( 0,15,  2, 3,     dA, "AMD Athlon 64 X2 (Toledo JH)"),
   _IT_FM  ( 0,15,  2, 3,         "AMD Athlon 64 (unknown type) (Toledo/Manchester/Denmark JH)"),
   _IT_FMSQ( 0,15,  2, 4,  2, MA, "AMD mobile Athlon 64 (Newark SH-E5)"),
   _IT_FMSQ( 0,15,  2, 4,  2, MT, "AMD mobile Turion (Lancaster/Richmond SH-E5)"),
   _IT_FMS ( 0,15,  2, 4,  2,     "AMD mobile Athlon 64 (unknown type) (Newark/Lancaster/Richmond SH-E5)"),
   _IT_FMQ ( 0,15,  2, 4,     MA, "AMD mobile Athlon 64 (Newark SH)"),
   _IT_FMQ ( 0,15,  2, 4,     MT, "AMD mobile Turion (Lancaster/Richmond SH)"),
   _IT_FM  ( 0,15,  2, 4,         "AMD mobile Athlon 64 (unknown type) (Newark/Lancaster/Richmond SH)"),
   _IT_FMQ ( 0,15,  2, 5,     s8, "AMD Opteron (Athens SH-E4)"),
   _IT_FMQ ( 0,15,  2, 5,     sO, "AMD Opteron (Troy SH-E4)"),
   _IT_FM  ( 0,15,  2, 5,         "AMD Opteron (Troy/Athens SH-E4)"),
   _IT_FMSQ( 0,15,  2, 7,  1, sO, "AMD Opteron (Venus SH-E4)"),
   _IT_FMSQ( 0,15,  2, 7,  1, dF, "AMD Athlon 64 FX (San Diego SH-E4)"),
   _IT_FMSQ( 0,15,  2, 7,  1, dA, "AMD Athlon 64 (San Diego SH-E4)"),
   _IT_FMS ( 0,15,  2, 7,  1,     "AMD Athlon 64 (unknown type) (Venus/San Diego SH-E4)"),
   _IT_FMQ ( 0,15,  2, 7,     sO, "AMD Opteron (San Diego SH)"),
   _IT_FMQ ( 0,15,  2, 7,     dF, "AMD Athlon 64 FX (San Diego SH)"),
   _IT_FMQ ( 0,15,  2, 7,     dA, "AMD Athlon 64 (San Diego SH)"),
   _IT_FM  ( 0,15,  2, 7,         "AMD Athlon 64 (unknown type) (San Diego SH)"),
   _IT_FM  ( 0,15,  2,11,         "AMD Athlon 64 X2 (Manchester BH-E4)"),
   _IT_FMS ( 0,15,  2,12,  0,     "AMD Sempron (Palermo DH-E3)"),
   _IT_FMSQ( 0,15,  2,12,  2, MS, "AMD mobile Sempron (Albany/Roma DH-E6)"),
   _IT_FMSQ( 0,15,  2,12,  2, dS, "AMD Sempron (Palermo DH-E6)"),
   _IT_FMSQ( 0,15,  2,12,  2, dA, "AMD Athlon 64 (Venice DH-E6)"),
   _IT_FMS ( 0,15,  2,12,  2,     "AMD Athlon 64 (Venice/Palermo/Albany/Roma DH-E6)"),
   _IT_FMQ ( 0,15,  2,12,     MS, "AMD mobile Sempron (Albany/Roma DH)"),
   _IT_FMQ ( 0,15,  2,12,     dS, "AMD Sempron (Palermo DH)"),
   _IT_FMQ ( 0,15,  2,12,     dA, "AMD Athlon 64 (Venice DH)"),
   _IT_FM  ( 0,15,  2,12,         "AMD Athlon 64 (Venice/Palermo/Albany/Roma DH)"),
   _IT_FMSQ( 0,15,  2,15,  0, dS, "AMD Sempron (Palermo DH-E3)"),
   _IT_FMSQ( 0,15,  2,15,  0, dA, "AMD Athlon 64 (Venice DH-E3)"),
   _IT_FMS ( 0,15,  2,15,  0,     "AMD Athlon 64 (Venice/Palermo DH-E3)"),
   _IT_FMSQ( 0,15,  2,15,  2, dS, "AMD Sempron (Palermo DH-E6)"),
   _IT_FMSQ( 0,15,  2,15,  2, dA, "AMD Athlon 64 (Venice DH-E6)"),
   _IT_FMS ( 0,15,  2,15,  2,     "AMD Athlon 64 (Venice/Palermo DH-E6)"),
   _IT_FMQ ( 0,15,  2,15,     dS, "AMD Sempron (Palermo DH)"),
   _IT_FMQ ( 0,15,  2,15,     dA, "AMD Athlon 64 (Venice DH)"),
   _IT_FM  ( 0,15,  2,15,         "AMD Athlon 64 (Venice/Palermo DH)"),
   _IT_FMS ( 0,15,  4, 1,  2,     "AMD Dual-Core Opteron (Santa Rosa JH-F2)"),
   _IT_FMS ( 0,15,  4, 1,  3,     "AMD Dual-Core Opteron (Santa Rosa JH-F3)"),
   _IT_FM  ( 0,15,  4, 1,         "AMD Dual-Core Opteron (Santa Rosa)"),
   _IT_FMSQ( 0,15,  4, 3,  2, DO, "AMD Dual-Core Opteron (Santa Rosa JH-F2)"),
   _IT_FMSQ( 0,15,  4, 3,  2, sO, "AMD Opteron (Santa Rosa JH-F2)"),
   _IT_FMSQ( 0,15,  4, 3,  2, dF, "AMD Athlon 64 FX Dual-Core (Windsor JH-F2)"),
   _IT_FMSQ( 0,15,  4, 3,  2, dA, "AMD Athlon 64 X2 Dual-Core (Windsor JH-F2)"),
   _IT_FMS ( 0,15,  4, 3,  2,     "AMD Athlon 64 (unknown type) (Windsor JH-F2)"),
   _IT_FMSQ( 0,15,  4, 3,  3, DO, "AMD Dual-Core Opteron (Santa Rosa JH-F3)"),
   _IT_FMSQ( 0,15,  4, 3,  3, sO, "AMD Opteron (Santa Rosa JH-F3)"),
   _IT_FMSQ( 0,15,  4, 3,  3, dF, "AMD Athlon 64 FX Dual-Core (Windsor JH-F3)"),
   _IT_FMSQ( 0,15,  4, 3,  3, dA, "AMD Athlon 64 X2 Dual-Core (Windsor JH-F3)"),
   _IT_FMS ( 0,15,  4, 3,  3,     "AMD Athlon 64 (unknown type) (Windsor/Santa Rosa JH-F3)"),
   _IT_FMQ ( 0,15,  4, 3,     DO, "AMD Dual-Core Opteron (Santa Rosa)"),
   _IT_FMQ ( 0,15,  4, 3,     sO, "AMD Opteron (Santa Rosa)"),
   _IT_FMQ ( 0,15,  4, 3,     dF, "AMD Athlon 64 FX Dual-Core (Windsor)"),
   _IT_FMQ ( 0,15,  4, 3,     dA, "AMD Athlon 64 X2 Dual-Core (Windsor)"),
   _IT_FM  ( 0,15,  4, 3,         "AMD Athlon 64 (unknown type) (Windsor/Santa Rosa)"),
   _IT_FMSQ( 0,15,  4, 8,  2, dA, "AMD Athlon 64 X2 Dual-Core (Windsor BH-F2)"),
   _IT_FMSQ( 0,15,  4, 8,  2, Mt, "AMD Turion 64 X2 (Trinidad BH-F2)"),
   _IT_FMSQ( 0,15,  4, 8,  2, MT, "AMD Turion 64 X2 (Taylor BH-F2)"),
   _IT_FMS ( 0,15,  4, 8,  2,     "AMD Athlon 64 (unknown type) (Windsor/Taylor/Trinidad BH-F2)"),
   _IT_FMQ ( 0,15,  4, 8,     dA, "AMD Athlon 64 X2 Dual-Core (Windsor)"),
   _IT_FMQ ( 0,15,  4, 8,     Mt, "AMD Turion 64 X2 (Trinidad)"),
   _IT_FMQ ( 0,15,  4, 8,     MT, "AMD Turion 64 X2 (Taylor)"),
   _IT_FM  ( 0,15,  4, 8,         "AMD Athlon 64 (unknown type) (Windsor/Taylor/Trinidad)"),
   _IT_FMS ( 0,15,  4,11,  2,     "AMD Athlon 64 X2 Dual-Core (Windsor BH-F2)"),
   _IT_FM  ( 0,15,  4,11,         "AMD Athlon 64 X2 Dual-Core (Windsor)"),
   _IT_FMSQ( 0,15,  4,12,  2, MS, "AMD mobile Sempron (Keene BH-F2)"),
   _IT_FMSQ( 0,15,  4,12,  2, dS, "AMD Sempron (Manila BH-F2)"),
   _IT_FMSQ( 0,15,  4,12,  2, Mt, "AMD Turion (Trinidad BH-F2)"),
   _IT_FMSQ( 0,15,  4,12,  2, MT, "AMD Turion (Taylor BH-F2)"),
   _IT_FMSQ( 0,15,  4,12,  2, dA, "AMD Athlon 64 (Orleans BH-F2)"),
   _IT_FMS ( 0,15,  4,12,  2,     "AMD Athlon 64 (unknown type) (Orleans/Manila/Keene/Taylor/Trinidad BH-F2)"),
   _IT_FMQ ( 0,15,  4,12,     MS, "AMD mobile Sempron (Keene)"),
   _IT_FMQ ( 0,15,  4,12,     dS, "AMD Sempron (Manila)"),
   _IT_FMQ ( 0,15,  4,12,     Mt, "AMD Turion (Trinidad)"),
   _IT_FMQ ( 0,15,  4,12,     MT, "AMD Turion (Taylor)"),
   _IT_FMQ ( 0,15,  4,12,     dA, "AMD Athlon 64 (Orleans)"),
   _IT_FM  ( 0,15,  4,12,         "AMD Athlon 64 (unknown type) (Orleans/Manila/Keene/Taylor/Trinidad)"),
   _IT_FMSQ( 0,15,  4,15,  2, MS, "AMD mobile Sempron (Keene DH-F2)"),
   _IT_FMSQ( 0,15,  4,15,  2, dS, "AMD Sempron (Manila DH-F2)"),
   _IT_FMSQ( 0,15,  4,15,  2, dA, "AMD Athlon 64 (Orleans DH-F2)"),
   _IT_FMS ( 0,15,  4,15,  2,     "AMD Athlon 64 (unknown type) (Orleans/Manila/Keene DH-F2)"),
   _IT_FMQ ( 0,15,  4,15,     MS, "AMD mobile Sempron (Keene)"),
   _IT_FMQ ( 0,15,  4,15,     dS, "AMD Sempron (Manila)"),
   _IT_FMQ ( 0,15,  4,15,     dA, "AMD Athlon 64 (Orleans)"),
   _IT_FM  ( 0,15,  4,15,         "AMD Athlon 64 (unknown type) (Orleans/Manila/Keene)"),
   _IT_FMS ( 0,15,  5,13,  3,     "AMD Opteron (Santa Rosa JH-F3)"),
   _IT_FM  ( 0,15,  5,13,         "AMD Opteron (Santa Rosa)"),
   _IT_FMSQ( 0,15,  5,15,  2, MS, "AMD mobile Sempron (Keene DH-F2)"),
   _IT_FMSQ( 0,15,  5,15,  2, dS, "AMD Sempron (Manila DH-F2)"),
   _IT_FMSQ( 0,15,  5,15,  2, dA, "AMD Athlon 64 (Orleans DH-F2)"),
   _IT_FMS ( 0,15,  5,15,  2,     "AMD Athlon 64 (unknown type) (Orleans/Manila/Keene DH-F2)"),
   _IT_FMS ( 0,15,  5,15,  3,     "AMD Athlon 64 (Orleans DH-F3)"),
   _IT_FMQ ( 0,15,  5,15,     MS, "AMD mobile Sempron (Keene)"),
   _IT_FMQ ( 0,15,  5,15,     dS, "AMD Sempron (Manila)"),
   _IT_FMQ ( 0,15,  5,15,     dA, "AMD Athlon 64 (Orleans)"),
   _IT_FM  ( 0,15,  5,15,         "AMD Athlon 64 (unknown type) (Orleans/Manila/Keene)"),
   _IT_FM  ( 0,15,  5,15,         "AMD Athlon 64 (Orleans)"),
   _IT_FMS ( 0,15,  6, 8,  1,     "AMD Turion 64 X2 (Tyler BH-G1)"),
   _IT_FMSQ( 0,15,  6, 8,  2, MT, "AMD Turion 64 X2 (Tyler BH-G2)"),
   _IT_FMSQ( 0,15,  6, 8,  2, dS, "AMD Sempron Dual-Core (Tyler BH-G2)"),
   _IT_FMS ( 0,15,  6, 8,  2,     "AMD Turion 64 (unknown type) (Tyler BH-G2)"),
   _IT_FMQ ( 0,15,  6, 8,     MT, "AMD Turion 64 X2 (Tyler)"),
   _IT_FMQ ( 0,15,  6, 8,     dS, "AMD Sempron Dual-Core (Tyler)"),
   _IT_FM  ( 0,15,  6, 8,         "AMD Turion 64 (unknown type) (Tyler)"),
   _IT_FMSQ( 0,15,  6,11,  1, dS, "AMD Sempron Dual-Core (Sparta BH-G1)"),
   _IT_FMSQ( 0,15,  6,11,  1, dA, "AMD Athlon 64 X2 Dual-Core (Brisbane BH-G1)"),
   _IT_FMS ( 0,15,  6,11,  1,     "AMD Athlon 64 (unknown type) (Brisbane/Sparta BH-G1)"),
   _IT_FMSQ( 0,15,  6,11,  2, dA, "AMD Athlon 64 X2 Dual-Core (Brisbane BH-G2)"),
   _IT_FMSQ( 0,15,  6,11,  2, Mn, "AMD Turion Neo X2 Dual-Core (Huron BH-G2)"),
   _IT_FMSQ( 0,15,  6,11,  2, MN, "AMD Athlon Neo X2 (Huron BH-G2)"),
   _IT_FMS ( 0,15,  6,11,  2,     "AMD Athlon 64 (unknown type) (Brisbane/Huron BH-G2)"),
   _IT_FMQ ( 0,15,  6,11,     dS, "AMD Sempron Dual-Core (Sparta)"),
   _IT_FMQ ( 0,15,  6,11,     Mn, "AMD Turion Neo X2 Dual-Core (Huron)"),
   _IT_FMQ ( 0,15,  6,11,     MN, "AMD Athlon Neo X2 (Huron)"),
   _IT_FMQ ( 0,15,  6,11,     dA, "AMD Athlon 64 X2 Dual-Core (Brisbane)"),
   _IT_FM  ( 0,15,  6,11,         "AMD Athlon 64 (unknown type) (Brisbane/Sparta/Huron)"),
   _IT_FMSQ( 0,15,  6,12,  2, MS, "AMD mobile Sempron (Sherman DH-G2)"),
   _IT_FMSQ( 0,15,  6,12,  2, dS, "AMD Sempron (Sparta DH-G2)"),
   _IT_FMSQ( 0,15,  6,12,  2, dA, "AMD Athlon 64 (Lima DH-G2)"),
   _IT_FMS ( 0,15,  6,12,  2,     "AMD Athlon 64 (unknown type) (Lima/Sparta/Sherman DH-G2)"),
   _IT_FMQ ( 0,15,  6,12,     MS, "AMD mobile Sempron (Sherman)"),
   _IT_FMQ ( 0,15,  6,12,     dS, "AMD Sempron (Sparta)"),
   _IT_FMQ ( 0,15,  6,12,     dA, "AMD Athlon 64 (Lima)"),
   _IT_FM  ( 0,15,  6,12,         "AMD Athlon 64 (unknown type) (Lima/Sparta/Sherman)"),
   _IT_FMSQ( 0,15,  6,15,  2, MS, "AMD mobile Sempron (Sherman DH-G2)"),
   _IT_FMSQ( 0,15,  6,15,  2, dS, "AMD Sempron (Sparta DH-G2)"),
   _IT_FMSQ( 0,15,  6,15,  2, MN, "AMD Athlon Neo (Huron DH-G2)"),
   _IT_FMS ( 0,15,  6,15,  2,     "AMD Athlon Neo (unknown type) (Huron/Sparta/Sherman DH-G2)"),
   _IT_FMQ ( 0,15,  6,15,     MS, "AMD mobile Sempron (Sherman)"),
   _IT_FMQ ( 0,15,  6,15,     dS, "AMD Sempron (Sparta)"),
   _IT_FMQ ( 0,15,  6,15,     MN, "AMD Athlon Neo (Huron)"),
   _IT_FM  ( 0,15,  6,15,         "AMD Athlon Neo (unknown type) (Huron/Sparta/Sherman)"),
   _IT_FMSQ( 0,15,  7,12,  2, MS, "AMD mobile Sempron (Sherman DH-G2)"),
   _IT_FMSQ( 0,15,  7,12,  2, dS, "AMD Sempron (Sparta DH-G2)"),
   _IT_FMSQ( 0,15,  7,12,  2, dA, "AMD Athlon (Lima DH-G2)"),
   _IT_FMS ( 0,15,  7,12,  2,     "AMD Athlon (unknown type) (Lima/Sparta/Sherman DH-G2)"),
   _IT_FMQ ( 0,15,  7,12,     MS, "AMD mobile Sempron (Sherman)"),
   _IT_FMQ ( 0,15,  7,12,     dS, "AMD Sempron (Sparta)"),
   _IT_FMQ ( 0,15,  7,12,     dA, "AMD Athlon (Lima)"),
   _IT_FM  ( 0,15,  7,12,         "AMD Athlon (unknown type) (Lima/Sparta/Sherman)"),
   _IT_FMSQ( 0,15,  7,15,  1, MS, "AMD mobile Sempron (Sherman DH-G1)"),
   _IT_FMSQ( 0,15,  7,15,  1, dS, "AMD Sempron (Sparta DH-G1)"),
   _IT_FMSQ( 0,15,  7,15,  1, dA, "AMD Athlon 64 (Lima DH-G1)"),
   _IT_FMS ( 0,15,  7,15,  1,     "AMD Athlon 64 (unknown type) (Lima/Sparta/Sherman DH-G1)"),
   _IT_FMSQ( 0,15,  7,15,  2, MS, "AMD mobile Sempron (Sherman DH-G2)"),
   _IT_FMSQ( 0,15,  7,15,  2, dS, "AMD Sempron (Sparta DH-G2)"),
   _IT_FMSQ( 0,15,  7,15,  2, MN, "AMD Athlon Neo (Huron DH-G2)"),
   _IT_FMSQ( 0,15,  7,15,  2, dA, "AMD Athlon 64 (Lima DH-G2)"),
   _IT_FMS ( 0,15,  7,15,  2,     "AMD Athlon 64 (unknown type) (Lima/Sparta/Sherman/Huron DH-G2)"),
   _IT_FMQ ( 0,15,  7,15,     MS, "AMD mobile Sempron (Sherman)"),
   _IT_FMQ ( 0,15,  7,15,     dS, "AMD Sempron (Sparta)"),
   _IT_FMQ ( 0,15,  7,15,     MN, "AMD Athlon Neo (Huron)"),
   _IT_FMQ ( 0,15,  7,15,     dA, "AMD Athlon 64 (Lima)"),
   _IT_FM  ( 0,15,  7,15,         "AMD Athlon 64 (unknown type) (Lima/Sparta/Sherman/Huron)"),
   _IT_FMS ( 0,15, 12, 1,  3,     "AMD Athlon 64 FX Dual-Core (Windsor JH-F3)"),
   _IT_FM  ( 0,15, 12, 1,         "AMD Athlon 64 FX Dual-Core (Windsor)"),
   _IT_F   ( 0,15,                "AMD (unknown model)"),
   _IT_FMS ( 1,15,  0, 0,  0,     "AMD (unknown type) (Barcelona DR-A0)"), // sandpile.org
   _IT_FMS ( 1,15,  0, 0,  1,     "AMD (unknown type) (Barcelona DR-A1)"), // sandpile.org
   _IT_FMS ( 1,15,  0, 0,  2,     "AMD (unknown type) (Barcelona DR-A2)"), // sandpile.org
   _IT_FMS ( 1,15,  0, 2,  0,     "AMD (unknown type) (Barcelona DR-B0)"), // sandpile.org
   _IT_FMSQ( 1,15,  0, 2,  1, sO, "AMD Quad-Core Opteron (Barcelona DR-B1)"),
   _IT_FMS ( 1,15,  0, 2,  1,     "AMD (unknown type) (Barcelona DR-B1)"),
   _IT_FMSQ( 1,15,  0, 2,  2, EO, "AMD Embedded Opteron (Barcelona DR-B2)"),
   _IT_FMSQ( 1,15,  0, 2,  2, sO, "AMD Quad-Core Opteron (Barcelona DR-B2)"),
   _IT_FMSQ( 1,15,  0, 2,  2, Tp, "AMD Phenom Triple-Core (Toliman DR-B2)"),
   _IT_FMSQ( 1,15,  0, 2,  2, Qp, "AMD Phenom Quad-Core (Agena DR-B2)"),
   _IT_FMS ( 1,15,  0, 2,  2,     "AMD (unknown type) (Barcelona/Toliman/Agena DR-B2)"),
   _IT_FMSQ( 1,15,  0, 2,  3, EO, "AMD Embedded Opteron (Barcelona DR-B3)"),
   _IT_FMSQ( 1,15,  0, 2,  3, sO, "AMD Quad-Core Opteron (Barcelona DR-B3)"),
   _IT_FMSQ( 1,15,  0, 2,  3, Tp, "AMD Phenom Triple-Core (Toliman DR-B3)"),
   _IT_FMSQ( 1,15,  0, 2,  3, Qp, "AMD Phenom Quad-Core (Agena DR-B3)"),
   _IT_FMSQ( 1,15,  0, 2,  3, dA, "AMD Athlon Dual-Core (Kuma DR-B3)"),
   _IT_FMS ( 1,15,  0, 2,  3,     "AMD (unknown type) (Barcelona/Toliman/Agena/Kuma DR-B3)"),
   _IT_FMS ( 1,15,  0, 2, 10,     "AMD Quad-Core Opteron (Barcelona DR-BA)"),
   _IT_FMQ ( 1,15,  0, 2,     EO, "AMD Embedded Opteron (Barcelona)"),
   _IT_FMQ ( 1,15,  0, 2,     sO, "AMD Quad-Core Opteron (Barcelona)"),
   _IT_FMQ ( 1,15,  0, 2,     Tp, "AMD Phenom Triple-Core (Toliman)"),
   _IT_FMQ ( 1,15,  0, 2,     Qp, "AMD Phenom Quad-Core (Agena)"),
   _IT_FMQ ( 1,15,  0, 2,     dA, "AMD Athlon Dual-Core (Kuma)"),
   _IT_FM  ( 1,15,  0, 2,         "AMD (unknown type) (Barcelona/Toliman/Agena/Kuma)"),
   _IT_FMS ( 1,15,  0, 4,  0,     "AMD Athlon (unknown type) (Regor/Propus/Shanghai/Callisto/Heka/Deneb RB-C0)"), // sandpile.org
   _IT_FMS ( 1,15,  0, 4,  1,     "AMD Athlon (unknown type) (Regor/Propus/Shanghai/Callisto/Heka/Deneb RB-C1)"), // sandpile.org
   _IT_FMSQ( 1,15,  0, 4,  2, EO, "AMD Embedded Opteron (Shanghai RB-C2)"),
   _IT_FMSQ( 1,15,  0, 4,  2, sO, "AMD Quad-Core Opteron (Shanghai RB-C2)"),
   _IT_FMSQ( 1,15,  0, 4,  2, dr, "AMD Athlon Dual-Core (Propus RB-C2)"),
   _IT_FMSQ( 1,15,  0, 4,  2, dA, "AMD Athlon Dual-Core (Regor RB-C2)"),
   _IT_FMSQ( 1,15,  0, 4,  2, Dp, "AMD Phenom II X2 (Callisto RB-C2)"),
   _IT_FMSQ( 1,15,  0, 4,  2, Tp, "AMD Phenom II X3 (Heka RB-C2)"),
   _IT_FMSQ( 1,15,  0, 4,  2, Qp, "AMD Phenom II X4 (Deneb RB-C2)"),
   _IT_FMS ( 1,15,  0, 4,  2,     "AMD Athlon (unknown type) (Regor/Propus/Shanghai/Callisto/Heka/Deneb RB-C2)"),
   _IT_FMSQ( 1,15,  0, 4,  3, Dp, "AMD Phenom II X2 (Callisto RB-C3)"),
   _IT_FMSQ( 1,15,  0, 4,  3, Tp, "AMD Phenom II X3 (Heka RB-C3)"),
   _IT_FMSQ( 1,15,  0, 4,  3, Qp, "AMD Phenom II X4 (Deneb RB-C3)"),
   _IT_FMS ( 1,15,  0, 4,  3,     "AMD Phenom II (unknown type) (Callisto/Heka/Deneb RB-C3)"),
   _IT_FMQ ( 1,15,  0, 4,     EO, "AMD Embedded Opteron (Shanghai)"),
   _IT_FMQ ( 1,15,  0, 4,     sO, "AMD Quad-Core Opteron (Shanghai)"),
   _IT_FMQ ( 1,15,  0, 4,     dr, "AMD Athlon Dual-Core (Propus)"),
   _IT_FMQ ( 1,15,  0, 4,     dA, "AMD Athlon Dual-Core (Regor)"),
   _IT_FMQ ( 1,15,  0, 4,     Dp, "AMD Phenom II X2 (Callisto)"),
   _IT_FMQ ( 1,15,  0, 4,     Tp, "AMD Phenom II X3 (Heka)"),
   _IT_FMQ ( 1,15,  0, 4,     Qp, "AMD Phenom II X4 (Deneb)"),
   _IT_FM  ( 1,15,  0, 4,         "AMD Athlon (unknown type) (Regor/Propus/Shanghai/Callisto/Heka/Deneb)"),
   _IT_FMS ( 1,15,  0, 5,  0,     "AMD Athlon (unknown type) (Regor/Rana/Propus BL-C0)"), // sandpile.org
   _IT_FMS ( 1,15,  0, 5,  1,     "AMD Athlon (unknown type) (Regor/Rana/Propus BL-C1)"), // sandpile.org
   _IT_FMSQ( 1,15,  0, 5,  2, DA, "AMD Athlon II X2 (Regor BL-C2)"),
   _IT_FMSQ( 1,15,  0, 5,  2, TA, "AMD Athlon II X3 (Rana BL-C2)"),
   _IT_FMSQ( 1,15,  0, 5,  2, QA, "AMD Athlon II X4 (Propus BL-C2)"),
   _IT_FMS ( 1,15,  0, 5,  2,     "AMD Athlon (unknown type) (Regor/Rana/Propus BL-C2)"),
   _IT_FMSQ( 1,15,  0, 5,  3, TA, "AMD Athlon II X3 (Rana BL-C3)"),
   _IT_FMSQ( 1,15,  0, 5,  3, QA, "AMD Athlon II X4 (Propus BL-C3)"),
   _IT_FMSQ( 1,15,  0, 5,  3, Tp, "AMD Phenom II Triple-Core (Heka BL-C3)"),
   _IT_FMSQ( 1,15,  0, 5,  3, Qp, "AMD Phenom II Quad-Core (Deneb BL-C3)"),
   _IT_FMS ( 1,15,  0, 5,  3,     "AMD Athlon (unknown type) (Regor/Rana/Propus/Callisto/Heka/Deneb BL-C3)"),
   _IT_FMQ ( 1,15,  0, 5,     DA, "AMD Athlon II X2 (Regor)"),
   _IT_FMQ ( 1,15,  0, 5,     TA, "AMD Athlon II X3 (Rana)"),
   _IT_FMQ ( 1,15,  0, 5,     QA, "AMD Athlon II X4 (Propus)"),
   _IT_FMQ ( 1,15,  0, 5,     Tp, "AMD Phenom II Triple-Core (Heka)"),
   _IT_FMQ ( 1,15,  0, 5,     Qp, "AMD Phenom II Quad-Core (Deneb)"),
   _IT_FM  ( 1,15,  0, 5,         "AMD Athlon (unknown type) (Regor/Rana/Propus/Callisto/Heka/Deneb)"),
   _IT_FMS ( 1,15,  0, 6,  0,     "AMD Athlon (unknown type) (Regor/Sargas/Caspain DA-C0)"),
   _IT_FMS ( 1,15,  0, 6,  1,     "AMD Athlon (unknown type) (Regor/Sargas/Caspain DA-C1)"),
   _IT_FMSQ( 1,15,  0, 6,  2, MS, "AMD Sempron Mobile (Sargas DA-C2)"),
   _IT_FMSQ( 1,15,  0, 6,  2, dS, "AMD Sempron II (Sargas DA-C2)"),
   _IT_FMSQ( 1,15,  0, 6,  2, MT, "AMD Turion II Dual-Core Mobile (Caspian DA-C2)"),
   _IT_FMSQ( 1,15,  0, 6,  2, MA, "AMD Athlon II Dual-Core Mobile (Regor DA-C2)"),
   _IT_FMSQ( 1,15,  0, 6,  2, DA, "AMD Athlon II X2 (Regor DA-C2)"),
   _IT_FMSQ( 1,15,  0, 6,  2, dA, "AMD Athlon II (Sargas DA-C2)"),
   _IT_FMS ( 1,15,  0, 6,  2,     "AMD Athlon (unknown type) (Regor/Sargas/Caspain DA-C2)"),
   _IT_FMSQ( 1,15,  0, 6,  3, Ms, "AMD V-Series Mobile (Champlain DA-C3)"),
   _IT_FMSQ( 1,15,  0, 6,  3, DS, "AMD Sempron II X2 (Regor DA-C3)"),
   _IT_FMSQ( 1,15,  0, 6,  3, dS, "AMD Sempron II (Sargas DA-C3)"),
   _IT_FMSQ( 1,15,  0, 6,  3, MT, "AMD Turion II Dual-Core Mobile (Champlain DA-C3)"),
   _IT_FMSQ( 1,15,  0, 6,  3, Mp, "AMD Phenom II Dual-Core Mobile (Champlain DA-C3)"),
   _IT_FMSQ( 1,15,  0, 6,  3, MA, "AMD Athlon II Dual-Core Mobile (Champlain DA-C3)"),
   _IT_FMSQ( 1,15,  0, 6,  3, DA, "AMD Athlon II X2 (Regor DA-C3)"),
   _IT_FMSQ( 1,15,  0, 6,  3, dA, "AMD Athlon II (Sargas DA-C3)"),
   _IT_FMS ( 1,15,  0, 6,  3,     "AMD Athlon (unknown type) (Regor/Sargas/Champlain DA-C3)"),
   _IT_FMQ ( 1,15,  0, 6,     Ms, "AMD V-Series Mobile (Champlain)"),
   _IT_FMQ ( 1,15,  0, 6,     MS, "AMD Sempron Mobile (Sargas)"),
   _IT_FMQ ( 1,15,  0, 6,     DS, "AMD Sempron II X2 (Regor)"),
   _IT_FMQ ( 1,15,  0, 6,     dS, "AMD Sempron II (Sargas)"),
   _IT_FMQ ( 1,15,  0, 6,     MT, "AMD Turion II Dual-Core Mobile (Caspian / Champlain)"),
   _IT_FMQ ( 1,15,  0, 6,     Mp, "AMD Phenom II Dual-Core Mobile (Champlain)"),
   _IT_FMQ ( 1,15,  0, 6,     MA, "AMD Athlon II Dual-Core Mobile (Regor / Champlain)"),
   _IT_FMQ ( 1,15,  0, 6,     DA, "AMD Athlon II X2 (Regor)"),
   _IT_FMQ ( 1,15,  0, 6,     dA, "AMD Athlon II (Sargas)"),
   _IT_FM  ( 1,15,  0, 6,         "AMD Athlon (unknown type) (Regor/Sargas/Caspian/Champlain)"),
   _IT_FMSQ( 1,15,  0, 8,  0, SO, "AMD Six-Core Opteron (Istanbul HY-D0)"),
   _IT_FMSQ( 1,15,  0, 8,  0, sO, "AMD Opteron 4100 (Lisbon HY-D0)"),
   _IT_FMS ( 1,15,  0, 8,  0,     "AMD Opteron (unknown type) (Lisbon/Istanbul HY-D0)"),
   _IT_FMS ( 1,15,  0, 8,  1,     "AMD Opteron 4100 (Lisbon HY-D1)"),
   _IT_FMQ ( 1,15,  0, 8,     SO, "AMD Six-Core Opteron (Istanbul)"),
   _IT_FMQ ( 1,15,  0, 8,     sO, "AMD Opteron 4100 (Lisbon)"),
   _IT_FM  ( 1,15,  0, 8,         "AMD Opteron (unknown type) (Lisbon/Istanbul)"),
   _IT_FMS ( 1,15,  0, 9,  0,     "AMD Opteron 6100 (Magny-Cours HY-D0)"), // sandpile.org
   _IT_FMS ( 1,15,  0, 9,  1,     "AMD Opteron 6100 (Magny-Cours HY-D1)"),
   _IT_FM  ( 1,15,  0, 9,         "AMD Opteron 6100 (Magny-Cours)"),
   _IT_FMSQ( 1,15,  0,10,  0, Qp, "AMD Phenom II X4 (Zosma PH-E0)"),
   _IT_FMSQ( 1,15,  0,10,  0, Sp, "AMD Phenom II X6 (Thuban PH-E0)"),
   _IT_FMS ( 1,15,  0,10,  0,     "AMD Phenom II (unknown type) (Zosma/Thuban PH-E0)"),
   _IT_FMQ ( 1,15,  0,10,     Qp, "AMD Phenom II X4 (Zosma)"),
   _IT_FMQ ( 1,15,  0,10,     Sp, "AMD Phenom II X6 (Thuban)"),
   _IT_FM  ( 1,15,  0,10,         "AMD Phenom II (unknown type) (Zosma/Thuban)"),
   _IT_F   ( 1,15,                "AMD (unknown model)"),
   _IT_FMSQ( 2,15,  0, 3,  1, MU, "AMD Turion X2 Ultra Dual-Core Mobile (Griffin LG-B1)"),
   _IT_FMSQ( 2,15,  0, 3,  1, MT, "AMD Turion X2 Dual-Core Mobile (Lion LG-B1)"),
   _IT_FMSQ( 2,15,  0, 3,  1, DS, "AMD Sempron X2 Dual-Core (Sable LG-B1)"),
   _IT_FMSQ( 2,15,  0, 3,  1, dS, "AMD Sempron (Sable LG-B1)"),
   _IT_FMSQ( 2,15,  0, 3,  1, DA, "AMD Athlon X2 Dual-Core (Lion LG-B1)"),
   _IT_FMSQ( 2,15,  0, 3,  1, dA, "AMD Athlon (Lion LG-B1)"),
   _IT_FMS ( 2,15,  0, 3,  1,     "AMD Athlon (unknown type) (Lion/Sable LG-B1)"),
   _IT_FMQ ( 2,15,  0, 3,     MU, "AMD Turion X2 Ultra (Griffin)"),
   _IT_FMQ ( 2,15,  0, 3,     MT, "AMD Turion X2 (Lion)"),
   _IT_FMQ ( 2,15,  0, 3,     DS, "AMD Sempron X2 Dual-Core (Sable)"),
   _IT_FMQ ( 2,15,  0, 3,     dS, "AMD Sempron (Sable)"),
   _IT_FMQ ( 2,15,  0, 3,     DA, "AMD Athlon X2 Dual-Core (Lion)"),
   _IT_FMQ ( 2,15,  0, 3,     dA, "AMD Athlon (Lion)"),
   _IT_FM  ( 2,15,  0, 3,         "AMD Athlon (unknown type) (Lion/Sable)"),
   _IT_F   ( 2,15,                "AMD (unknown model)"),
   _IT_FMS ( 3,15,  0, 0,  0,     "AMD Athlon (unknown type) (Llano LN-A0)"), // sandpile.org
   _IT_FMS ( 3,15,  0, 0,  1,     "AMD Athlon (unknown type) (Llano LN-A1)"), // sandpile.org
   _IT_FMSQ( 3,15,  0, 1,  0, dS, "AMD Sempron Dual-Core (Llano LN-B0)"),
   _IT_FMSQ( 3,15,  0, 1,  0, dA, "AMD Athlon II Dual-Core (Llano LN-B0)"),
   _IT_FMSQ( 3,15,  0, 1,  0, Sa, "AMD A-Series (Llano LN-B0)"),
   _IT_FMSQ( 3,15,  0, 1,  0, Se, "AMD E2-Series (Llano LN-B0)"),
   _IT_FMS ( 3,15,  0, 1,  0,     "AMD Athlon (unknown type) (Llano LN-B0)"),
   _IT_FMQ ( 3,15,  0, 1,     dS, "AMD Sempron Dual-Core (Llano)"),
   _IT_FMQ ( 3,15,  0, 1,     dA, "AMD Athlon II Dual-Core (Llano)"),
   _IT_FMQ ( 3,15,  0, 1,     Sa, "AMD A-Series (Llano)"),
   _IT_FMQ ( 3,15,  0, 1,     Se, "AMD E2-Series (Llano)"),
   _IT_FM  ( 3,15,  0, 1,         "AMD Athlon (unknown type) (Llano)"),
   _IT_FMS ( 3,15,  0, 2,  0,     "AMD Athlon (unknown type) (Llano LN-B0)"), // sandpile.org
   _IT_F   ( 3,15,                "AMD (unknown model) (Llano)"),
   _IT_FMSQ( 5,15,  0, 1,  0, Sc, "AMD C-Series (Ontario ON-B0)"),
   _IT_FMSQ( 5,15,  0, 1,  0, Se, "AMD E-Series (Zacate ON-B0)"),
   _IT_FMSQ( 5,15,  0, 1,  0, Sg, "AMD G-Series (Ontario/Zacate ON-B0)"),
   _IT_FMSQ( 5,15,  0, 1,  0, Sz, "AMD Z-Series (Desna ON-B0)"),
   _IT_FMS ( 5,15,  0, 1,  0,     "AMD (unknown type) (Ontario/Zacate/Desna ON-B0)"),
   _IT_FM  ( 5,15,  0, 1,         "AMD (unknown type) (Ontario/Zacate/Desna)"),
   _IT_FMSQ( 5,15,  0, 2,  0, Sc, "AMD C-Series (Ontario ON-C0)"),
   _IT_FMSQ( 5,15,  0, 2,  0, Se, "AMD E-Series (Zacate ON-C0)"),
   _IT_FMSQ( 5,15,  0, 2,  0, Sg, "AMD G-Series (Ontario/Zacate ON-C0)"),
   _IT_FMSQ( 5,15,  0, 2,  0, Sz, "AMD Z-Series (Desna ON-C0)"),
   _IT_FMS ( 5,15,  0, 2,  0,     "AMD (unknown type) (Ontario/Zacate/Desna ON-C0)"),
   _IT_FM  ( 5,15,  0, 2,         "AMD (unknown type) (Ontario/Zacate/Desna)"),
   _IT_F   ( 5,15,                "AMD (unknown model)"),
   _IT_FMS ( 6,15,  0, 0,  0,     "AMD (unknown type) (Interlagos/Valencia/Zurich/Zambezi OR-A0)"), // sandpile.org
   _IT_FMS ( 6,15,  0, 0,  1,     "AMD (unknown type) (Interlagos/Valencia/Zurich/Zambezi OR-A1)"), // sandpile.org
   _IT_FMS ( 6,15,  0, 1,  0,     "AMD (unknown type) (Interlagos/Valencia/Zurich/Zambezi OR-B0)"), // sandpile.org
   _IT_FMS ( 6,15,  0, 1,  1,     "AMD (unknown type) (Interlagos/Valencia/Zurich/Zambezi OR-B1)"), // sandpile.org
   _IT_FMSQ( 6,15,  0, 1,  2, sO, "AMD Opteron 6200 (Interlagos OR-B2) / Opteron 4200 (Valencia OR-B2) / Opteron 3200 (Zurich OR-B2)"),
   _IT_FMSQ( 6,15,  0, 1,  2, df, "AMD FX-Series (Zambezi OR-B2)"),
   _IT_FMS ( 6,15,  0, 1,  2,     "AMD (unknown type) (Interlagos/Valencia/Zurich/Zambezi OR-B2)"),
   _IT_FMQ ( 6,15,  0, 1,     sO, "AMD Opteron 6200 (Interlagos) / Opteron 4200 (Valencia) / Opteron 3200 (Zurich)"),
   _IT_FMQ ( 6,15,  0, 1,     df, "AMD FX-Series (Zambezi)"),
   _IT_FM  ( 6,15,  0, 1,         "AMD (unknown type) (Interlagos/Valencia/Zurich/Zambezi)"),
   _IT_FMSQ( 6,15,  0, 2,  0, sO, "AMD Opteron 6300 (Abu Dhabi OR-C0) / Opteron 4300 (Seoul OR-C0) / Opteron 3300 (Delhi OR-C0)"),
   _IT_FMSQ( 6,15,  0, 2,  0, df, "AMD FX-Series (Vishera OR-C0)"),
   _IT_FMS ( 6,15,  0, 2,  0,     "AMD (unknown type) (Abu Dhabi/Seoul/Delhi/Vishera OR-C0)"),
   _IT_FMQ ( 6,15,  0, 2,     sO, "AMD Opteron 6300 (Abu Dhabi) / Opteron 4300 (Seoul) / Opteron 3300 (Delhi)"),
   _IT_FMQ ( 6,15,  0, 2,     df, "AMD FX-Series (Vishera)"),
   _IT_FM  ( 6,15,  0, 2,         "AMD (unknown type) (Abu Dhabi/Seoul/Delhi/Vishera)"),
   _IT_FMSQ( 6,15,  1, 0,  1, Sa, "AMD A-Series (Trinity TN-A1)"),
   _IT_FMSQ( 6,15,  1, 0,  1, Sr, "AMD R-Series (Trinity TN-A1)"),
   _IT_FMSQ( 6,15,  1, 0,  1, dA, "AMD Athlon Dual-Core / Athlon Quad-Core (Trinity TN-A1)"),
   _IT_FMSQ( 6,15,  1, 0,  1, dS, "AMD Sempron Dual-Core (Trinity TN-A1)"),
   _IT_FMSQ( 6,15,  1, 0,  1, dI, "AMD FirePro (Trinity TN-A1)"),
   _IT_FMS ( 6,15,  1, 0,  1,     "AMD (unknown type) (Trinity TN-A1)"),
   _IT_FMQ ( 6,15,  1, 0,     Sa, "AMD A-Series (Trinity)"),
   _IT_FMQ ( 6,15,  1, 0,     Sr, "AMD R-Series (Trinity)"),
   _IT_FMQ ( 6,15,  1, 0,     dA, "AMD Athlon Dual-Core / Athlon Quad-Core (Trinity)"),
   _IT_FMQ ( 6,15,  1, 0,     dS, "AMD Sempron Dual-Core (Trinity)"),
   _IT_FMQ ( 6,15,  1, 0,     dI, "AMD FirePro (Trinity)"),
   _IT_FM  ( 6,15,  1, 0,         "AMD (unknown type) (Trinity TN-A1)"),
   _IT_FMSQ( 6,15,  1, 3,  1, Sa, "AMD A-Series (Richland RL-A1)"),
   _IT_FMSQ( 6,15,  1, 3,  1, Sr, "AMD R-Series (Richland RL-A1)"),
   _IT_FMSQ( 6,15,  1, 3,  1, dA, "AMD Athlon Dual-Core / Athlon Quad-Core (Richland RL-A1)"),
   _IT_FMSQ( 6,15,  1, 3,  1, dS, "AMD Sempron Dual-Core (Richland RL-A1)"),
   _IT_FMSQ( 6,15,  1, 3,  1, dI, "AMD FirePro (Richland RL-A1)"),
   _IT_FMS ( 6,15,  1, 3,  1,     "AMD (unknown type) (Richland RL-A1)"),
   _IT_FMQ ( 6,15,  1, 3,     Sa, "AMD A-Series (Richland)"),
   _IT_FMQ ( 6,15,  1, 3,     Sr, "AMD R-Series (Richland)"),
   _IT_FMQ ( 6,15,  1, 3,     dA, "AMD Athlon Dual-Core / Athlon Quad-Core (Richland)"),
   _IT_FMQ ( 6,15,  1, 3,     dS, "AMD Sempron Dual-Core (Richland)"),
   _IT_FMQ ( 6,15,  1, 3,     dI, "AMD FirePro (Richland)"),
   _IT_FM  ( 6,15,  1, 3,         "AMD (unknown type) (Richland)"),
   _IT_FMS ( 6,15,  3, 0,  0,     "AMD (unknown type) (Kaveri KV-A0)"),
   _IT_FMSQ( 6,15,  3, 0,  1, Sa, "AMD Elite Performance A-Series (Kaveri KV-A1)"),
   _IT_FMSQ( 6,15,  3, 0,  1, Mr, "AMD Mobile R-Series (Kaveri KV-A1)"),
   _IT_FMSQ( 6,15,  3, 0,  1, sO, "AMD Opteron X1200 / X2200 (Kaveri KV-A1)"),
   _IT_FMS ( 6,15,  3, 0,  1,     "AMD (unknown type) (Kaveri KV-A1)"),
   _IT_FMQ ( 6,15,  3, 0,     Sa, "AMD Elite Performance A-Series (Kaveri)"),
   _IT_FMQ ( 6,15,  3, 0,     Mr, "AMD Mobile R-Series (Kaveri)"),
   _IT_FMQ ( 6,15,  3, 0,     sO, "AMD Opteron X1200 / X2200 (Kaveri)"),
   _IT_FM  ( 6,15,  3, 0,         "AMD (unknown type) (Kaveri)"),
   _IT_FMSQ( 6,15,  3, 8,  1, Sa, "AMD A-Series (Godavari A1)"), // sandpile.org
   _IT_FMS ( 6,15,  3, 8,  1,     "AMD (unknown type) (Godavari A1)"), // sandpile.org
   _IT_FMQ ( 6,15,  3, 8,     Sa, "AMD A-Series (Godavari)"),
   _IT_FM  ( 6,15,  3, 8,         "AMD (unknown type) (Godavari)"),
   _IT_FMS ( 6,15,  6, 0,  0,     "AMD (unknown type) (Carrizo/Toronto CZ-A0)"), // sandpile.org
   _IT_FMSQ( 6,15,  6, 0,  1, sO, "AMD Opteron (Toronto CZ-A1)"), // undocumented, but instlatx64 sample
   _IT_FMSQ( 6,15,  6, 0,  1, df, "AMD FX-Series (Carrizo CZ-A1)"), // undocumented, but instlatx64 sample
   _IT_FMS ( 6,15,  6, 0,  1,     "AMD (unknown type) (Carrizo/Toronto CZ-A1)"), // undocumented, but instlatx64 sample
   _IT_FMQ ( 6,15,  6, 0,     sO, "AMD Opteron (Toronto)"), // undocumented, but instlatx64 sample
   _IT_FMQ ( 6,15,  6, 0,     df, "AMD FX-Series (Carrizo)"), // undocumented, but instlatx64 sample
   _IT_FM  ( 6,15,  6, 0,         "AMD (unknown type) (Carrizo/Toronto)"), // undocumented, but instlatx64 sample
   _IT_FMSQ( 6,15,  6, 5,  1, Sa, "AMD A-Series (Carrizo/Bristol Ridge/Stoney Ridge CZ-A1/BR-A1)"), // undocumented, but samples from Alexandros Couloumbis & instlatx64; sandpile.org stepping
   _IT_FMSQ( 6,15,  6, 5,  1, Se, "AMD E-Series (Stoney Ridge CZ-A1/BR-A1)"), // undocumented; sandpile.org stepping
   _IT_FMSQ( 6,15,  6, 5,  1, Sg, "AMD G-Series (Brown Falcon/Prairie Falcon CZ-A1/BR-A1)"), // undocumented; sandpile.org stepping
   _IT_FMSQ( 6,15,  6, 5,  1, Sr, "AMD R-Series (Merlin Falcon CZ-A1/BR-A1)"), // undocumented; sandpile.org stepping
   _IT_FMS ( 6,15,  6, 5,  1,     "AMD (unknown type) (Carrizo/Bristol Ridge/Stoney Ridge/Toronto/Brown Falcon/Merlin Falcon/Prairie Falcon CZ-A1/BR-A1)"), // sandpile.org
   _IT_FMQ ( 6,15,  6, 5,     Sa, "AMD A-Series (Carrizo/Bristol Ridge/Stoney Ridge)"), // undocumented, but samples from Alexandros Couloumbis & instlatx64
   _IT_FMQ ( 6,15,  6, 5,     Se, "AMD E-Series (Stoney Ridge)"), // undocumented
   _IT_FMQ ( 6,15,  6, 5,     Sg, "AMD G-Series (Brown Falcon/Prairie Falcon)"), // undocumented
   _IT_FMQ ( 6,15,  6, 5,     Sr, "AMD R-Series (Merlin Falcon)"), // undocumented
   _IT_FM  ( 6,15,  6, 5,         "AMD (unknown type) (Carrizo/Bristol Ridge/Stoney Ridge/Toronto/Brown Falcon/Merlin Falcon/Prairie Falcon)"), // undocumented, but sample from Alexandros Couloumbis
   _IT_FMSQ( 6,15,  7, 0,  0, Sa, "AMD A-Series (Carrizo/Bristol Ridge/Stoney Ridge ST-A0)"),
   _IT_FMSQ( 6,15,  7, 0,  0, Se, "AMD E-Series (Stoney Ridge ST-A0)"),
   _IT_FMSQ( 6,15,  7, 0,  0, Sg, "AMD G-Series (Brown Falcon/Prairie Falcon ST-A0)"),
   _IT_FMSQ( 6,15,  7, 0,  0, Sr, "AMD R-Series (Merlin Falcon ST-A0)"),
   _IT_FMS ( 6,15,  7, 0,  0,     "AMD (unknown type) (Carrizo/Bristol Ridge/Stoney Ridge/Toronto/Brown Falcon/Merlin Falcon/Prairie Falcon ST-A0)"),
   _IT_FMQ ( 6,15,  7, 0,     Sa, "AMD A-Series (Carrizo/Bristol Ridge/Stoney Ridge)"),
   _IT_FMQ ( 6,15,  7, 0,     Se, "AMD E-Series (Stoney Ridge)"),
   _IT_FMQ ( 6,15,  7, 0,     Sg, "AMD G-Series (Brown Falcon/Prairie Falcon)"),
   _IT_FMQ ( 6,15,  7, 0,     Sr, "AMD R-Series (Merlin Falcon)"),
   _IT_FM  ( 6,15,  7, 0,         "AMD (unknown type) (Carrizo/Bristol Ridge/Stoney Ridge/Toronto/Brown Falcon/Merlin Falcon/Prairie Falcon)"),
   _IT_F   ( 6,15,                "AMD (unknown model)"),
   _IT_FMS ( 7,15,  0, 0,  0,     "AMD (unknown type) (Kabini/Temash/Kyoto KB-A0)"), // sandpile.org
   _IT_FMSQ( 7,15,  0, 0,  1, dA, "AMD Athlon (Kabini KB-A1)"),
   _IT_FMSQ( 7,15,  0, 0,  1, Sa, "AMD A-Series (Kabini/Temash KB-A1)"),
   _IT_FMSQ( 7,15,  0, 0,  1, Se, "AMD E-Series (Kabini KB-A1)"),
   _IT_FMSQ( 7,15,  0, 0,  1, Sg, "AMD G-Series (Kabini KB-A1)"),
   _IT_FMSQ( 7,15,  0, 0,  1, sO, "AMD Opteron X1100/X2100 Series (Kyoto KB-A1)"),
   _IT_FMS ( 7,15,  0, 0,  1,     "AMD (unknown type) (Kabini/Temash/Kyoto KB-A1)"),
   _IT_FMQ ( 7,15,  0, 0,     dA, "AMD Athlon (Kabini)"),
   _IT_FMQ ( 7,15,  0, 0,     Sa, "AMD A-Series (Kabini/Temash)"),
   _IT_FMQ ( 7,15,  0, 0,     Se, "AMD E-Series (Kabini)"),
   _IT_FMQ ( 7,15,  0, 0,     Sg, "AMD G-Series (Kabini)"),
   _IT_FMQ ( 7,15,  0, 0,     sO, "AMD Opteron X1100/X2100 Series (Kyoto)"),
   _IT_FM  ( 7,15,  0, 0,         "AMD (unknown type) (Kabini/Temash/Kyoto)"),
   // sandpile.org mentions (7,15),(0,4) Jaguar-esque "BV" cores
   // (with stepping 1 = A1), but I have no idea of any such code name.
   // The AMD docs (53072) omit the CPUID entirely.  But if this sticks to the
   // recent AMD pattern, these must be (7,15),(3,0).
   _IT_FMSQ( 7,15,  3, 0,  1, Sa, "AMD A-Series (Beema ML-A1)"),
   _IT_FMSQ( 7,15,  3, 0,  1, Se, "AMD E-Series (Beema ML-A1)"),
   _IT_FMSQ( 7,15,  3, 0,  1, Ta, "AMD A-Series Micro (Mullins ML-A1)"),
   _IT_FMSQ( 7,15,  3, 0,  1, Te, "AMD E-Series Micro (Mullins ML-A1)"),
   _IT_FMS ( 7,15,  3, 0,  1,     "AMD (unknown type) (Beema/Mullins ML-A1)"),
   _IT_FMQ ( 7,15,  3, 0,     Sa, "AMD A-Series (Beema)"),
   _IT_FMQ ( 7,15,  3, 0,     Se, "AMD E-Series (Beema)"),
   _IT_FMQ ( 7,15,  3, 0,     Ta, "AMD A-Series Micro (Mullins)"),
   _IT_FMQ ( 7,15,  3, 0,     Te, "AMD E-Series Micro (Mullins)"),
   _IT_FM  ( 7,15,  3, 0,         "AMD (unknown type) (Beema/Mullins)"),
   // sandpile.org mentions (7,15),(6,0) Puma-esque "NL" cores
   // (with stepping 1 = A1), but I have no idea of any such code name.
   _IT_F   ( 7,15,                "AMD (unknown model)"),
   _IT_FMS ( 8,15,  0, 0,  1,     "AMD (unknown type) (Summit Ridge/Naples ZP-A1)"), // sandpile.org
   _IT_FMSQ( 8,15,  0, 1,  0, EE, "AMD EPYC (Snowy Owl ZP-B0)"),
   _IT_FMSQ( 8,15,  0, 1,  0, sE, "AMD EPYC (Naples ZP-B0)"),
   _IT_FMSQ( 8,15,  0, 1,  0, dR, "AMD Ryzen (Summit Ridge ZP-B0)"),
   _IT_FMS ( 8,15,  0, 1,  0,     "AMD (unknown type) (Summit Ridge/Naples ZP-B0)"),
   _IT_FMSQ( 8,15,  0, 1,  1, EE, "AMD EPYC (Snowy Owl ZP-B1)"),
   _IT_FMSQ( 8,15,  0, 1,  1, sE, "AMD EPYC (Naples ZP-B1)"),
   _IT_FMSQ( 8,15,  0, 1,  1, dR, "AMD Ryzen (Summit Ridge ZP-B1)"),
   _IT_FMS ( 8,15,  0, 1,  1,     "AMD (unknown type) (Summit Ridge/Naples ZP-B1)"),
   _IT_FMSQ( 8,15,  0, 1,  2, EE, "AMD EPYC (Snowy Owl ZP-B2)"),
   _IT_FMSQ( 8,15,  0, 1,  2, sE, "AMD EPYC (Naples ZP-B2)"),
   _IT_FMSQ( 8,15,  0, 1,  2, dR, "AMD Ryzen (Summit Ridge ZP-B2)"),
   _IT_FMS ( 8,15,  0, 1,  2,     "AMD (unknown type) (Summit Ridge/Naples ZP-B2)"),
   _IT_FMQ ( 8,15,  0, 1,     EE, "AMD EPYC (Snowy Owl)"),
   _IT_FMQ ( 8,15,  0, 1,     sE, "AMD EPYC (Naples)"),
   _IT_FMQ ( 8,15,  0, 1,     dR, "AMD Ryzen (Summit Ridge)"),
   _IT_FM  ( 8,15,  0, 1,         "AMD (unknown type) (Summit Ridge/Naples)"),
   _IT_FMS ( 8,15,  0, 8,  2,     "AMD Ryzen (Pinnacle Ridge PiR-B2)"),
   _IT_FM  ( 8,15,  0, 8,         "AMD Ryzen (Pinnacle Ridge)"),
   _IT_FMS ( 8,15,  1, 0,  1,     "AMD Ryzen (unknown type) (Raven Ridge/Snowy Owl/Great Horned Owl/Banded Kestrel RV-A1)"), // found only on en.wikichip.org & instlatx64 examples; sandpile.org
   _IT_FMSQ( 8,15,  1, 1,  0, ER, "AMD Ryzen Embedded (Great Horned Owl/Banded Kestrel RV-B0)"), // only instlatx64 example; stepping from usual pattern
   _IT_FMSQ( 8,15,  1, 1,  0, AR, "AMD Ryzen (Raven Ridge RV-B0)"), // found only on en.wikichip.org & instlatx64 examples; stepping from usual pattern
   _IT_FMS ( 8,15,  1, 1,  0,     "AMD Ryzen (unknown type) (Raven Ridge/Snowy Owl/Great Horned Owl/Banded Kestrel RV-B0)"), // found only on en.wikichip.org & instlatx64 examples; stepping from usual pattern
   _IT_FMQ ( 8,15,  1, 1,     ER, "AMD Ryzen Embedded (Great Horned Owl/Banded Kestrel)"), // only instlatx64 example
   _IT_FMQ ( 8,15,  1, 1,     AR, "AMD Ryzen (Raven Ridge)"), // found only on en.wikichip.org & instlatx64 examples
   _IT_FM  ( 8,15,  1, 1,         "AMD Ryzen (unknown type) (Raven Ridge/Snowy Owl/Great Horned Owl/Banded Kestrel)"), // found only on en.wikichip.org & instlatx64 examples
   _IT_FM  ( 8,15,  1, 8,         "AMD Ryzen (Picasso)"), // found only on en.wikichip.org
   _IT_FMSQ( 8,15,  3, 1,  0, dR, "AMD Ryzen (Castle Peak B0)"),
   _IT_FMQ ( 8,15,  3, 1,     dR, "AMD Ryzen (Castle Peak)"),
   _IT_FMSQ( 8,15,  3, 1,  0, sE, "AMD EPYC (Rome B0)"),
   _IT_FMQ ( 8,15,  3, 1,     sE, "AMD EPYC (Rome)"),
   _IT_FMS ( 8,15,  3, 1,  0,     "AMD Ryzen (Castle Peak B0) / EPYC (Rome B0)"),
   _IT_FM  ( 8,15,  3, 1,         "AMD Ryzen (Castle Peak) / EPYC (Rome)"),
   _IT_FM  ( 8,15,  5, 0,         "AMD DG02SRTBP4MFA (Fenghuang 15FF)"), // internal model, only instlatx64 example
   _IT_FM  ( 8,15,  6, 0,         "AMD Ryzen (Renoir)"), // undocumented, geekbench.com example (with stepping 1)
   _IT_FMS ( 8,15,  7, 1,  0,     "AMD Ryzen (Matisse B0)"), // undocumented, but samples from Steven Noonan
   _IT_FM  ( 8,15,  7, 1,         "AMD Ryzen (Matisse)"), // undocumented, but samples from Steven Noonan
   _IT_F   ( 8,15,                "AMD (unknown model)"),
   _IT_F   (10,15,                "AMD (unknown model)"), // undocumented, but samples from Steven Noonan

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
x86_amd_uarch[] =
{
_IT_FM  ( 0, 4,  0, 3,         "Am486",         0),
_IT_FM  ( 0, 4,  0, 7,         "Am486",         0),
_IT_FM  ( 0, 4,  0, 8,         "Am486",         0),
_IT_FM  ( 0, 4,  0, 9,         "Am486",         0),
_IT_F   ( 0, 4,                "Am5x86",        0),
_IT_FM  ( 0, 5,  0, 6,         "K6",          ".30um"),
_IT_FM  ( 0, 5,  0, 7,         "K6",          ".25um"), // *p from sandpile.org
_IT_FM  ( 0, 5,  0,13,         "K6",          ".18um"), // *p from sandpile.org
_IT_F   ( 0, 5,                "K6",            0),
_IT_FM  ( 0, 6,  0, 1,         "K7",          ".25um"),
_IT_FM  ( 0, 6,  0, 2,         "K7",          ".18um"),
_IT_F   ( 0, 6,                "K7",            0),
_IT_FMS ( 0,15,  0, 4,  8,     "K8",          "754-pin, .13um"),
_IT_FM  ( 0,15,  0, 4,         "K8",          "940-pin, .13um"),
_IT_FM  ( 0,15,  0, 5,         "K8",          "940-pin, .13um"),
_IT_FM  ( 0,15,  0, 7,         "K8",          "939-pin, .13um"),
_IT_FM  ( 0,15,  0, 8,         "K8",          "754-pin, .13um"),
_IT_FM  ( 0,15,  0,11,         "K8",          "939-pin, .13um"),
_IT_FM  ( 0,15,  0,12,         "K8",          "754-pin, .13um"),
_IT_FM  ( 0,15,  0,14,         "K8",          "754-pin, .13um"),
_IT_FM  ( 0,15,  0,15,         "K8",          "939-pin, .13um"),
_IT_FM  ( 0,15,  1, 4,         "K8",          "754-pin, 90nm"),
_IT_FM  ( 0,15,  1, 5,         "K8",          "940-pin, 90nm"),
_IT_FM  ( 0,15,  1, 7,         "K8",          "939-pin, 90nm"),
_IT_FM  ( 0,15,  1, 8,         "K8",          "754-pin, 90nm"),
_IT_FM  ( 0,15,  1,11,         "K8",          "939-pin, 90nm"),
_IT_FM  ( 0,15,  1,12,         "K8",          "754-pin, 90nm"),
_IT_FM  ( 0,15,  1,15,         "K8",          "939-pin, 90nm"),
_IT_FM  ( 0,15,  2, 1,         "K8",          "940-pin, 90nm"),
_IT_FM  ( 0,15,  2, 3,         "K8",          "939-pin, 90nm"),
_IT_FM  ( 0,15,  2, 4,         "K8",          "754-pin, 90nm"),
_IT_FM  ( 0,15,  2, 5,         "K8",          "940-pin, 90nm"),
_IT_FM  ( 0,15,  2, 7,         "K8",          "939-pin, 90nm"),
_IT_FM  ( 0,15,  2,11,         "K8",          "939-pin, 90nm"),
_IT_FM  ( 0,15,  2,12,         "K8",          "754-pin, 90nm"),
_IT_FM  ( 0,15,  2,15,         "K8",          "939-pin, 90nm"),
_IT_FM  ( 0,15,  4, 1,         "K8",          "90nm"),
_IT_FM  ( 0,15,  4, 3,         "K8",          "90nm"),
_IT_FM  ( 0,15,  4, 8,         "K8",          "90nm"),
_IT_FM  ( 0,15,  4,11,         "K8",          "90nm"),
_IT_FM  ( 0,15,  4,12,         "K8",          "90nm"),
_IT_FM  ( 0,15,  4,15,         "K8",          "90nm"),
_IT_FM  ( 0,15,  5,13,         "K8",          "90nm"),
_IT_FM  ( 0,15,  5,15,         "K8",          "90nm"),
_IT_FM  ( 0,15,  6, 8,         "K8",          "65nm"),
_IT_FM  ( 0,15,  6,11,         "K8",          "65nm"),
_IT_FM  ( 0,15,  6,12,         "K8",          "65nm"),
_IT_FM  ( 0,15,  6,15,         "K8",          "65nm"),
_IT_FM  ( 0,15,  7,12,         "K8",          "65nm"),
_IT_FM  ( 0,15,  7,15,         "K8",          "65nm"),
_IT_FM  ( 0,15, 12, 1,         "K8",          "90nm"),
_IT_FM  ( 1,15,  0, 0,         "K10",         "65nm"), // sandpile.org
_IT_FM  ( 1,15,  0, 2,         "K10",         "65nm"),
_IT_FM  ( 1,15,  0, 4,         "K10",         "45nm"),
_IT_FM  ( 1,15,  0, 5,         "K10",         "45nm"),
_IT_FM  ( 1,15,  0, 6,         "K10",         "45nm"),
_IT_FM  ( 1,15,  0, 8,         "K10",         "45nm"),
_IT_FM  ( 1,15,  0, 9,         "K10",         "45nm"),
_IT_FM  ( 1,15,  0,10,         "K10",         "45nm"),
_IT_F   ( 2,15,                "Puma 2008",   "65nm"),
_IT_F   ( 3,15,                "K10",         "32nm"),
_IT_F   ( 5,15,                "Bobcat",      "40nm"),
_IT_FM  ( 6,15,  0, 0,         "Bulldozer",   "32nm"), // instlatx64 engr sample
_IT_FM  ( 6,15,  0, 1,         "Bulldozer",   "32nm"),
_IT_FM  ( 6,15,  0, 2,         "Piledriver",  "32nm"),
_IT_FM  ( 6,15,  1, 0,         "Piledriver",  "32nm"),
_IT_FM  ( 6,15,  1, 3,         "Piledriver",  "32nm"),
_IT_FM  ( 6,15,  3, 0,         "Steamroller", "28nm"),
_IT_FM  ( 6,15,  3, 8,         "Steamroller", "28nm"),
_IT_FM  ( 6,15,  4, 0,         "Steamroller", "28nm"), // Software Optimization Guide (15h) says it has the same inst latencies as (6,15),(3,x).
_IT_FM  ( 6,15,  6, 0,         "Excavator",   "28nm"), // undocumented, but instlatx64 samples
_IT_FM  ( 6,15,  6, 5,         "Excavator",   "28nm"), // undocumented, but sample from Alexandros Couloumbis
_IT_FM  ( 6,15,  7, 0,         "Excavator",   "28nm"),
_IT_FM  ( 7,15,  0, 0,         "Jaguar",      "28nm"),
_IT_FM  ( 7,15,  3, 0,         "Puma 2014",   "28nm"),
_IT_FM  ( 8,15,  0, 0,         "Zen",         "14nm"), // instlatx64 engr sample
_IT_FM  ( 8,15,  0, 1,         "Zen",         "14nm"),
_IT_FM  ( 8,15,  0, 8,         "Zen+",        "12nm"),
_IT_FM  ( 8,15,  1, 1,         "Zen",         "14nm"), // found only on en.wikichip.org & instlatx64 examples
_IT_FM  ( 8,15,  1, 8,         "Zen+",        "12nm"), // found only on en.wikichip.org
_IT_FM  ( 8,15,  3, 1,         "Zen 2",       "7nm"),  // found only on en.wikichip.org
_IT_FM  ( 8,15,  6, 0,         "Zen 2",       "7nm"),  // undocumented, geekbench.com example
_IT_FM  ( 8,15,  7, 1,         "Zen 2",       "7nm"),  // undocumented, but samples from Steven Noonan
_IT_F   (10,15,                "Zen 3",       "7nm"),  // undocumented, LX*
};

static const struct
{
    const unsigned int ands;
    const unsigned int equel;
    const unsigned int ext1;
    const char *const  str;
}
x86_amd_brand[] =
{
   _IT_FM  (0, 5,  0, 0,     "AMD SSA5 (PR75, PR90, PR100)"),
   _IT_FM  (0, 5,  0, 1,     "AMD 5k86 (PR120, PR133)"),
   _IT_FM  (0, 5,  0, 2,     "AMD 5k86 (PR166)"),
   _IT_FM  (0, 5,  0, 3,     "AMD 5k86 (PR200)"),
   _IT_F   (0, 5,            "AMD 5k86 (unknown model)"),
   _IT_FM  (0, 6,  0, 6,     "AMD K6, .30um"),
   _IT_FM  (0, 6,  0, 7,     "AMD K6 (Little Foot), .25um"),
   _IT_FMS (0, 6,  0, 8,  0, "AMD K6-2 (Chomper A)"),
   _IT_FMS (0, 6,  0, 8, 12, "AMD K6-2 (Chomper A)"),
   _IT_FM  (0, 6,  0, 8,     "AMD K6-2 (Chomper)"),
   _IT_FMS (0, 6,  0, 9,  1, "AMD K6-III (Sharptooth B)"),
   _IT_FM  (0, 6,  0, 9,     "AMD K6-III (Sharptooth)"),
   _IT_FM  (0, 6,  0,13,     "AMD K6-2+, K6-III+"),
   _IT_F   (0, 6,            "AMD K6 (unknown model)"),
   _IT_FM  (0, 7,  0, 1,     "AMD Athlon, .25um"),
   _IT_FM  (0, 7,  0, 2,     "AMD Athlon (K75 / Pluto / Orion), .18um"),
   _IT_FMS (0, 7,  0, 3,  0, "AMD Duron / mobile Duron (Spitfire A0)"),
   _IT_FMS (0, 7,  0, 3,  1, "AMD Duron / mobile Duron (Spitfire A2)"),
   _IT_FM  (0, 7,  0, 3,     "AMD Duron / mobile Duron (Spitfire)"),
   _IT_FMS (0, 7,  0, 4,  2, "AMD Athlon (Thunderbird A4-A7)"),
   _IT_FMS (0, 7,  0, 4,  4, "AMD Athlon (Thunderbird A9)"),
   _IT_FM  (0, 7,  0, 4,     "AMD Athlon (Thunderbird)"),
   _IT_FMS (0, 7,  0, 6,  0, "AMD Athlon / Athlon MP mobile Athlon 4 / mobile Duron (Palomino A0)"),
   _IT_FMS (0, 7,  0, 6,  1, "AMD Athlon / Athlon MP / Duron / mobile Athlon / mobile Duron (Palomino A2)"),
   _IT_FMS (0, 7,  0, 6,  2, "AMD Athlon MP / Athlon XP / Duron / Duron MP / mobile Athlon / mobile Duron (Palomino A5)"),
   _IT_FM  (0, 7,  0, 6,     "AMD Athlon / Athlon MP / Athlon XP / Duron / Duron MP / mobile Athlon / mobile Duron (Palomino)"),
   _IT_FMS (0, 7,  0, 7,  0, "AMD Duron / Duron MP / mobile Duron (Morgan A0)"),
   _IT_FMS (0, 7,  0, 7,  1, "AMD Duron / Duron MP / mobile Duron (Morgan A1)"),
   _IT_FM  (0, 7,  0, 7,     "AMD Duron / Duron MP / mobile Duron (Morgan)"),
   _IT_FMS (0, 7,  0, 8,  0, "AMD Athlon XP / Athlon MP / Sempron / Duron / Duron MP (Thoroughbred A0)"),
   _IT_FMS (0, 7,  0, 8,  1, "AMD Athlon XP / Athlon MP / Sempron / Duron / Duron MP (Thoroughbred B0)"),
   _IT_FM  (0, 7,  0, 8,     "AMD Athlon XP / Athlon MP / Sempron / Duron / Duron MP (Thoroughbred)"),
   _IT_FMS (0, 7,  0,10,  0, "AMD Athlon XP / Athlon MP / Sempron / mobile Athlon XP-M / mobile Athlon XP-M (LV) (Barton A2)"),
   _IT_FM  (0, 7,  0,10,     "AMD Athlon XP / Athlon MP / Sempron / mobile Athlon XP-M / mobile Athlon XP-M (LV) (Barton)"),
   _IT_F   (0, 7,            "AMD Athlon XP / Athlon MP / Sempron / Duron / Duron MP / mobile Athlon / mobile Athlon XP-M / mobile Athlon XP-M (LV) / mobile Duron (unknown model)")
};
#endif
