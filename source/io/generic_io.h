#ifndef GENERIC_IO_H
#define GENERIC_IO_H

#include "ky_define.h"
#include "ky_string.h"
#include "iio.h"

#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include <dirent.h>
#include <libgen.h>
#include <sys/stat.h>

#if kyOSIsLinux
#include <sys/mman.h>
#endif
#include <sys/types.h>

class generic_io : public iio
{
    friend class ky_fsys;
public:
    explicit generic_io(const eIoTypeFlags &type);
    virtual ~generic_io();

    virtual bool open(const eIoModeFlags &mode = Io_ReadWrite);
    virtual void close ();

    virtual ky_string errors()const ;

    virtual bool seek(i64 pos);

    virtual i64 read(void *buf, i64 len)const;
    virtual i64 write(void *buf, i64 len)const ;
    virtual i64 read(void *, i64, i64)const;
    virtual i64 write(void *, i64, i64)const;

    virtual i64 tell()const;
    virtual bool at_end()const ;

public:
    static bool create (const ky_string &file, int  mode);
    static bool remove (const ky_string &file) ;
    static bool rename (const ky_string &oldfile, const ky_string& newfile);
    static bool access (const ky_string &path);

    static i64 size(const ky_string &path);

public:
    static FILE *out;
    static FILE *err;
    static FILE *in;

protected:
    bool              is_std;
    int               fd;
#if kyOSIsLinux
    struct stat       file_stat;
#elif kyOSIsWin32
    struct _stat64i32 file_stat;
#endif
};

#endif // GENERIC_IO_H
