
/**
 * Basic tool library
 * Copyright (C) 2014 kunyang kunyang.yk@gmail.com
 *
 * @file     ky_type.h
 * @brief    基础数据类型定义
 *       1.实现了一个bool_t数据类型，用于真正占位为1bit的bool类型
 *       2.实现数据类型的判断
 *         is_int, is_flt, is_void, is_array, is_pointer, is_const,
 *         is_volatile, is_enum, is_union, is_class, is_function
 *       3.实现编译过程断言机制
 *         kyCompilerAssert
 *       4.支持16bit的浮点数
 *         half_float
 *
 * @author   kunyang
 * @email    kunyang.yk@gmail.com
 * @version  1.2.2.1
 * @date     2011/03/01
 * @license  GNU General Public License (GPL)
 *
 * Change History :
 *    Date    |  Version  |  Author  |   Description
 * 2011/03/01 | 1.0.0.1   | kunyang  | 创建文件实现gcc的数据类型定义
 * 2011/03/02 | 1.0.0.2   | kunyang  | 修改数据类型根据平台进行判断
 * 2013/09/13 | 1.0.1.0   | kunyang  | 修改64位的整形类型，加入指数类型浮点数
 * 2014/05/08 | 1.0.2.0   | kunyang  | 修改数据类型根据编译器 用__attribute__ ((mode (...))) 实现数据类型
 * 2016/03/04 | 1.1.0.1   | kunyang  | 加入boolean数据类型
 * 2016/05/11 | 1.1.0.2   | kunyang  | 加入数据类型的判断
 * 2016/06/03 | 1.1.1.0   | kunyang  | 修改boolean为boolean_t和有些系统下有重复类型
 * 2017/05/01 | 1.2.0.1   | kunyang  | 加入数据类型的判断
 * 2017/07/23 | 1.2.1.1   | kunyang  | 加入编译阶段断言宏
 * 2018/01/15 | 1.2.2.1   | kunyang  | 去除long double,修改boolean_t为bool_t,加入half_float
 */
#ifndef KY_TYPE_H
#define KY_TYPE_H

#include "ky_macro.h"
#if defined(kyHasIntrin)
#include "ky_intrin.h"
#else
#include "ky_main.h"
#endif

//! 实现bool类型的原因是，当采用union类型bool时，编译器认为bool类型为int型,
//! 此时要实现其他类型和bool类型进行位置匹配无法实现。
typedef struct _boolean_
{
    unsigned char __bln__:1;
    _boolean_():__bln__(0){}

    _boolean_ (const _boolean_ & v):__bln__(v.__bln__){}
    _boolean_ & operator = (const _boolean_ & v){__bln__ = v.__bln__;return *this;}

    template<typename U>
    explicit _boolean_ (U v){__bln__ = (bool)v == true ? 1 : 0;}

    template<typename U>
    _boolean_ & operator = (U v){__bln__ = (bool)v == true ? 1 : 0;return *this;}

    template<typename U>
    inline operator const U()const {return (U)__bln__;}

}bool_t;

//! 半字节浮点数类型
typedef struct _half_float_
{
    unsigned short data;

    _half_float_():data(0){}
    _half_float_(const _half_float_ &f):data(f.data){}

    template<typename U>
    _half_float_(U f){data = from_float((float)f);}
    template<typename U>
    inline operator const U()const  {return (U)to_float(data);}
    template<typename U>
    inline _half_float_ &operator =(U f) {data = from_float((float)f); return *this;}
    inline _half_float_ &operator =(const _half_float_ &f){data = f.data; return *this;}

    inline _half_float_ operator+ (const _half_float_ &f){return _half_float_((float)*this + (float)f);}
    inline _half_float_ operator- (const _half_float_ &f){return _half_float_((float)*this - (float)f);}
    inline _half_float_ operator* (const _half_float_ &f){return _half_float_((float)*this * (float)f);}
    inline _half_float_ operator/ (const _half_float_ &f){return _half_float_((float)*this / (float)f);}
    inline _half_float_ operator- (){return _half_float_(-(float)*this);}

    inline _half_float_& operator+=(const _half_float_ &f){*this = *this + f;return *this;}
    inline _half_float_& operator-=(const _half_float_ &f){*this = *this - f;return *this;}
    inline _half_float_& operator*=(const _half_float_ &f){*this = *this * f;return *this;}
    inline _half_float_& operator/=(const _half_float_ &f){*this = *this / f;return *this;}

    inline _half_float_ operator++(int)
    {
        const float f = ((float)*this);
        *this = (float)f+1.f;
        return _half_float_(f);
    }
    inline _half_float_ operator--(int)
    {
        const float f = ((float)*this);
        *this = (float)f-1.f;
        return _half_float_(f);
    }
    inline _half_float_ operator++()
    {
        float f = ((float)*this);
        *this = ++f;
        return *this;
    }
    inline _half_float_ operator--()
    {
        float f = ((float)*this);
        *this = --f;
        return *this;
    }

    bool operator==(const _half_float_ &f) {return this->data == f.data;}
    bool operator!=(const _half_float_ &f) {return this->data != f.data;}
    bool operator<(const _half_float_ &f){return ((float)*this) < ((float)f);}
    bool operator<=(const _half_float_ &f){return ((float)*this) <= ((float)f);}
    bool operator>(const _half_float_ &f){return ((float)*this) > ((float)f);}
    bool operator>=(const _half_float_ &f){return ((float)*this) >= ((float)f);}

    static unsigned short from_float(float f)
    {
        unsigned short   fltint16;
        union {
            float         ff32;
            unsigned int  fi32[sizeof(float)/sizeof(unsigned int)];
        }fltint;
        fltint.ff32 = f;
        fltint16 =  ((fltint.fi32[0] & 0x7fffffff) >> 13) - (0x38000000 >> 13);
        fltint16 |= ((fltint.fi32[0] & 0x80000000) >> 16);

        return fltint16;
    }
    static float to_float(unsigned short d)
    {
        union
        {
            float         ff32;
            unsigned int  fi32[sizeof(float)/sizeof(unsigned int)];
        }fltint;
        fltint.fi32[0] =  ((d & 0x8000) << 16);
        fltint.fi32[0] |= ((d & 0x7fff) << 13) + 0x38000000;

        return fltint.ff32;
    }

}half_float;

