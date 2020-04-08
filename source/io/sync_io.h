#ifndef SYNC_IO_H
#define SYNC_IO_H

#include "generic_io.h"

class sync_io : public generic_io
{
public:
    sync_io(const eIoTypeFlags & type);
    virtual ~sync_io();

    virtual bool open(const eIoModeFlags &mode = Io_ReadWrite);

    virtual i64 read(void *buf, i64 len)const;
    virtual i64 write(void *buf, i64 len)const ;
    virtual i64 read(void *, i64, i64)const;
    virtual i64 write(void *, i64, i64)const;
};

#endif // SYNC_IO_H
