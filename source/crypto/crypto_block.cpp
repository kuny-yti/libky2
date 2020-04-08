#include "crypto_block.h"

// 多余出字节数
#define kyBlockExtra ((uint8)sizeof(uint8))

static void iv_increase(uint8 *iv, uint32 inx)
{
    ++iv[inx % 16];
}

//!
//! \brief Electronic codebook encrypt
//!             cleartext
//!                 |
//! key -- block cipher encrypt  ... ...
//!                 |
//!             ciphertext
ky_byte crypto_block::encrypt_ecb(const ky_byte &cleartext)
{
    if (!priv || !byte)
        return ky_byte();

    // 明文信息
    uint8 *in = (uint8 *)cleartext.data();
    int32 oldsize = cleartext.bytecount() + kyBlockExtra;

    // 计算出块数和余数
    const int32 blocks = oldsize / byte;
    const uint8 extra = oldsize % byte;
    const int32 newsize = blocks *byte + (extra ?byte :0);

    // 申请密文内存
    ky_byte ciphertext;
    ciphertext.resize(newsize);
    uint8 *out = ciphertext.data();

    uint8 buf[byte];
    buf[0] = extra;
    uint8 x = 1;
    for (int32 z = 0; z < blocks; ++z)
    {
        for (; x < byte; ++x, ++in)
            buf[x] = *in;

        priv->encrypt(buf);
        ky_memory::copy(out, buf, sizeof(buf));
        out += byte;
        x = 0;
    }

    // 多余加入
    if (extra)
    {
        for (; x < extra; ++x, ++in)
            buf[x] = *in;
        priv->encrypt(buf);
        ky_memory::copy(out, buf, sizeof(buf));
    }

    return ciphertext;
}
//!
//! \brief Electronic codebook decrypt
//!             ciphertext
//!                 |
//! key -- block cipher decrypt  ... ...
//!                 |
//!             cleartext
ky_byte crypto_block::decrypt_ecb(const ky_byte &ciphertext)
{
    uint8 *in = (uint8*)ciphertext.data();
    const uint32 size = ciphertext.bytecount();

    if (!byte || (size % byte) || (!priv))
        return ky_byte();

    // 分配明文内存
    ky_byte cleartext;
    cleartext.resize(size);
    uint8 *out = cleartext.data();

    uint8 buf[byte];
    for (uint32 x = 0; x < size; x += byte, in += byte, out += byte)
    {
        ky_memory::copy(buf, in, sizeof(buf));
        priv->decrypt(buf);
        ky_memory::copy(out, buf, sizeof(buf));
    }

    // 还原明文
    const uint8 extra = *cleartext.data();
    if (extra > 0xF)
        return ky_byte();

    int32 old = size - kyBlockExtra;
    if (extra)
        old -= (byte - extra);
    ky_byte ct(cleartext.data()+kyBlockExtra, old);

    return ct;
}

//!
//! \brief Cipher-block chaining encrypt
//!                cleartext                  cleartext
//!                    |                          |
//!    IV(iv) ------- xor              |-------- xor
//!                    |               |          |
//!  key ----- block cipher encrypt    |  block cipher encrypt  ... ...
//!                    |---------------|          |-------------
//!                    |                          |
//!               ciphertext                  ciphertext
ky_byte crypto_block::encrypt_cbc(const ky_byte &cleartext, const uint8* iv)
{
    if (!priv || !byte)
        return ky_byte();

    // 明文信息
    uint8 *in = (uint8 *)cleartext.data();
    int32 oldsize = cleartext.bytecount() + kyBlockExtra;

    // 计算出块数和余数
    const int32 blocks = oldsize / byte;
    const uint8 extra = oldsize % byte;
    const int32 newsize = blocks *byte + (extra ?byte :0);

    // 申请密文内存
    ky_byte ciphertext;
    ciphertext.resize(newsize);
    uint8 *out = ciphertext.data();

    uint8 buf[byte];
    uint8 iv_buf[byte];
    ky_memory::copy(iv_buf, iv, sizeof(iv_buf));
    buf[0] = extra ^ iv_buf[0];
    uint8 x = 1;

    // 块数据加入
    for (int32 b = 0; b < blocks; ++b)
    {
        for (; x < byte; ++x, ++in)
            buf[x] = *in ^ iv_buf[x]; // xor

        priv->encrypt(buf);
        for (x = 0; x < byte; ++x, ++out)
            *out = iv_buf[x] = buf[x];
        x = 0;
    }

    // 多余加入
    if (extra)
    {
        for (; x < extra; ++x, ++in)
            buf[x] = *in ^ iv_buf[x];
        priv->encrypt(buf);
        ky_memory::copy(out, buf, sizeof(iv_buf));
    }

    return ciphertext;
}
//!
//! \brief Cipher-block chaining decrypt
//!                ciphertext                  ciphertext
//!                     |------------|             |------------
//!                     |            |             |
//!  key ---- block cipher decrypt   |    block cipher decrypt  ... ...
//!                     |            |             |
//!    IV(iv) -------- xor           |----------- xor
//!                     |                          |
//!                 cleartext                  cleartext
ky_byte crypto_block::decrypt_cbc(const ky_byte &ciphertext, const uint8* iv)
{
    uint8 *in = (uint8*)ciphertext.data();
    const uint32 size = ciphertext.bytecount();

    if (!byte || (size % byte) || (!priv))
        return ky_byte();

    // 分配明文内存
    ky_byte cleartext;
    cleartext.resize(size);
    uint8 *out = cleartext.data();

    uint8 buf[byte];
    uint8 iv_buf[byte], tv_buf[byte];
    ky_memory::copy(iv_buf, iv, sizeof(iv_buf));

    for (uint32 b = 0; b < size; b += byte)
    {
        for (uint8 x = 0; x < byte; ++x, ++in)
            tv_buf[x] = buf[x] = *in;
        priv->decrypt(buf);
        for (uint8 x = 0; x < byte; ++x, ++out)
        {
            *out = buf[x] ^ iv_buf[x];
            iv_buf[x] = tv_buf[x];
        }
    }

    // 还原明文
    const uint8 extra = *cleartext.data();
    if (extra > 0xF)
        return ky_byte();

    int32 old = size - kyBlockExtra;
    if (extra)
        old -= (byte - extra);
    ky_byte ct(cleartext.data()+kyBlockExtra, old);

    return ct;
}

