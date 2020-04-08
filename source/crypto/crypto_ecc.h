#ifndef CRYPTO_ECC_H
#define CRYPTO_ECC_H

#include "crypto/ky_crypto.h"
#include "crypto.h"

class crypto_ecc : public icrypto
{
public:
    typedef ky_crypto::key key;
    typedef struct hash_ctx
    {
        void (*init_hash)(const struct hash_ctx *context);
        void (*update_hash)(const struct hash_ctx *context, const uint8 *message,
                            unsigned message_size);
        void (*finish_hash)(const struct hash_ctx *context, uint8 *hash_result);
        unsigned block_size; /* Hash function block size in bytes, eg 64 for SHA-256. */
        unsigned result_size; /* Hash function result size in bytes, eg 32 for SHA-256. */
        uint8 *tmp; /* Must point to a buffer of at least (2 * result_size + block_size) bytes. */
    } hash_ctx;

public:
    explicit crypto_ecc(eCryptoSecpECC cse);

    virtual ~crypto_ecc();

    //!
    //! \brief keysize 返回密钥的字节数
    //! \param flag
    //! \return
    //!
    int size_public()const;
    int size_private()const;
    //!
    //! \brief key_valid 返回公钥是否有效
    //! \return
    //!
    bool is_valid()const;

    //!
    //! \brief generate 生成密钥
    //! \return
    //!
    bool generate() ;

    //!
    //! \brief set 设置密钥
    //! \param flag
    //!
    void set_public(const key &k);
    void set_private(const key &k);
    //!
    //! \brief get 获取密钥
    //! \param flag
    //! \return
    //!
    key key_public()const;
    key key_private()const;

    //!
    //! \brief shared_secret 获取共享密钥
    //! \param pubkey 对端公钥
    //! \param secret
    //! \return
    //!
    int shared_secret(const key &pubkey, uint8 *secret);

#if CryptoSupportCompressed
    //!
    //! \brief compress 压缩公有密钥
    //! \param compressed
    //!
    void compress( uint8 *compressed);
    //!
    //! \brief decompress 解压公有密钥
    //! \param compressed
    //!
    void decompress(const uint8 *compressed);
#endif

    //!
    //! \brief sign 生成ECDSA签名
    //! \param hash
    //! \param hash_size
    //! \param signature
    //! \return
    //!
    bool sign(const uint8 *hash, uint hash_size, uint8 *signature);

    //!
    //! \brief sign_deterministic 生成ECDSA签名(确定性)
    //! \param hash
    //! \param hash_size
    //! \param hash_context
    //! \param signature
    //! \return
    //!
    bool sign_deterministic(const uint8 *hash, unsigned hash_size,
                            const hash_ctx *hash_context, uint8 *signature);

    //!
    //! \brief sign_verify 验证ECDSA签名
    //! \param hash
    //! \param hash_size
    //! \param signature
    //! \return
    //!
    bool sign_verify(const uint8 *hash, unsigned hash_size,
                     const uint8 *signature);
protected:
    //!
    //! \brief compute_key 根据私钥计算公钥
    //! \param private_key
    //! \param public_key
    //! \return
    //!
    bool compute_key(const uint8 *private_key, uint8 *public_key);

private:
    key               pubkey;
    key               prikey;

    struct ecc_curve *priv;
};

#endif // CRYPTO_ECC_H
