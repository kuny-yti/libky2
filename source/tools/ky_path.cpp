#include "ky_path.h"

static ky_string backslash(ky_string out)
{
#   if !kyOSIsWin32
    out.replace("\\", "/");
#   else
    out.replace("/", "\\");
#   endif
    return out;
}

static void splitpath( ushort* path, ushort *sEnd,
                       ushort** drive, ushort** dir, ushort** fname, ushort** ext , ushort** end)
{
    const ushort slash1 = '/';
    const ushort slash2 = '\\';
    ushort* f_file_;
    ushort* f_ext_;
    ushort* s;

    if ( 0 != drive ) *drive = 0;
    if ( 0 != dir ) *dir = 0;
    if ( 0 != fname ) *fname = 0;
    if ( 0 != ext ) *ext = 0;
    if ( 0 != end ) *end = 0;

    if ( 0 != path && 0 != (ushort)*path )
    {
        if ((':' == (ushort)path[1]) && ky_char(path[0]).is_letter() )
        {
            if ( drive ) *drive = path;
            path += 2;
            if ( 0 == (ushort)*path )
                return;

        }
    }

    if ( 0 != path && 0 != (ushort)*path )
    {
        f_ext_ = 0;
        f_file_ = 0;

        s = (sEnd > path) ? sEnd - 1 : path;
        *end = s+1;

        //从字符串最后查找'.'-'/'-'\\'字符 ，不是将s指针向前进
        while ( (s > path) && ('.' != (ushort)*s) && (slash1 != *s) && (slash2 != *s) )
            s--;

        if ( ('.' == (ushort)*s) && (0 != (ushort)s[1]) )
        {
            // 若扩展文件是以'.'做扩展名后缀则取出扩展名
            f_ext_ = s;
            sEnd = f_ext_;
            s--;
        }

        //从字符串最后查找'/'-'\\'字符 ，不是将s指针向前进
        while ( (s > path) && (slash1 != *s) && (slash2 != *s) )
            s--;

        // 取出文件名
        if ( (s >= path) && (s < sEnd) )
        {
            if ((slash1 == *s) || (slash2 == *s ))
            {
                if ( (s+1) < sEnd )
                    f_file_ = s+1;
            }
            else if ( s == path )
            {
                f_file_ = s;
            }
        }

        if ( 0 == f_file_ )
        {
            f_file_ = f_ext_;
            f_ext_ = 0;
        }

        if ( (0 != dir) && (0 == f_file_ || path < f_file_) )
            *dir = path;

        if ( (0 != f_file_) && (0 != fname) )
            *fname = f_file_;

        if ( (0 != f_ext_) && (0 != ext) )
            *ext = f_ext_;
    }

}
static bool split_path( const ky_string &path,
                        ky_string& drive, ky_string& dir,
                        ky_string& fname, ky_string& fext )
{
    ushort *rdrive = 0;
    ushort *rdir = 0;
    ushort *rfile = 0;
    ushort *rext = 0;
    ushort *rend = 0;
    splitpath((ushort*)path.data(), (ushort*)path.data()+path.length(), &rdrive, &rdir, &rfile, &rext, &rend);

    if (  0 != rdrive )
    {
        if ( 0 != rdir )
            drive = ky_string((ky_char*)rdrive, (int)(rdir - rdrive));
        else if ( 0 != rfile )
            drive = ky_string((ky_char*)rdrive, (int)(rfile - rdrive));
        else if ( 0 != rext )
            drive = ky_string((ky_char*)rdrive, (int)(rext - rdrive));
        else
            drive = ky_string((ky_char*)rdrive, path.length());

    }
    else
        drive = ky_string();

    if ( 0 != rdir )
    {
        if ( 0 != rfile )
            dir = ky_string((ky_char*)rdir, (int)(rfile - rdir));
        else if ( 0 != rext )
            dir = ky_string((ky_char*)rdir, (int)(rext - rdir));
        else
            dir = ky_string((ky_char*)rdir, path.length());

    }
    else
        dir = ky_string();


    if ( 0 != rfile )
    {
        if ( 0 != rext )
            fname = ky_string((ky_char*)rfile, (int)(rext - rfile));
        else
            fname = ky_string((ky_char*)rfile, path.length());
    }
    else
        fname = ky_string();


    if ( 0 != rext )
    {
        fext = ky_string((ky_char*)rext, (int)(rend - rext));
    }
    else
        fext = ky_string();
    if (drive.is_empty() && dir.is_empty() && fname.is_empty() && fext.is_empty())
        return false;
    return true;
}

ky_path::ky_path(const char *path):
    _path(backslash(ky_string(path)))
{
    split_path(_path, _drive, _dir, _filename, _suffix);
}
ky_path::ky_path(const wchar_t *path):
    _path(backslash(ky_string(path)))
{
    split_path(_path, _drive, _dir, _filename, _suffix);
}
ky_path::ky_path(const ky_string &path):
    _path(backslash(path))
{
    split_path(_path, _drive, _dir, _filename, _suffix);
}
ky_path::ky_path(const ky_path &path):
    _path(path._path),
    _drive(path._drive),
    _dir(path._dir),
    _filename(path._filename),
    _suffix(path._suffix)
{
}
ky_path& ky_path::operator = (const ky_path& rhs)
{
    *this = ky_path(rhs);
    return *this;
}
ky_path ky_path::parent() const
{
    // 未实现
    return ky_path();
}
ky_path ky_path::before_common(const ky_path &other) const
{
    if(is_valid() || other.is_valid())
        return ky_path();

    const int idx = _path.find(other._path);
    if(idx < 0)
        return ky_path();

    return ky_path(_path.start(idx));
}

bool ky_path::is_valid()const
{
    return !path.is_empty();
}
//!
//! \brief is_directory 路径是否为目录
//! \return
//!
bool ky_path::is_directory()const
{
    return _filename.is_empty();
}

bool ky_path::is_url()const
{

}
//!
//! \brief drive 当前文件的驱动器
//! \return
//!
ky_string ky_path::drive()const
{
    return _drive;
}
//!
//! \brief path 当前文件的路径，不含驱动器
//! \return
//!
ky_string ky_path::path()const
{
    return _dir;
}
ky_string ky_path::path_all()const
{
    return drive() + path() + file_name();
}
//!
//! \brief name 文件名称，包含扩展名
//! \return
//!
ky_string ky_path::file_name()const
{
    return _filename + _suffix;
}
//!
//! \brief name_base 文件名称，不含扩展名
//! \return
//!
ky_string ky_path::base_name()const
{
    return _filename;
}
//!
//! \brief suffix 文件的扩展名
//! \return
//!
ky_string ky_path::suffix()const
{
    return _suffix;
}
ky_string ky_path::suffixs()const
{
    ky_string suffx = _suffix;
    suffx.remove('.');
    return suffx;
}
ky_path ky_path::operator / (const ky_path &rhs) const
{
    ky_string ret = _path;
    if(is_directory())
        ret += "/";

    ret += rhs._path;
    return ky_path(ret);
}
bool ky_path::operator < (const ky_path& rhs) const
{
    return path < rhs.path;
}

bool ky_path::operator == (const ky_path& rhs) const
{
    return path == rhs.path;
}

bool ky_path::operator != (const ky_path& rhs) const
{
    return !operator==(rhs);
}
