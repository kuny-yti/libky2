#include "crypto_ecc.h"
#include "ecc_curve.h"
#include "vli_math.h"

#if (CryptoPlatform == CryptoARM || \
     CryptoPlatform == CryptoARMThumb || \
     CryptoPlatform == CryptoARMThumb2)
    #include "arch/asm_arm.inc"
#elif (CryptoPlatform == CryptoAVR)
    #include "arch/asm_avr.inc"
#endif

crypto_ecc::crypto_ecc(eCryptoSecpECC cse)
{
    priv =
            cse == ECC_Secp160R1 ? ecc_secp160r1() :
            cse == ECC_Secp192R1 ? ecc_secp192r1() :
            cse == ECC_Secp224R1 ? ecc_secp224r1() :
            cse == ECC_Secp256R1 ? ecc_secp256r1() :
            cse == ECC_Secp256K1 ? ecc_secp256k1() :
            0;
}
crypto_ecc::~crypto_ecc()
{

}

int crypto_ecc::size_public()const
{
    return priv->num_bytes * 2;
}
int crypto_ecc::size_private()const
{
    return BITS_TO_BYTES(priv->num_n_bits);
}

bool crypto_ecc::is_valid()const
{
#if CryptoNativeLittleEndian
    word_t *_public = pubkey.to<word_t>();
#else
    word_t _public[CryptoMaxWord * 2];
#endif

#if CryptoNativeLittleEndian == 0
    vli_bytesToNative(_public, pubkey.data(), priv->num_bytes);
    vli_bytesToNative(_public + priv->num_words,
                      pubkey.data() + priv->num_bytes, priv->num_bytes);
#endif
    return valid_point(_public, priv);
}

void crypto_ecc::set_public(const key &k)
{
    pubkey = k;
}
void crypto_ecc::set_private(const key &k)
{
    prikey = k;
}

crypto_ecc::key crypto_ecc::key_public()const
{
    return pubkey;
}
crypto_ecc::key crypto_ecc::key_private()const
{
   return prikey;
}

bool crypto_ecc::generate()
{
#if CryptoNativeLittleEndian
    word_t *_private = prikey.to<word_t>();
    word_t *_public = pubkey.to<word_t>();
#else
    word_t _private[CryptoMaxWord];
    word_t _public[CryptoMaxWord * 2];
#endif

    for (word_t tries = 0; tries < CryptoRNGMaxTries; ++tries)
    {
        if (!generate_random_int(_private, priv->n,
                                 BITS_TO_WORDS(priv->num_n_bits)))
            return 0;

        if (EccPoint_compute_public_key(_public, _private, priv))
        {
#if CryptoNativeLittleEndian == 0
            vli_nativeToBytes(prikey.data(), BITS_TO_BYTES(priv->num_n_bits), _private);
            vli_nativeToBytes(pubkey.data(), priv->num_bytes, _public);
            vli_nativeToBytes(pubkey.data() + priv->num_bytes, priv->num_bytes,
                               _public + priv->num_words);
#endif
            return 1;
        }
    }
    return 0;
}
int crypto_ecc::shared_secret(const key &pubkey, uint8 *secret)
{
    word_t _public[CryptoMaxWord * 2];
    word_t _private[CryptoMaxWord];

    word_t tmp[CryptoMaxWord];
    word_t *p2[2] = {_private, tmp};
    word_t *initial_Z = 0;
    word_t carry;
    int8 num_words = priv->num_words;
    int8 num_bytes = priv->num_bytes;

#if CryptoNativeLittleEndian
    bcopy((uint8 *) _private, prikey.data(), num_bytes);
    bcopy((uint8 *) _public, pubkey.data(), num_bytes*2);
#else
    vli_bytesToNative(_private, prikey.data(), BITS_TO_BYTES(priv->num_n_bits));
    vli_bytesToNative(_public, pubkey.data(), num_bytes);
    vli_bytesToNative(_public + num_words, pubkey.data() + num_bytes, num_bytes);
#endif

    /* Regularize the bitcount for the private key so that attackers cannot use a side channel
       attack to learn the number of leading zeros. */
    carry = regularize_k(_private, _private, tmp, priv);

    /* If an RNG function was specified, try to get a random initial Z value to improve
       protection against side-channel attacks. */
    if (vli_get_rng())
    {
        if (!generate_random_int(p2[carry], priv->p, num_words))
            return 0;
        initial_Z = p2[carry];
    }

    EccPoint_mult(_public, _public, p2[!carry], initial_Z, priv->num_n_bits + 1, priv);
#if CryptoNativeLittleEndian
    bcopy((uint8 *) secret, (uint8 *) _public, num_bytes);
#else
    vli_nativeToBytes(secret, num_bytes, _public);
#endif
    return !EccPoint_isZero(_public, priv);
}

