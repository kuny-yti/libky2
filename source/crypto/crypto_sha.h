#ifndef CRYPTO_SHA_H
#define CRYPTO_SHA_H

#include"crypto/ky_crypto.h"
#include "crypto.h"

class crypto_sha
{
public:
    explicit crypto_sha(eCryptoSHA cs);
    virtual ~crypto_sha();

    void set(eCryptoSHA cs);

    void update(const ky_byte &cd);

    ky_byte final();

    ky_byte hash(const ky_byte &cd);
    static ky_byte hash(const ky_byte &cd, eCryptoSHA cs);
private:
    struct sha_priv *priv;
};

#endif // CRYPTO_SHA_H
