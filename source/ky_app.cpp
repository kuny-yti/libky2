#include "ky_app.h"
#include "ky_fsys.h"
#include "ky_debug.h"
#include "thread/thread_dispatch.h"

struct __pretreat_struct__ *__pretreat_global__ = 0;

namespace impl {

class pkg_manage
{
public:
    ky_string scan_path;//模块路径

    ky_string_list name;//保存搜索到的名称
    ky_string_list path;//保存搜索到的路径

    ky_list<piio> dlib;//保存load成功的插件动态库
    ky_list<pipackage > loaders;//load 后在插件启动函数中添加。并赋予info

public:
    explicit pkg_manage(const ky_string &path = ky_string())
    {
        if (path.is_empty())
            scan_path = ky_fsys::current();
        else
            scan_path = path;
    }
    ~pkg_manage()
    {
        uninitialize();
    }

     void initialize(const ky_string &dir = ky_string())
     {
         uninitialize();

         if (scan(dir) > 0)
         {
             for (int64 i = 0; i < name.count(); ++i)
             {
                 if (!load(path[i] + name[i]))
                 {
                     //加载失败
                 }
             }
         }
     }
     void uninitialize(void)
     {
         for (int64 i = 0; i < name.count(); ++i)
             unload(path[i] + name[i]);

         name.clear();
         path.clear();
         dlib.clear();
         loaders.clear();
     }

protected:
     int scan(const ky_string& pathfile = ky_string())
     {
         //scan 到的包信息存入结构中。并保持扫描到的名称
         ky_string_list dir_list = ky_fsys::scan (pathfile);
         const ky_string so_sufx = ky_fsys::suffix();
         for (int64 i = 0; i < dir_list.count(); ++i)
         {
             ky_finfo finfo(dir_list.at (i));
             ky_string fnm = finfo.name ();
             if ((finfo.suffix () == so_sufx) && fnm.contains ("pack"))
             {
                 name.append(finfo.name());
                 path.append(finfo.drive()+finfo.path());
                 dlib.append (piio());
             }
         }
         for (int64 i = 0; i < name.count(); ++i)
         {
             if (!check(name[i]))
             {
                 name.remove(i);
                 path.remove(i);
             }
         }
         return name.count();
     }
     bool check(const ky_string& )
     {
         //校验插件的合法性和依赖
         return true;
     }

     bool load(const ky_string& pathfile)
     {
         if (pathfile.is_empty())
             return false;

         package_begin begin = 0;
         package_end end = 0;
         piio lib = ky_fsys::create (System_Dynamic);

         ky_fsys fsys(lib);
         if (lib->open(pathfile))
         {
             begin = (package_begin)fsys.symbol(kyPackageBeginFunc);
             if (!begin)
                 return false;

             intptr ids = begin();//因为内部不知道动态库的句柄，所以在此将句柄赋值给内部
             for (uint i = 0; i < loaders.count(); ++i)
             {
                 if (loaders[i]->id() == ids)
                 {
                     dlib[i] = (lib);
                     return true;
                 }
             }

             end = (package_end)fsys.symbol(kyPackageEndFunc);
             if (end)
                 end();
         }
         return false;
     }
     bool unload(const ky_string& pathfile)
     {
         package_end end = 0;
         if (pathfile.is_empty())
             return false;

         foreach (piio &dylib, dlib)
         {
             if (dylib->path() == pathfile)
             {
                 ky_fsys fsys(dylib);
                 end = (package_end)fsys.symbol(kyPackageEndFunc);
                 if (end)
                 {
                     end();
                     return true;
                 }
             }
         }

         return false;
     }

};

struct application
{
    ky_string      app_path;
    ky_string      app_name;
    ky_string_list app_parame;

    ky_debug*       app_log;
    ky_string      log_path;
    ky_string      log_name;

    ky_string      org_name;
    ky_string      org_info;

    ky_string      pkg_path;
    pkg_manage     pkg_core;

    ky_map<intptr, pipackage>         pkg_list;

    bool recreate(_in ky_string _ref pn)
    {
        return app_log->recreate (pn.to_latin1 (), false);
    }
};

}
static ky_app *gApp = 0;

ky_app::ky_app(int argc, char **argv):
    ky_object(),
    impl(kyNew (impl::application()))
{
    kyASSERT(argc < 1, "");

    gApp = this;

    impl->app_log = kyNew (ky_debug());
    impl->app_path = ky_fsys::current();
    impl->app_name = ky_string();
    impl->app_parame.clear();

    for (int i= 1; i < argc; ++i)
        impl->app_parame.append(argv[i]);

    ky_finfo finfo(argv[0]);
    impl->app_name = finfo.name ();
    impl->app_path = finfo.drive () + finfo.path ();

