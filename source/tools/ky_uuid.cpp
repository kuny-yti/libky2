#include "tools/ky_uuid.h"
#include "tools/ky_datetime.h"
#include "crypto/crypto_md5.h"

#include <sys/timeb.h>

/* microsecond per second. 1s=1000000us=1000000000ns*/
#define NSec100_Per_Sec        10000000
#define USec_Per_Sec        1000000
#define USec_Per_MSec        1000
#define NSec_Since_1582        ((int64)(0x01B21DD213814000))

static bool isbigendian()
{
    int c = 1;
    return ( *((uchar *) &c) == 1 )? false: true;
}
static void swap_word( int size_bytes, void * ptr_word )
{
    int        i;
    uchar       temp;
    for( i=0; i < size_bytes/2; i++ )
    {
        temp = ((uchar *) ptr_word)[i];
        ((uchar *) ptr_word)[i] = ((uchar *) ptr_word)[size_bytes-i-1];
        ((uchar *) ptr_word)[size_bytes-i-1] = temp;
    }
}
static void write_word( uchar* stream, uint16 val )
{
    memcpy(stream, &val, 2);
    if( isbigendian() ) swap_word( 2, stream );
}
static void write_dword( uchar* stream, uint32 val )
{
    memcpy(stream, &val, 4);
    if( isbigendian() ) swap_word( 4, stream );
}
static void read_word( const uchar* stream, uint16* val )
{
    memcpy( val, stream, 2 );
    if( isbigendian() )    swap_word( 2, val );
}
static void read_dword( const uchar* stream, uint32* val )
{
    memcpy( val, stream, 4 );
    if( isbigendian() )    swap_word( 4, val );
}
static bool is_xdigit(char c)
{
    /* isxdigit returns a non-zero value if c is a hexadecimal digit (A – F, a – f, or 0 – 9). */
    return ((c>='A'&&c<='F')||(c>='a'&&c<='f')||(c>='0'&&c<='9'))? true : false;
}
/* make a pseudorandom numbel based on current time*/
static int pseudo_rand()
{
    struct timeb  timebuf;
    ftime(&timebuf);
    srand((uint32) ((((uint32)timebuf.time & 0xFFFF)+
                     (uint32)timebuf.millitm) ^ (uint32)timebuf.millitm));

    return rand();
}

bool ky_uuid::is_uuid(const char *uuid)
{
    static const char fmt[] = "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx";
    if (uuid == NULL)
        return false;
    for (int i = 0; i < (int)sizeof(fmt); i++)
        if (fmt[i] == 'x')
        {
            if (!is_xdigit(uuid[i]))
                return false;
        }
        else if (uuid[i] != fmt[i])
            return false;

    return true;
}


/**
 * internal
 * ingroup uuid
 * The thread synchronization lock used to guarantee UUID uniqueness
 * for all the threads running within a process.
 */
ky_uuid ky_uuid::create()
{
    static bool      initialized = false;
    static int64     timestamp;
    static uint32    advance;
    static uint16    clockseq;
    static uint16    node_high;
    static uint32    node_low;
    int64            time;    /* unit of 100ns */
    uint16           nowseq;
    int              r;
    ky_uuid u;

    //struct __timeb64  tv;
    //_ftime64(&tv);
    /* time is counter of 100ns time interval since Oct.15, 1582 (NOT 1852) */
    time = ky_datetime::microsec() / 1000; //((int64) tv.time) * USec_Per_Sec + ((int64) tv.millitm*USec_Per_MSec);
    time = time * 10 + NSec_Since_1582;

    if (!initialized)
    {
        timestamp = time;
        advance = 0;

        r = pseudo_rand();

        clockseq = r >> 16;
        node_high = r | 0x0100;

        node_low = pseudo_rand();

        initialized = true;
    }
    else if (time < timestamp)
    {
        timestamp = time;
        advance = 0;
        clockseq++;
    }
    else if (time == timestamp)
    {
        advance++;
        time += advance;
    }
    else
    {
        timestamp = time;
        advance = 0;
    }
    nowseq = clockseq;

    u.data1 = (uint32) time;
    u.data2 = (uint16) ((time >> 32) & 0xffff);
    u.data3 = (uint16) (((time >> 48) & 0x0ffff) | 0x1000);
    write_word(&(u.data4[6]), (uint16) ((nowseq & 0x3fff) | 0x8000));
    write_word(&(u.data4[4]), (uint16) (node_high));
    write_dword(&(u.data4[0]), (uint32) (node_low));
    return u;
}
/**
 * internal
 * ingroup uuid
 * The thread synchronization lock used to guarantee UUID uniqueness
 * for all the threads running within a process.
 */
