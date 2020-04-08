#include "crypto/ky_hash.h"
#include "crypto_sha.h"
#include "crypto_md5.h"

struct hash_priv
{
    bool is_md5;
    union
    {
        crypto_md5 *md5;
        crypto_sha *sha;
    };
};

ky_hash::ky_hash()
{
    priv = kyNew(hash_priv);
    priv->md5 = 0;
    priv->is_md5 = false;
}
ky_hash::ky_hash(const eHashFlags flag)
{
    priv = kyNew(hash_priv);
    priv->md5 = 0;
    priv->is_md5 = false;
    set(flag);
}
ky_hash::~ky_hash()
{
    kyDelete(priv);
}

void ky_hash::set(const eHashFlags flag)
{
    priv->is_md5 = (flag == Hash_MD5) || (flag == Hash_None);
    if (priv->is_md5)
        priv->md5 = kyNew(crypto_md5());
    else
    {
        eCryptoSHA csf = SHA_1;
        switch ((int)flag)
        {
        case Hash_SHA224:
            csf = SHA_224;
            break;
        case Hash_SHA256:
            csf = SHA_256;
            break;
        case Hash_SHA384:
            csf = SHA_384;
            break;
        case Hash_SHA512:
            csf = SHA_512;
            break;
        };
        priv->sha = kyNew(crypto_sha(csf));
    }
}

void ky_hash::update(const ky_byte &cd)
{
    if (priv->is_md5)
        priv->md5->update(cd);
    else
        priv->sha->update(cd);
}

ky_byte ky_hash::final()
{
    if (priv->is_md5)
        return priv->md5->final();
    return priv->sha->final();
}

ky_byte ky_hash::hash(const ky_byte &d)
{
    update(d);
    return final();
}

ky_byte ky_hash::hash(const ky_byte &d, const eHashFlags flag)
{
    ky_hash oj(flag);
    return oj.hash(d);
}
