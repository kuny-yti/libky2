
#include "directory_io.h"
#include "ky_fsys.h"
#include "ky_debug.h"

directory_io::directory_io()
{
    dir_handle = 0;
    this->io_eof = false;
    this->io_error = Io_ErrorNot;
    this->io_open = false;
    this->io_mode = 0;
    this->io_pos = 0;
    this->io_size = 0;
}
directory_io::~directory_io()
{
    close ();
    dir_handle = 0;
    this->io_eof = false;
    this->io_error = Io_ErrorNot;
    this->io_open = false;
    this->io_mode = 0;
    this->io_pos = 0;
    this->io_size = 0;
}

bool directory_io::open(int)
{
    if (!io_path.is_valid())
        return false;

    if (is_open ())
        close ();

    ky_string tpath;
    if (io_path.is_directory())
        tpath = ky_fsys::slash (io_path);
    else
        io_error = Io_ErrorOpen;

#   if !kyOSIsWin32
    dir_handle = dirOpen((char*)tpath.to_latin1());
#   else
    dir_handle = dirOpen(tpath.to_wchar());
#   endif
    io_open = dir_handle != 0 ? true: false;
    return io_open;
}
void directory_io::close()
{
    if (dir_handle)
        dirClose(dir_handle);

    dir_handle = 0;
    reset();
}

ky_string directory_io::next()
{
    while (is_open () && !at_end ())
    {
        struct dirDirent *ptr = dirRead(dir_handle);
        if (ptr == 0)
        {
            io_eof = true;
            return ky_string();
        }

        offset.push (dirTell(dir_handle));
#  if !kyOSIsWin32
        if (::strcmp(ptr->d_name, ".") && ::strcmp(ptr->d_name,  ".."))
            return (io_path / ky_path(ptr->d_name));
#  else
        if (wcscmp(ptr->d_name, L".") && wcscmp(ptr->d_name,  L".."))
            return (io_url.path () +"\\"+ ptr->d_name);
#  endif
    }
    return ky_string();
}
ky_string directory_io::prev()
{
    while (is_open () && offset.count ())
    {
        dirSeek(dir_handle, offset.pop ());
        struct dirDirent *ptr = dirRead(dir_handle);
        io_eof = false;
#  if !kyOSIsWin32
        if (strcmp(ptr->d_name, ".") && strcmp(ptr->d_name,  ".."))
            return (io_path / ky_path(ptr->d_name));
#  else
        if (wcscmp(ptr->d_name, L".") && wcscmp(ptr->d_name,  L".."))
            return (io_url.path () +"\\"+ ptr->d_name);
#  endif
    }
    return ky_string();
}
static ky_string filter_suffix;
static int filter_fn(const struct dirent *ent)
{
#  if !kyOSIsWin32
    if (ent->d_type != DT_REG)
        return 0;
    return (::strcmp(ent->d_name, filter_suffix.to_latin1 ()) == 0);
#  else
    return 0;
#endif
}
ky_string_list directory_io::scan(const ky_string& dir, const ky_string &filter)
{
    ky_string_list strout;
    if (!dir.is_empty ())
    {
#  if !kyOSIsWin32
        struct dirDirent **ptr;
        filter_suffix = filter;

        int count = dirScan((char*)dir.to_utf8 ().data (), &ptr, filter.is_empty () ? 0 : filter_fn, alphasort);

        ky_string old ;
        while (count > 0 && count --)
        {
            ky_string finm;
            finm.append(dir);
            finm.append("/");
            finm.append(ptr[count]->d_name);
            old = finm;
            strout.append(finm);
        }

#  else
        dirHandle *dp = 0;
        if((dp = dirOpen(dir.to_wchar())) == 0)
            return strout;
        _wchdir(dir.to_wchar());
        dirDirent *entry = 0;
        struct stat stbuf;
        while((entry = dirRead(dp)) != 0)
        {
            wstat(entry->d_name, &stbuf);
            if(S_IFDIR & stbuf.st_mode)
            {
                if(wcscmp(L".", entry->d_name) ==0 || wcscmp(L"..",entry->d_name ) == 0)
                   continue;
                ky_string finm;
                finm.append(dir);
                finm.append("\\");
                finm.append(entry->d_name);
                strout.append(finm);
                scan(finm, filter);
            }
            else
            {
                ky_string finm;
                finm.append(dir);
                finm.append("\\");
                finm.append(entry->d_name);
                strout.append(finm);
            }
        }
        _wchdir(L"..");
        dirClose(dp);
#  endif
    }
    return strout;
}

bool directory_io::is_directory(const ky_string &path)
{
    bool rc = false;

    if (!path.is_empty ())
    {
        ky_string buffer = path;
        if ( '\\' == buffer.at (buffer.length () -1)||
             '/' == buffer.at (buffer.length () -1) )
        {
            if ( buffer.length() <= 0 )
                return false;
        }

        // this works on Apple and gcc implentations.
        struct stat buf;
        memset(&buf,0,sizeof(buf));
        int stat_errno = stat( buffer, &buf );
        if ( 0 == stat_errno && S_ISDIR(buf.st_mode) )
            rc = true;
    }

    return rc;
}

bool directory_io::remove(const ky_string& dir)
{
    ky_string tpath = ky_fsys::slash (dir);
#if !kyOSIsWin32
    return ::dirRm ((char*)tpath.to_latin1()) == 0 ;
#else
    return ::dirRm (tpath.to_wchar()) == 0 ;
#endif
}
bool directory_io::create(const ky_string& dir)
{
    ky_string tpath  = ky_fsys::slash (dir);
#if !kyOSIsWin32
    return ::dirMk ((char*)tpath.to_latin1(), 777) == 0 ;
#else
    return ::dirMk (tpath.to_wchar()) == 0 ;
#endif
}
bool directory_io::change(const ky_string & dir)
{
    ky_string tpath = ky_fsys::slash (dir);
#if !kyOSIsWin32
    return ::dirCh ((char*)tpath.to_latin1()) == 0 ;
#else
    return ::dirCh (tpath.to_wchar()) == 0 ;
#endif
}
ky_string directory_io::current(i64 size)
{
    static ky_string dirs;
    if (dirs.is_empty ())
    {
#if !kyOSIsWin32
        char *dn = get_current_dir_name();
        if (!dn)
        {
            char *buff = (char*)kyMalloc(size);
            ky_memory::zero (buff, size);
            dirs = getcwd(buff, size-1);
            kyFree(buff);
        }
        else
            dirs = ky_string(dn);
#else
        wchar_t *buff = (wchar_t*)kyMalloc(sizeof(wchar_t) * size);
        ky_memory::zero (buff, sizeof(wchar_t) * size);
        _wgetcwd(buff, size-1);
        dirs = ky_string(buff);
        kyFree(buff);
#endif
    }
    return dirs;
}
