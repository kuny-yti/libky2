#include "crypto_sha.h"

// 循环移位
#define RLS(a,b,c)    (((a) << (b)) | ((a) >> ((c)-(b))))
#define RRS(a,b,c)    (((a) >> (b)) | ((a) << ((c)-(b))))
// 循环左移
#define RLS32(a,b)    RLS(a,b,32)
#define RLS64(a,b)    RLS(a,b,64)
// 循环右移
#define RRS32(a,b)    RRS(a,b,32)
#define RRS64( a, b ) RRS(a,b,64)

#define RS32(x, n)    (((x) & 0xFFFFFFFF) >> ((uint32_t)n))
#define RS64(x, n)    (((x) & 0xFFFFFFFFFFFFFFFFULL) >> ((uint64)n))


// #define Ch(x, y, z)  (z ^ (x & (y ^ z)))
// #define Maj(x, y, z) (((x | y) & z) | (x & y))
#define CH(x, y, z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x, y, z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))

#define EP0(x) (RRS32(x,2) ^ RRS32(x,13) ^ RRS32(x,22))
#define EP1(x) (RRS32(x,6) ^ RRS32(x,11) ^ RRS32(x,25))

#define Sigma032(x) (RRS32(x,7) ^ RRS32(x,18) ^ ((x) >> 3))
#define Sigma132(x) (RRS32(x,17) ^ RRS32(x,19) ^ ((x) >> 10))

#define Sigma064(x)       (RRS64(x, 28) ^ RRS64(x, 34) ^ RRS64(x, 39))
#define Sigma164(x)       (RRS64(x, 14) ^ RRS64(x, 18) ^ RRS64(x, 41))
#define Gamma064(x)       (RRS64(x, 1 ) ^ RRS64(x, 8 ) ^ RS64(x, 7))
#define Gamma164(x)       (RRS64(x, 19) ^ RRS64(x, 61) ^ RS64(x, 6))

#define Sha512Round( a, b, c, d, e, f, g, h, i )       \
     t0 = h + Sigma164(e) + CH(e, f, g) + (K)[i] + W[i]; \
     t1 = Sigma064(a) + MAJ(a, b, c);                  \
     d += t0;                                          \
     h  = t0 + t1;

#define ShaEnc32(Dst, Src)                                                      \
{                                                                               \
    (Dst) =                                                                     \
    (((uint64)((Src)[0] & 0xff))<<24) | (((uint64)((Src)[1] & 0xff))<<16) | \
    (((uint64)((Src)[2] & 0xff))<<8 ) | (((uint64)((Src)[3] & 0xff)));      \
}

#define ShaDec32(Dst, Src)                    \
{                                             \
    (Dst)[0] = (uint8)(((Src)>>24) & 0xff); \
    (Dst)[1] = (uint8)(((Src)>>16) & 0xff); \
    (Dst)[2] = (uint8)(((Src)>>8 ) & 0xff); \
    (Dst)[3] = (uint8)( (Src)      & 0xff); \
}
#define ShaEnc64(Dst, Src)                                                      \
{                                                                               \
    (Dst) =                                                                     \
    (((uint64)((Src)[0] & 0xff))<<56) | (((uint64)((Src)[1] & 0xff))<<48) | \
    (((uint64)((Src)[2] & 0xff))<<40) | (((uint64)((Src)[3] & 0xff))<<32) | \
    (((uint64)((Src)[4] & 0xff))<<24) | (((uint64)((Src)[5] & 0xff))<<16) | \
    (((uint64)((Src)[6] & 0xff))<<8 ) | (((uint64)((Src)[7] & 0xff)));      \
}

#define ShaDec64(Dst, Src)                    \
{                                             \
    (Dst)[0] = (uint8)(((Src)>>56) & 0xff); \
    (Dst)[1] = (uint8)(((Src)>>48) & 0xff); \
    (Dst)[2] = (uint8)(((Src)>>40) & 0xff); \
    (Dst)[3] = (uint8)(((Src)>>32) & 0xff); \
    (Dst)[4] = (uint8)(((Src)>>24) & 0xff); \
    (Dst)[5] = (uint8)(((Src)>>16) & 0xff); \
    (Dst)[6] = (uint8)(((Src)>>8 ) & 0xff); \
    (Dst)[7] = (uint8)( (Src)      & 0xff); \
}


# define SHA1_LENGTH      20
# define SHA224_LENGTH    28
# define SHA256_LENGTH    32
# define SHA384_LENGTH    48
# define SHA512_LENGTH    64