    impl->pkg_core.initialize (impl->app_path + pkg_subdir);
}

ky_app::~ky_app()
{
    gApp = 0;
    kyDelete (impl->app_log);
    kyDelete (impl);
}
ky_app *ky_app::instance ()
{
    return gApp;
}

void ky_app::exit(_in int code)
{
    ky_thread *self_thread = ky_thread::current ();
    kyASSERT(self_thread == 0, "exit: Cannot find the corresponding thread!");
    self_thread->exit (code);
}
void ky_app::quit()
{
    exit(0);
}

ky_string ky_app::app_path()const
{
    if (ky_fsys::is_directory(impl->app_path))
        return impl->app_path;

    return ky_fsys::current();
}
ky_string ky_app::app_paths()const
{
    if (ky_fsys::is_directory(impl->app_path))
        return impl->app_path;

    return ky_fsys::current();
}
ky_string ky_app::app_name()const
{
    return impl->app_name;
}

process_id ky_app::pid() const
{
    return ky_thread::pid();
}

// 组织信息
ky_string ky_app::org_name() const
{
    return impl->org_name;
}

ky_string ky_app::org_info() const
{
    return impl->org_info;
}

void ky_app::set_org_name (_in ky_string _ref on)
{
    impl->org_name = on;
}

void ky_app::set_org_info (_in ky_string _ref oi)
{
    impl->org_info = oi;
}

ky_string ky_app::log_path()const
{
    return impl->log_path;
}

ky_string ky_app::log_paths()const
{
    return impl->log_path;
}

ky_string ky_app::log_name()const
{
    return impl->log_name;
}


void ky_app::set_log_path (_in ky_string _ref lp)
{
    impl->log_path = lp;
    impl->recreate(lp + impl->log_name);
}

void ky_app::set_log_name (_in ky_string _ref ln)
{
    impl->log_name = ln;
    impl->recreate(impl->log_path + ln);
}
void ky_app::set_log_config(const ky_debug::config &cfg)
{
   impl->app_log->set (cfg);
}
void ky_app::set_log_level(const eLogLevels level)
{
    impl->app_log->set(level);
}

void ky_app::set_pkg_path (_in ky_string _ref pp)
{
    impl->pkg_path = pp;

    // reload package

}

ky_string ky_app::pkg_path ()_fix
{
    return impl->pkg_path;
}

pimodule ky_app::create(_in ky_uuid _ref id )_fix
{
    foreach (const pipackage &var, impl->pkg_list)
    {
        if (var->has (id))
            return var->create (id);
    }
    return pimodule();
}
void ky_app::install(_in pipackage _ref ii)
{
    ii->install ();
    if (impl->pkg_list.contains (ii->id()))
        return ;
    impl->pkg_list.append (ii->id(), ii);
    impl->pkg_core.loaders.append (ii);
}

void ky_app::uninstall(_in pipackage _ref ii)
{
    impl->pkg_list.remove (ii->id());
    ii->uninstall ();
}

bool ky_app::event(ievent *e)
{
    if (!(e && e->is_notify ()))
        return false;

#  if kyOSIsWin32
    const int SC = SIGBREAK;
#  else
    const int SC = SIGKILL;
#  endif

    if ((e->code() & (~Event_Notify)) == SIGSEGV)
    {
        log_error("Program Access Memory Overflow (SIGSEGV)");
        return true;
    }
#  if !kyOSIsWin32
    else if ((e->code() & (~Event_Notify)) == SIGTSTP)
    {
        ::raise(SC);
        return true;
    }
#  endif
    else if ((e->code() & (~Event_Notify)) == SIGFPE)
    {
        log_error("Program Access Memory Overflow (SIGFPE)");
        ::raise(SC);
        return true;
    }
    else if ((e->code() & (~Event_Notify)) == SIGABRT)
    {
        log_error("Interrupt program run(SIGABRT)");
        ::raise(SC);
        return true;
    }
    return false;
}

int ky_app::run()
{
    registered(SIGSEGV, Notify_Signal);
#if !kyOSIsWin32
    registered(SIGTSTP, Notify_Signal);
#endif
    registered(SIGFPE, Notify_Signal);
    registered(SIGABRT, Notify_Signal);

    ky_thread *self_thread = ky_thread::current ();
    kyASSERT(self_thread == 0, "run: Cannot find the corresponding thread!");

    return self_thread->exec ();
}
bool ky_app::posted(ky_object *o, ievent *e)
{
    ky_thread *self_thread = ky_thread::current ();
    kyASSERT(self_thread == 0, "post: Cannot find the corresponding thread!");
    return self_thread->dispatch->posted (e, o);
}
