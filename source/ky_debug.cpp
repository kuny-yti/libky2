#include "ky_debug.h"
#include "tools/ky_datetime.h"
#include "io/generic_io.h"
#include "ky_hooks.h"

#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>


// 当出现错误时用户并单击退出时，我们不希望出现任何其他错误
//（例如，由atexit处理程序引起的错误），这可能会导致无限循环。
// 隐藏错误不是很好，但是我们假设单击退出的人确实不想再看到任何消息。
static volatile uint64 isExiting;

// 此逻辑适用于任何类型的错误。 诸如抑制某些预期的WARN_ERR的特殊情况在此完成。
static bool ShouldSuppressError(volatile uint64* suppress)
{
    if(isExiting)
        return true;

    if(!suppress)
        return false;

    if(*suppress == __Dbg_Suppress)
        return true;

    return false;
}
static eErrorReactionInternals CallDisplayError(const wchar_t* text, size_t flags)
{
    eErrorReactionInternals er = ky_hooks::err_display(text, flags);
    if(er == Eri_NotImplemented)
        er = sys_display_error(text, flags);

    return er;
}

eErrorReactions dbg_assert_fail(const char* expr, volatile uintptr* suppress,
                                const char* file, int line, const char* func)
{
    kyCpuCacheAligned(uint8) context[kyDebugContextSize];
    (void)dbg_capture_context(context);

    const char * lastFuncToSkip = "dbg_assert_fail";
    char buf[400];
    ::snprintf(buf, kyArraySizeOf(buf), "Assertion failed: \"%ls\"", expr);
    return dbg_err_display(buf, Dbg_ManualBreak, context, lastFuncToSkip,
                           file, line,func, suppress);
}


eErrorReactions dbg_err_display(const char* description, uint64 flags, void* context,
                                const char* lastFuncToSkip,
                                const char* pathname, int line, const char* func,
                                volatile uint64* suppress)
{
    // "suppressing" 此错误意味着不执行任何操作并返回 Err_Continue
    if(ShouldSuppressError(suppress))
        return Err_Continue;

    // 修正参数
    description = ky_hooks::translate(description);
    if(suppress)
        flags |= Dbg_AllowSuppress;

    if(flags & Dbg_NoDebugInfo)
    {
        // 在非调试信息模式下，只需显示给定的描述
        eErrorReactionInternals er = CallDisplayError(description, flags);
        return PerformErrorReaction(er, flags, suppress);
    }

    // .. deal with incomplete file/line info
    if(!pathname || pathname[0] == '\0')
        pathname = L"unknown";
    if(line <= 0)
        line = 0;
    if(!func || func[0] == '\0')
        func = "?";
    // .. _FILE__ evaluates to the full path (albeit without drive letter)
    //    which is rather long. we only display the base name for clarity.
    const wchar_t* filename = path_name_only(pathname);

    // display in output window; double-click will navigate to error location.
    debug_printf("%s(%d): %s\n", utf8_from_wstring(filename).c_str(), line, utf8_from_wstring(description).c_str());

    ErrorMessageMem emm;
    const wchar_t* text = debug_BuildErrorMessage(description, filename, line, func, context, lastFuncToSkip, &emm);

    (void)debug_WriteCrashlog(text);
    ErrorReactionInternal er = CallDisplayError(text, flags);

    // note: debug_break-ing here to make sure the app doesn't continue
    // running is no longer necessary. debug_DisplayError now determines our
    // window handle and is modal.

    // must happen before PerformErrorReaction because that may exit.
    debug_FreeErrorMessage(&emm);

    return PerformErrorReaction(er, flags, suppress);
}


#if kyOSIsWin32

#elif kyOSIsApple

void* dbg_get_caller(void* kyUnused(context), const char* kyUnused(lastFuncToSkip))
{
    return 0;
}

status_t dbg_dump_stack(char* kyUnused(buf), uint64 kyUnused(max_chars),
                        void* kyUnused(context), const char* kyUnused(lastFuncToSkip))
{
    return system::NotSupported;
}

Status dbg_resolve_symbol(void* kyUnused(ptr_of_interest), char* kyUnused(sym_name),
                          char* kyUnused(file), int* kyUnused(line))
{
    return system::NotSupported;
}
#elif kyOSIsIos

#elif kyOSIsUnix

int64 dbg_capture_context(void* kyUnused(context))
{
    // (not needed unless/until we support stack traces)
    return system::Skipped;
}