static const uint32_t sha_1k[4] =
{
    0x5A827999, 0x6ED9EBA1, 0x8F1BBCDC, 0xCA62C1D6
};
static const uint32_t sha_1s[5] =
{
    0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0
};

static const uint32_t sha_256k[64] =
{
    0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,
    0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
    0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,
    0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
    0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,
    0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
    0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,
    0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
    0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,
    0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
    0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,
    0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
    0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,
    0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
    0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,
    0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};
static const uint32_t sha_256s[8] =
{
    0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
    0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
};
static const uint32_t sha_224s[8] =
{
    0xc1059ed8, 0x367cd507, 0x3070dd17, 0xf70e5939,
    0xffc00b31, 0x68581511, 0x64f98fa7, 0xbefa4fa4
};

static const uint64 sha_512k[80] =
{
    (0x428a2f98d728ae22), (0x7137449123ef65cd),
    (0xb5c0fbcfec4d3b2f), (0xe9b5dba58189dbbc),
    (0x3956c25bf348b538), (0x59f111f1b605d019),
    (0x923f82a4af194f9b), (0xab1c5ed5da6d8118),
    (0xd807aa98a3030242), (0x12835b0145706fbe),
    (0x243185be4ee4b28c), (0x550c7dc3d5ffb4e2),
    (0x72be5d74f27b896f), (0x80deb1fe3b1696b1),
    (0x9bdc06a725c71235), (0xc19bf174cf692694),
    (0xe49b69c19ef14ad2), (0xefbe4786384f25e3),
    (0x0fc19dc68b8cd5b5), (0x240ca1cc77ac9c65),
    (0x2de92c6f592b0275), (0x4a7484aa6ea6e483),
    (0x5cb0a9dcbd41fbd4), (0x76f988da831153b5),
    (0x983e5152ee66dfab), (0xa831c66d2db43210),
    (0xb00327c898fb213f), (0xbf597fc7beef0ee4),
    (0xc6e00bf33da88fc2), (0xd5a79147930aa725),
    (0x06ca6351e003826f), (0x142929670a0e6e70),
    (0x27b70a8546d22ffc), (0x2e1b21385c26c926),
    (0x4d2c6dfc5ac42aed), (0x53380d139d95b3df),
    (0x650a73548baf63de), (0x766a0abb3c77b2a8),
    (0x81c2c92e47edaee6), (0x92722c851482353b),
    (0xa2bfe8a14cf10364), (0xa81a664bbc423001),
    (0xc24b8b70d0f89791), (0xc76c51a30654be30),
    (0xd192e819d6ef5218), (0xd69906245565a910),
    (0xf40e35855771202a), (0x106aa07032bbd1b8),
    (0x19a4c116b8d2d0c8), (0x1e376c085141ab53),
    (0x2748774cdf8eeb99), (0x34b0bcb5e19b48a8),
    (0x391c0cb3c5c95a63), (0x4ed8aa4ae3418acb),
    (0x5b9cca4f7763e373), (0x682e6ff3d6b2b8a3),
    (0x748f82ee5defb2fc), (0x78a5636f43172f60),
    (0x84c87814a1f0ab72), (0x8cc702081a6439ec),
    (0x90befffa23631e28), (0xa4506cebde82bde9),
    (0xbef9a3f7b2c67915), (0xc67178f2e372532b),
    (0xca273eceea26619c), (0xd186b8c721c0c207),
    (0xeada7dd6cde0eb1e), (0xf57d4f7fee6ed178),
    (0x06f067aa72176fba), (0x0a637dc5a2c898a6),
    (0x113f9804bef90dae), (0x1b710b35131c471b),
    (0x28db77f523047d84), (0x32caab7b40c72493),
    (0x3c9ebe0a15c9bebc), (0x431d67c49c100d4c),
    (0x4cc5d4becb3e42b6), (0x597f299cfc657e2a),
    (0x5fcb6fab3ad6faec), (0x6c44198c4a475817)
};
static const uint64 sha_512s224[8] =
{
    (0x8c3d37c819544da2), (0x73e1996689dcd4d6),
    (0x1dfab7ae32ff9c82), (0x679dd514582f9fcf),
    (0x0f6d2b697bd44da8), (0x77e36f7304c48942),
    (0x3f9d85a86a1d36c8), (0x1112e6ad91d692a1)
};
static const uint64 sha_512s256[8] =
{
    (0x22312194fc2bf72c), (0x9f555fa3c84c64c2),
    (0x2393b86b6f53b151), (0x963877195940eabd),
    (0x96283ee2a88effe3), (0xbe5e1e2553863992),
    (0x2b0199fc2c85b8aa), (0x0eb72ddc81c52ca2)
};
static const uint64 sha_384s[8] =
{
    (0xcbbb9d5dc1059ed8), (0x629a292a367cd507),
    (0x9159015a3070dd17), (0x152fecd8f70e5939),
    (0x67332667ffc00b31), (0x8eb44a8768581511),
    (0xdb0c2e0d64f98fa7), (0x47b5481dbefa4fa4)
};
static const uint64 sha_512s[8] =
{
    (0x6a09e667f3bcc908), (0xbb67ae8584caa73b),
    (0x3c6ef372fe94f82b), (0xa54ff53a5f1d36f1),
    (0x510e527fade682d1), (0x9b05688c2b3e6c1f),
    (0x1f83d9abfb41bd6b), (0x5be0cd19137e2179)
};

