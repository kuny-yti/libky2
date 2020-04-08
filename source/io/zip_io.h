#ifndef ZIP_IO_H
#define ZIP_IO_H

#include "interface/iio.h"
#include "tools/ky_date.h"

const static int aMaxFileNameLength = 256;
struct tArInfos
{
    /// File name.
    ky_string name;
    /// Version created by.
    uint16 version_created;
    /// Version needed to extract.
    uint16 version_needed;
    /// General purpose flags.
    uint16 flags;
    /// Compression method.
    uint16 method;
    /// Last modification date and time.
    ky_datetime datetime;
    /// CRC.
    uint32 crc;
    /// Compressed file size.
    int64 compressed_size;
    /// Uncompressed file size.
    int64 uncompressed_size;
    /// Disk number start.
    uint16 disk_start;
    /// Internal file attributes.
    uint16 internal_attr;
    /// External file attributes.
    uint32 external_attr;
    /// Comment.
    ky_string comment;
    /// Extra field.
    ky_vector<char> extra;
};
typedef ky_list<tArInfos> tArInfoLists;

namespace impl
{
struct zip;
struct decompress;
struct compress;
}
class zip_io : public iio
{
public:
    zip_io(eArchiveModes m);
    zip_io(const ky_string &f, eArchiveModes m);
    ~zip_io();

    virtual bool open(int m = Archive_DeCompress);
    virtual void close();
    tArInfoLists info();
private:
    virtual int64 read(void *, int64 )const
    {
        return -1;
    }
    virtual int64 write(void *, int64 )const
    {
        return -1;
    }
    virtual bool seek(int64 )
    {
        return false;
    }
    virtual ky_string errors()const{return ky_string();}

private:
    impl::zip *impl;
    friend class zip_decompress;
    friend class zip_compress;
    friend struct impl::decompress;
    friend struct impl::compress;
};

class zip_decompress : public iio
{
public:
    zip_decompress(const piio &zipio);
    ~zip_decompress();

    virtual bool open(int = Archive_DeCompress);
    virtual void close();

    virtual int64 read(void *buff, int64 max_len)const;
    virtual int64 tell()const;
    virtual bool at_end()const;

private:
    virtual int64 write(void *, int64 )const
    {
        return -1;
    }
    virtual bool seek(int64 )
    {
        return false;
    }
    virtual ky_string errors()const{return ky_string();}

private:
    impl::decompress *impl;
};

class zip_compress : public iio
{
public:
    zip_compress(const piio &zipio);
    ~zip_compress();

    virtual bool open(int = Archive_Compress);
    virtual void close();

    virtual int64 write(void *, int64 )const;

private:
    virtual int64 read(void *, int64 )const{return -1;}
    virtual bool seek(int64 ){return false;}
    virtual ky_string errors()const{return ky_string();}

private:
    impl::compress *impl;

};

#endif // ZIP_IO_H