#elif kyOSIsAndroid

// Android NDK doesn't support backtrace()
// TODO: use unwind.h or similar?
void* dbg_get_caller(void* , const char* )
{
    return 0;
}

int64 dbg_dump_stack(char* , uint64 , void* , const char* )
{
    return system::NotSupported;
}

int64 dbg_resolve_symbol(void* , char* , char* , int* )
{
    return system::NotSupported;
}

#elif kyOSIsLinux
#include <execinfo.h>

void* dbg_get_caller(void* , const char*)
{
    // bt[0] == 本函数
    // bt[1] == 调用者
    // bt[2] == 第一个调用者
    // 目前不支持lastFuncToSkip（将需要调试信息），而不是只返回调用函数的调用者
    void *bt[3];
    int bt_size = ::backtrace(bt, 3);
    if (bt_size < 3)
        return 0;
    return bt[2];
}

int64 dbg_dump_stack(char* buf, uint64 max_chars, void* , const char* )
{
    static const uint64 N_FRAMES = 16;
    void *bt[N_FRAMES];
    int bt_size = 0;
    char *bufpos = buf;
    char *bufend = buf + max_chars;

    bt_size = ::backtrace(bt, kyArraySizeOf(bt));

    // Assumed max length of a single print-out
    static const uint64 MAX_OUT_CHARS = 1024;

    for (int i = 0; ((int)i < bt_size) && (bufpos + MAX_OUT_CHARS < bufend); i++)
    {
        char file[kyDebugFileChars];
        char symbol[kyDebugSymbolChars];
        int line;
        int len;

        if (dbg_resolve_symbol(bt[i], symbol, file, &line) == 0)
        {
            if (file[0])
                len = sprintf(bufpos, MAX_OUT_CHARS, "(%p) %ls:%d %ls\n", bt[i], file, line, symbol);
            else
                len = sprintf(bufpos, MAX_OUT_CHARS, "(%p) %ls\n", bt[i], symbol);
        }
        else
        {
            len = sprintf(bufpos, MAX_OUT_CHARS, L"(%p)\n", bt[i]);
        }

        if (len < 0)
        {
            // MAX_OUT_CHARS exceeded, realistically this was caused by some
            // mindbogglingly long symbol name... replace the end with an
            // ellipsis and a newline
            ::memcpy(&bufpos[MAX_OUT_CHARS-6], "...\n", 5*sizeof(char));
            len = MAX_OUT_CHARS;
        }

        bufpos += len;
    }

    return 0;
}

int64 dbg_resolve_symbol(void* ptr_of_interest, char* sym_name, char* file, int* line)
{
    if (sym_name)
        *sym_name = 0;
    if (file)
        *file = 0;
    if (line)
        *line = 0;

    char** symbols = ::backtrace_symbols(&ptr_of_interest, 1);
    if (symbols)
    {
        snprintf(sym_name, kyDebugSymbolChars, "%hs", symbols[0]);
        free(symbols);

        // (Note that this will usually return a pretty useless string,
        // because we compile with -fvisibility=hidden and there won't be
        // any exposed symbols for backtrace_symbols to report.)
        return 0;
    }
    return -1;
}

#endif

#if !kyCompilerIsMSVC
void __dbg_break()
{
    ::kill(::getpid(), SIGTRAP);
}

status_t dbg_capture_context(void* kyUnused(context))
{
    return system::Skipped;
}
// 启动调试器，并告诉它附加到当前进程/线程
void dbg_launch_debugger()
{
#define DEBUGGER_WAIT 3
#define DEBUGGER_CMD "gdb"
#define DEBUGGER_ARG_FORMAT "--pid=%d"
#define DEBUGGER_BREAK_AFTER_WAIT 0

    pid_t orgpid = ::getpid();
    pid_t ret = ::fork();
    if (ret == 0)
    {
        char buf[16];
        ::snprintf(buf, 16, DEBUGGER_ARG_FORMAT, orgpid);

        if (::execlp(DEBUGGER_CMD, DEBUGGER_CMD, buf, NULL) != 0)
            ::perror("Debugger launch failed");
    }
    else if (ret > 0)
    {
        debug_printf("Sleeping until debugger attaches.\nPlease wait.\n");
        ::sleep(DEBUGGER_WAIT);
    }
    else
    {
        ::perror("Debugger launch: fork failed");
    }
}