#if CryptoSupportCompressed
void crypto_ecc::compress(uint8 *compressed)
{
    for (int8 i = 0; i < priv->num_bytes; ++i)
        compressed[i+1] = pubkey[i];

#if CryptoNativeLittleEndian
    compressed[0] = 2 + (pubkey[priv->num_bytes] & 0x01);
#else
    compressed[0] = 2 + (pubkey[priv->num_bytes * 2 - 1] & 0x01);
#endif
}

void crypto_ecc::decompress(const uint8 *compressed)
{
#if CryptoNativeLittleEndian
    word_t *point = (word_t *)public_key;
#else
    word_t point[CryptoMaxWord * 2];
#endif
    word_t *y = point + priv->num_words;
#if CryptoNativeLittleEndian
    bcopy(pubkey, compressed+1, priv->num_bytes);
#else
    vli_bytesToNative(point, compressed + 1, priv->num_bytes);
#endif
    priv->x_side(y, point, priv);
    priv->mod_sqrt(y, priv);

    if ((y[0] & 0x01) != (compressed[0] & 0x01))
        vli_sub(y, priv->p, y, priv->num_words);

#if CryptoNativeLittleEndian == 0
    vli_nativeToBytes(pubkey, priv->num_bytes, point);
    vli_nativeToBytes(pubkey.data() + priv->num_bytes, priv->num_bytes, y);
#endif
}
#endif
bool crypto_ecc::compute_key(const uint8 *private_key, uint8 *public_key)
{
#if CryptoNativeLittleEndian
    word_t *_private = (word_t *)private_key;
    word_t *_public = (word_t *)public_key;
#else
    word_t _private[CryptoMaxWord];
    word_t _public[CryptoMaxWord * 2];
#endif

#if CryptoNativeLittleEndian == 0
    vli_bytesToNative(_private, private_key, BITS_TO_BYTES(priv->num_n_bits));
#endif

    /* Make sure the private key is in the range [1, n-1]. */
    if (vli_isZero(_private, BITS_TO_WORDS(priv->num_n_bits)))
        return false;

    if (vli_cmp(priv->n, _private, BITS_TO_WORDS(priv->num_n_bits)) != 1)
        return false;

    /* Compute public key. */
    if (!EccPoint_compute_public_key(_public, _private, priv))
        return false;

#if CryptoNativeLittleEndian == 0
    vli_nativeToBytes(public_key, priv->num_bytes, _public);
    vli_nativeToBytes(public_key + priv->num_bytes, priv->num_bytes,
                      _public + priv->num_words);
#endif
    return true;
}


/* -------- ECDSA code -------- */
static void bits2int(word_t *native, const uint8 *bits, unsigned bits_size,
                     ecc_curve* curve)
{
    unsigned num_n_bytes = BITS_TO_BYTES(curve->num_n_bits);
    unsigned num_n_words = BITS_TO_WORDS(curve->num_n_bits);
    int shift;
    word_t carry;
    word_t *ptr;

    if (bits_size > num_n_bytes)
        bits_size = num_n_bytes;

    vli_clear(native, num_n_words);
#if CryptoNativeLittleEndian
    bcopy((uint8 *) native, bits, bits_size);
#else
    vli_bytesToNative(native, bits, bits_size);
#endif
    if (bits_size * 8 <= (unsigned)curve->num_n_bits)
        return;

    shift = bits_size * 8 - curve->num_n_bits;
    carry = 0;
    ptr = native + num_n_words;
    while (ptr-- > native)
    {
        word_t temp = *ptr;
        *ptr = (temp >> shift) | carry;
        carry = temp << (WORD_BITS - shift);
    }

    /* Reduce mod curve_n */
    if (vli_cmp_unsafe(curve->n, native, num_n_words) != 1)
        vli_sub(native, native, curve->n, num_n_words);
}

