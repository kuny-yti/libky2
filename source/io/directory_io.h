#ifndef DIRECTOR_IO_H
#define DIRECTOR_IO_H

#include "interface/iio.h"
#include "tools/ky_stack.h"

#if kyCompilerIsGNUC || kyCompilerIsCLANG
#  include <unistd.h>
#  include <sys/stat.h>
#  include <sys/types.h>
#  include <dirent.h>
#endif

namespace impl
{
#if !kyOSIsWin32
#   define dirHandle  DIR
#   define dirDirent dirent
#   define dirOpen  opendir
#   define dirRead  readdir
#   define dirScan  scandir
#   define dirTell  telldir
#   define dirSeek  seekdir
#   define dirClose closedir
#   define dirRm rmdir
#   define dirMk mkdir
#   define dirCh chdir
#else
#   define dirHandle  _WDIR
#   define dirDirent _wdirent
#   define dirOpen  _wopendir
#   define dirRead  _wreaddir
#   define dirScan  _wscandir
#   define dirTell  _wtelldir
#   define dirSeek  _wseekdir
#   define dirClose _wclosedir
#   define dirRm _wrmdir
#   define dirMk _wmkdir
#   define dirCh _wchdir
#endif

}

class directory_io : public iio
{
    friend class ky_fsys;
public:
    directory_io();
    virtual ~directory_io();

    virtual bool open(int mode = Io_ReadWrite);
    virtual void close();

    virtual ky_string errors()const{return ky_string();}

    ky_string next();
    ky_string prev();

    static ky_string_list scan(const ky_string& dir, const ky_string &filter = ky_string());

protected:
    static bool remove(const ky_string& dir);
    static bool create(const ky_string& dir);
    static bool change(const ky_string & dir);
    static ky_string current(i64 size);
    static bool is_directory(const ky_string &path);

private:
    virtual bool seek(i64){return false;}
    virtual i64 read(void *, i64 )const{return -1;}
    virtual i64 write(void *, i64 )const{return -1;}

    dirHandle      *dir_handle;
    ky_stack<i64> offset;
};

#endif // DIRECTOR_IO_H
