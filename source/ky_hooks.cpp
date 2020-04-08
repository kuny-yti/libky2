#include "ky_hooks.h"


static void def_override_upload_caps()
{
}


static const OsPath& def_get_log_dir()
{
    static OsPath logDir;
    if(logDir.empty())
        logDir = sys_ExecutablePathname().Parent();
    return logDir;
}


static void def_bundle_logs(FILE* kyUnused(f))
{

}


static const char* def_translate(const char* text)
{
    return text;
}


static void def_translate_free(const char* kyUnused(text))
{

}


static void def_log(const char* text)
{

    printf("%ls", text);

}
static eErrorReactionInternals def_display_error(const char* kyUnused(text), uint64 kyUnused(flags))
{
    return Eri_NotImplemented;
}

static ky_hooks ah =
{
    def_override_upload_caps,
    def_get_log_dir,
    def_bundle_logs,
    def_translate,
    def_translate_free,
    def_log,
    def_display_error
};
static ky_hooks default_ah = ah;

void ky_hooks::regisered(ky_hooks *hook)
{
    kyENSURE(hook);

#define OVERRIDE_IF_NONZERO(HOOKNAME) if(hook->HOOKNAME) ah.HOOKNAME = hook->HOOKNAME;
    OVERRIDE_IF_NONZERO(override_gl_upload_caps)
    OVERRIDE_IF_NONZERO(get_log_dir)
    OVERRIDE_IF_NONZERO(bundle_logs)
    OVERRIDE_IF_NONZERO(translate)
    OVERRIDE_IF_NONZERO(translate_free)
    OVERRIDE_IF_NONZERO(log)
    OVERRIDE_IF_NONZERO(display_error)
#undef OVERRIDE_IF_NONZERO
}
void ky_hooks::override_upload()
{
    ah.override_upload_caps();
}
eErrorReactionInternals ky_hooks::err_display(const char* text, uint64 flags)
{
    return ah.display_error(text, flags);
}
const char *ky_hooks::translate(const char *text)
{
    return ah.translate(text);
}

