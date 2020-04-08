#include "sync_io.h"

sync_io::sync_io(const eIoTypeFlags &type):
    generic_io(type)
{

}

sync_io::~sync_io()
{

}
bool sync_io::open(const eIoModeFlags & mode)
{
    // mode &= ~Io_ASync;
    return generic_io::open (mode & (~Io_ASync));
}
i64 sync_io::read(void *vbuf, i64 len)const
{
    sync_io *take = (sync_io *)this;

    char *buf = (char*)vbuf;
    i64 left = len;
    while ((left > 0) && (buf != 0))
    {
        const i64 amount = generic_io::read(buf, left);

        if (amount > 0)
        {
            buf += (i64)(amount);
            left -= (i64)(amount);
            ky_memory::zero (buf, left);
        }
        else if (amount == 0)
        {
            take->io_eof = true;
            break;
        }
        else
        {
            take->io_error = Io_ErrorRead;
            return -1;
        }
    }

    return len - left;
}
i64 sync_io::write(void *vbuf, i64 len) const
{
    sync_io *take = (sync_io *)this;
    char *buf = ( char *)vbuf;
    i64 size = len;
    while ((size > 0) && (buf != 0))
    {
        const i64 amount = generic_io::write(buf, size);
        if (amount > 0)
        {
            buf += (i64)(amount);
            size -= (i64)(amount);
        }
        else
        {
            take->io_error = Io_ErrorWrite;
            return -1;
        }
    }
    return len - size;
}

i64 sync_io::read(void *vbuf, i64 len, i64 offset)const
{
    sync_io *take = (sync_io *)this;
    char *buf = ( char *)vbuf;
    i64 left = len;
    while ((left > 0) && (buf != 0))
    {
        const i64 amount = generic_io::read(buf, left, offset);

        if (amount > 0)
        {
            buf += (i64)(amount);
            left -= (i64)(amount);
            ky_memory::zero (buf, left);
        }
        else if (amount == 0)
        {
            take->io_eof = true;
            break;
        }
        else
        {
            take->io_error = Io_ErrorRead;
            return -1;
        }
    }

    return len - left;
}
i64 sync_io::write(void *vbuf, i64 len, i64 offset)const
{
    sync_io *take = (sync_io *)this;
    char *buf = ( char *)vbuf;
    i64 size = len;
    while ((size > 0) && (buf != 0))
    {
        const i64 amount = generic_io::write(buf, size, offset);
        if (amount > 0)
        {
            buf += (i64)(amount);
            size -= (i64)(amount);
        }
        else
        {
            take->io_error = Io_ErrorWrite;
            return -1;
        }
    }
    return len - size;
}
