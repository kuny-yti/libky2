
/**
 * Basic tool library
 * Copyright (C) 2014 kunyang kunyang.yk@gmail.com
 *
 * @file     ky_define.h
 * @brief    基础定义
 *       1.定义了使用的类型.
 *
 * @author   kunyang
 * @email    kunyang.yk@gmail.com
 * @version  1.0.0.1
 * @date     2012/01/02
 * @license  GNU General Public License (GPL)
 *
 * Change History :
 *    Date    |  Version  |  Author  |   Description
 * 2012/01/02 | 1.0.0.1   | kunyang  | 创建文件
 * 2012/01/10 | 1.0.1.0   | kunyang  | 加入引用的可共享属性
 */
#ifndef KY_DEFINE_H
#define KY_DEFINE_H

#include "ky_type.h"

#include <cstddef>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <cfloat>
#include <stdint.h>
#include <limits.h>
#include <typeinfo>

///< 版本号组成数字
#ifndef kyVersionNumber
#define kyVersionNumber(major, minor, path)\
    ((major << 16 & 0xffffff) | (minor << 8 & 0xffff) | path & 0xff)
#endif

///< 版本号组成字符串
#ifndef kyVersionStr
#define kyVersionStr(major, minor, path) \
    (#major"."#minor"."#path)
#endif

///< 浮点数位数
#ifndef kyFloatBit
#define kyFloatBit 7
#endif

///< 浮点数精度
#ifndef kyFloatAcc
#define kyFloatAcc 0.0000001
#endif

///< 浮点数相等
#ifndef kyEqualf
#define kyEqualf(v1 ,v2) (((v1 + kyFloatAcc) > v2) && ((v1 - kyFloatAcc) < v2))
#endif

///< 浮点数不等
#ifndef kyUnEqualf
#define kyUnEqualf(v1 ,v2) (!kyEqualf(v1 ,v2))
#endif

///< 浮点数为空
#ifndef kyEmptyf
#define kyEmptyf(v1) ((v1 < kyFloatAcc) && (v1 > -kyFloatAcc))
#endif

///< 具备UTF8编码
#ifndef kyHasUTF8
#define kyHasUTF8 1
#endif

///< 具备UTF16编码
#ifndef kyHasUTF16
#define kyHasUTF16 1
#endif

///< 具备UTF32编码
#ifndef kyHasUTF32
#define kyHasUTF32 (sizeof(wchar_t) != sizeof(unsigned short))
#endif

///< 软件包声明关键字
#ifndef kyPackage
#define kyPackage struct
#endif

///< 软件包动态转换关键字
#ifdef kyPackage
#define kyAsCast(type) dynamic_cast<type>
#else
#define kyAsCast(type) (type)
#endif

///< 四字码定义
#ifndef kyFourWord
#define kyFourWord(A) (((#A)[0])) | (((#A)[1]) << 8) | (((#A)[2]) << 16) | (((#A)[3]) << 24)
typedef u32 fourwd_t;
#endif

///< 计算固定数组的数量
#ifndef kyArraySizeOf
#define kyArraySizeOf(array) (sizeof(array) / sizeof(array[0]))
#endif

///< 修饰变量，类型，函数
/**
  * 1.变量修饰: int a kyQualifyAligned(8) = 0;
  * 2.类型修饰: struct test {int a;} kyQualifyAligned(8);
  * 3.函数修饰: void test(void) kyQualifyAligned(2);
***/
#if kyCompilerIsMSVC
#  define kyQualifyAligned(byte) __declspec(align(byte))
#else
#  define kyQualifyAligned(byte) __attribute__ ((aligned(byte)))
#endif
/// <将函数强制为内联函数
/**
  * static inline void test2(void) kyQualifyAlwaysInline;
***/
#define kyQualifyAlwaysInline __attribute__((always_inline))

///< 修饰结构体，枚举，联合体的字节数为紧凑结构
#define kyQualifyPacked __attribute__ ((packed))
///< 修饰函数没有返回
#define kyQualifyNoReture __attribute__((noreturn))

///< 修饰函数在main之前执行
#define kyQualifyMainBefore(level) __attribute__((constructor(100+level)))
///< 修饰函数在main退出后执行
#define kyQualifyMainAfter(level) __attribute__((destructor(100+level)))

///< 优化表达式结果所影响的性能
#if kyCompilerIsGNUC || kyCompilerIsCLANG
#  define kyLikely(expr) (__builtin_expect (!!(expr), 1))
#  define kyUnLikely(expr) (__builtin_expect (!!(expr), 0))
#else
#  define kyLikely(expr)  (!!(expr) ? 1 : 0)
#  define kyUnLikely(expr) (!!(expr) ? 0 : 1)
#endif

///< 将函数参数标记为未使用，避免相应的编译器警告
#ifndef kyUnused
#define kyUnused(param)
#endif

///< 将函数局部变量或参数标记为未使用，避免相应的编译器警告
///< _Pragma from C99, unused from GCC
#if (kyLanguageC == kyLanguageC99) && kyCompilerIsGNUC
#  define kyUnused2(param) _Pragma("unused " #param)
#else
#  define kyUnused2(param) ((void)(param))
#endif

template <bool Test>
class __CompilerAssertFailure__;
template <>
class __CompilerAssertFailure__<true> {};

#ifndef kyCompilerAssert
//! 判断给出条件是否成立，不成立则不能定义类型就会编译出错
#define kyCompilerAssert(Cond) \
    enum {kyStrCat2(__compiler_assert_, __LINE__) = \
    sizeof(__CompilerAssertFailure__<!!(Cond)>)}
#endif

///< 注释printf样式的函数，以进行编译时类型检查。
#if kyCompilerIsGNUC
#  define kyPrintfArgs(fmtpos) __attribute__ ((format (printf, fmtpos, fmtpos+1)))
#  define kyVPrintfArgs(fmtpos) __attribute__ ((format (printf, fmtpos, 0)))
#  if kyHasWideCharSet
#    define kyWPrintfArgs(fmtpos) __attribute__ ((user("format, w, printf, " #fmtpos ", +1")))
#  else
#    define kyWPrintfArgs(fmtpos) ///< GCC 不支持宽字符
#  endif
#  define kyVWPrintfArgs(fmtpos) ///< GCC 不支持宽字符
#else
#  define kyPrintfArgs(fmtpos)
#  define kyVPrintfArgs(fmtpos)
#  define kyWPrintfArgs(fmtpos)
#  define kyVWPrintfArgs(fmtpos)
#endif

//!
//! \brief kyIsAligned
//! \param t
//! \param multiple
//! \return
//!
template<typename T>
inline bool kyIsAligned(T t, uintptr multiple)
{
    return (uintptr(t) % multiple) == 0;
}

//!
//! \brief kyAlign
//! \param n
//! \return
//!
template<u64 multiple>
inline u64 kyAlign(u64 n)
{
    assert(multiple != 0 && ((multiple & (multiple-1)) == 0));	// is power of 2
    return (n + multiple-1) & ~(multiple-1);
}

// 修饰字节对齐之间的差异。
// example: kyAligned(int, 8) myAlignedVariable = 0;
#if kyCompilerIsMSVC
# define kyAligned(type, multiple) kyQualifyAligned(multiple) type
#elif kyCompilerIsGNUC
# define kyAligned(type, multiple) type kyQualifyAligned(multiple)
#else
# define kyAligned(type, multiple) type
#endif

// SIMD vector
static const u32 kyVectorSize = 16;
#define kyVectorAligned(type) kyAligned(type, 16)

// CPU cache
static const u32 kyCpuCacheLineSize = 64;	// (L2)
#define kyCpuCacheAligned(type) kyAligned(type, 64)

// MMU pages
static const u32 kyMmuPageSize = 4 *1024;	// 4 KB
static const u32 kyMmuLargePageSize = 2 *1024 *1024;	// 2 MB

static const u32 kyAllocateAlign = 16;

static const u32 kyKiB = u32(1) << 10;
static const u32 kyMiB = u32(1) << 20;
static const u32 kyGiB = u32(1) << 30;


// 共享状态码定义
typedef i32 status_t;
struct system
{
    enum
    {

        Ok        = 0,
        // 通知调用者未做任何事情
        Skipped  = 100001,

        // 函数无法使用给定的输入执行请求的任务。
        // 这意味着可以跳过，但还可以传达更多信息。
        Cannot  = 100002,

        // 所有作业均已完成
        Complete  = 100003
    };

    enum
    {

        Unknown   = -1,	     // 未知失败

        // general
        Logic     = -100010, // 逻辑
        Exception = -100011, // 异常
        Timedout  = -100012, // 时间到
        Reentered = -100013, // 重新进入
        Corrupted = -100014,
        Aborted   = -100015, // 中止

        // 无效值
        Invalid_Alignment = -100020,
        Invalid_Offset    = -100021,
        Invalid_Handle    = -100022,
        Invalid_Pointer   = -100023,
        Invalid_Size      = -100024,
        Invalid_Flag      = -100025,
        Invalid_Param     = -100026,
        Invalid_Version   = -100027,

        // 系统限制
        Again           = -100030,
        Limit           = -100031,
        NotSupported    = -100032,
        NoMemory        = -100033,

    };

};

#ifndef repeat
///< 执行特定次数
#define repeat(num) for(i64 __i___ = 0; __i___ < (i64)num; ++__i___)

#endif

#ifndef forever
///< 执行条件型 (加入条件为了防止死循环)
#define forever(cond) for(;cond;)

#endif

#ifndef foreach
///< 容器操作型 (c++98 不能操作数组类型)
#  if kyLanguage >= kyLanguage11
#    define foreach(var, container) for(var : container)
#  else

struct kyContainerBase
{
    virtual bool condition()const = 0;
    virtual void operator ++()const = 0;
    virtual void operator --()const = 0;
    virtual void operator ++(int)const = 0;
    virtual void operator --(int)const = 0;

    virtual bool cond_less()const =0;
    virtual bool cond_add()const =0;
};

template <typename T>
class kyContainerOpe :public kyContainerBase
{
    kyContainerOpe & operator=(const kyContainerOpe &) = delete;
public:
    inline kyContainerOpe(const T & t):
        cond(0),
        container(t),
        b(t.begin()),
        e(t.end()){}

    kyContainerOpe(const kyContainerOpe & rhs):
        cond(rhs.cond),
        container(rhs.container),
        b(rhs.b),
        e(rhs.e) {}

    bool condition()const {return b != e && !cond_add();}
    void operator ++()const {++b;}
    void operator --()const  {--b;}
    void operator ++(int)const {++b;}
    void operator --(int)const {--b;}

    bool cond_less()const {kyContainerOpe* co = (kyContainerOpe*)this; return co->cond > 0 ? co->cond--: false;}
    bool cond_add()const {kyContainerOpe* co = (kyContainerOpe*)this; return co->cond++;}

    int cond;
    const T container;
    mutable typename T::iterator b, e;
};

template <typename T>
T *kyContainerAffirm(const T &) {return 0;}
template <typename T>
kyContainerOpe<T> kyContainerNew(const T & conta) {return kyContainerOpe<T>(conta);}
template <typename T>
const kyContainerOpe<T> *kyContainerAs(const kyContainerBase * fc, const T *) {return (const kyContainerOpe<T>*)fc;}
#    define foreach(val, container) \
       for (const kyContainerBase &_conta_ = kyContainerNew(container); _conta_.condition(); ++_conta_) \
       for (val = *kyContainerAs(&_conta_, kyContainerAffirm(container))->b; _conta_.cond_less() ; )

#  endif
#endif

#endif // KY_DEFINE_H
