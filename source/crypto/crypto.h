#ifndef CRYPTO_H
#define CRYPTO_H

#include "ky_define.h"
#include "ky_flags.h"

///! CryptoPlatform 值
#define CryptoOther     0
#define CryptoX86       1
#define CryptoX86_64    2
#define CryptoARM       3
#define CryptoARMThumb  4
#define CryptoARMThumb2 5
#define CryptoARM64     6
#define CryptoAVR       7

#ifndef CryptoPlatform
    #if __AVR__
        #define CryptoPlatform CryptoAVR
    #elif defined(__thumb2__) || defined(_M_ARMT)
        #define CryptoPlatform CryptoARMThumb2
    #elif defined(__thumb__)
        #define CryptoPlatform CryptoARMThumb
    #elif defined(__arm__) || defined(_M_ARM)
        #define CryptoPlatform CryptoARM
    #elif defined(__aarch64__)
        #define CryptoPlatform CryptoARM64
    #elif defined(__i386__) || defined(_M_IX86) || defined(_X86_) || defined(__I86__)
        #define CryptoPlatform CryptoX86
    #elif defined(__amd64__) || defined(_M_X64)
        #define CryptoPlatform CryptoX86_64
    #else
        #define CryptoPlatform CryptoOther
    #endif
#endif

#ifndef CryptoARMUseUMAAL
    #if (CryptoPlatform == CryptoARM) && (__ARM_ARCH >= 6)
        #define CryptoARMUseUMAAL 1
    #elif (CryptoPlatform == CryptoARMThumb2) && (__ARM_ARCH >= 6) && !__ARM_ARCH_7M__
        #define CryptoARMUseUMAAL 1
    #else
        #define CryptoARMUseUMAAL 0
    #endif
#endif

#ifndef CryptoWordSize
    #if CryptoPlatform == CryptoAVR
        #define CryptoWordSize 1
    #elif (CryptoPlatform == CryptoX86_64 || CryptoPlatform == CryptoARM64)
        #define CryptoWordSize 8
    #else
        #define CryptoWordSize 4
    #endif
#endif

#if (CryptoWordSize != 1) && (CryptoWordSize != 4) && (CryptoWordSize != 8)
    #error "Unsupported value for CryptoWordSize"
#endif

#if ((CryptoPlatform == CryptoAVR) && (CryptoWordSize != 1))
    #pragma message ("CryptoWordSize must be 1 for AVR")
    #undef CryptoWordSize
    #define CryptoWordSize 1
#endif

#if ((CryptoPlatform == CryptoARM || \
      CryptoPlatform == CryptoARMThumb || \
       CryptoPlatform ==  CryptoARMThumb2) && \
     (CryptoWordSize != 4))
    #pragma message ("CryptoWordSize must be 4 for ARM")
    #undef CryptoWordSize
    #define CryptoWordSize 4
#endif

#if defined(__SIZEOF_INT128__) || ((__clang_major__ * 100 + __clang_minor__) >= 302)
    #define CryptoSupportINT128 1
#else
    #define CryptoSupportINT128 0
#endif


#if (CryptoWordSize == 1)

typedef uint8_t word_t;
typedef uint16_t dword_t;

#define HIGH_BIT_SET 0x80
#define WORD_BITS 8
#define WORD_BITS_SHIFT 3
#define WORD_BITS_MASK 0x07

#elif (CryptoWordSize == 4)

typedef uint32_t word_t;
typedef uint64_t dword_t;

#define HIGH_BIT_SET 0x80000000
#define WORD_BITS 32
#define WORD_BITS_SHIFT 5
#define WORD_BITS_MASK 0x01F

#elif (CryptoWordSize == 8)

typedef uint64_t word_t;
#if CryptoSupportINT128
typedef unsigned __int128 dword_t;
#endif

#define HIGH_BIT_SET 0x8000000000000000ull
#define WORD_BITS 64
#define WORD_BITS_SHIFT 6
#define WORD_BITS_MASK 0x03F