ky_string ky_uuid::create_string()
{
    ky_uuid ud = create();
    return ud.to_string();
}


ky_string ky_uuid::to_string()
{
    ushort a,b; uint32  c;
    read_word(&(data4[6]), &a);
    read_word(&(data4[4]), &b);
    read_dword(&(data4[0]), &c);
    ky_string ustr;
    ustr.format("%08lx-%04x-%04x-%04x-%04x%08lx",
                data1,
                data2,
                data3,
                a, b, c);
    return ustr;
}

/**
 * internal
 * ingroup uuid
 * The predefined namespace UUID. Expressed in binary format
 * to avoid unnecessary conversion when generating name based UUIDs.
 */
static const ky_byte namespace_uuid = {
        0x9c, 0xfb, 0xd9, 0x1f, 0x11, 0x72, 0x4a, 0xf6,
        0xbd, 0xcb, 0x9f, 0x34, 0xe4, 0x6f, 0xa0, 0xfb
};

ky_uuid  ky_uuid::create_external(const ky_string &external)
{
    crypto_md5 md5;
    ky_uuid u;

    md5.update(namespace_uuid);
    md5.update(external.to_byte());
    ky_byte uuid = md5.final();

    u.data1 = (uint32) (uuid[0] << 24 | uuid[1] << 16 | uuid[2] << 8 | uuid[3]);
    u.data2 = (uint16) (uuid[4] << 8 | uuid[5]);
    u.data3 = (uint16) (((uuid[6] & 0x0f) | 0x30) << 8 | uuid[7]);

    /* BYTE 6-7 */
    write_word(&(u.data4[6]), (uint16) (((uuid[8] & 0x3f) | 0x80) << 8 | uuid[9]));
    /* BYTE 4-5 */
    write_word(&(u.data4[4]), (uint16) (uuid[10] << 8 | uuid[11]));
    /* BYTE 0-3 */
    write_dword(&(u.data4[0]), (uint32) (uuid[12] << 24 | uuid[13] << 16 | uuid[14] << 8 | uuid[15]));
    return u;
}

/**
 * Get timestamp from a UUID
 **/
ky_datetime ky_uuid::to_timestamp()
{
    int64   time, t2, t3;

    t2 = data2;
    t3 = data3;

    time = data1 + (t2<<32) + ((t3&0x0fff)<<48);        /* 100ns */
    time -= NSec_Since_1582;

    time /= 10;
    return ky_datetime(time *1000);
}

#include "tools/ky_stream.h"
ky_streamb &operator << (ky_streamb &in, const ky_uuid &v)
{
    in << v.data1;
    in << v.data2 ;
    in << v.data3 ;
    in << v.data4[0] ;
    in << v.data4[1] ;
    in << v.data4[2] ;
    in << v.data4[3] ;
    in << v.data4[4] ;
    in << v.data4[5] ;
    in << v.data4[6] ;
    in << v.data4[7] ;
    return in;
}
ky_streamb &operator >> (ky_streamb &out, ky_uuid &v)
{
    out >> v.data1;
    out >> v.data2 ;
    out >> v.data3 ;
    out >> v.data4[0] ;
    out >> v.data4[1] ;
    out >> v.data4[2] ;
    out >> v.data4[3] ;
    out >> v.data4[4] ;
    out >> v.data4[5] ;
    out >> v.data4[6] ;
    out >> v.data4[7] ;
    return out;
}
ky_streamt &operator << (ky_streamt &in, const ky_uuid &v)
{
    in << v.data1;
    in << v.data2 ;
    in << v.data3 ;
    in << v.data4[0] ;
    in << v.data4[1] ;
    in << v.data4[2] ;
    in << v.data4[3] ;
    in << v.data4[4] ;
    in << v.data4[5] ;
    in << v.data4[6] ;
    in << v.data4[7] ;
    return in;
}
ky_streamt &operator >> (ky_streamt &out, ky_uuid &v)
{
    out >> v.data1;
    out >> v.data2 ;
    out >> v.data3 ;
    out >> v.data4[0] ;
    out >> v.data4[1] ;
    out >> v.data4[2] ;
    out >> v.data4[3] ;
    out >> v.data4[4] ;
    out >> v.data4[5] ;
    out >> v.data4[6] ;
    out >> v.data4[7] ;
    return out;
}


ky_variant &operator << (ky_variant &va, const ky_uuid &uid)
{
    va.set(kyUuidCode, 0, (void*)&uid, sizeof(ky_uuid));
    return va;
}
ky_variant &operator >> (ky_variant &va, ky_uuid &uid)
{
    uid = va.get<ky_uuid>(kyUuidCode);
    return va;
}
