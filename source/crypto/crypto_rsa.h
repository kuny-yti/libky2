#ifndef CRYPTO_RSA_H
#define CRYPTO_RSA_H

#include "crypto/ky_crypto.h"
#include "crypto.h"

class crypto_rsa : public icrypto
{
public:
    typedef ky_crypto::key key;

public:
    //!
    //! \brief crypto_rsa 构造函数，可指定素数表数量
    //! \param prime_base 素数生成基数
    //! \param prime_count 素数生成个数
    //!
    crypto_rsa(const int prime_base, const int64 prime_count = 1024);
    virtual ~crypto_rsa();

    //!
    //! \brief set 设置密钥
    //! \param pp 密钥
    //!
    void set_public(const key &pp);
    void set_private(const key &pp);

    //!
    //! \brief get 获取指定密钥
    //! \return
    //!
    key key_public()const;
    key key_private()const;

    //!
    //! \brief generate 生成公私密钥
    //! \return
    //!
    void generate();

    virtual void encrypt(uint8 *block) {}
    virtual void decrypt(uint8 *block) {}

    //!
    //! \brief encrypt 加密明文，返回密文
    //! \param cleartext
    //! \return
    //!
    ky_byte encrypt(const ky_byte &cleartext);
    //!
    //! \brief decrypt 解密密文，返回明文
    //! \param ciphertext
    //! \return
    //!
    ky_byte decrypt(const ky_byte &ciphertext);

private:
    key          pubkey;
    key          prikey;
    int64       *primes;
    int64        count;
};

#endif // CRYPTO_RSA_H