struct sha_priv
{
    eCryptoSHA  cs;
    union {
        struct
        {
            uint32_t    digest;
            uint32_t    length;
            uint64    bitlen;
            uint32_t   *k;
            uint8    *data;
            uint32_t   *state;
        };
        struct
        {
            uint32_t    digest;
            uint32_t    length;
            uint64    bitlen;
            uint32_t   *k;
            uint8    *data;
            uint32_t   *state;

#define SHA_BLOCK_BYTE 64
#define SHA_STATE 5
            uint8   data_memory[SHA_BLOCK_BYTE];
            uint32_t  state_memory[SHA_STATE];
#undef SHA_BLOCK
#undef SHA_STATE

            void transform(uint8 *data)
            {
                uint32_t a, b, c, d, e, i, j, t, m[80];

                for (i = 0, j = 0; i < 16; ++i, j += 4)
                    m[i] = (data[j] << 24) + (data[j + 1] << 16) +
                            (data[j + 2] << 8) + (data[j + 3]);
                for ( ; i < 80; ++i)
                {
                    m[i] = (m[i - 3] ^ m[i - 8] ^ m[i - 14] ^ m[i - 16]);
                    m[i] = (m[i] << 1) | (m[i] >> 31);
                }

                a = state[0];
                b = state[1];
                c = state[2];
                d = state[3];
                e = state[4];

                for (i = 0; i < 20; ++i)
                {
                    t = RLS32(a, 5) + ((b & c) ^ (~b & d)) + e + k[0] + m[i];
                    e = d;
                    d = c;
                    c = RLS32(b, 30);
                    b = a;
                    a = t;
                }
                for ( ; i < 40; ++i)
                {
                    t = RLS32(a, 5) + (b ^ c ^ d) + e + k[1] + m[i];
                    e = d;
                    d = c;
                    c = RLS32(b, 30);
                    b = a;
                    a = t;
                }
                for ( ; i < 60; ++i)
                {
                    t = RLS32(a, 5) + ((b & c) ^ (b & d) ^ (c & d)) + e +
                            k[2] + m[i];
                    e = d;
                    d = c;
                    c = RLS32(b, 30);
                    b = a;
                    a = t;
                }
                for ( ; i < 80; ++i)
                {
                    t = RLS32(a, 5) + (b ^ c ^ d) + e + k[3] + m[i];
                    e = d;
                    d = c;
                    c = RLS32(b, 30);
                    b = a;
                    a = t;
                }

                state[0] += a;
                state[1] += b;
                state[2] += c;
                state[3] += d;
                state[4] += e;
            }

        }sha1;
        struct
        {
            uint32_t    digest;
            uint32_t    length;
            uint64    bitlen;
            uint32_t   *k;
            uint8    *data;
            uint32_t   *state;

#define SHA_BLOCK_BYTE 64
#define SHA_STATE 8
            uint8  data_memory[SHA_BLOCK_BYTE];
            uint32_t state_memory[SHA_STATE];
#undef SHA_STATE
#undef SHA_BLOCK_BYTE

