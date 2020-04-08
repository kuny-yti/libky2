#ifndef CRYPTO_AES_H
#define CRYPTO_AES_H

#include "crypto/ky_crypto.h"
#include "crypto.h"

class crypto_aes : public icrypto
{
public:
    typedef ky_crypto::key key;

public:
    explicit crypto_aes();
    virtual ~crypto_aes();

    //!
    //! \brief set 设置密钥
    //! \param pp 密钥
    //! \param ac
    //! \note
    //!
    void set(const key &pp, const eCryptoAES ac = AES_256);

    //!
    //! \brief get 获取指定密钥
    //! \param cf
    //! \return
    //!
    key get()const;

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

private:
    struct aes_priv * priv;
};

#endif // CRYPTO_AES_H
