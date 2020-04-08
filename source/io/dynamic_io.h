#ifndef DYNAMIC_IO_H
#define DYNAMIC_IO_H

#include "interface/iio.h"

#if kyOSIsWin32
#  include <windows.h>
#elif kyOSIsApple || kyOSIsUnix || kyOSIsLinux || kyOSIsAndroid || kyOSIsIos
#  include <dlfcn.h>
#else

#endif

class dynamic_io : public iio
{
    friend class ky_fsys;
public:
    dynamic_io();
    ~dynamic_io();

    virtual ky_string errors()const;

    virtual bool open(int = Io_ReadWrite);
    virtual void close() ;

    static ky_string suffix() ;
    void *symbol(const ky_string &name);

private:
    virtual i64 read(void *, i64 )const{return -1;}
    virtual i64 write(void *, i64)const{return -1;}
    virtual bool seek(i64){return false;}

    void* _hinst;
};

#endif // DYNAMIC_IO_H