static bool sign_with_k(const uint8 *private_key,
                        const uint8 *message_hash, unsigned hash_size,
                        word_t *k, uint8 *signature, ecc_curve* curve)
{

    word_t tmp[CryptoMaxWord];
    word_t s[CryptoMaxWord];
    word_t *k2[2] = {tmp, s};
#if CryptoNativeLittleEndian
    word_t *p = (word_t *)signature;
#else
    word_t p[CryptoMaxWord * 2];
#endif
    word_t carry;
    int8 num_words = curve->num_words;
    int8 num_n_words = BITS_TO_WORDS(curve->num_n_bits);
    int16_t num_n_bits = curve->num_n_bits;

    /* Make sure 0 < k < curve_n */
    if (vli_isZero(k, num_words) || vli_cmp(curve->n, k, num_n_words) != 1)
        return false;

    carry = regularize_k(k, tmp, s, curve);
    EccPoint_mult(p, curve->G, k2[!carry], 0, num_n_bits + 1, curve);
    if (vli_isZero(p, num_words))
        return false;

    /* If an RNG function was specified, get a random number
       to prevent side channel analysis of k. */
    if (!vli_get_rng())
    {
        vli_clear(tmp, num_n_words);
        tmp[0] = 1;
    }
    else if (!generate_random_int(tmp, curve->n, num_n_words))
        return false;

    /* Prevent side channel analysis of vli_modInv() to determine
       bits of k / the private key by premultiplying by a random number */
    vli_modMult(k, k, tmp, curve->n, num_n_words); /* k' = rand * k */
    vli_modInv(k, k, curve->n, num_n_words);       /* k = 1 / k' */
    vli_modMult(k, k, tmp, curve->n, num_n_words); /* k = 1 / k */

#if CryptoNativeLittleEndian == 0
    vli_nativeToBytes(signature, curve->num_bytes, p); /* store r */
#endif

#if CryptoNativeLittleEndian
    bcopy((uint8 *) tmp, private_key, BITS_TO_BYTES(curve->num_n_bits));
#else
    vli_bytesToNative(tmp, private_key, BITS_TO_BYTES(curve->num_n_bits)); /* tmp = d */
#endif

    s[num_n_words - 1] = 0;
    vli_set(s, p, num_words);
    vli_modMult(s, tmp, s, curve->n, num_n_words); /* s = r*d */

    bits2int(tmp, message_hash, hash_size, curve);
    vli_modAdd(s, tmp, s, curve->n, num_n_words); /* s = e + r*d */
    vli_modMult(s, s, k, curve->n, num_n_words);  /* s = (e + r*d) / k */
    if (vli_numBits(s, num_n_words) > (int16_t)curve->num_bytes * 8)
        return false;

#if CryptoNativeLittleEndian
    bcopy((uint8 *) signature + curve->num_bytes, (uint8 *) s, curve->num_bytes);
#else
    vli_nativeToBytes(signature + curve->num_bytes, curve->num_bytes, s);
#endif
    return true;
}

bool crypto_ecc::sign(const uint8 *hash, uint hash_size, uint8 *signature)
{
    word_t k[CryptoMaxWord];
    word_t tries;

    for (tries = 0; tries < CryptoRNGMaxTries; ++tries)
    {
        if (!generate_random_int(k, priv->n, BITS_TO_WORDS(priv->num_n_bits)))
            return false;

        if (sign_with_k(prikey.data(), hash, hash_size, k, signature, priv))
            return true;
    }
    return false;
}


/* Compute an HMAC using K as a key (as in RFC 6979). Note that K is always
   the same size as the hash result size. */
static void HMAC_init(const crypto_ecc::hash_ctx *hash_context, const uint8 *K)
{
    uint8 *pad = hash_context->tmp + 2 * hash_context->result_size;
    unsigned i;
    for (i = 0; i < hash_context->result_size; ++i)
        pad[i] = K[i] ^ 0x36;
    for (; i < hash_context->block_size; ++i)
        pad[i] = 0x36;

    hash_context->init_hash(hash_context);
    hash_context->update_hash(hash_context, pad, hash_context->block_size);
}

static void HMAC_update(const crypto_ecc::hash_ctx *hash_context,
                        const uint8 *message, unsigned message_size)
{
    hash_context->update_hash(hash_context, message, message_size);
}