//! 以下是浮点数类型的别名
typedef half_float  float16;
typedef float       float32;
typedef double      float64;
typedef float16     flt16;
typedef float32     flt32;
typedef float64     flt64;
typedef flt64       real;

//! 以下是整数类型的别名
typedef unsigned char      uchar;
typedef long long          longlong;
typedef long long          llong;
typedef unsigned long long ulonglong;
typedef unsigned long long ullong;

#if (kyCompiler != kyCompiler_GNUC) || !(defined __USE_MISC)
typedef unsigned short     ushort;
typedef unsigned int       uint;
typedef unsigned long      ulong;
#endif

//! 以下是根据编译器和体系架构定义字节相关数据类型
#if kyCompiler == kyCompiler_GNUC || kyCompiler == kyCompiler_CLANG
#   if kyCompilerVersion <  270
    typedef char            int8;
    typedef unsigned char   uint8;
    typedef short           int16;
    typedef unsigned short  uint16;
    typedef int             int32;
    typedef unsigned int    uint32;
#     if ((kyArchitecture & kyArchBit_64) == kyArchBit_64)
        typedef long int int64;
        typedef unsigned long int uint64;
#     else
        typedef long long int int64
        typedef unsigned long long int uint64;
#     endif
#   else
//!
//! QI: An integer that is as wide as the smallest addressable unit, usually 8 bits.
//! HI: An integer, twice as wide as a QI mode integer, usually 16 bits.
//! SI: An integer, four times as wide as a QI mode integer, usually 32 bits.
//! DI: An integer, eight times as wide as a QI mode integer, usually 64 bits.
//! SF: A floating point value, as wide as a SI mode integer, usually 32 bits.
//! DF: A floating point value, as wide as a DI mode integer, usually 64 bits.
//!
typedef int int8  __attribute__ ((mode (QI)));
typedef int int16 __attribute__ ((mode (HI)));
typedef int int32 __attribute__ ((mode (SI)));
typedef int int64 __attribute__ ((mode (DI)));

typedef unsigned int uint8  __attribute__ ((mode (QI)));
typedef unsigned int uint16 __attribute__ ((mode (HI)));
typedef unsigned int uint32 __attribute__ ((mode (SI)));
typedef unsigned int uint64 __attribute__ ((mode (DI)));

#   endif

#elif kyCompilerIsMSVC
    typedef char    int8;
    typedef uchar   uint8;
    typedef short   int16;
    typedef ushort  uint16;
    typedef int     int32;
    typedef uint    uint32;
    typedef unsigned __int64 uint64;
    typedef __int64          int64;
#endif

//! 根据体系结构定义指针类型
#if kyArchIs64Bit
    typedef int64  intptr;
    typedef uint64 uintptr;
    typedef uint64 size_t;
#else
    typedef int32  intptr;
    typedef uint32 uintptr;
    typedef uint32 size_t;
#endif

typedef int8   i8;
typedef int16  i16;
typedef int32  i32;
typedef int64  i64;
typedef int8   s8;
typedef int16  s16;
typedef int32  s32;
typedef int64  s64;

typedef uint8  u8;
typedef uint16 u16;
typedef uint32 u32;
typedef uint64 u64;

typedef flt16  f16;
typedef flt32  f32;
typedef flt64  f64;

/* Limits of exact-width integer types */
#ifndef I8_MIN
#define I8_MIN        (-127 - 1)
#endif
#ifndef I16_MIN
#define I16_MIN       (-32767 - 1)
#endif
#ifndef I32_MIN
#define I32_MIN       (-2147483647 - 1)
#endif
#ifndef I64_MIN
#define I64_MIN       (-9223372036854775807i64 - 1)
#endif

#ifndef I8_MAX
#define I8_MAX        127
#endif
#ifndef I16_MAX
#define I16_MAX       32767
#endif
#ifndef I32_MAX
#define I32_MAX       2147483647
#endif
#ifndef I64_MAX
#define I64_MAX       9223372036854775807LL
#endif

#ifndef UI8_MAX
#define UI8_MAX       0xFF
#endif
#ifndef UI16_MAX
#define UI16_MAX      0xFFFF
#endif
#ifndef UI32_MAX
#define UI32_MAX      0xFFFFFFFFU
#endif
#ifndef UI64_MAX
#define UI64_MAX      0xFFFFFFFFFFFFFFFFULL
#endif

#ifndef INTPTR_SIZE
#define INTPTR_SIZE sizeof(intptr)
#endif
#ifndef UINTPTR_SIZE
#define UINTPTR_SIZE sizeof(uintptr)
#endif

#include "ky_type.inl"
#endif // KY_TYPE_H
