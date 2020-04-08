
/**
 * Basic tool library
 * Copyright (C) 2014 kunyang kunyang.yk@gmail.com
 *
 * @file     ky_debug.h
 * @brief    调试信息和日志打印基本类
 *       1.调试信息的输出
 *       2.全局日志功能打印
 *       3.加入日志和调试信息过滤配置
 *       4.统一所有程序的断言
 *
 * @author   kunyang
 * @email    kunyang.yk@gmail.com
 * @version  1.2.0.1
 * @date     2012/04/01
 * @license  GNU General Public License (GPL)
 *
 * Change History :
 *    Date    |  Version  |  Author  |   Description
 * 2012/04/01 | 1.0.0.1   | kunyang  | 创建文件
 * 2012/04/05 | 1.0.1.0   | kunyang  | 修改调试输出为标准输出
 * 2014/03/10 | 1.0.2.0   | kunyang  | 加入日志打印方式
 * 2014/04/06 | 1.1.0.1   | kunyang  | 修改日志模式为统一方式实现
 * 2015/06/06 | 1.1.2.0   | kunyang  | 修改日志打印函数并加入日志等级过滤
 * 2018/11/23 | 1.1.3.0   | kunyang  | 修改debug定义
 * 2020/02/11 | 1.2.0.1   | kunyang  | 加入hook功能，并剥离调试和日志功能
 *
 */
#ifndef KY_DEBUG_H
#define KY_DEBUG_H
#include "ky_define.h"

#if defined(kyHasLog) || defined(kyHasDebug)
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>

static const uint64 kyDebugContextSize = 2048;	// Win32 is 1232 bytes

/**
 * dbg_resolve_symbol写入用户缓冲区的最大字符数（包括空终止符）
 **/
static const size_t kyDebugSymbolChars = 1000;
static const size_t kyDebugFileChars = 100;

// debug
#if kyCompilerIsMSVC
# define __dbg_break __debugbreak	// intrinsic "function"
#else
extern void __dbg_break();
#endif

typedef enum
{
    Log_Fatal = 0,   ///< 致命
    Log_Alert = 1,   ///< 警报
    Log_Critical = 2,///< 危急
    Log_Error = 3,   ///< 错误
    Log_Warning = 4, ///< 警告
    Log_Notice = 5,  ///< 通知
    Log_Info = 6,    ///< 信息
    Log_Debug = 7,   ///< 调试
    Log_Count
}eLogLevels;

class ky_debug
{
    friend class ky_app;
private:
    ky_debug(const char* fn = NULL, bool clr = true);
    ~ky_debug();

public:
    struct filter
    {
        char file[128];
        int  line;
        char func[128];
        char subs[128];
    };
    static int def_level[Log_Count];

public:
    //!
    //! \brief set 设置日志级别
    //! \param level
    //!
    void set(eLogLevels level);

    // filter operate
    //!
    //! \brief append 附加日志过滤
    //! \param flt
    //! \return  返回过滤的id
    //!
    int append(const filter &flt);

    //!
    //! \brief remove 删除过滤
    //! \param h 过滤的id
    //!

    void remove(int h);

    //!
    //! \brief clear 清除所有过滤
    //!
    void clear();

    //!
    //! \brief recreate 重新创建记录对象
    //! \param fn 记录目标名(文件、0=标准io)
    //! \param clr 若存在是否清空记录
    //! \return
    //!
    static bool recreate(const char* fn = 0, bool clr = true);

public:
    kyPrintfArgs(7)
    static void formats(const char *file, int line, const char *func,
                        const int *config, const char *subs, eLogLevels level,
                        const char *format, ...);

protected:
    kyVPrintfArgs(7)
    static void submit(const char *file, int line, const char *func,
                       const int *config, const char *subs, eLogLevels level,
                       const char *format, va_list args);
};

// 可以定义后完成子系统打印
#ifndef kyLogSubSystem
#define kyLogSubSystem NULL
#endif

#define kyLogDefaultBase __FILE__, __LINE__, __func__
#define kyLogDefault kyLogDefaultBase, &ky_debug::def_level, kyLogSubSystem