//!
//! \brief Cipher feedback encrypt
//!                IV(iv)         |-----------------|
//!                  |            |                 |
//!  key --- block cipher encrypt | key --- block cipher encrypt  ... ...
//!                  |            |                 |
//!  cleartext ---- xor           | cleartext ---- xor
//!                  |------------|                 |-------------
//!                  |                              |
//!             ciphertext                     ciphertext
ky_byte crypto_block::encrypt_cfb(const ky_byte &cleartext, const uint8* iv)
{
    if (!priv || !byte)
        return ky_byte();

    // 明文信息
    uint8 *in = (uint8 *)cleartext.data();
    int32 oldsize = cleartext.bytecount() + kyBlockExtra;

    // 计算出块数和余数
    const int32 blocks = oldsize / byte;
    const uint8 extra = oldsize % byte;
    const int32 newsize = blocks *byte + (extra ?byte :0) ;

    // 申请密文内存
    ky_byte ciphertext;
    ciphertext.resize(newsize);
    uint8 *out = ciphertext.data();

    uint8 buf[byte];
    ky_memory::copy(buf, iv, sizeof(buf));
    uint8 x = 1;
    priv->encrypt(buf);
    *out++ = buf[0] = buf[0] ^ extra;
    // 块数据加入
    for (int32 b = 0; b < blocks; ++b)
    {
        for (; x < byte; ++x, ++out, ++in)
            *out = buf[x] = buf[x] ^ *in;
        priv->encrypt(buf);
        x = 0;
    }

    if (extra)
    {
        // 多余加入
        for (; x < extra; ++x, ++in, ++out)
            *out = buf[x] = buf[x] ^ *in;
    }

    return ciphertext;
}
//!
//! \brief Cipher feedback decrypt
//!                IV(iv)         |-----------------|             |----
//!                  |            |                 |             |
//!  key --- block cipher encrypt | key --- block cipher encrypt  |     ... ...
//!                  |            |                 |             |
//!                 xor ----- ciphertext           xor ----- ciphertext
//!                  |                              |
//!              cleartext                      cleartext
ky_byte crypto_block::decrypt_cfb(const ky_byte &ciphertext, const uint8* iv)
{
    uint8 *in = (uint8*)ciphertext.data();
    const uint32 size = ciphertext.bytecount();

    if (!byte || (size % byte) || (!priv))
        return ky_byte();

    // 分配明文内存
    ky_byte cleartext;
    cleartext.resize(size);
    uint8 *out = cleartext.data();

    uint8 buf[byte];
    ky_memory::copy(buf, iv, sizeof(buf));

    for (uint32 b = 0; b < size; b += byte)
    {
        priv->encrypt(buf);
        for (uint8 x = 0; x < byte; ++x, ++out, ++in)
        {
            *out = buf[x] ^ *in;
            buf[x] = *in;
        }
    }

    // 还原明文
    const uint8 extra = *cleartext.data();
    if (extra > 0xF)
        return ky_byte();

    int32 old = size - kyBlockExtra;
    if (extra)
        old -= (byte - extra);
    ky_byte ct(cleartext.data()+kyBlockExtra, old);

    return ct;
}