#if kyOSIsApple || kyOSIsAndroid
static eErrorReactionInternals try_gui_display_error(const char* text, bool manual_break,
                                                     bool allow_suppress, bool no_continue)
{
    return Eri_NotImplemented;
}
#else
static eErrorReactionInternals try_gui_display_error(const char* text, bool manual_break,
                                                     bool allow_suppress, bool no_continue)
{
    Status err; // ignore UTF-8 errors
    std::string message = utf8_from_wstring(text, &err);

    // Replace CRLF->LF
    boost::algorithm::replace_all(message, "\r\n", "\n");

    // TODO: we ought to wrap the text if it's very long,
    // since xmessage doesn't do that and it'll get clamped
    // to the screen width

    const char* cmd = "/usr/bin/xmessage";

    char buttons[256] = "";
    const char* defaultButton = "Exit";

    if(!no_continue)
    {
        strcat_s(buttons, sizeof(buttons), "Continue:100,");
        defaultButton = "Continue";
    }

    if(allow_suppress)
        strcat_s(buttons, sizeof(buttons), "Suppress:101,");

    strcat_s(buttons, sizeof(buttons), "Break:102,Debugger:103,Exit:104");

    // Since execv wants non-const strings, we strdup them all here
    // and will clean them up later (except in the child process where
    // memory leaks don't matter)
    char* const argv[] = {
        strdup(cmd),
        strdup("-geometry"), strdup("x500"), // set height so the box will always be very visible
        strdup("-title"), strdup("0 A.D. message"), // TODO: maybe shouldn't hard-code app name
        strdup("-buttons"), strdup(buttons),
        strdup("-default"), strdup(defaultButton),
        strdup(message.c_str()),
        NULL
    };

    pid_t cpid = fork();
    if(cpid == -1)
    {
        for(char* const* a = argv; *a; ++a)
            free(*a);
        return ERI_NOT_IMPLEMENTED;
    }

    if(cpid == 0)
    {
        // This is the child process

        // Set ASCII charset, to avoid font warnings from xmessage
        setenv("LC_ALL", "C", 1);

        // NOTE: setenv is not async-signal-safe, so we shouldn't really use
        // it here (it might want some mutex that was held by another thread
        // in the parent process and that will never be freed within this
        // process). But setenv/getenv are not guaranteed reentrant either,
        // and this error-reporting function might get called from a non-main
        // thread, so we can't just call setenv before forking as it might
        // break the other threads. And we can't just clone environ manually
        // inside the parent thread and use execve, because other threads might
        // be calling setenv and will break our iteration over environ.
        // In the absence of a good easy solution, and given that this is only
        // an error-reporting function and shouldn't get called frequently,
        // we'll just do setenv after the fork and hope that it fails
        // extremely rarely.

        execv(cmd, argv);

        // If exec returns, it failed
        //fprintf(stderr, "Error running %s: %d\n", cmd, errno);
        exit(-1);
    }

    // This is the parent process

    // Avoid memory leaks
    for(char* const* a = argv; *a; ++a)
        free(*a);

    int status = 0;
    waitpid(cpid, &status, 0);

    // If it didn't exist successfully, fall back to the non-GUI prompt
    if(!WIFEXITED(status))
        return ERI_NOT_IMPLEMENTED;

    switch(WEXITSTATUS(status))
    {
    case 103: // Debugger
        udbg_launch_debugger();
        FALLTHROUGH;

    case 102: // Break
        if(manual_break)
            return ERI_BREAK;
        debug_break();
        return ERI_CONTINUE;

    case 100: // Continue
        if(!no_continue)
            return ERI_CONTINUE;
        // continue isn't allowed, so this was invalid input.
        return ERI_NOT_IMPLEMENTED;

    case 101: // Suppress
        if(allow_suppress)
            return ERI_SUPPRESS;
        // suppress isn't allowed, so this was invalid input.
        return ERI_NOT_IMPLEMENTED;

    case 104: // Exit
        abort();
        return ERI_EXIT;	// placebo; never reached

    }

    // Unexpected return value - fall back to the non-GUI prompt
    return ERI_NOT_IMPLEMENTED;
}
#endif
eErrorReactionInternals sys_display_error(const char* text, uint64 flags)
{
    debug_printf("%s\n\n", utf8_from_wstring(text).c_str());

    const bool manual_break   = (flags & Dbg_ManualBreak ) != 0;
    const bool allow_suppress = (flags & Dbg_AllowSuppress) != 0;
    const bool no_continue    = (flags & Dbg_NoContinue   ) != 0;

    // Try the GUI prompt if possible
    ErrorReactionInternal ret = try_gui_display_error(text, manual_break, allow_suppress, no_continue);
    if (ret != Eri_NotImplemented)
        return ret;

#if kyOSIsAndroid
    // Android没有简单的方法来获取用户输入，因此自动继续或退出
    if(no_continue)
        abort();
    else
        return Eri_Continue;
#else
    // Otherwise fall back to the terminal-based input

    // Loop until valid input given:
    for(;;)
    {
        if(!no_continue)
            printf("(C)ontinue, ");
        if(allow_suppress)
            printf("(S)uppress, ");
        printf("(B)reak, Launch (D)ebugger, or (E)xit?\n");
        // TODO Should have some kind of timeout here.. in case you're unable to
        // access the controlling terminal (As might be the case if launched
        // from an xterm and in full-screen mode)
        int c = ::getchar();
        // note: don't use tolower because it'll choke on EOF
        switch(c)
        {
        case EOF:
        case 'd': case 'D':
            udbg_launch_debugger();
            FALLTHROUGH;

        case 'b': case 'B':
            if(manual_break)
                return ERI_BREAK;
            debug_break();
            return ERI_CONTINUE;

        case 'c': case 'C':
            if(!no_continue)
                return ERI_CONTINUE;
            // continue isn't allowed, so this was invalid input. loop again.
            break;
        case 's': case 'S':
            if(allow_suppress)
                return ERI_SUPPRESS;
            // suppress isn't allowed, so this was invalid input. loop again.
            break;

        case 'e': case 'E':
            abort();
            return ERI_EXIT;	// placebo; never reached
        }
    }
#endif
}
#endif


