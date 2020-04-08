#include "zip_io.h"
#include "tools/ky_map.h"

#include <io/zip/zip.h>
#include <io/zip/unzip.h>

namespace impl
{

struct zip
{
    zip_io *self;
    ky_string comment;
    voidp     handle;
    ky_string passwd;
    ky_string filename;

    unz64_file_pos unz_last_map;
    ky_map<ky_string, unz64_file_pos> unz_map;

    bool first_unmapped_file()
    {
        self->io_error = UNZ_OK;

        if (this->unz_last_map.pos_in_zip_directory == 0)
            self->io_error = unzGoToFirstFile(handle);
        else
        {
            self->io_error = unzGoToFilePos64(handle, &this->unz_last_map);
            self->io_error = unzGoToNextFile(handle);
        }

        return (int)self->io_error == UNZ_OK;
    }
    bool first_file()
    {
        self->io_error = UNZ_OK;
        self->io_error = unzGoToFirstFile(handle);
        return (int)self->io_error == UNZ_OK;
    }
    bool next_file()
    {
        self->io_error = UNZ_OK;
        self->io_error = unzGoToNextFile(handle);
        return (int)self->io_error == UNZ_OK;
    }
    void add_map(const ky_string &fileName)
    {
        if (fileName.is_empty())
            return;

        unz64_file_pos fdPos;
        unzGetFilePos64(handle, &fdPos);
        unz_map.insert(fileName, fdPos);

        if (fdPos.pos_in_zip_directory > unz_last_map.pos_in_zip_directory)
            unz_last_map = fdPos;
    }

    bool select(const ky_string& fileName)
    {
        self->io_error = UNZ_OK;
        if(fileName.is_empty())
            return false;

        if(handle == NULL)
            return false;

        if(fileName.length() > aMaxFileNameLength)
            return false;

        ky_string current;
        unz64_file_pos fileDirPos;
        fileDirPos.pos_in_zip_directory = 0;
        ky_map<ky_string, unz64_file_pos>::iterator it;
        for (it = unz_map.begin (); it != unz_map.end (); ++it)
        {
            if (it.key() == fileName)
                fileDirPos =  it.value();
        }

        if (fileDirPos.pos_in_zip_directory != 0)
        {
            self->io_error = unzGoToFilePos64(handle, &fileDirPos);
            if ((int)self->io_error == UNZ_OK)
                return true;
        }

        for(bool more = first_unmapped_file(); more; more = next_file())
        {
            current = select();
            if(current.is_empty())
                return false;

            if(current == fileName)
                break;
        }
        return (int)self->io_error == UNZ_OK;
    }
    ky_string select()
    {
        self->io_error = UNZ_OK;
        if(!self->is_open ())
            return ky_string();

        ky_vector<char> fileName;
        fileName.resize(aMaxFileNameLength, 0);
        if((self->io_error = unzGetCurrentFileInfo64(handle, NULL, fileName.data(), fileName.count(),
                                                     NULL, 0, NULL, 0)) != UNZ_OK)
            return ky_string();
        ky_string result = ky_string(fileName.data());
        if (result.is_empty())
            return result;
        this->add_map(result);
        return result;
    }

    bool info(tArInfos *info)
    {
        self->io_error = UNZ_OK;
        unz_file_info64 info_z;
        ky_vector<char> fileName, extra, comment;

        if ((info == NULL) || !handle)
            return false;

        if((self->io_error = unzGetCurrentFileInfo64(handle, &info_z, NULL, 0, NULL, 0, NULL, 0)) != UNZ_OK)
            return false;
        fileName.resize(info_z.size_filename);
        extra.resize(info_z.size_file_extra);
        comment.resize(info_z.size_file_comment);
        if((self->io_error = unzGetCurrentFileInfo64(handle, NULL, fileName.data(), fileName.count(),
                                                     extra.data(), extra.count(), comment.data(), comment.count()))!=UNZ_OK)
            return false;
        info->version_created = info_z.version;
        info->version_needed = info_z.version_needed;
        info->flags = info_z.flag;
        info->method = info_z.compression_method;
        info->crc = info_z.crc;
        info->compressed_size = info_z.compressed_size;
        info->uncompressed_size = info_z.uncompressed_size;
        info->disk_start = info_z.disk_num_start;
        info->internal_attr = info_z.internal_fa;
        info->external_attr = info_z.external_fa;
        info->name = ky_string(fileName.data(), fileName.count());
        info->comment = ky_string(comment.data(), comment.count());
        info->extra = extra;
        info->datetime = ky_datetime(
                    info_z.tmu_date.tm_year, info_z.tmu_date.tm_mon+1, info_z.tmu_date.tm_mday,
                    info_z.tmu_date.tm_hour, info_z.tmu_date.tm_min, info_z.tmu_date.tm_sec);
        // Add to directory map
        this->add_map(info->name);
        return true;
    }

