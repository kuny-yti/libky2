#ifndef CRYPTO_BASE64_H
#define CRYPTO_BASE64_H

#include "crypto/ky_crypto.h"
#include "crypto.h"

class crypto_base64
{
public:
    crypto_base64(bool is_wrap);

    ky_byte encode(const ky_byte &td);
    ky_byte decode(const ky_byte &ed);

private:
    bool hav_wrap;
};

#endif // CRYPTO_BASE64_H
