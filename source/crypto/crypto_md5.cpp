#include "crypto_md5.h"

// 循环移位
#define RLS(a,b,c)    (((a) << (b)) | ((a) >> ((c)-(b))))
#define RRS(a,b,c)    (((a) >> (b)) | ((a) << ((c)-(b))))
// 循环左移
#define RLS32(a,b)    RLS(a,b,32)
#define RLS64(a,b)    RLS(a,b,64)
// 循环右移
#define RRS32(a,b)    RRS(a,b,32)
#define RRS64( a, b ) RRS(a,b,64)

#define F(x,y,z) ((x & y) | (~x & z))
#define G(x,y,z) ((x & z) | (y & ~z))
#define H(x,y,z) (x ^ y ^ z)
#define I(x,y,z) (y ^ (x | ~z))

#define FF(a,b,c,d,m,s,t) { a += F(b,c,d) + m + t; \
                            a = b + RLS32(a,s); }
#define GG(a,b,c,d,m,s,t) { a += G(b,c,d) + m + t; \
                            a = b + RLS32(a,s); }
#define HH(a,b,c,d,m,s,t) { a += H(b,c,d) + m + t; \
                            a = b + RLS32(a,s); }
#define II(a,b,c,d,m,s,t) { a += I(b,c,d) + m + t; \
                            a = b + RLS32(a,s); }

#define MD5_LENGTH           ( 128 / 8 )
static const uint32_t hash_md5s[4] =
{
    0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476
};

struct md5_priv
{
    uint32_t    digest;
    uint32_t    length;
    uint64    bitlen;
    uint32_t   *k;
    uint8    *data;
    uint32_t   *state;

#define SHA_BLOCK_BYTE 64
#define SHA_STATE 4
    uint8   data_memory[SHA_BLOCK_BYTE];
    uint32_t  state_memory[SHA_STATE];
#undef SHA_BLOCK
#undef SHA_STATE

    void transform (uint8* data)
    {
        uint32_t a, b, c, d, m[16], i, j;

        for (i = 0, j = 0; i < 16; ++i, j += 4)
            m[i] = (data[j]) + (data[j + 1] << 8) + (data[j + 2] << 16) +
                    (data[j + 3] << 24);

        a = state[0];
        b = state[1];
        c = state[2];
        d = state[3];

        FF(a,b,c,d,m[0],  7,0xd76aa478);
        FF(d,a,b,c,m[1], 12,0xe8c7b756);
        FF(c,d,a,b,m[2], 17,0x242070db);
        FF(b,c,d,a,m[3], 22,0xc1bdceee);
        FF(a,b,c,d,m[4],  7,0xf57c0faf);
        FF(d,a,b,c,m[5], 12,0x4787c62a);
        FF(c,d,a,b,m[6], 17,0xa8304613);
        FF(b,c,d,a,m[7], 22,0xfd469501);
        FF(a,b,c,d,m[8],  7,0x698098d8);
        FF(d,a,b,c,m[9], 12,0x8b44f7af);
        FF(c,d,a,b,m[10],17,0xffff5bb1);
        FF(b,c,d,a,m[11],22,0x895cd7be);
        FF(a,b,c,d,m[12], 7,0x6b901122);
        FF(d,a,b,c,m[13],12,0xfd987193);
        FF(c,d,a,b,m[14],17,0xa679438e);
        FF(b,c,d,a,m[15],22,0x49b40821);

        GG(a,b,c,d,m[1],  5,0xf61e2562);
        GG(d,a,b,c,m[6],  9,0xc040b340);
        GG(c,d,a,b,m[11],14,0x265e5a51);
        GG(b,c,d,a,m[0], 20,0xe9b6c7aa);
        GG(a,b,c,d,m[5],  5,0xd62f105d);
        GG(d,a,b,c,m[10], 9,0x02441453);
        GG(c,d,a,b,m[15],14,0xd8a1e681);
        GG(b,c,d,a,m[4], 20,0xe7d3fbc8);
        GG(a,b,c,d,m[9],  5,0x21e1cde6);
        GG(d,a,b,c,m[14], 9,0xc33707d6);
        GG(c,d,a,b,m[3], 14,0xf4d50d87);
        GG(b,c,d,a,m[8], 20,0x455a14ed);
        GG(a,b,c,d,m[13], 5,0xa9e3e905);
        GG(d,a,b,c,m[2],  9,0xfcefa3f8);
        GG(c,d,a,b,m[7], 14,0x676f02d9);
        GG(b,c,d,a,m[12],20,0x8d2a4c8a);

        HH(a,b,c,d,m[5],  4,0xfffa3942);
        HH(d,a,b,c,m[8], 11,0x8771f681);
        HH(c,d,a,b,m[11],16,0x6d9d6122);
        HH(b,c,d,a,m[14],23,0xfde5380c);
        HH(a,b,c,d,m[1],  4,0xa4beea44);
        HH(d,a,b,c,m[4], 11,0x4bdecfa9);
        HH(c,d,a,b,m[7], 16,0xf6bb4b60);
        HH(b,c,d,a,m[10],23,0xbebfbc70);
        HH(a,b,c,d,m[13], 4,0x289b7ec6);
        HH(d,a,b,c,m[0], 11,0xeaa127fa);
        HH(c,d,a,b,m[3], 16,0xd4ef3085);
        HH(b,c,d,a,m[6], 23,0x04881d05);
        HH(a,b,c,d,m[9],  4,0xd9d4d039);
        HH(d,a,b,c,m[12],11,0xe6db99e5);
        HH(c,d,a,b,m[15],16,0x1fa27cf8);
        HH(b,c,d,a,m[2], 23,0xc4ac5665);

        II(a,b,c,d,m[0],  6,0xf4292244);
        II(d,a,b,c,m[7], 10,0x432aff97);
        II(c,d,a,b,m[14],15,0xab9423a7);
        II(b,c,d,a,m[5], 21,0xfc93a039);
        II(a,b,c,d,m[12], 6,0x655b59c3);
        II(d,a,b,c,m[3], 10,0x8f0ccc92);
        II(c,d,a,b,m[10],15,0xffeff47d);
        II(b,c,d,a,m[1], 21,0x85845dd1);
        II(a,b,c,d,m[8],  6,0x6fa87e4f);
        II(d,a,b,c,m[15],10,0xfe2ce6e0);
        II(c,d,a,b,m[6], 15,0xa3014314);
        II(b,c,d,a,m[13],21,0x4e0811a1);
        II(a,b,c,d,m[4],  6,0xf7537e82);
        II(d,a,b,c,m[11],10,0xbd3af235);
        II(c,d,a,b,m[2], 15,0x2ad7d2bb);
        II(b,c,d,a,m[9], 21,0xeb86d391);

        state[0] += a;
        state[1] += b;
        state[2] += c;
        state[3] += d;
    }
};