    ky_string _comment_()
    {
        self->io_error = UNZ_OK;
        unz_global_info64 globalInfo;
        ky_vector<char> comment;
        if((self->io_error = unzGetGlobalInfo64(handle, &globalInfo)) != UNZ_OK)
            return ky_string();
        comment.resize(globalInfo.size_comment);
        if((self->io_error = unzGetGlobalComment(handle, comment.data(), comment.count())) < 0)
            return ky_string();
        return ky_string(comment.data(), comment.count());
    }

    static ky_string zip_passwd(const ky_string &inurl, ky_string &path)
    {
        if (inurl.is_empty ())
            return ky_string();

        ky_string_list stli = inurl.split ("@");
        if (stli.count () >= 1)
            path = inurl;
        if (stli.count () >= 2)
            return stli[1];
        return ky_string();
    }
    bool zip_open(const ky_string &fpn, zlib_filefunc_def* ioApi)
    {
        if(fpn.is_empty())
            return false;
        if ((int)self->mode() == Archive_DeCompress)
        {
            if (ioApi == NULL)
                handle = unzOpen2_64(fpn.to_utf8().data(), NULL);
            else
                handle = unzOpen2(fpn.to_utf8().data(), ioApi);

            if(handle == NULL)
                return false;
        }
        else
        {
            if (ioApi == NULL)
            {
                handle = zipOpen2_64(fpn.to_utf8().data(),
                                     (int)self->mode() == Archive_Create ? APPEND_STATUS_CREATE:
                                     (int)self->mode() == Archive_Append ? APPEND_STATUS_CREATEAFTER:
                                                                           APPEND_STATUS_ADDINZIP,
                                     NULL, NULL);
            } else {
                handle = zipOpen2(fpn.to_utf8().data(),
                                  (int)self->mode() == Archive_Create ? APPEND_STATUS_CREATE:
                                  (int)self->mode() == Archive_Append ? APPEND_STATUS_CREATEAFTER:
                                                                        APPEND_STATUS_ADDINZIP,
                                  NULL, ioApi);
            }
            if(handle == NULL)
                return false;
        }

        self->io_open = true;
        return true;
    }
};
struct decompress
{
    zip_decompress *self;
    piio io;
    tArInfoLists infolist;
    tArInfos curinfo;

    zip_io *zipio()const
    {
        static zip_io *sio = 0;
        if (sio == 0 || io.get () != sio)
            sio = io.as<zip_io>().get ();
        return sio;
    }

    bool read_open(const ky_string &f_name, const char *password, int *method = 0, int *level = 0)
    {
        if (zipio() == 0 || !zipio()->is_open () || self->is_open ())
            return false;

        if(!zipio()->impl->select(f_name))
        {
            self->close();
            return false;
        }

        self->io_error = UNZ_OK;
        self->io_error = (unzOpenCurrentFile3(zipio()->impl->handle, method, level, (int)self->io_error, password));
        if(self->io_error != UNZ_OK)
            return false;

        infolist = zipio()->info ();
        foreach (const tArInfos &inf, infolist)
        {
            if (inf.name == f_name)
            {
                curinfo = inf;
                break;
            }
        }
        self->io_size = curinfo.uncompressed_size;
        self->io_open = true;
        return true;
    }

};

struct compress
{
    piio io;
    zip_compress *self;

    int32 zip_crc;
    bool is_raw;

