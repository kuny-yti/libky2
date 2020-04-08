#include "crypto/ky_crypto.h"
#include "crypto/crypto_block.h"
#include "ky_datetime.h"

ky_crypto::key::key():
    ky_array<uint8>()
{
    p_m = p_e = 0;
}
ky_crypto::key::key(int32 byte):
    ky_array<uint8>()
{
    ky_array<uint8>::resize(byte);
    ky_array<uint8>::shrink();
    p_m = (int64*)ky_array<uint8>::data();
    p_e = p_m + 1;
}
ky_crypto::key::key(int64 m, int64 e):
    ky_array<uint8>()
{
    ky_array<uint8>::resize(sizeof(int64)*2);
    ky_array<uint8>::shrink();
    p_m = (int64*)ky_array<uint8>::data();
    p_e = p_m + 1;
    *p_m = m;
    *p_e = e;
}

ky_crypto::key::key(const std::initializer_list<uint8> & il):
    ky_array<uint8>(il)
{
    p_m = (int64*)ky_array<uint8>::data();
    p_e = p_m + 1;
}

int64 &ky_crypto::key::modulus()
{
    return *p_m;
}
int64 &ky_crypto::key::modulus()const
{
    return *p_m;
}
int64 &ky_crypto::key::exponent()
{
    return *p_e;
}
int64 &ky_crypto::key::exponent()const
{
    return *p_e;
}

struct crypto_priv : public crypto_block
{
    ky_crypto::key  k_p;
    ky_crypto::key  k_s;
    ky_crypto::key  k_v;
    eCryptoFlags    flag;

    union
    {
        crypto_aes      * aes;
        crypto_blowfish * blowfish;
        crypto_rsa      * rsa;
        crypto_rc4      * rc4;
        crypto_rc6      * rc6;
    };

    crypto_priv(eCryptoFlags f)
    {
        priv = 0;
        byte = 0;
        flag = Crypto_None;
        switch ((int)(f & Crypto_CryptoMask))
        {
        case Crypto_Aes256:
        case Crypto_Aes192:
        case Crypto_Aes128:
            priv = kyAsCast(icrypto*)(aes = kyNew(crypto_aes()));
            break;
        case Crypto_Blowfish:
            priv = kyAsCast(icrypto*)(blowfish = kyNew(crypto_blowfish()));
            break;
        case Crypto_Rsa:
        {
            // 生成素数的基数用时间作为基数
            ky_datetime dt = ky_datetime::current_time();
            int64 base = dt.year() + dt.month() + dt.day() + dt.hour();
            priv = kyAsCast(icrypto*)(rsa = kyNew(crypto_rsa(base)));
        }
            break;
        case Crypto_Rc4 :
            priv = kyAsCast(icrypto*)(rc4 = kyNew(crypto_rc4()));
            break;
        case Crypto_Rc6 :
            priv = kyAsCast(icrypto*)(rc6 = kyNew(crypto_rc6()));
            break;
        }
        if (priv)
        {
            flag = f;
            if ((flag & Crypto_BlockMask) == Crypto_Block2x2)
                byte = 4;
            else if ((flag & Crypto_BlockMask) == Crypto_Block2x4)
                byte = 8;
            else
                byte = 16;
        }
    }
    virtual ~crypto_priv()
    {
        switch ((int)(flag & Crypto_CryptoMask))
        {
        case Crypto_Aes256:
        case Crypto_Aes192:
        case Crypto_Aes128:
            kyDelete(aes);
            break;
        case Crypto_Blowfish:
            kyDelete(blowfish);
            break;
        case Crypto_Rsa:
            kyDelete(rsa);
            break;
        case Crypto_Rc4 :
            kyDelete(rc4);
            break;
        case Crypto_Rc6 :
            kyDelete(rc6);
            break;
        }
    }
};

ky_crypto::ky_crypto()
{
    priv = 0;
}
ky_crypto::ky_crypto(const eCryptoFlags f)
{
    priv = 0;
    set(f);
}
ky_crypto::ky_crypto(const key &pp, const eCryptoFlags f)
{
    priv = 0;
    set(f);
    set_public(pp);
}
ky_crypto::~ky_crypto()
{
    if (priv)
        kyDelete(priv);
}

