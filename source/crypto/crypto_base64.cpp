#include "crypto_base64.h"

static const uint8_t  charset[]={"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"};

uint8_t revchar(char ch)
{
    if (ch >= 'A' && ch <= 'Z')
        ch -= 'A';
    else if (ch >= 'a' && ch <='z')
        ch = ch - 'a' + 26;
    else if (ch >= '0' && ch <='9')
        ch = ch - '0' + 52;
    else if (ch == '+')
        ch = 62;
    else if (ch == '/')
        ch = 63;

    return(ch);
}

crypto_base64::crypto_base64(bool is_wrap)
{
    hav_wrap = is_wrap;
}

ky_byte crypto_base64::encode(const ky_byte &td)
{
    size_t idx, idx2, newline_count = 0;

    int64 blks = (td.bytecount() / 3);
    int64 left_over = td.bytecount() % 3;
    int64 blk_ceiling = blks * 3;

    ky_byte outd;
    outd.resize(blks*4+4);
    uint8_t *out = outd.data();
    const uint8_t *in = td.data();

    for (idx = 0, idx2 = 0; idx < blk_ceiling; idx += 3, idx2 += 4)
    {
        out[idx2]     = charset[in[idx] >> 2];
        out[idx2 + 1] = charset[((in[idx] & 0x03) << 4) |
                (in[idx + 1] >> 4)];
        out[idx2 + 2] = charset[((in[idx + 1] & 0x0f) << 2) |
                (in[idx + 2] >> 6)];
        out[idx2 + 3] = charset[in[idx + 2] & 0x3F];

        if (((idx2 - newline_count + 4) % 76 == 0) && hav_wrap)
        {
            out[idx2 + 4] = '\n';
            idx2++;
            newline_count++;
        }
    }

    if (left_over == 1)
    {
        out[idx2]     = charset[in[idx] >> 2];
        out[idx2 + 1] = charset[(in[idx] & 0x03) << 4];
        out[idx2 + 2] = '=';
        out[idx2 + 3] = '=';
        idx2 += 4;
    }
    else if (left_over == 2)
    {
        out[idx2]     = charset[in[idx] >> 2];
        out[idx2 + 1] = charset[((in[idx] & 0x03) << 4) | (in[idx + 1] >> 4)];
        out[idx2 + 2] = charset[(in[idx + 1] & 0x0F) << 2];
        out[idx2 + 3] = '=';
        idx2 += 4;
    }

    return outd;
}
ky_byte crypto_base64::decode(const ky_byte &ed)
{
    int64 idx, idx2;

    int64 len = ed.bytecount();
    const uint8_t *in = ed.data();

    if (in[len - 1] == '=')
        len--;
    if (in[len - 1] == '=')
        len--;

    int64 blks = len / 4;
    int64 left_over = len % 4;
    int64 blk_ceiling = blks * 4;

    ky_byte outd;
    outd.resize(blks*3+(left_over == 2 ? 2 : 3));
    uint8_t *out = outd.data();

    for (idx = 0, idx2 = 0; idx2 < blk_ceiling; idx += 3, idx2 += 4)
    {
        if (in[idx2] == '\n')
            idx2++;
        out[idx]     = (revchar(in[idx2]) << 2) | ((revchar(in[idx2 + 1]) & 0x30) >> 4);
        out[idx + 1] = (revchar(in[idx2 + 1]) << 4) | (revchar(in[idx2 + 2]) >> 2);
        out[idx + 2] = (revchar(in[idx2 + 2]) << 6) | revchar(in[idx2 + 3]);
    }

    if (left_over == 2)
    {
        out[idx] = (revchar(in[idx2]) << 2) | ((revchar(in[idx2 + 1]) & 0x30) >> 4);
        idx++;
    }
    else if (left_over == 3)
    {
        out[idx] = (revchar(in[idx2]) << 2) | ((revchar(in[idx2 + 1]) & 0x30) >> 4);
        out[idx + 1] = (revchar(in[idx2 + 1]) << 4) | (revchar(in[idx2 + 2]) >> 2);
        idx += 2;
    }
    return outd;
}
