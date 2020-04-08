#ifndef CRYPTO_MD5_H
#define CRYPTO_MD5_H
#include "crypto/ky_crypto.h"
#include "crypto.h"

class crypto_md5
{
public:
    crypto_md5();
    ~crypto_md5();

    void update(const ky_byte &cd);

    ky_byte final();

    static ky_byte hash(const ky_byte &cd);
private:
    struct md5_priv *priv;
};

#endif // CRYPTO_MD5_H