//!
//! \brief Output feedback encrypt
//!                IV(iv)         |-----------------|
//!                  |            |                 |
//!  key --- block cipher encrypt | key --- block cipher encrypt  ... ...
//!                  |------------|                 |-------------
//!  cleartext ---- xor              cleartext --- xor
//!                  |                              |
//!              ciphertext                     ciphertext
ky_byte crypto_block::encrypt_ofb(const ky_byte &cleartext, const uint8* iv)
{
    if (!priv || !byte)
        return ky_byte();

    // 明文信息
    uint8 *in = (uint8 *)cleartext.data();
    int32 oldsize = cleartext.bytecount() + kyBlockExtra;

    // 计算出块数和余数
    const int32 blocks = oldsize / byte;
    const uint8 extra = oldsize % byte;
    const int32 newsize = blocks *byte + (extra ?byte :0);

    // 申请密文内存
    ky_byte ciphertext;
    ciphertext.resize(newsize);
    uint8 *out = ciphertext.data();

    uint8 buf[byte];
    uint8 x = 1;
    ky_memory::copy(buf, iv, sizeof(buf));
    priv->encrypt(buf);
    *out++ = buf[0] ^ extra;

    // 块数据加入
    for (int32 b = 0; b < blocks; ++b)
    {
        for (; x < byte; ++x, ++out, ++in)
            *out = buf[x] ^ *in;
        priv->encrypt(buf);
        x = 0;
    }

    // 多余加入
    if (extra)
    {
        for (; x < extra; ++x, ++in, ++out)
            *out = buf[x] ^ *in;
    }

    return ciphertext;
}
//!
//! \brief Output feedback decrypt
//!                IV(iv)         |-----------------|
//!                  |            |                 |
//!  key --- block cipher encrypt | key --- block cipher encrypt  ... ...
//!                  |------------|                 |-------------
//!  ciphertext --- xor              ciphertext -- xor
//!                  |                              |
//!              cleartext                     cleartext
ky_byte crypto_block::decrypt_ofb(const ky_byte &ciphertext, const uint8* iv)
{
    uint8 *in = (uint8*)ciphertext.data();
    const uint32 size = ciphertext.bytecount();

    if (!byte || (size % byte) || (!priv))
        return ky_byte();

    // 分配明文内存
    ky_byte cleartext;
    cleartext.resize(size);
    uint8 *out = cleartext.data();

    uint8 buf[byte];
    ky_memory::copy(buf, iv, sizeof(buf));

    for (uint32 b = 0; b < size; b += byte)
    {
        priv->encrypt(buf);
        for (uint8 x = 0; x < byte; ++x, ++out, ++in)
            *out = buf[x] ^ *in;
    }

    // 还原明文
    const uint8 extra = *cleartext.data();
    if (extra > 0xF)
        return ky_byte();

    int32 old = size - kyBlockExtra;
    if (extra)
        old -= (byte - extra);
    ky_byte ct(cleartext.data()+kyBlockExtra, old);

    return ct;
}

//!
//! \brief Counter mode encrypt
//!                IV(iv)(var)                   IV(iv)(var++)
//!                  |                             |
//!  key --- block cipher encrypt  key --- block cipher encrypt  ... ...
//!                  |                             |
//!  cleartext ---- xor            cleartext ---- xor
//!                  |                             |
//!             ciphertext                     ciphertext
ky_byte crypto_block::encrypt_ctr(const ky_byte &cleartext, const uint8* iv)
{
    if (!priv || !byte)
        return ky_byte();

    // 明文信息
    uint8 *in = (uint8 *)cleartext.data();
    int32 oldsize = cleartext.bytecount() + kyBlockExtra;

    // 计算出块数和余数
    const int32 blocks = oldsize / byte;
    const uint8 extra = oldsize % byte;
    const int32 newsize = blocks *byte + (extra ?byte :0) ;

    // 申请密文内存
    ky_byte ciphertext;
    ciphertext.resize(newsize);
    uint8 *out = ciphertext.data();

    uint8 buf[byte];
    ky_memory::copy(buf, iv, sizeof(buf));
    uint8 x = 1;
    priv->encrypt(buf);
    *out++ = buf[0] ^ extra;
    // 块数据加入
    for (int32 b = 0; b < blocks; ++b)
    {
        for (; x < byte; ++x, ++out, ++in)
            *out = buf[x] ^ *in;
        iv_increase(buf, b);
        priv->encrypt(buf);
        x = 0;
    }

    if (extra)
    {
        // 多余加入
        for (; x < extra; ++x, ++in, ++out)
            *out = buf[x] ^ *in;
    }

    return ciphertext;
}
ky_byte crypto_block::decrypt_ctr(const ky_byte &ciphertext, const uint8* iv)
{
    uint8 *in = (uint8*)ciphertext.data();
    const uint32 size = ciphertext.bytecount();

    if (!byte || (size % byte) || (!priv))
        return ky_byte();

    // 分配明文内存
    ky_byte cleartext;
    cleartext.resize(size);
    uint8 *out = cleartext.data();

    uint8 buf[byte];
    ky_memory::copy(buf, iv, sizeof(buf));

    for (uint32 b = 0, inx = 0; b < size; b += byte, ++inx)
    {
        priv->encrypt(buf);
        for (uint8 x = 0; x < byte; ++x, ++out, ++in)
            *out = buf[x] ^ *in;
        iv_increase(buf, inx);
    }

    // 还原明文
    const uint8 extra = *cleartext.data();
    if (extra > 0xF)
        return ky_byte();

    int32 old = size - kyBlockExtra;
    if (extra)
        old -= (byte - extra);
    ky_byte ct(cleartext.data()+kyBlockExtra, old);

    return ct;
}