#endif

#ifndef CryptoRNGMaxTries
    #define CryptoRNGMaxTries 64
#endif

#define CONCATX(a, ...) a ## __VA_ARGS__
#define CONCAT(a, ...) CONCATX(a, __VA_ARGS__)

#define STRX(a) #a
#define STR(a) STRX(a)

#define EVAL(...)  EVAL1(EVAL1(EVAL1(EVAL1(__VA_ARGS__))))
#define EVAL1(...) EVAL2(EVAL2(EVAL2(EVAL2(__VA_ARGS__))))
#define EVAL2(...) EVAL3(EVAL3(EVAL3(EVAL3(__VA_ARGS__))))
#define EVAL3(...) EVAL4(EVAL4(EVAL4(EVAL4(__VA_ARGS__))))
#define EVAL4(...) __VA_ARGS__

#define DEC_1  0
#define DEC_2  1
#define DEC_3  2
#define DEC_4  3
#define DEC_5  4
#define DEC_6  5
#define DEC_7  6
#define DEC_8  7
#define DEC_9  8
#define DEC_10 9
#define DEC_11 10
#define DEC_12 11
#define DEC_13 12
#define DEC_14 13
#define DEC_15 14
#define DEC_16 15
#define DEC_17 16
#define DEC_18 17
#define DEC_19 18
#define DEC_20 19
#define DEC_21 20
#define DEC_22 21
#define DEC_23 22
#define DEC_24 23
#define DEC_25 24
#define DEC_26 25
#define DEC_27 26
#define DEC_28 27
#define DEC_29 28
#define DEC_30 29
#define DEC_31 30
#define DEC_32 31

#define DEC_M(N) CONCAT(DEC_, N)

#define SECOND_ARG(_, val, ...) val
#define SOME_CHECK_0 ~, 0
#define GET_SECOND_ARG(...) SECOND_ARG(__VA_ARGS__, SOME,)
#define SOME_OR_0(N) GET_SECOND_ARG(CONCAT(SOME_CHECK_, N))

#define EMPTY(...)
#define DEFER(...) __VA_ARGS__ EMPTY()

#define REPEAT_NAME_0() REPEAT_0
#define REPEAT_NAME_SOME() REPEAT_SOME
#define REPEAT_0(...)
#define REPEAT_SOME(N, stuff) DEFER(CONCAT(REPEAT_NAME_, SOME_OR_0(DEC_M(N))))()(DEC_M(N), stuff) stuff
#define REPEAT(N, stuff) EVAL(REPEAT_SOME(N, stuff))

#define REPEATM_NAME_0() REPEATM_0
#define REPEATM_NAME_SOME() REPEATM_SOME
#define REPEATM_0(...)
#define REPEATM_SOME(N, macro) macro(N) \
    DEFER(CONCAT(REPEATM_NAME_, SOME_OR_0(DEC_M(N))))()(DEC_M(N), macro)
#define REPEATM(N, macro) EVAL(REPEATM_SOME(N, macro))

#define BITS_TO_WORDS(num_bits) \
    ((num_bits + ((CryptoWordSize * 8) - 1)) / (CryptoWordSize * 8))
#define BITS_TO_BYTES(num_bits) ((num_bits + 7) / 8)

//! ECC支持的曲线标准
#ifndef CryptoSupportSecp160R1
  #define CryptoSupportSecp160R1 1
#endif
#ifndef CryptoSupportSecp192R1
  #define CryptoSupportSecp192R1 1
#endif
#ifndef CryptoSupportSecp224R1
  #define CryptoSupportSecp224R1 1
#endif
#ifndef CryptoSupportSecp256R1
  #define CryptoSupportSecp256R1 1
#endif
#ifndef CryptoSupportSecp256K1
  #define CryptoSupportSecp256K1 1
#endif

#include "crypto_patform_specific.inl"