static void HMAC_finish(const crypto_ecc::hash_ctx *hash_context,
                        const uint8 *K, uint8 *result)
{
    uint8 *pad = hash_context->tmp + 2 * hash_context->result_size;
    unsigned i;
    for (i = 0; i < hash_context->result_size; ++i)
        pad[i] = K[i] ^ 0x5c;
    for (; i < hash_context->block_size; ++i)
        pad[i] = 0x5c;

    hash_context->finish_hash(hash_context, result);

    hash_context->init_hash(hash_context);
    hash_context->update_hash(hash_context, pad, hash_context->block_size);
    hash_context->update_hash(hash_context, result, hash_context->result_size);
    hash_context->finish_hash(hash_context, result);
}

/* V = HMAC_K(V) */
static void update_V(const crypto_ecc::hash_ctx *hash_context, uint8 *K, uint8 *V)
{
    HMAC_init(hash_context, K);
    HMAC_update(hash_context, V, hash_context->result_size);
    HMAC_finish(hash_context, K, V);
}

/* Deterministic signing, similar to RFC 6979. Differences are:
    * We just use H(m) directly rather than bits2octets(H(m))
      (it is not reduced modulo curve_n).
    * We generate a value for k (aka T) directly rather than converting endianness.

   Layout of hash_context->tmp: <K> | <V> | (1 byte overlapped 0x00 or 0x01) / <HMAC pad> */
bool crypto_ecc::sign_deterministic(const uint8 *message_hash,
                                    unsigned hash_size,
                                    const hash_ctx *hash_context,
                                    uint8 *signature)
{
    uint8 *K = hash_context->tmp;
    uint8 *V = K + hash_context->result_size;
    int8 num_bytes = priv->num_bytes;
    int8 num_n_words = BITS_TO_WORDS(priv->num_n_bits);
    int16_t num_n_bits = priv->num_n_bits;
    word_t tries;
    unsigned i;
    for (i = 0; i < hash_context->result_size; ++i)
    {
        V[i] = 0x01;
        K[i] = 0;
    }

    /* K = HMAC_K(V || 0x00 || int2octets(x) || h(m)) */
    HMAC_init(hash_context, K);
    V[hash_context->result_size] = 0x00;
    HMAC_update(hash_context, V, hash_context->result_size + 1);
    HMAC_update(hash_context, prikey.data(), num_bytes);
    HMAC_update(hash_context, message_hash, hash_size);
    HMAC_finish(hash_context, K, K);

    update_V(hash_context, K, V);

    /* K = HMAC_K(V || 0x01 || int2octets(x) || h(m)) */
    HMAC_init(hash_context, K);
    V[hash_context->result_size] = 0x01;
    HMAC_update(hash_context, V, hash_context->result_size + 1);
    HMAC_update(hash_context, prikey.data(), num_bytes);
    HMAC_update(hash_context, message_hash, hash_size);
    HMAC_finish(hash_context, K, K);

    update_V(hash_context, K, V);

    for (tries = 0; tries < CryptoRNGMaxTries; ++tries)
    {
        word_t T[CryptoMaxWord];
        uint8 *T_ptr = (uint8 *)T;
        int8 T_bytes = 0;
        for (;;)
        {
            update_V(hash_context, K, V);
            for (i = 0; i < hash_context->result_size; ++i)
            {
                T_ptr[T_bytes++] = V[i];
                if (T_bytes >= num_n_words * CryptoMaxWord)
                    goto filled;
            }
        }
    filled:
        if ((int16_t)num_n_words * CryptoWordSize * 8 > num_n_bits)
        {
            word_t mask = (word_t)-1;
            T[num_n_words - 1] &=
                mask >> ((int16_t)(num_n_words * CryptoWordSize * 8 - num_n_bits));
        }

        if (sign_with_k(prikey.data(), message_hash, hash_size, T, signature, priv))
            return true;

        /* K = HMAC_K(V || 0x00) */
        HMAC_init(hash_context, K);
        V[hash_context->result_size] = 0x00;
        HMAC_update(hash_context, V, hash_context->result_size + 1);
        HMAC_finish(hash_context, K, K);

        update_V(hash_context, K, V);
    }
    return false;
}

static int16_t smax(int16_t a, int16_t b)
{
    return (a > b ? a : b);
}

