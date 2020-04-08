#include "generic_io.h"
#include "ky_debug.h"
#include "iio.h"

#if kyOSIsLinux

#  ifndef O_ATOMIC
#    define O_ATOMIC	040000000
#  endif

#  ifndef O_NOATIME
#    define O_NOATIME	0
#  endif

#else

#  define O_ATOMIC      0
#  define O_NOATIME	    0

#endif


struct sGlobalIOs
{
    bool ddir_stdio;
};

static sGlobalIOs aGlobalIo = {false};
static sGlobalIOs *gio = &aGlobalIo;

FILE *generic_io::out = stdout;
FILE *generic_io::err = stderr;
FILE *generic_io::in = stdin;


generic_io::generic_io(const eIoTypeFlags &type)
{
    reset ();
    iio::io_type = type;
    is_std = false;
    fd = -1;
}
generic_io::~generic_io()
{

}

// O_CREAT 若欲打开的文件不存在则自动建立该文件
// O_NONBLOCK 以不可阻断的方式打开文件
// O_TRUNC 若文件存在并且以可写的方式打开时，此旗标会令文件长度清为0
// O_APPEND 当读写文件时会从文件尾开始移动
// O_DIRECT 直接访问磁盘,无缓冲的输入、输出
// O_ATOMIC 支持原子IO
// O_SYNC 以同步IO方式打开文件
// O_NOATIME 文件进行读操作时,不更新文件的最后访问时间
bool generic_io::open(const eIoModeFlags & mode)
{
    if (is_open ())
        close ();

    if (!io_path.is_valid ())
    {
        io_error = Io_ErrorPathEmpty;
        return false;
    }

    ky_string file = io_path.path_all();

    // 为系统标志io
    if (file == "-")
    {
        if ((mode & Io_ReadWrite) == Io_ReadWrite)
        {
            io_error = Io_ErrorAlsoWR;
            return false;
        }
        if (gio->ddir_stdio)
        {
            io_error = Io_ErrorNot;
            return false;
        }
        is_std = true;
        gio->ddir_stdio = true;
    }

    int flag = 0;

    if (mode.is(Io_ReadWrite))
        flag = O_RDWR | O_CREAT;
    else if (mode.is(Io_WriteOnly))
        flag = O_WRONLY | O_CREAT;
    else
        flag = O_RDONLY;

    if (!mode.is(Io_ReadOnly))
    {
        if (mode & Io_Append)
            flag |= O_APPEND;
        else
            flag |= O_TRUNC;
    }

#if !kyOSIsWin32
    if (mode & Io_NonBlocking)
        flag |= O_NONBLOCK;

    // 异步io或指定atomic则采用直接访问
    if (mode & Io_ASync)
        flag |= O_DIRECT;
    if (mode & Io_Atomic)
        flag |= O_DIRECT | O_ATOMIC;
#endif

    // 不为文件io时不更新文件的最后访问时间
    if (!(io_type & Io_TypeFile))
        flag |= O_NOATIME;

    // 标准系统io则拷贝fd
    if (is_std)
    {
        fd = ::dup((mode & Io_ReadOnly) ? STDIN_FILENO : STDOUT_FILENO);
        if (fd < 0)
        {
            io_error = Io_ErrorOpen;
            return false;
        }
    }
    else
    {
#  if kyOSIsWin32
        flag |= _O_BINARY;
#  endif
        const ky_utf8 of = file.to_utf8 ();
        do
        {
            fd = ::open((char*)of.data (), flag, S_IRWXU);
            if (fd != -1)
                break;
            // 设置了O_NOATIME，出错则去除标志
            else if ((errno == EPERM) && (flag & O_NOATIME))
                flag &= ~O_NOATIME;
            else
            {
#  if !kyOSIsWin32
                // 设置了O_DIRECT，说明不支持直接访问
                if ((errno == EINVAL) && (flag & O_DIRECT))
                    io_error = Io_ErrorNotDirect;
                else
#  endif
                    io_error = Io_ErrorOpen;
                return false;
            }
        } while (1);

#  if kyOSIsWin32
        if (::_stat64i32((const char*)of.data(), &file_stat) != 0)
#else
        if (::stat((const char*)of.data(), &file_stat) != 0)
#endif
        {
            io_error = Io_ErrorStat;
            this->close ();
            return false;
        }
        // S_ISREG是否是一个常规文件
        // S_ISDIR是否是一个目录
        // S_ISSOCK是否是一个SOCKET文件.
        if ((io_type & Io_TypeFile) && (!S_ISREG(file_stat.st_mode) ||
                                         S_ISDIR(file_stat.st_mode)))
        {
            io_error = Io_ErrorRegularFile;
            this->close ();
            return false;
        }
        if ((io_type & Io_TypeDirectory) && !S_ISDIR(file_stat.st_mode))
        {
            io_error = Io_ErrorDirectory;
            this->close ();
            return false;
        }

        io_size = file_stat.st_size;
    }

    io_open = true;
    io_mode = mode;
    return true;
}
void generic_io::close()
{
    if (is_open ())
        ::close(fd);
    if (is_std)
        gio->ddir_stdio = false;
    io_open = false;
    io_eof = false;
    io_error = Io_ErrorNot;
    io_statu = Io_StateNot;
    io_size = 0;
    io_pos = 0;
    fd = -1;
}

i64 generic_io::tell()const
{
    generic_io *io = (generic_io *)this;
    if (!is_open ())
    {
        io->io_error = Io_ErrorNotOpen;
        return -1;
    }

    io->io_pos = ::lseek(fd, 0, SEEK_CUR);
    return io_pos;
}