#define ky_log_printf(level, format, ...) \
    ky_debug::formats(kyLogDefault, (level), (format), ##__VA_ARGS__)

#ifdef kyHasDebug
    #define log_debug(format, ...) \
        ky_log_printf(Log_Debug, (format), ##__VA_ARGS__)
#else
static inline __attribute__((format(printf, 2, 3)))
void ky_log_dummyf(unsigned int , const char *, ...){}
#define log_debug(format, ...) \
    ky_log_dummyf(Log_Debug, (format), ##__VA_ARGS__)
#endif

#define log_info(format, ...) \
    ky_log_printf(Log_Info, (format), ##__VA_ARGS__)
#define log_notice(format, ...) \
    ky_log_printf(Log_Notice, (format), ##__VA_ARGS__)
#define log_warning(format, ...) \
    ky_log_printf(Log_Warning, (format), ##__VA_ARGS__)
#define log_error(format, ...) \
    ky_log_printf(Log_Error, (format), ##__VA_ARGS__)
#define log_critical(format, ...) \
    ky_log_printf(Log_Critical, (format), ##__VA_ARGS__)
#define log_alert(format, ...) \
    ky_log_printf(Log_Alert, (format), ##__VA_ARGS__)
#define log_fatal(format, ...) \
    ky_log_printf(Log_Fatal, (format), ##__VA_ARGS__)

#else // no logs
struct ky_debug{};
#define log_info(format, ...) do{}while(0)
#define log_notice(format, ...) do{}while(0)
#define log_warning(format, ...) do{}while(0)
#define log_error(format, ...) do{}while(0)
#define log_critical(format, ...) do{}while(0)
#define log_alert(format, ...) do{}while(0)
#define log_fatal(format, ...) do{}while(0)

#endif

#define log_dbg log_debug
#define log_warn log_warning
#define log_err log_error
#define log_crit log_critical

#ifdef kyASSERT
#undef kyASSERT
#endif

#define kyASSERT(expr)\
    do\
    {\
        static volatile uintptr __suppress_;\
        if(!(expr))\
        {\
            switch(dbg_assert_fail(#expr, &__suppress_, kyLogDefaultBase))\
            {\
            case Err_Continue:\
                break;\
            case Err_Break:\
            default:\
                __dbg_break();\
                break;\
            }\
        }\
    }\
    while(0)

#ifdef kyHasAssert

#    define kyASSERT(val, ...) \
           if (!!(val)) {log_fatal("assert[%s]", str);abort ();}
#else
#  define kyASSERT(val, ...)
#endif

#ifdef kyENSURE
#undef kyENSURE
#endif

#ifdef kyHasEnsure

#else
#  define kyENSURE(val, str)
#endif


/**
 * translates and displays the given strings in a dialog.
 * this is typically only used when debug_DisplayError has failed or
 * is unavailable because that function is much more capable.
 * implemented via sys_display_msg; see documentation there.
 **/
void dbg_msg_display(const wchar_t* caption, const wchar_t* msg);

/// 用于自定义dbg_err_display行为的标志
typedef enum DebugDisplayError
{
    ///< 禁止继续按钮
    Dbg_NoContinue = 1,

    ///< 开启禁止按钮
    ///< 如果接收到非空禁止指针，则由该参数自动设置。
    ///< 这个标记是必需的，因为sys_display_error接口不会获取该指针。
    Dbg_AllowSuppress = 2,

    ///< 不要在内部触发断点
    ///< 如果返回Err_Break，则调用者将处理此问题。
    ///< 这样调试器可以进入有问题的函数。
    Dbg_ManualBreak = 4,

    ///< 仅显示给定的消息
    ///< 不要添加有关调用堆栈的任何信息，不要编写崩溃日志，等等。
    Dbg_NoDebugInfo = 8
}eDebugDisplayErrors;

static const volatile intptr __Dbg_Suppress = 0xAB;
/**
 * 错误对话框提供的由debug_DisplayError返回的选项。
 **/
typedef enum ErrorReaction
{
    /**
     * 忽略，继续进行，好像什么都没发生。
     **/
    Err_Continue = 1,

    /**
     * 触发断点，即进入调试器。
     * 仅在通过 Dbg_ManualBreak 时返回；
     * 否则，debug_DisplayError本身将触发一个断点。
     **/
    Err_Break
}eErrorReactions;

/**
 * all choices offered by the error dialog. those not defined in
 * ErrorReaction are acted upon by debug_DisplayError and
 * never returned to callers.
 * (this separation avoids enumerator-not-handled warnings)
 **/
typedef enum ErrorReactionInternal
{
    Eri_Continue = Err_Continue,
    Eri_Break = Err_Break,

    ///< 忽略并且不再报告。
    ///< 注：非持久性； 仅在此程序运行期间适用。
    Eri_Suppress,

    ///< 立即退出程序。
    Eri_Exit,
    ///< display_error应用程序钩子根的特殊返回值，表明它没有执行任何操作，而应调用常规的sys_display_error实现。
    Eri_NotImplemented
}eErrorReactionInternals;

/**
 * 断言失败时调用
 *
 * @param assert_expr 失败的表达式字符串
 * @param suppress see debug_DisplayError.
 * @param file, line 失败点的源文件名和行号
 * @param func 函数的名称
 * @return eErrorReactions （用户选择：继续运行还是停止？）
 **/
eErrorReactions dbg_assert_fail(const char* assert_expr, volatile uintptr* suppress,
                                const char* file, int line, const char* func);

/**
 * 显示带有消息和堆栈跟踪的错误对话框。
 *
 * @param description 要显示的文字。
 * @param flags: eDebugDisplayErrors
 * @param context, lastFuncToSkip: dbg_dump_stack.
 * @param file, line, func: 错误位置 (__FILE__, __LINE__, __func__)
 * @param suppress 指向可用于禁止此错误的调用方分配标志的指针。 如果为NULL，则将跳过此功能，并且将禁用“禁止”对话框按钮。
 * @return eErrorReactions
 **/
eErrorReactions dbg_err_display(const char* description, uint64 flags,
                                void* context, const char* lastFuncToSkip,
                                const char* file, int line, const char* func,
                                volatile intptr* suppress);




/**
 * @param context must point to an instance of the platform-specific type
 *   (e.g. CONTEXT) or CACHE_ALIGNED storage of DEBUG_CONTEXT_SIZE bytes.
 **/
status_t dbg_capture_context(void* context);

/**
 * 将完整的堆栈跟踪（包括局部变量的值）写入指定的缓冲区
 *
 * @param buf 目标缓冲区
 * @param maxChars 缓冲区的最大字符数
 * @param context 特定平台的上下文
 * @param lastFuncToSkip 用于从堆栈跟踪中省略错误报告功能
 * @return Err_Reentered 如果通过递归或多线程重新输入（由于使用了静态数据，所以不允许）
 **/
status_t dbg_dump_stack(char* buf, uint64 maxChars, void* context, const char* lastFuncToSkip);

#endif