    zip_io *zipio()const
    {
        static zip_io *sio = 0;
        if (sio == 0 || io.get () != sio)
            sio = io.as<zip_io>().get ();
        return sio;
    }

    bool write_open(const tArInfos& info,
                    const char *password, bool DDWE = false,
                    uint32 crc = 0, int method = Z_DEFLATED,
                    int level = Z_DEFAULT_COMPRESSION, bool raw = false,
                    int windowBits = -MAX_WBITS, int memLevel = DEF_MEM_LEVEL,
                    int strategy = Z_DEFAULT_STRATEGY)
    {
        zip_fileinfo info_z;
        self->io_error = ZIP_OK;
        if(!io->is_open() || self->is_open ())
            return false;

        if((self->mode ()!= Archive_Create) && (self->mode () != Archive_Append) &&
                (self->mode () != Archive_Add))
            return false;

        info_z.tmz_date.tm_year = info.datetime.year();
        info_z.tmz_date.tm_mon = info.datetime.month() - 1;
        info_z.tmz_date.tm_mday = info.datetime.day();
        info_z.tmz_date.tm_hour = info.datetime.hour();
        info_z.tmz_date.tm_min = info.datetime.minute();
        info_z.tmz_date.tm_sec = info.datetime.second();
        info_z.dosDate = 0;
        info_z.internal_fa = (uLong)info.internal_attr;
        info_z.external_fa = (uLong)info.external_attr;

        if (!DDWE)
            zipClearFlags(zipio()->impl->handle, ZIP_WRITE_DATA_DESCRIPTOR);

        self->io_error = zipOpenNewFileInZip3_64(zipio()->impl->handle,
                                        (char*)info.name.to_utf8().data(), &info_z,
                                        info.extra.data(), info.extra.size(),
                                        info.extra.data(), info.extra.size(),
                                        (char*)info.comment.to_utf8().data(),
                                        method, level, (int)raw,
                                        windowBits, memLevel, strategy,
                                        password, (uLong)crc, true);
        if((int)self->io_error == UNZ_OK)
        {
            self->io_pos = 0;

            if(raw) {
                zip_crc = crc;
                self->io_size = info.uncompressed_size;
            }
            self->io_open = true;
            return true;
        }

        return false;
    }
};
}
zip_io::zip_io(eArchiveModes m)
{
    impl = kyNew (impl::zip);
    impl->unz_last_map.num_of_file = 0;
    impl->unz_last_map.pos_in_zip_directory = 0;
    impl->unz_map.clear ();
    impl->self = this;

    io_mode = m;
    io_eof  = false;
    io_error = 0;
    io_open = false;
    io_pos = 0;
    io_size = 0;
    impl->handle = 0;
}
zip_io::zip_io(const ky_string &f, eArchiveModes m)
{
    impl = kyNew (impl::zip);
    impl->unz_last_map.num_of_file = 0;
    impl->unz_last_map.pos_in_zip_directory = 0;
    impl->unz_map.clear ();
    impl->self = this;

    io_mode = m;
    io_eof  = false;
    io_error = 0;
    io_open = false;
    io_pos = 0;
    io_size = 0;
    io_url = ky_url(f);
    impl->handle = 0;
    if (!f.is_empty ())
        open(m);
}
zip_io::~zip_io()
{
    close ();
    kyDelete (impl);
}

bool zip_io::open(int)
{
    impl->passwd = impl->zip_passwd(io_url.path (), impl->filename);
    return impl->zip_open(impl->filename, NULL);
}
void zip_io::close()
{
    if (!is_open())
        return ;
    io_error = UNZ_OK;
    if ((int)io_mode ==  Archive_DeCompress)
        io_error = unzClose(impl->handle);
    else
        io_error = zipClose(impl->handle, impl->comment.is_empty() ? NULL : (char*)impl->comment.to_utf8().data());
    if ((int)io_error == UNZ_OK)
    {
        io_open = false;
        impl->handle = NULL;
    }
    impl->unz_last_map.num_of_file = 0;
    impl->unz_last_map.pos_in_zip_directory = 0;
    impl->unz_map.clear ();
}
tArInfoLists zip_io::info()
{
    tArInfoLists result;
    if (!is_open())
        return result;
    if (impl->first_file())
    {
        do
        {
            tArInfos ai;
            bool ok = impl->info(&ai);
            result.append(ai);
            if (!ok)
                return result;
        } while (impl->next_file());
    }
    return result;
}