i64 generic_io::read  (void *vbuf, i64 len)const
{
    generic_io *io = (generic_io *)this;
    if (!is_open ())
    {
        io->io_error = Io_ErrorNotOpen;
        return -1;
    }

    i64 ret = ::read(fd, vbuf, len);
    if (ret < 0)
    {
        io->io_error = Io_ErrorRead;
        return -1;
    }

    io->io_pos += ret;
    return ret;
}
i64 generic_io::write  (void *vbuf, i64 len)const
{
    generic_io *io = (generic_io *)this;
    if (!is_open ())
    {
        io->io_error = Io_ErrorNotOpen;
        return -1;
    }

    i64 ret = ::write(fd, vbuf, len);
    if (ret < 0)
    {
        io->io_error = Io_ErrorWrite;
        return -1;
    }

    io->io_pos += ret;
    return ret;
}

i64 generic_io::read(void *vbuf, i64 len, i64 offset)const
{
    generic_io *io = (generic_io *)this;
    if (!is_open ())
    {
        io->io_error = Io_ErrorNotOpen;
        return -1;
    }

#if kyOSIsWin32
    i64 ret = -1;
#else
    i64 ret = ::pread(fd, vbuf, len, offset);
#endif
    if (ret < 0)
    {
        io->io_error = Io_ErrorRead;
        return -1;
    }

    io->io_pos += ret;
    return ret;
}
i64 generic_io::write(void * vbuf, i64 len, i64 offset)const
{
    generic_io *io = (generic_io *)this;
    if (!is_open ())
    {
        io->io_error = Io_ErrorNotOpen;
        return -1;
    }
#if kyOSIsWin32
    i64 ret = -1;
#else
    i64 ret = ::pwrite(fd, vbuf, len, offset);
#endif
    if (ret < 0)
    {
        io->io_error = Io_ErrorWrite;
        return -1;
    }

    io->io_pos += ret;
    return ret;
}
bool generic_io::seek  (i64 pos)
{
    if (!is_open ())
    {
        io_error = Io_ErrorNotOpen;
        return false;
    }

    io_pos = (::lseek(fd, pos, SEEK_SET));
    return io_pos;
}

bool generic_io::at_end()const
{
    generic_io *io = (generic_io *)this;
    if (!is_open ())
    {
        io->io_error = Io_ErrorNotOpen;
        return false;
    }
    i64 cur = io->tell();
    i64 end = ::lseek(fd, 0, SEEK_END);
    io->io_eof = cur >= end;
    return io_eof;
}

ky_string generic_io::errors()const
{
    switch (error())
    {
    case Io_ErrorNotReady:
        return "Request submitted in asynchronous or non-blocking mode is not completed";
    case Io_ErrorStat:
        return "Get IO status error";
    case Io_ErrorOpen:
        return "Open IO error";
    case Io_ErrorRead:
        return "An IO occurred while reading the file content";
    case Io_ErrorWrite:
        return "An IO occurred while writing to the file content";
    case Io_ErrorClose:
        return "Close IO error";
    case Io_ErrorNotOpen:
        return "IO is not open";
    case Io_ErrorBusy:
        return "IO is busy when submitting requests in asynchronous or non-blocking mode";
    case Io_ErrorPathEmpty:
        return "Unspecified IO Path";
    case Io_ErrorAlsoWR:
        return "Some IOs do not support simultaneous read and write operations";
    case Io_ErrorNotDirect:
        return "Some IOs do not support direct manipulation";
    case Io_ErrorRegularFile:
        return "Opened IO is not a regular file";
    case Io_ErrorDirectory:
        return "Opened IO is not a directory";
    }
    return "Unknown IO error";
}

bool generic_io::create (const ky_string &path, int  mode)
{
#  if kyOSIsWin32
    int ffd = ::_wcreat(path.to_wchar(), mode);
    if (ffd != -1)
        return ::close(ffd) == 0;
    return false;
#  else
    int ffd = ::creat((const char*)path.to_utf8().data(), mode);
    if (ffd != -1)
        return ::close(ffd) == 0;
    return false;
#  endif
}
bool generic_io::remove (const ky_string &path)
{
#  if kyOSIsWin32
    return ::_wunlink (path.to_wchar()) == 0 ;
#  else
    return ::unlink ((const char*)path.to_utf8().data()) == 0 ;
#  endif
}
bool generic_io::rename (const ky_string &path_s, const ky_string& path_d)
{
#  if kyOSIsWin32
    return ::_wrename (path_s.to_wchar(), path_d.to_wchar()) == 0 ;
#  else
    return ::rename ((const char*)path_s.to_utf8().data(),
                     (const char*)path_d.to_utf8().data()) == 0 ;
#  endif
}
bool generic_io::access (const ky_string &path)
{
#  if kyOSIsWin32
    return ::_waccess(path.to_wchar(), 0) == 0;
#  else
    return ::access((const char*)path.to_utf8().data(), 0) == 0;
#  endif
}
i64 generic_io::size(const ky_string &path)
{
#if kyOSIsWin32
    struct _stat64i32 file_stat;
    if (_wstat64i32(path.to_wchar(), &file_stat) == 0)
    {
        return file_stat.st_size;
    }
#else
    struct stat file_stat;
    if (::stat((const char*)path.to_utf8 ().data(), &file_stat) == 0)
    {
        return file_stat.st_size;
    }
#endif
    return -1;
}