            void transform(uint8 *data)
            {
                uint32_t a, b, c, d, e, f, g, h, i, j, t1, t2, m[64];

                for (i = 0, j = 0; i < 16; ++i, j += 4)
                    m[i] = (data[j] << 24) | (data[j + 1] << 16) |
                            (data[j + 2] << 8) | (data[j + 3]);
                for ( ; i < 64; ++i)
                    m[i] =  Sigma132(m[i - 2]) + m[i - 7] +
                            Sigma032(m[i - 15]) + m[i - 16];

                a = state[0];
                b = state[1];
                c = state[2];
                d = state[3];
                e = state[4];
                f = state[5];
                g = state[6];
                h = state[7];

                for (i = 0; i < 64; ++i)
                {
                    t1 = h + EP1(e) + CH(e,f,g) + k[i] + m[i];
                    t2 = EP0(a) + MAJ(a,b,c);
                    h = g;
                    g = f;
                    f = e;
                    e = d + t1;
                    d = c;
                    c = b;
                    b = a;
                    a = t1 + t2;
                }

                state[0] += a;
                state[1] += b;
                state[2] += c;
                state[3] += d;
                state[4] += e;
                state[5] += f;
                state[6] += g;
                state[7] += h;
            }

        }sha256;
        struct
        {
            uint32_t    digest;
            uint32_t    length;
            uint64    bitlen;
            uint32_t   *k;
            uint8    *data;
            uint32_t   *state;

#define SHA_BLOCK_BYTE 128
#define SHA_STATE 16
            uint8  data_memory[SHA_BLOCK_BYTE];
            uint32_t state_memory[SHA_STATE];
#undef SHA_STATE
#undef SHA_BLOCK_BYTE

            void transform(uint8 *data)
            {
                uint64    S[8];
                uint64    W[80];
                uint64    t0, t1;

                uint64 *K = (uint64*)k;
                uint64 *s_p = (uint64*)(state);
                for (int i = 0; i < 8; ++i)
                    S[i] = s_p[i];

                for (int i = 0; i < 16; ++i)
                    ShaEnc64(W[i], data + (8*i));

                for (int i = 16; i < 80; ++i)
                {
                    W[i] = Gamma164(W[i - 2 ]) + W[i - 7 ] +
                            Gamma064(W[i - 15]) + W[i - 16];
                }

                for (int i=0; i<80; i+=8 )
                {
                    Sha512Round(S[0],S[1],S[2],S[3],S[4],S[5],S[6],S[7],i+0);
                    Sha512Round(S[7],S[0],S[1],S[2],S[3],S[4],S[5],S[6],i+1);
                    Sha512Round(S[6],S[7],S[0],S[1],S[2],S[3],S[4],S[5],i+2);
                    Sha512Round(S[5],S[6],S[7],S[0],S[1],S[2],S[3],S[4],i+3);
                    Sha512Round(S[4],S[5],S[6],S[7],S[0],S[1],S[2],S[3],i+4);
                    Sha512Round(S[3],S[4],S[5],S[6],S[7],S[0],S[1],S[2],i+5);
                    Sha512Round(S[2],S[3],S[4],S[5],S[6],S[7],S[0],S[1],i+6);
                    Sha512Round(S[1],S[2],S[3],S[4],S[5],S[6],S[7],S[0],i+7);
                }
                for (int i = 0; i < 8; ++i)
                    s_p[i] = s_p[i] + S[i];
            }

        }sha512;
    };
};

crypto_sha::crypto_sha(eCryptoSHA cs)
{
    priv = kyNew(sha_priv) ;
    set(cs);
}
crypto_sha::~crypto_sha()
{
    kyDelete(priv);
}

void crypto_sha::set(eCryptoSHA cs)
{
    priv->cs = cs;
    priv->length   = 0;
    priv->bitlen   = 0;
    priv->data = 0;
    priv->state = 0;

    if (cs == SHA_1)
    {
        priv->digest = SHA1_LENGTH;
        priv->state = priv->sha1.state_memory;
        priv->data = priv->sha1.data_memory;
        priv->k     = (uint32_t*)sha_1k;
        ky_memory::copy(priv->state, sha_1s, sizeof(sha_1s));
    }
    else if (cs == SHA_224)
    {
        priv->digest = SHA224_LENGTH;
        priv->state = priv->sha256.state_memory;
        priv->data = priv->sha256.data_memory;
        priv->k     = (uint32_t*)sha_256k;
        ky_memory::copy(priv->state, sha_224s, sizeof(sha_224s));
    }
    else if (cs == SHA_256)
    {
        priv->digest = SHA256_LENGTH;
        priv->state = priv->sha256.state_memory;
        priv->data = priv->sha256.data_memory;
        priv->k     = (uint32_t*)sha_256k;
        ky_memory::copy(priv->state, sha_256s, sizeof(sha_256s));
    }
    else if (cs == SHA_384)
    {
        priv->digest = SHA384_LENGTH;
        priv->state = priv->sha512.state_memory;
        priv->data = priv->sha512.data_memory;
        priv->k     = (uint32_t*)sha_512k;
        ky_memory::copy(priv->state, sha_384s, sizeof(sha_384s));
    }
    else if (cs == SHA_512)
    {
        priv->digest = SHA512_LENGTH;
        priv->state = priv->sha512.state_memory;
        priv->data = priv->sha512.data_memory;
        priv->k     = (uint32_t*)sha_512k;
        ky_memory::copy(priv->state, sha_512s, sizeof(sha_512s));
    }
}