bool crypto_ecc::sign_verify(const uint8 *message_hash, unsigned hash_size,
                             const uint8 *signature)
{
    word_t u1[CryptoMaxWord], u2[CryptoMaxWord];
    word_t z[CryptoMaxWord];
    word_t sum[CryptoMaxWord * 2];
    word_t rx[CryptoMaxWord];
    word_t ry[CryptoMaxWord];
    word_t tx[CryptoMaxWord];
    word_t ty[CryptoMaxWord];
    word_t tz[CryptoMaxWord];
    const word_t *points[4];
    const word_t *point;
    int16_t num_bits;
    int16_t i;
#if CryptoNativeLittleEndian
    word_t *_public = pubkey.to<word_t>();
#else
    word_t _public[CryptoMaxWord * 2];
#endif
    word_t r[CryptoMaxWord], s[CryptoMaxWord];
    int8 num_words = priv->num_words;
    int8 num_n_words = BITS_TO_WORDS(priv->num_n_bits);

    rx[num_n_words - 1] = 0;
    r[num_n_words - 1] = 0;
    s[num_n_words - 1] = 0;

#if CryptoNativeLittleEndian
    bcopy((uint8 *) r, signature, priv->num_bytes);
    bcopy((uint8 *) s, signature + priv->num_bytes, priv->num_bytes);
#else
    vli_bytesToNative(_public, pubkey.data(), priv->num_bytes);
    vli_bytesToNative(
        _public + num_words, pubkey.data() + priv->num_bytes, priv->num_bytes);
    vli_bytesToNative(r, signature, priv->num_bytes);
    vli_bytesToNative(s, signature + priv->num_bytes, priv->num_bytes);
#endif

    /* r, s must not be 0. */
    if (vli_isZero(r, num_words) || vli_isZero(s, num_words))
        return false;

    /* r, s must be < n. */
    if (vli_cmp_unsafe(priv->n, r, num_n_words) != 1 ||
            vli_cmp_unsafe(priv->n, s, num_n_words) != 1)
        return false;

    /* Calculate u1 and u2. */
    vli_modInv(z, s, priv->n, num_n_words); /* z = 1/s */
    u1[num_n_words - 1] = 0;
    bits2int(u1, message_hash, hash_size, priv);
    vli_modMult(u1, u1, z, priv->n, num_n_words); /* u1 = e/s */
    vli_modMult(u2, r, z, priv->n, num_n_words); /* u2 = r/s */

    /* Calculate sum = G + Q. */
    vli_set(sum, _public, num_words);
    vli_set(sum + num_words, _public + num_words, num_words);
    vli_set(tx, priv->G, num_words);
    vli_set(ty, priv->G + num_words, num_words);
    vli_modSub(z, sum, tx, priv->p, num_words); /* z = x2 - x1 */
    XYcZ_add(tx, ty, sum, sum + num_words, priv);
    vli_modInv(z, z, priv->p, num_words); /* z = 1/z */
    apply_z(sum, sum + num_words, z, priv);

    /* Use Shamir's trick to calculate u1*G + u2*Q */
    points[0] = 0;
    points[1] = priv->G;
    points[2] = _public;
    points[3] = sum;
    num_bits = smax(vli_numBits(u1, num_n_words),
                    vli_numBits(u2, num_n_words));

    point = points[(!!vli_testBit(u1, num_bits - 1)) |
                   ((!!vli_testBit(u2, num_bits - 1)) << 1)];
    vli_set(rx, point, num_words);
    vli_set(ry, point + num_words, num_words);
    vli_clear(z, num_words);
    z[0] = 1;

    for (i = num_bits - 2; i >= 0; --i)
    {
        word_t index;
        priv->double_jacobian(rx, ry, z, priv);

        index = (!!vli_testBit(u1, i)) | ((!!vli_testBit(u2, i)) << 1);
        point = points[index];
        if (point) {
            vli_set(tx, point, num_words);
            vli_set(ty, point + num_words, num_words);
            apply_z(tx, ty, z, priv);
            vli_modSub(tz, rx, tx, priv->p, num_words); /* Z = x2 - x1 */
            XYcZ_add(tx, ty, rx, ry, priv);
            vli_modMult_fast(z, z, tz, priv);
        }
    }

    vli_modInv(z, z, priv->p, num_words); /* Z = 1/Z */
    apply_z(rx, ry, z, priv);

    /* v = x1 (mod n) */
    if (vli_cmp_unsafe(priv->n, rx, num_n_words) != 1)
        vli_sub(rx, rx, priv->n, num_n_words);

    /* Accept only if v == r. */
    return !!(vli_equal(rx, r, num_words));
}