zip_decompress::zip_decompress(const piio &zipio)
{
    impl = kyNew (impl::decompress);
    impl->io = zipio;
    impl->self = this;
    if (impl->io.get () && impl->io->io_mode != Archive_DeCompress)
    {
        impl->io.reset ();
        return ;
    }

    io_mode = Archive_DeCompress;
    io_eof  = false;
    io_error = 0;
    io_open = false;
    io_pos = 0;
    io_size = 0;
}
zip_decompress::~zip_decompress()
{
    close();
    kyDelete (impl);
}
bool zip_decompress::open(int)
{
    if (impl->io.get () && !impl->io->is_open ())
    {
        if (!impl->io->open ())
            return false;
    }

    return impl->read_open(io_url.path (),
                           impl->zipio()->impl->passwd.is_empty ()? 0 :
                           (const char *)impl->zipio()->impl->passwd.to_utf8 ().data ());
}
void zip_decompress::close()
{
    if (is_open ())
    {
        io_error = (unzCloseCurrentFile(impl->zipio()->impl->handle));
        if(io_error != UNZ_OK)
            return;
    }

    io_open = false;
}

int64 zip_decompress::read(void *buff, int64 max_len)const
{
    if (!is_open () || impl->zipio() == 0)
        return -1;

    int64 bytesRead=  unzReadCurrentFile(impl->zipio()->impl->handle, buff, (unsigned)max_len);
    if (bytesRead < 0)
        return -1;

    return bytesRead;
}
int64 zip_decompress::tell()const
{
    zip_decompress * fake = (zip_decompress *)(this);
    if(fake->impl->zipio() == 0 || fake->impl->io.get () == 0 || !fake->impl->io->is_open () || !is_open())
        return -1;
    fake->io_pos = unztell(fake->impl->zipio()->impl->handle);
    return io_pos;
}
bool zip_decompress::at_end()const
{
    zip_decompress * fake = (zip_decompress *)(this);
    if(fake->impl->zipio() == 0 || fake->impl->io.get () == 0 || !fake->impl->io->is_open () || !is_open())
        return true;
    fake->io_eof = unzeof(fake->impl->zipio()->impl->handle ) == 1;
    return io_eof;
}

zip_compress::zip_compress(const piio &zipio)
{
    impl = kyNew (impl::compress);
    impl->io = zipio;
    impl->self = this;
}
zip_compress::~zip_compress()
{
    close();
    kyDelete (impl);
}

bool zip_compress::open(int)
{
    static int32 crc;
    tArInfos info;
    info.crc = crc;
    info.datetime = ky_datetime::current_time ();
    info.name = this->path ();
    info.uncompressed_size = this->io_size;

    return impl->write_open(info, impl->zipio ()->impl->passwd.is_empty () ? 0 :
                                (const char *)impl->zipio ()->impl->passwd.to_utf8 ().data ());
}
void zip_compress::close()
{
    if (is_open ())
    {
        if(impl->is_raw)
            io_error = zipCloseFileInZipRaw64(impl->zipio ()->impl->handle, io_size, impl->zip_crc);
        else
            io_error = zipCloseFileInZip(impl->zipio ()->impl->handle);

        if(io_error != UNZ_OK)
            return;
    }

    impl->is_raw = false;
    impl->zip_crc = 0;
    io_open = false;
}

int64 zip_compress::write(void *buff, int64 max_len)const
{
    if (!impl->io->is_open() || !is_open ())
        return -1;

    zip_compress * fake = (zip_compress *)(this);
    fake->io_error = ZIP_OK;
    fake->io_error = zipWriteInFileInZip(impl->zipio ()->impl->handle, buff, (uint)max_len);
    if(io_error != ZIP_OK)
        return -1;

    fake->io_pos += max_len;
    return max_len;
}