crypto_md5::crypto_md5()
{
    priv = kyNew(md5_priv) ;
    priv->bitlen = 0;
    priv->length = 0;
    priv->k = 0;
    priv->digest = MD5_LENGTH;
    priv->data = priv->data_memory;
    priv->state = priv->state_memory;
    ky_memory::copy(priv->state, hash_md5s, sizeof(hash_md5s));
}
crypto_md5::~crypto_md5()
{
    kyDelete(priv);
}
void crypto_md5::update(const ky_byte &cd)
{
    const uint8 *data = cd.data();
    const int64 size = cd.size();
    for (int64 i = 0; i < size; ++i)
    {
        priv->data[priv->length] = data[i];
        priv->length++;
        if (priv->length == 64)
        {
            priv->transform(priv->data);
            priv->bitlen += 512;
            priv->length = 0;
        }
    }
}

ky_byte crypto_md5::final()
{
    ky_byte hash(priv->digest);
    int64 i = priv->length;

    // Pad whatever data is left in the buffer.
    if (i < 56)
    {
        priv->data[i++] = 0x80;
        while (i < 56)
            priv->data[i++] = 0x00;
    }
    else if (i >= 56)
    {
        priv->data[i++] = 0x80;
        while (i < 64)
            priv->data[i++] = 0x00;
        priv->transform(priv->data);
        ky_memory::zero(priv->data, 56);
    }

    priv->bitlen += priv->length * 8;
    priv->data[56] = priv->bitlen;
    priv->data[57] = priv->bitlen >> 8;
    priv->data[58] = priv->bitlen >> 16;
    priv->data[59] = priv->bitlen >> 24;
    priv->data[60] = priv->bitlen >> 32;
    priv->data[61] = priv->bitlen >> 40;
    priv->data[62] = priv->bitlen >> 48;
    priv->data[63] = priv->bitlen >> 56;
    priv->transform(priv->data);

    for (i = 0; i < 4; ++i)
    {
        hash.data()[i]      = (priv->state[0] >> (i * 8)) & 0xff;
        hash.data()[i + 4]  = (priv->state[1] >> (i * 8)) & 0xff;
        hash.data()[i + 8]  = (priv->state[2] >> (i * 8)) & 0xff;
        hash.data()[i + 12] = (priv->state[3] >> (i * 8)) & 0xff;
    }
    return hash;
}

ky_byte crypto_md5::hash(const ky_byte &cd)
{
    crypto_md5 md5;
    md5.update(cd);
    return md5.final();
}
