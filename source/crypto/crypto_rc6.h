#ifndef CRYPTO_RC6_H
#define CRYPTO_RC6_H

#include "crypto/ky_crypto.h"
#include "crypto.h"

class crypto_rc6 : public icrypto
{
public:
    typedef ky_crypto::key key;
public:
    crypto_rc6();
    virtual ~crypto_rc6();

    //!
    //! \brief set 设置密钥
    //! \param pp 密钥
    //! \param word word size [32/64/128]
    //! \param round 加密轮数
    //! \note
    //!
    void set(const key &pp, int8 word = 32, int8 round = 20);

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
    struct rc6_priv * priv;
};

#endif // CRYPTO_RC6_H
