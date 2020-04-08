#include "ky_codec.h"
#include "crypto/crypto_base64.h"

static const char hex_upper[] = "0123456789ABCDEF";
static const char hex_lower[] = "0123456789abcdef";
static const int8 hex_array[256] =
{
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, -1, -1, -1, -1, -1,
    -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

ky_byte ky_codec::to_hex(const ky_byte &b, bool is_lower)
{
    ky_byte res;
    res.resize(b.size()*2);
    uint8 *in = b.data();
    uint8 *out = res.data();
    const char *hex = is_lower ? hex_lower : hex_upper;
    for (int i = 0; i < b.size(); ++i, ++in, out += 2)
    {
        out[0] = hex[((*in) >> 4) & 0xF];
        out[1] = hex[(*in) & 0xF];
    }
    return res;
}
ky_byte ky_codec::from_hex(const ky_byte &b)
{
    ky_byte res;
    res.resize(b.size() / 2);
    uint8 *in = b.data();
    uint8 *out = res.data();

    uint8 accum = 0;
    for (int32 i = 0; i < b.size(); ++i)
    {
        int8 value = hex_array[in[i]];
        if (value < 0)
            return ky_byte();

        accum = (accum << 4) | value;

        if (i & 1)
        {
            *out++ = accum;
            accum = 0;
        }
    }

    return res;
}

ky_byte ky_codec::to_base64(const ky_byte &b, bool is_wrap)
{
    crypto_base64 b64(is_wrap);
    return b64.encode(b);
}

ky_byte ky_codec::from_base64(const ky_byte &b)
{
    crypto_base64 b64(false);
    return b64.decode(b);
}