static const char *__log_level_str[] =
{
    "Fatal",
    "Alert",
    "Critical",
    "Error",
    "Warning",
    "Notice",
    "Info",
    "Debug",
};

static struct log_time
{
    struct timeval time;
    log_time(){::gettimeofday(&time, 0);}
    ~log_time(){}

    void get(long long *sec, long long *usec)
    {
        struct timeval tv;
        ::gettimeofday(&tv, 0);
        *sec = tv.tv_sec - __log_time_.tv_sec;
        *usec = (long long)tv.tv_usec - (long long)__log_time_.tv_usec;
        if (*usec < 0)
        {
            *sec -= 1;
            *usec = 1000000 + *usec;
        }
    }
}__log_time;
#define log_time_get(sec, usec) __log_time.get(sec, usec)

struct log_core
{
    pthread_mutex_t   mutex;
    int               level[Log_Count];
    log_dynconf      *dcfg;
    FILE             *file;

    void lock()
    {
        pthread_mutex_lock(&mutex);
    }
    void unlock()
    {
        pthread_mutex_unlock(&mutex);
    }

    static bool matches(const ky_debug::filter *filter,
                        const char *file, int line,
                        const char *func, const char *subs)
    {
        if (*filter->file)
        {
            if (!file || ::strncmp(filter->file, file, 128))
                return false;
        }
        if (filter->line >= 0 && filter->line != line)
            return false;
        if (*filter->func)
        {
            if (!func || ::strncmp(filter->func, func, 128))
                return false;
        }
        if (*filter->subs)
        {
            if (!subs || ::strncmp(filter->subs, subs, 128))
                return false;
        }
        return true;
    }
    static bool omit(const char *file, int line, const char *func,
                     const ky_debug::config *config,
                     const char *subs,
                     eLogLevels level)
    {
        int val = 0;
        log_dynconf *dconf = 0;

        if (level >= Log_Count)
            return false;

        if (config)
        {
            val = config->level[level];
            if (val == 0)
                return true;
            if (val == 1)
                return false;
        }

        for (dconf = ky_debug::impl->dcfg; dconf; dconf = dconf->next)
        {
            if (matches(&dconf->filter, file, line, func, subs))
            {
                val = dconf->config.level[level];
                if (val == 0)
                    return true;
                if (val == 1)
                    return false;
            }
        }

        val = ky_debug::impl->config.level[level];
        if (val == 0)
            return true;
        if (val == 1)
            return false;

        return false;
    }

