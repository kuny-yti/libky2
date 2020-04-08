#include "crypto_rc6.h"
#include "ky_algorlthm.h"
#include "ky_maths.h"

struct rc6_priv
{
    uint32    w;     // w:     word size in bits (default: 32 bits / 4 bytes)
    uint32    r;     // r:     nonnegative number of rounds
    uint32    b;     // b:     length of the encryption key in bytes
    uint32    t;     // t:
    uint32    log_w; // log_w: base-two logarithm of w
    int64     modulo;// modulo:2 ^ w
    uint32   *S;     // s:     stores the number of w-bits words
                     //        that will be generated for the
                     //        additive round keys is 2r+4
    uint32   *L;

    crypto_rc6::key key; // key:   user supplied key

    uint32 left_rot(uint32 a, uint32 b, uint32 w)
    {
        b <<= w - log_w;
        b >>= w - log_w;
        return (a << b) | (a >> (w - b));
    }
    uint32 right_rot(uint32 a, uint32 b, uint32 w)
    {
        b <<= w - log_w;
        b >>= w - log_w;
        return (a >> b) | (a << (w - b));
    }

    static void rc_constraints(const uint32 &w, uint32 &p, uint32 &q)
    {
        p = (uint32)ky_iceil(((M_E - 2) * ky_pow(2, w)));
        q = (uint32)((1.618033988749895 - 1) * ky_pow(2, w));    // Golden Ratio
    }
};

crypto_rc6::crypto_rc6()
{
    priv = kyNew(rc6_priv);
    priv->S = 0;
    priv->L = 0;
}
crypto_rc6::~crypto_rc6()
{
    if (priv->S)
        kyFree(priv->S);
    if (priv->L)
        kyFree(priv->L);
    kyDelete(priv);
}

void crypto_rc6::set(const key &pp, int8 word, int8 round)
{
    priv->w = word;
    priv->r = round;
    priv->b = pp.size();
    priv->log_w = (uint32)ky_log2(word);
    priv->modulo = ky_pow(2, word);

    if (priv->S)
        kyFree(priv->S);
    priv->S = (uint32*)kyMalloc((2 * round + 4) * sizeof(uint32));

    const uint32 w_bytes = ky_iceil((float)priv->w / 8);
    const uint32 c = ky_iceil((float)priv->b / w_bytes);

    uint32 p = 0xb7e15163, q = 0x9e3779b9;
    //priv->rc_constraints(priv->w, p, q);

    if (priv->L)
        kyFree(priv->L);
    priv->L = (uint32*)kyMalloc(c * sizeof(uint32));
    uint32 *_key = pp.to<uint32>();
    for(uint32 i = 0; i < c; i++)
        priv->L[i] = _key[i];

    priv->S[0] = p;
    for(int32 i = 1; i <= (2 * round + 3); ++i)
    {
        priv->S[i] = (priv->S[i - 1] + q) % priv->modulo;
    }

    uint32 A = 0, B = 0, i = 0, j = 0;
    int32 v = 3 * ky_max(c, uint32(2 * round + 4));
    for(int32 s = 1; s <= v; s++)
    {
        A = priv->S[i] = priv->left_rot((priv->S[i] + A + B) % priv->modulo,
                                        3, priv->w);
        B = priv->L[j] = priv->left_rot((priv->L[j] + A + B) % priv->modulo,
                                        (A + B), priv->w);
        i = (i + 1) % (2 * round + 4);
        j = (j + 1) % c;
    }
}

void crypto_rc6::encrypt(uint8 *block)
{
    uint32 *in = (uint32*)block;
    uint32 A, B, C, D;
    A = *in++;
    B = *in++;
    C = *in++;
    D = *in++;

    int32 t, u, temp;

    B += priv->S[0];
    D += priv->S[1];
    for(int32 i = 1; i <= priv->r; ++i)
    {
        t = priv->left_rot((B * (2 * B + 1)) % priv->modulo, priv->log_w, priv->w);
        u = priv->left_rot((D * (2 * D + 1)) % priv->modulo, priv->log_w, priv->w);
        A = priv->left_rot((A ^ t), u , priv->w) + priv->S[2 * i];
        C = priv->left_rot((C ^ u), t, priv->w) + priv->S[2 * i + 1];
        temp = A;
        A = B;
        B = C;
        C = D;
        D = temp;
    }

    A += priv->S[2 * priv->r + 2];
    C += priv->S[2 * priv->r + 3];

    uint32 *out = (uint32*)block;
    *out++ = A;
    *out++ = B;
    *out++ = C;
    *out++ = D;
}

void crypto_rc6::decrypt(uint8 *block)
{
    uint32 *in = (uint32*)block;
    uint32 A, B, C, D;
    A = *in++;
    B = *in++;
    C = *in++;
    D = *in++;

    uint32 t, u, temp;

    C -= priv->S[2 * priv->r + 3];
    A -= priv->S[2 * priv->r + 2];
    for(int32 i = priv->r; i >= 1; --i)
    {
        temp = D;
        D = C;
        C = B;
        B = A;
        A = temp;
        u = priv->left_rot((D * (2 * D + 1)) % priv->modulo, priv->log_w, priv->w);
        t = priv->left_rot((B * (2 * B + 1)) % priv->modulo, priv->log_w, priv->w);
        C = priv->right_rot((C - priv->S[2 * i + 1]) % priv->modulo, t, priv->w) ^ u;
        A = priv->right_rot((A - priv->S[2 * i]) % priv->modulo, u, priv->w) ^ t;
    }
    D -= priv->S[1];
    B -= priv->S[0];

    uint32 *out = (uint32*)block;
    *out++ = A;
    *out++ = B;
    *out++ = C;
    *out++ = D;
}

