/**
 * Basic tool library
 * Copyright (C) 2014 kunyang kunyang.yk@gmail.com
 *
 * @file     ky_intrin.h
 * @brief    CPU的加速指令引用
 *
 * @author   kunyang
 * @email    kunyang.yk@gmail.com
 * @version  1.1.0.1
 * @date     2011/05/20
 * @license  GNU General Public License (GPL)
 *
 * Change History :
 *    Date    |  Version  |  Author  |   Description
 * 2011/05/02 | 1.0.0.1   | kunyang  | 创建文件
 * 2015/03/26 | 1.1.0.1   | kunyang  | 修改以体系架构选择加速指令
 */
#ifndef KY_INTRIN
#define KY_INTRIN
#include "ky_main.h"

///< x86架构加速指令
#if kyArchIsX86

#ifdef __AVX__ ///< -mavx	AVX: Advanced Vector   Extensions
#define kyHAS_AVX 1
#include <immintrin.h>
#endif

#ifdef __AVX2__ ///< -mavx2	AVX2
#define kyHAS_AVX2 1
#include <immintrin.h>
#endif

#ifdef __BMI__ ///< -mbmi	BMI: Bit Manipulation   Instruction
#define kyHAS_BMI 1
#include <bmiintrin.h>
#endif

#ifdef __BMI2__ ///< -mbmi2	BMI2
#define kyHAS_BMI2 1
#include <bmi2intrin.h>
#endif

#ifdef __MMX__ ///< -mmmx	MMX: MultiMedia eXtension
#define kyHAS_MMX 1
#include <mmintrin.h>
#endif

#ifdef __SSE__ ///< -msse	SSE: Streaming SIMD Extensions
#define kyHAS_SSE 1
#include <xmmintrin.h>
#endif

#ifdef __SSE2__ ///< -msse2	SSE2
#define kyHAS_SSE2 1
#include <emmintrin.h>
#endif

#ifdef __SSE3__ ///< -msse3	SSE3
#define kyHAS_SSE3 1
#include <pmmintrin.h>
#endif

///< -msse4	SSE4.1, SSE4.2
#ifdef __SSE4_1__
#define kyHAS_SSE4_1 1
#include <smmintrin.h>
#endif

#ifdef __SSE4_2__ ///< -msse4.2	SSE4.2
#define kyHAS_SSE4_2 1
#include <smmintrin.h>
#include <nmmintrin.h>
#endif

#ifdef __SSE4A__ ///< -msse4a	SSE4A
#define kyHAS_SSE4A 1
#include <ammintrin.h>
#endif

#ifdef __F16C__ ///< -mf16c	F16C
#define kyHAS_F16C 1
#include <f16cintrin.h>
#endif

#ifdef __FMA__ ///< -mfma	FMA
#define kyHAS_FMA 1
#include <fmaintrin.h>
#endif

#ifdef __FMA4__ ///< -mfma4	FMA4
#define kyHAS_FMA4 1
#include <x86intrin.h>
#endif

///< -mrdrnd -mfsgsbase	Intel提出的扩展指令.
#ifdef __RDRND__
#define kyHAS_RDRND 1
#include <immintrin.h>
#endif
#ifdef __FSGSBASE__
#define kyHAS_FSGSBASE 1
#include <immintrin.h>
#endif

#ifdef __LWP__ ///< -mlwp	LWP: Light Weight Profiling
#define kyHAS_LWP 1
#include <lwpintrin.h>
#endif

#ifdef __LZCNT__ ///< -mlzcnt	LZCNT
#define kyHAS_LZCNT 1
#include <x86intrin.h>
#endif

#ifdef __3dNOW__ ///< -m3dnow	3DNow!
#define kyHAS_3dNOW 1
#include <mm3dnow.h>
#endif

#ifdef __POPCNT__ ///< -mpopcnt	POPCNT
#define kyHAS_POPCNT 1
#include <popcntintrin.h>
#endif

#ifdef __TBM__ ///< -mtbm	TBM: Trailing bit manipulation.
#define kyHAS_TBM 1
#include <tbmintrin.h>
#endif

#ifdef __SSSE3__ ///< -mssse3	SSSE3
#define kyHAS_SSSE3 1
#include <tmmintrin.h>
#endif

///< -maes -mpclmul	AES, PCLMUL
#ifdef __AES__
#define kyHAS_AES 1
#include <wmmintrin.h>
#endif

#ifdef __PCLMUL__
#define kyHAS_PCLMUL 1
#include <wmmintrin.h>
#endif

#ifdef __XOP__ ///< -mxop	XOP
#define kyHAS_XOP 1
#include <xopintrin.h>
#endif

///< ARM架构加速指令
#elif kyArchIsARM

#endif

#endif // KY_INTRIN

