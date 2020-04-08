#ifndef CRYPTO_BLOWFISH_H
#define CRYPTO_BLOWFISH_H

#include "crypto/ky_crypto.h"
#include "crypto.h"

class crypto_blowfish : public icrypto
{
public:
    typedef ky_crypto::key key;

public:
    crypto_blowfish();
    virtual ~crypto_blowfish();

    //!
    //! \brief set
    //! \param pp
    //! \param ks 不得超过72
    //!
    void set(const key &pp, const int ks = 56);
    key get()const;

    virtual void encrypt(uint8 *block);
    virtual void decrypt(uint8 *block);

    ky_byte encrypt(const ky_byte &cleartext);
    ky_byte decrypt(const ky_byte &ciphertext);

protected:
    void encrypt(const uint8 *in, uint8 *out);
    void decrypt(const uint8 *in, uint8 *out);

private:
    struct bf_priv *priv;
};

#endif // CRYPTO_BLOWFISH_H
