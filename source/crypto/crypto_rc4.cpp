#include "crypto_rc4.h"
#include "ky_algorlthm.h"

#define rc4BlockByte 256
struct rc4_priv
{
    int32 m[rc4BlockByte];
};

crypto_rc4::crypto_rc4()
{
    priv = kyNew(rc4_priv);
}
crypto_rc4::~crypto_rc4()
{
    kyDelete(priv);
}

void crypto_rc4::set(const key &pp)
{
    int32 *m = priv->m;

    uint8 k[rc4BlockByte] = {0};
    const int32 len = pp.size();
    for (int32 i = 0; i < rc4BlockByte; i++)
    {
        m[i] = i;
        k[i] = pp[i % len];
    }

    int32 j = 0;
    for (int32 i = 0; i < rc4BlockByte; i++)
    {
        j = (j + m[i] + k[i]) % rc4BlockByte;
        ky_swap(m[i], m[j]);
    }
}

void crypto_rc4::encrypt(uint8 *block)
{
    int32 i = 0, j = 0, t = 0;
    int32 *m = priv->m;
    for(int32 k = 0; k < 16; k++)
    {
        i = (i + 1) % rc4BlockByte;
        j = (j + m[i]) % rc4BlockByte;
        ky_swap(m[i], m[j]);

        t = (m[i] + m[j]) % rc4BlockByte;
        block[k] ^= m[t];
    }
}

void crypto_rc4::decrypt(uint8 *block)
{
    int32 i = 0, j = 0, t = 0;
    int32 *m = priv->m;
    for(int32 k = 0; k < 16; k++)
    {
        i = (i + 1) % rc4BlockByte;
        j = (j + m[i]) % rc4BlockByte;
        ky_swap(m[i], m[j]);

        t = (m[i] + m[j]) % rc4BlockByte;
        block[k] ^= m[t];
    }
}

ky_byte crypto_rc4::encrypt(const ky_byte &cleartext)
{
    const int32 size = cleartext.bytecount();
    uint8 *in = cleartext.data();
    ky_byte ciphertext;
    ciphertext.resize(size);
    uint8 *out = ciphertext.data();

    int32 i = 0, j = 0, t = 0;
    int32 *m = priv->m;
    for(int32 k = 0; k < size; k++)
    {
        i = (i + 1) % rc4BlockByte;
        j = (j + m[i]) % rc4BlockByte;
        ky_swap(m[i], m[j]);

        t = (m[i] + m[j]) % rc4BlockByte;
        out[k] = in[k] ^ m[t];
    }
    return ciphertext;
}
ky_byte crypto_rc4::decrypt(const ky_byte &ciphertext)
{
    const int32 size = ciphertext.bytecount();
    uint8 *in = ciphertext.data();
    ky_byte cleartext;
    cleartext.resize(size);
    uint8 *out = cleartext.data();

    int32 i = 0, j = 0, t = 0;
    int32 *m = priv->m;
    for(int32 k = 0; k < size; k++)
    {
        i = (i + 1) % rc4BlockByte;
        j = (j + m[i]) % rc4BlockByte;
        ky_swap(m[i], m[j]);

        t = (m[i] + m[j]) % rc4BlockByte;
        out[k] = in[k] ^ m[t];
    }
    return cleartext;
}