void crypto_sha::update(const ky_byte &cd)
{
    const uint8 *in = cd.data();
    const int64  size = cd.size();

    for (int64 i = 0; i < size; ++i)
    {
        priv->data[priv->length] = in[i];
        priv->length++;

        if ((priv->cs <= SHA_256) && (priv->length == 64))
        {
            if (priv->cs == SHA_1)
                priv->sha1.transform(priv->data);
            else if (priv->cs <= SHA_256)
                priv->sha256.transform(priv->data);
            priv->bitlen += 512;
            priv->length = 0;
        }
        else if ((priv->cs <= SHA_512) && (priv->length == 128))
        {
            priv->sha512.transform(priv->data);
            priv->bitlen += 1024;
            priv->length = 0;
        }
    }
}

ky_byte crypto_sha::final()
{
    ky_byte hash(priv->digest);
    uint32_t i = priv->length;

    uint32_t dbase = 0;

    if (priv->cs <= SHA_256)
    {
        if (i < 56)
        {
            priv->data[i++] = 0x80;
            while (i < 56)
                priv->data[i++] = 0x00;
        }
        else
        {
            priv->data[i++] = 0x80;
            while (i < 64)
                priv->data[i++] = 0x00;

            if (priv->cs == SHA_1)
                priv->sha1.transform(priv->data);
            else if (priv->cs <= SHA_256)
                priv->sha256.transform(priv->data);
            memset(priv->data, 0, 64);
        }
        dbase = 56;
    }
    else if (priv->cs <= SHA_512)
    {
        if (i < 120)
        {
            priv->data[i++] = 0x80;
            while (i < 120)
                priv->data[i++] = 0x00;
        }
        else
        {
            priv->data[i++] = 0x80;
            while (i < 128)
                priv->data[i++] = 0x00;
            priv->sha512.transform(priv->data);
            memset(priv->data, 0, 128);
        }
        dbase = 120;
    }

    priv->bitlen       += priv->length * 8;
    ShaDec64(priv->data + dbase, priv->bitlen);

    if (priv->cs == SHA_1)
        priv->sha1.transform(priv->data);
    else if (priv->cs <= SHA_256)
        priv->sha256.transform(priv->data);
    else if (priv->cs <= SHA_512)
        priv->sha512.transform(priv->data);

    if (priv->cs <= SHA_256)
    {
        for (i = 0; i < 4; ++i)
        {
            hash.data()[i]      = (priv->state[0] >> (24 - i * 8)) & 0xff;
            hash.data()[i + 4]  = (priv->state[1] >> (24 - i * 8)) & 0xff;
            hash.data()[i + 8]  = (priv->state[2] >> (24 - i * 8)) & 0xff;
            hash.data()[i + 12] = (priv->state[3] >> (24 - i * 8)) & 0xff;
            hash.data()[i + 16] = (priv->state[4] >> (24 - i * 8)) & 0xff;
            if ((int)priv->cs > SHA_1)
            {
                hash.data()[i + 20] = (priv->state[5] >> (24 - i * 8)) & 0xff;
                hash.data()[i + 24] = (priv->state[6] >> (24 - i * 8)) & 0xff;
                hash.data()[i + 28] = (priv->state[7] >> (24 - i * 8)) & 0xff;
            }
        }
    }
    else if (priv->cs <= SHA_512)
    {
        for( i = 0; i < sizeof(uint64); i++)
        {
            uint8 *byte = hash.data() + (sizeof(uint64) * i);
            ShaDec64(byte, ((uint64*)priv->state)[i]);
        }
    }

    return hash;
}

ky_byte crypto_sha::hash(const ky_byte &cd)
{
    set(priv->cs);
    update(cd);
    return final();
}
ky_byte crypto_sha::hash(const ky_byte &cd, eCryptoSHA cs)
{
    static crypto_sha sha(cs);
    sha.set(cs);
    sha.update(cd);
    return sha.final();
}