void ky_crypto::set_public(const key &pk)
{
    if (!priv)
        return ;
    // 设置密钥
    switch (priv->flag & Crypto_CryptoMask)
    {
    case Crypto_Aes128:
        priv->aes->set(pk, AES_128);
        break;
    case Crypto_Aes192:
        priv->aes->set(pk, AES_192);
        break;
    case Crypto_Aes256:
        priv->aes->set(pk, AES_256);
        break;
    case Crypto_Blowfish:
        priv->blowfish->set(pk, pk.size());
        break;
    case Crypto_Rsa:
        priv->rsa->set_public(pk);
        break;
    case Crypto_Rc4:
        priv->rc4->set(pk);
        break;
    case Crypto_Rc6:
        priv->rc6->set(pk);
        break;
    }
    priv->k_p = pk;
}
void ky_crypto::set_private(const key &pk)
{
    if (!priv)
        return ;
    // 设置密钥
    switch (priv->flag & Crypto_CryptoMask)
    {
    case Crypto_Aes128:
        break;
    case Crypto_Aes192:
        break;
    case Crypto_Aes256:
        break;
    case Crypto_Blowfish:
        break;
    case Crypto_Rsa:
        priv->rsa->set_private(pk);
        break;
    case Crypto_Rc4:
        break;
    case Crypto_Rc6:
        break;
    }
    priv->k_s = pk;
}
void ky_crypto::set_vector(const key &iv)
{
    if (!priv)
        return ;
    priv->k_v = iv;
}

const ky_crypto::key &ky_crypto::key_public()const
{
    return priv->k_p;
}
const ky_crypto::key &ky_crypto::key_private()const
{
    return priv->k_s;
}
const ky_crypto::key &ky_crypto::key_vector()const
{
    return priv->k_v;
}

ky_crypto::key ky_crypto::gen_public(const int bits) const
{

}
ky_crypto::key ky_crypto::gen_paivate(const int bits) const
{

}
//!
//! \brief set
//! \param flag
//!
void ky_crypto::set(const eCryptoFlags f)
{
    if (f & Crypto_CryptoMask)
    {
        if (priv)
            kyDelete(priv);

        priv = kyNew(crypto_priv(f));
    }
}

//!
//! \brief encrypt
//! \param cleartext
//! \return
//!
ky_byte ky_crypto::encrypt(const ky_byte &cleartext)
{
    if (!priv || priv->k_p.is_empty())
        return ky_byte();
    if (priv->k_s.is_empty())
        priv->k_s = priv->k_p;
    // 非对称RSA流加密
    if ((priv->flag & Crypto_CryptoMask) == Crypto_Rsa)
    {
        priv->rsa->generate();
        return priv->rsa->encrypt(cleartext);
    }

    // 分组加密
    bool emp_iv = priv->k_v.is_empty();
    uint8 *iv = emp_iv ? priv->k_s.data(): priv->k_v.data();
    switch (priv->flag & Crypto_ModeMask)
    {
    case Crypto_CipherBlockChaining:
        return priv->encrypt_cbc(cleartext, iv);
    case Crypto_CipherFeedBack:
        return priv->encrypt_cfb(cleartext, iv);
    case Crypto_OutputFeedBack:
        return priv->encrypt_ofb(cleartext, iv);
    case Crypto_Counter:
        return priv->encrypt_ctr(cleartext, iv);
    case Crypto_ElectronicCodeBook:
    default:
        return priv->encrypt_ecb(cleartext);
    }
    return ky_byte();
}
//!
//! \brief decrypt
//! \param ciphertext
//! \return
//!
ky_byte ky_crypto::decrypt(const ky_byte &ciphertext)
{
    if (!priv || priv->k_p.is_empty())
        return ky_byte();
    if (priv->k_s.is_empty())
        priv->k_s = priv->k_p;

    // 非对称RSA流解密
    if ((priv->flag & Crypto_CryptoMask) == Crypto_Rsa)
        return priv->rsa->decrypt(ciphertext);

    // 分组解密
    bool emp_iv = priv->k_v.is_empty();
    uint8 *iv = emp_iv ? priv->k_s.data(): priv->k_v.data();
    switch (priv->flag & Crypto_ModeMask)
    {
    case Crypto_CipherBlockChaining:
        return priv->decrypt_cbc(ciphertext, iv);
    case Crypto_CipherFeedBack:
        return priv->decrypt_cfb(ciphertext, iv);
    case Crypto_OutputFeedBack:
        return priv->decrypt_ofb(ciphertext, iv);
    case Crypto_Counter:
        return priv->decrypt_ctr(ciphertext, iv);
    case Crypto_ElectronicCodeBook:
    default:
        return priv->decrypt_ecb(ciphertext);
    }
    return ky_byte();
}

//!
//! \brief encrypt
//! \param cleartext
//! \param flag
//! \return
//!
ky_byte ky_crypto::encrypt(const ky_byte &cleartext, const key &k, const eCryptoFlags flag)
{
    ky_crypto crypto(flag);
    crypto.set_public(k);
    return crypto.encrypt(cleartext);
}
//!
//! \brief decrypt
//! \param ciphertext
//! \param flag
//! \return
//!
ky_byte ky_crypto::decrypt(const ky_byte &ciphertext, const key &k, const eCryptoFlags flag)
{
    ky_crypto crypto(flag);
    crypto.set_public(k);
    return crypto.decrypt(ciphertext);
}
