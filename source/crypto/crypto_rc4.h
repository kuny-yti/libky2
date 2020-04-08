#ifndef CRYPTO_RC4_H
#define CRYPTO_RC4_H

#include "crypto/ky_crypto.h"
#include "crypto.h"

class crypto_rc4 : public icrypto
{
public:
    typedef ky_crypto::key key;
public:
    crypto_rc4();
    virtual ~crypto_rc4();

    //!
    //! \brief set 设置密钥
    //! \param pp 密钥
    //! \note
    //!
    void set(const key &pp);

    //!
    //! \brief encrypt 加密明文，返回密文
    //! \param cleartext
    //! \return
    //!
    virtual void encrypt(uint8 *block);
    //!
    //! \brief decrypt 解密密文，返回明文
    //! \param ciphertext
    //! \return
    //!
    virtual void decrypt(uint8 *block);

    //!
    //! \brief encrypt
    //! \param cleartext
    //! \return
    //!
    ky_byte encrypt(const ky_byte &cleartext);
    //!
    //! \brief decrypt
    //! \param ciphertext
    //! \return
    //!
    ky_byte decrypt(const ky_byte &ciphertext);

private:
    struct rc4_priv * priv;
};

#endif // CRYPTO_RC4_H