#if (CryptoWordSize == 1)
    #if CryptoSupportSecp160R1
        #define CryptoMaxWord 21 /* Due to the size of curve_n. */
    #endif
    #if CryptoSupportSecp192R1
        #undef CryptoMaxWord
        #define CryptoMaxWord 24
    #endif
    #if CryptoSupportSecp224R1
        #undef CryptoMaxWord
        #define CryptoMaxWord 28
    #endif
    #if (CryptoSupportSecp256R1 || CryptoSupportSecp256K1)
        #undef CryptoMaxWord
        #define CryptoMaxWord 32
    #endif
#elif (CryptoWordSize == 4)
    #if CryptoSupportSecp160R1
        #define CryptoMaxWord 6 /* Due to the size of curve_n. */
    #endif
    #if CryptoSupportSecp192R1
        #undef CryptoMaxWord
        #define CryptoMaxWord 6
    #endif
    #if CryptoSupportSecp224R1
        #undef CryptoMaxWord
        #define CryptoMaxWord 7
    #endif
    #if (CryptoSupportSecp256R1 || CryptoSupportSecp256K1)
        #undef CryptoMaxWord
        #define CryptoMaxWord 8
    #endif
#elif (CryptoWordSize == 8)
    #if CryptoSupportSecp160R1
        #define CryptoMaxWord 3
    #endif
    #if CryptoSupportSecp192R1
        #undef CryptoMaxWord
        #define CryptoMaxWord 3
    #endif
    #if CryptoSupportSecp224R1
        #undef CryptoMaxWord
        #define CryptoMaxWord 4
    #endif
    #if (CryptoSupportSecp256R1 || CryptoSupportSecp256K1)
        #undef CryptoMaxWord
        #define CryptoMaxWord 4
    #endif
#endif

#ifndef CryptoOptimizatLevel
  #define CryptoOptimizatLevel (CO_LevelLow)
#endif

//! 使用平方函数
//! 开启后使用自定义的平方函数而不是通用乘法函数
#ifndef CryptoSquareFunc
    #define CryptoSquareFunc 0
#endif
/* Specifies whether compressed point format is supported.
   Set to 0 to disable point compression/decompression functions. */
#ifndef CryptoSupportCompressed
    #define CryptoSupportCompressed 1
#endif

#ifndef CryptoNativeLittleEndian
    #define CryptoNativeLittleEndian 0
#endif

//! 优化级别
//! 值越大生成速度越快，但生成的代码较长
typedef enum
{
#ifndef CO_LevelSystem
    COL_System,  // 系统默认
#define CO_LevelSystem 0
#endif
#ifndef CO_LevelSlow
    COL_Slow,    // 慢
#define CO_LevelSlow 1
#endif
#ifndef CO_LevelLow
    COL_Low,    // 低
#define CO_LevelLow 2
#endif
#ifndef CO_LevelHigh
    COL_High,   // 高
#define CO_LevelHigh 3
#endif
#ifndef CO_LevelFast
    COL_Fast,   // 快速
#define CO_LevelFast 4
#endif
}eCryptoOptimizatLevel;

typedef enum
{
#if CryptoSupportSecp160R1
    ECC_Secp160R1,
#endif
#if CryptoSupportSecp192R1
    ECC_Secp192R1,
#endif
#if CryptoSupportSecp224R1
    ECC_Secp224R1,
#endif
#if CryptoSupportSecp256R1
    ECC_Secp256R1,
#endif
#if CryptoSupportSecp256K1
    ECC_Secp256K1,
#endif
}eCryptoSecpECC;

typedef enum
{
    AES_128 = 16,
    AES_192 = 24,
    AES_256 = 32
}eCryptoAES;

typedef enum
{
    SHA_1,
    SHA_224,
    SHA_256,
    SHA_384,
    SHA_512
}eCryptoSHA;

struct icrypto
{
    virtual ~icrypto(){}
    virtual void encrypt(uint8 *block) = 0;
    virtual void decrypt(uint8 *block) = 0;
};

#endif // CRYPTO_H