    static void submit(const char *file, int line, const char *func,
                       const ky_debug::config *config,
                       const char *subs, eLogLevels level,
                       const char *format, va_list args)
    {
        const char *prefix = NULL;
        FILE *out = generic_io::err;
        long long sec, usec;

        if (omit(file, line, func, config, subs, level))
            return;

        if (ky_debug::impl->file)
            out = ky_debug::impl->file;

        time_get (&sec, &usec);

        if (level < Log_Count)
            prefix = log_level_str[level];

        if (prefix)
        {
            if (subs)
                ::fprintf(out, "[%.4lld.%.6lld] %s: %s: ", sec, usec, prefix, subs);
            else
                ::fprintf(out, "[%.4lld.%.6lld] %s: ", sec, usec, prefix);
        }
        else
        {
            if (subs)
                ::fprintf(out, "[%.4lld.%.6lld] %s: ", sec, usec, subs);
            else
                ::fprintf(out, "[%.4lld.%.6lld] ", sec, usec);
        }

        int64 len = ::strlen(format);
        bool nl = format[len - 1] == '\n';

        if (!func)
            func = "<unknown>";
        if (!file)
            file = "<unknown>";
        if (line < 0)
            line = 0;

        vfprintf(out, format, args);

        if (!nl)
            fprintf(out, " (%s() in %s:%d)\n", func, file, line);
        fflush(out);
    }

    static void format(const char *file, int line, const char *func,
                       const ky_debug::config *config,
                       const char *subs, eLogLevels level,
                       const char *format, ...)
    {
        va_list list;

        va_start(list, format);
        submit(file, line, func, config, subs, level, format, list);
        va_end(list);
    }

    log_core():
        mutex(PTHREAD_MUTEX_INITIALIZER)
    {
        config = kyLogConfigAll(0,0,1,1,1,1,1,1);
        dcfg = 0;
        file = 0;
    }
    static struct timeval _ftime_;
};

static struct log_output
{
    ky_atomic<FILE*> out;

    log_output(){out.store(generic_io::err);}
    ~log_output()
    {
        if (out.load() != generic_io::err)
            ::fclose(out.load());
    }

    void set (FILE *io){out.store(io);}
    void set_err(){out.store(generic_io::err);}

    void submit(const char *file, int line, const char *func,
                const char *subs, eLogLevels level,
                const char *format, va_list args)
    {
        const char *prefix = __log_level_str[level];;
        long long sec, usec;
        log_time_get (&sec, &usec);

        if (subs)
            ::fprintf(out, "[%.4lld.%.6lld] %s: %s: ", sec, usec, prefix, subs);
        else
            ::fprintf(out, "[%.4lld.%.6lld] %s: ", sec, usec, prefix);

        if (!func) func = "<unknown>";
        if (!file) file = "<unknown>";
        if (line < 0) line = 0;

        ::vfprintf(out, format, args);

        if (!(format[::strlen(format) - 1] == '\n'))
            ::fprintf(out, " (%s() in %s:%d)\n", func, file, line);
        ::fflush(out);
    }

}__log_output;
#define log_set(io) __log_output.set(io)
#define log_set_err() __log_output.set_err()
#define log_set_in() __log_output.set_in()
#define log_set_out() __log_output.set_out()
#define log_output_equal(io) (__log_output.out.load() == io)
#define log_output_get() __log_output.out.load()
#define log_submit(f, l, fu, s, l, fo, a) __log_output.submit(f, l, fu, s, l, fo, a)

struct log_dynconf
{
    struct log_dynconf *next;
    int                 handle;
    ky_debug::filter    filter;
    int                 level[Log_Count];
};
static struct dbg_filter
{
    int                     level[Log_Count];
    ky_atomic<log_dynconf*> dyncfg;

    dbg_filter()
    {
        set(Log_Error);
    }
    ~dbg_filter()
    {
        clear();
    }

