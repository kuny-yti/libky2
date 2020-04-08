#include "dynamic_io.h"
#include "ky_fsys.h"
#include "ky_debug.h"

dynamic_io::dynamic_io()
{
    _hinst = 0;
    this->reset();
}
dynamic_io::~dynamic_io()
{
    if (is_open ())
        close();
    this->reset();
}
ky_string dynamic_io::suffix()
{
#if kyOSIsWin32
        return ky_string(".dll");
#elif kyOSIsApple || kyOSIsIos
        return ky_string(".dylib");
#else
        return ky_string(".so");
#endif
}

ky_string dynamic_io::errors()const
{
    return ky_string();
}

bool dynamic_io::open(int)
{
    if (!io_path.is_valid() || is_open () || _hinst)
        return false;
    ky_string tpath;
    if (!io_path.is_directory() && !io_path.is_url())
        tpath = ky_fsys::slash (io_path.path_all());
    else
        return false;
#if kyOSIsWin32
    _hinst = ::LoadLibraryA((char*)tpath.to_utf8().data());

    if (_hinst == 0)
    {
        io_error = Io_ErrorOpen;
        log_warning("dynamic load error(code=%d, %s)",
                     GetLastError(), (char*)path().to_utf8().data());
        return false;
    }
    this->io_open = true;
    return true;
#elif  kyOSIsApple || kyOSIsUnix || kyOSIsLinux || kyOSIsAndroid || kyOSIsIos
    char * latin1 = tpath.to_latin1();
    _hinst = ::dlopen(latin1, /*RTLD_NOW*/RTLD_LAZY | RTLD_LOCAL);

    if (_hinst == 0)
    {
        io_error = Io_ErrorOpen;
        log_warning("dynamic load error(code=%s)", dlerror());
        return false;
    }
    this->io_open = true;
    return true;
#endif
}
void dynamic_io::close()
{
    if (_hinst != 0)
    {
#if kyOSIsWin32
        ::FreeLibrary((HINSTANCE) _hinst);
#elif kyOSIsApple || kyOSIsUnix || kyOSIsLinux || \
    kyOSIsAndroid || kyOSIsIos
        ::dlclose(_hinst);
#else

#endif
        _hinst = 0;
    }
    this->reset();
}
void *dynamic_io::symbol(const ky_string &name)
{
    if (name.is_empty() || !is_open () || !_hinst)
        return 0;
#if kyOSIsWin32
    void* procAddr = (void*)::GetProcAddress((HINSTANCE) _hinst,
                                           (char*)name.to_utf8().data());

    if (procAddr == NULL)
    {
        //char* lpMsg = formatMessage(GetLastError());

        //aprintf("Unable to find symbol [%s]: %s\n", name, lpMsg);
        //LocalFree(lpMsg);
    }

    return procAddr;
#elif  kyOSIsApple || kyOSIsUnix || kyOSIsLinux || kyOSIsAndroid || kyOSIsIos
    void* procAddr = ::dlsym(_hinst, name.to_latin1());

    if (procAddr == 0)
        log_warning("dynamic symbol error(name=%s, %s)", name.to_latin1(), dlerror());

    return procAddr;
#endif
}