    void set(eLogLevels llev)
    {
        if (llev == Log_Count)
            llev = Log_Debug;
        for (int i = 0; i <= llev; ++i)
            level[i] = 1;
    }
    int append(const ky_debug::filter &flt)
    {
        log_dynconf *dconf = (log_dynconf *)kyMalloc(sizeof(*dconf));
        if (!dconf)
            return -ENOMEM;

        ::memset(dconf, 0, sizeof(*dconf));
        ::memcpy(&dconf->filter, &flt, sizeof(flt));
        ::memcpy(&dconf->level, &level, sizeof(level));

        if (dyncfg.load())
            dconf->handle = dyncfg.load()->handle + 1;
        dconf->next = dyncfg.load();
        dyncfg.store(dconf);

        return dconf->handle;
    }
    void remove(int h)
    {
        log_dynconf *last = dyncfg.load();
        log_dynconf *next = last;

        while (next)
        {
            if (next->handle == h)
            {
                if (next == last)
                {
                    dyncfg.store(last->next);
                    kyFree(last);
                }
                else
                {
                    last->next = next->next;
                    kyFree(next);
                }

                return ;
            }
            last = next;
            next = last->next;
        }
    }
    void clear ()
    {
        struct log_dynconf *dconf = dyncfg.load(), *tmp;
        dyncfg.store(0);

        while (dconf)
        {
            tmp = dconf;
            dconf = dconf->next;
            kyFree(tmp);
        }
    }

    bool omit(const char *file, int line, const char *func,
              const int *cfg, const char *subs, eLogLevels lnum)
    {
        if (cfg)
        {
            if (!cfg[lnum])
                return true;
            return false;
        }

        for (log_dynconf *dconf = dyncfg.load(); dconf; dconf = dconf->next)
        {
            if (matches(&dconf->filter, file, line, func, subs))
            {
                if (!dconf->level[lnum])
                    return true;
                return false;
            }
        }

        if (!level[lnum])
            return true;
        return false;
    }

    static bool matches(const ky_debug::filter *filter,
                        const char *file, int line, const char *func,
                        const char *subs)
    {
        if (*filter->file)
        {
            if (!file || ::strncmp(filter->file, file, 128))
                return false;
        }

        if (filter->line >= 0 && filter->line != line)
            return false;

        if (*filter->func)
        {
            if (!func || ::strncmp(filter->func, func, 128))
                return false;
        }

        if (*filter->subs)
        {
            if (!subs || ::strncmp(filter->subs, subs, 128))
                return false;
        }

        return true;
    }
}__log_filter;
#define log_filter_omit(file, line, func, cfg, subs, level) \
    __log_filter.omit(file, line, func, cfg, subs,level)
#define log_level_set(level) __log_filter.set(level)
#define log_filter_append(flt) __log_filter.append(flt)
#define log_filter_remove(h) __log_filter.remove(h)
#define log_filter_clear(h) __log_filter.clear()

static int def_level[Log_Count] = {1, 1, 1, 1, 0, 0, 0, 0};
ky_debug::ky_debug(const char *fn, bool clr)
{
    recreate(fn, clr);
}
ky_debug::~ky_debug()
{
}

bool ky_debug::recreate(const char* fn , bool clr )
{
    FILE *f = 0, *old = 0;
    if (fn)
    {
        f = ::fopen(fn, clr ? "w+": "a");
        if (!f)
        {
            log_err("cannot change log-file to %s (%d): %m", fn, errno);
            return false;
        }
    }
    else
    {
        log_set_err();
        fn = "stderr";
    }

    if (!log_output_equal(f))
    {
        log_notice("set log-file to %s", fn); // 以前的日志中
        old = log_output_get();
        log_set (f);
        log_notice("set log-file to %s", fn);// 新的日志中
        f = 0;
    }

    if (f)
        ::fclose(f);
    if (old && (old != generic_io::err))
        ::fclose(old);
    return true;
}
void ky_debug::set(eLogLevels level)
{
    log_level_set(level);
    log_notice ("Change the log level to %s", __log_level_str[level]);
}

int ky_debug::append(const filter &flt)
{
    log_filter_append (flt);
}
void ky_debug::remove(int h)
{
    log_filter_remove(h);
}
void ky_debug::clear ()
{
   log_filter_clear();
}


void ky_debug::submit(const char *file, int line, const char *func,
                  const int *config,
                  const char *subs, eLogLevels level,
                  const char *format, va_list args)
{
    int saved_errno = errno;
    // 属于过滤的，则返回。包括级别限制
    if (log_filter_omit(file, line, func, config, subs, level))
        return ;
    // 输出日志
    log_submit(file, line, func, subs, level, format, args);

    errno = saved_errno;
}
void ky_debug::formats(const char *file, int line, const char *func,
                       const int *config, const char *subs, eLogLevels level,
                       const char *format, ...)
{
    va_list list;
    int saved_errno = errno;

    va_start(list, format);
    submit(file, line, func, config, subs, level, format, list);
    va_end(list);

    errno = saved_errno;
}
