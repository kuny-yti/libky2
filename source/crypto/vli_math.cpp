#include "vli_math.h"


#if default_RNG_defined
static CryptoECC_RNG g_rng_function = &default_RNG;
#else
static CryptoECC_RNG g_rng_function = 0;
#endif

/* Counts the number of words in vli. */
static int8_t _vli_numDigits(const word_t *vli, const int8_t max_words)
{
    int8_t i = 0;
    /* Search from the end until we find a non-zero digit.
       We do it in reverse because we expect that most digits will be nonzero. */
    for (i = max_words - 1; i >= 0 && vli[i] == 0; --i) ;

    return (i + 1);
}

#if !asm_mult || (CryptoSquareFunc && !asm_square) || \
    (CryptoSupportSecp256K1 &&  \
        ((CryptoWordSize == 1) || (CryptoWordSize == 8)))

void vli_muladd(word_t a, word_t b, word_t *r0, word_t *r1, word_t *r2)
{
#if CryptoWordSize == 8 && !CryptoSupportINT128
    uint64_t a0 = a & 0xffffffffull;
    uint64_t a1 = a >> 32;
    uint64_t b0 = b & 0xffffffffull;
    uint64_t b1 = b >> 32;

    uint64_t i0 = a0 * b0;
    uint64_t i1 = a0 * b1;
    uint64_t i2 = a1 * b0;
    uint64_t i3 = a1 * b1;

    uint64_t p0, p1;

    i2 += (i0 >> 32);
    i2 += i1;
    if (i2 < i1)  /* overflow */
        i3 += 0x100000000ull;

    p0 = (i0 & 0xffffffffull) | (i2 << 32);
    p1 = i3 + (i2 >> 32);

    *r0 += p0;
    *r1 += (p1 + (*r0 < p0));
    *r2 += ((*r1 < p1) || (*r1 == p1 && *r0 < p0));
#else
    dword_t p = (dword_t)a * b;
    dword_t r01 = ((dword_t)(*r1) << WORD_BITS) | *r0;
    r01 += p;
    *r2 += (r01 < p);
    *r1 = r01 >> WORD_BITS;
    *r0 = (word_t)r01;
#endif
}
#endif /* muladd needed */


#if CryptoNativeLittleEndian
void bcopy(uint8_t *dst,
                  const uint8_t *src,
                  unsigned num_bytes)
{
    while (0 != num_bytes)
    {
        num_bytes--;
        dst[num_bytes] = src[num_bytes];
    }
}
#endif

word_t vli_sub(word_t *result, const word_t *left, const word_t *right,
               int8_t num_words);

#if !asm_clear
void vli_clear(word_t *vli, int8_t num_words)
{
    for (int8_t i = 0; i < num_words; ++i)
        vli[i] = 0;
}
#endif /* !asm_clear */

/* Constant-time comparison to zero - secure way to compare long integers */
/* Returns 1 if vli == 0, 0 otherwise. */
word_t vli_isZero(const word_t *vli, int8_t num_words)
{
    word_t bits = 0;
    for (int8_t i = 0; i < num_words; ++i)
        bits |= vli[i];

    return (bits == 0);
}

/* Returns nonzero if bit 'bit' of vli is set. */
word_t vli_testBit(const word_t *vli, int16_t bit)
{
    return (vli[bit >> WORD_BITS_SHIFT] &
            ((word_t)1 << (bit & WORD_BITS_MASK)));
}

/* Counts the number of bits required to represent vli. */
int16_t vli_numBits(const word_t *vli, const int8_t max_words)
{
    word_t i;

    int8_t num_digits = _vli_numDigits(vli, max_words);
    if (num_digits == 0)
        return 0;

    word_t digit = vli[num_digits - 1];
    for (i = 0; digit; ++i)
        digit >>= 1;

    return (((int16_t)(num_digits - 1) << WORD_BITS_SHIFT) + i);
}

/* Sets dest = src. */
#if !asm_set
void vli_set(word_t *dest, const word_t *src, int8_t num_words)
{
    for (int8_t i = 0; i < num_words; ++i)
        dest[i] = src[i];
}
#endif /* !asm_set */

/* Constant-time comparison function - secure way to compare long integers */
/* Returns one if left == right, zero otherwise. */
word_t vli_equal(const word_t *left, const word_t *right,
                  int8_t num_words)
{
    word_t diff = 0;
    for (int8_t i = num_words - 1; i >= 0; --i)
        diff |= (left[i] ^ right[i]);

    return (diff == 0);
}

/* Returns sign of left - right, in constant time. */
int8_t vli_cmp(const word_t *left, const word_t *right, int8_t num_words)
{
    word_t tmp[CryptoMaxWord];
    word_t neg = !!vli_sub(tmp, left, right, num_words);
    word_t equal = vli_isZero(tmp, num_words);
    return (!equal - 2 * neg);
}

/* Computes vli = vli >> 1. */
#if !asm_rshift1
void vli_rshift1(word_t *vli, int8_t num_words)
{
    word_t *end = vli;
    word_t carry = 0;

    vli += num_words;
    while (vli-- > end)
    {
        word_t temp = *vli;
        *vli = (temp >> 1) | carry;
        carry = temp << (WORD_BITS - 1);
    }
}
#endif /* !asm_rshift1 */

/* Computes result = left + right, returning carry. Can modify in place. */
#if !asm_add
word_t vli_add(word_t *result, const word_t *left, const word_t *right,
               int8_t num_words)
{
    word_t carry = 0;
    for (int8_t i = 0; i < num_words; ++i)
    {
        word_t sum = left[i] + right[i] + carry;
        if (sum != left[i])
            carry = (sum < left[i]);
        result[i] = sum;
    }
    return carry;
}
#endif /* !asm_add */

/* Computes result = left - right, returning borrow. Can modify in place. */
#if !asm_sub
word_t vli_sub(word_t *result, const word_t *left, const word_t *right,
                    int8_t num_words)
{
    word_t borrow = 0;
    for (int8_t i = 0; i < num_words; ++i)
    {
        word_t diff = left[i] - right[i] - borrow;
        if (diff != left[i])
            borrow = (diff > left[i]);
        result[i] = diff;
    }
    return borrow;
}
#endif /* !asm_sub */

#if !asm_mult
void vli_mult(word_t *result, const word_t *left, const word_t *right,
               int8_t num_words)
{
    word_t r0 = 0;
    word_t r1 = 0;
    word_t r2 = 0;
    int8_t i, k;

    /* Compute each digit of result in sequence, maintaining the carries. */
    for (k = 0; k < num_words; ++k)
    {
        for (i = 0; i <= k; ++i)
            vli_muladd(left[i], right[k - i], &r0, &r1, &r2);

        result[k] = r0;
        r0 = r1;
        r1 = r2;
        r2 = 0;
    }
    for (k = num_words; k < num_words * 2 - 1; ++k)
    {
        for (i = (k + 1) - num_words; i < num_words; ++i)
            vli_muladd(left[i], right[k - i], &r0, &r1, &r2);

        result[k] = r0;
        r0 = r1;
        r1 = r2;
        r2 = 0;
    }
    result[num_words * 2 - 1] = r0;
}
#endif /* !asm_mult */

#if CryptoSquareFunc

#if !asm_square
static void _mul2add(word_t a,
                    word_t b,
                    word_t *r0,
                    word_t *r1,
                    word_t *r2) {
#if CryptoWordSize == 8 && !CryptoSupportINT128
    uint64_t a0 = a & 0xffffffffull;
    uint64_t a1 = a >> 32;
    uint64_t b0 = b & 0xffffffffull;
    uint64_t b1 = b >> 32;

    uint64_t i0 = a0 * b0;
    uint64_t i1 = a0 * b1;
    uint64_t i2 = a1 * b0;
    uint64_t i3 = a1 * b1;

    uint64_t p0, p1;

    i2 += (i0 >> 32);
    i2 += i1;
    if (i2 < i1)
    { /* overflow */
        i3 += 0x100000000ull;
    }

    p0 = (i0 & 0xffffffffull) | (i2 << 32);
    p1 = i3 + (i2 >> 32);

    *r2 += (p1 >> 63);
    p1 = (p1 << 1) | (p0 >> 63);
    p0 <<= 1;

    *r0 += p0;
    *r1 += (p1 + (*r0 < p0));
    *r2 += ((*r1 < p1) || (*r1 == p1 && *r0 < p0));
#else
    dword_t p = (dword_t)a * b;
    dword_t r01 = ((dword_t)(*r1) << WORD_BITS) | *r0;
    *r2 += (p >> (WORD_BITS * 2 - 1));
    p *= 2;
    r01 += p;
    *r2 += (r01 < p);
    *r1 = r01 >> WORD_BITS;
    *r0 = (word_t)r01;
#endif
}

void vli_square(word_t *result, const word_t *left, int8_t num_words)
{
    word_t r0 = 0;
    word_t r1 = 0;
    word_t r2 = 0;

    int8_t i, k;

    for (k = 0; k < num_words * 2 - 1; ++k)
    {
        word_t min = (k < num_words ? 0 : (k + 1) - num_words);
        for (i = min; i <= k && i <= k - i; ++i)
        {
            if (i < k-i)
                _mul2add(left[i], left[k - i], &r0, &r1, &r2);
             else
                vli_muladd(left[i], left[k - i], &r0, &r1, &r2);

        }
        result[k] = r0;
        r0 = r1;
        r1 = r2;
        r2 = 0;
    }

    result[num_words * 2 - 1] = r0;
}
#endif /* !asm_square */

#else

void vli_square(word_t *result, const word_t *left, int8_t num_words)
{
    vli_mult(result, left, left, num_words);
}

#endif

/* Computes result = (left + right) % mod.
   Assumes that left < mod and right < mod, and that result does not overlap mod. */
void vli_modAdd(word_t *result, const word_t *left, const word_t *right,
                 const word_t *mod, int8_t num_words)
{
    word_t carry = vli_add(result, left, right, num_words);
    if (carry ||vli_cmp_unsafe(mod, result, num_words) != 1)
        /* result > mod (result = mod + remainder), so subtract mod to get remainder. */
        vli_sub(result, result, mod, num_words);
}

/* Computes result = (left - right) % mod.
   Assumes that left < mod and right < mod, and that result does not overlap mod. */
void vli_modSub(word_t *result, const word_t *left, const word_t *right,
                 const word_t *mod, int8_t num_words)
{
    word_t l_borrow = vli_sub(result, left, right, num_words);
    if (l_borrow) {
        /* In this case, result == -diff == (max int) - diff. Since -x % d == d - x,
           we can get the correct result from result + mod (with overflow). */
        vli_add(result, result, mod, num_words);
    }
}

/* Computes result = product % mod, where product is 2N words long. */
/* Currently only designed to work for curve_p or curve_n. */
void vli_mmod(word_t *result, word_t *product, const word_t *mod,
               int8_t num_words)
{
    word_t mod_multiple[2 * CryptoMaxWord];
    word_t tmp[2 * CryptoMaxWord];
    word_t *v[2] = {tmp, product};
    word_t index;

    /* Shift mod so its highest set bit is at the maximum position. */
    int16_t shift = (num_words * 2 * WORD_BITS) - vli_numBits(mod, num_words);
    int8_t word_shift = shift / WORD_BITS;
    int8_t bit_shift = shift % WORD_BITS;
    word_t carry = 0;
    vli_clear(mod_multiple, word_shift);
    if (bit_shift > 0)
    {
        for(index = 0; index < (word_t)num_words; ++index)
        {
            mod_multiple[word_shift + index] = (mod[index] << bit_shift) | carry;
            carry = mod[index] >> (WORD_BITS - bit_shift);
        }
    }
    else
        vli_set(mod_multiple + word_shift, mod, num_words);


    for (index = 1; shift >= 0; --shift)
    {
        word_t borrow = 0;
        for (int8_t i = 0; i < num_words * 2; ++i)
        {
            word_t diff = v[index][i] - mod_multiple[i] - borrow;
            if (diff != v[index][i])
                borrow = (diff > v[index][i]);

            v[1 - index][i] = diff;
        }
        index = !(index ^ borrow); /* Swap the index if there was no borrow */
        vli_rshift1(mod_multiple, num_words);
        mod_multiple[num_words - 1] |= mod_multiple[num_words] << (WORD_BITS - 1);
        vli_rshift1(mod_multiple + num_words, num_words);
    }
    vli_set(result, v[index], num_words);
}

/* Computes result = (left * right) % mod. */
void vli_modMult(word_t *result, const word_t *left, const word_t *right,
                  const word_t *mod, int8_t num_words)
{
    word_t product[2 * CryptoMaxWord];
    vli_mult(product, left, right, num_words);
    vli_mmod(result, product, mod, num_words);
}

#if CryptoSquareFunc

/* Computes result = left^2 % mod. */
void vli_modSquare(word_t *result, const word_t *left, const word_t *mod,
                    int8_t num_words)
{
    word_t product[2 * CryptoMaxWord];
    vli_square(product, left, num_words);
    vli_mmod(result, product, mod, num_words);
}

#else

void vli_modSquare(word_t *result, const word_t *left, const word_t *mod,
                        int8_t num_words)
{
    vli_modMult(result, left, left, mod, num_words);
}

#endif

#define EVEN(vli) (!(vli[0] & 1))
static void vli_modInv_update(word_t *uv, const word_t *mod, int8_t num_words)
{
    word_t carry = 0;
    if (!EVEN(uv))
        carry = vli_add(uv, uv, mod, num_words);

    vli_rshift1(uv, num_words);
    if (carry)
        uv[num_words - 1] |= HIGH_BIT_SET;
}

/* Computes result = (1 / input) % mod. All VLIs are the same size.
   See "From Euclid's GCD to Montgomery Multiplication to the Great Divide" */
void vli_modInv(word_t *result, const word_t *input, const word_t *mod,
                int8_t num_words)
{
    word_t  a[CryptoMaxWord],
            b[CryptoMaxWord],
            u[CryptoMaxWord],
            v[CryptoMaxWord];
    int8_t cmpResult;

    if (vli_isZero(input, num_words))
    {
        vli_clear(result, num_words);
        return;
    }

    vli_set(a, input, num_words);
    vli_set(b, mod, num_words);
    vli_clear(u, num_words);
    u[0] = 1;
    vli_clear(v, num_words);
    while ((cmpResult = vli_cmp_unsafe(a, b, num_words)) != 0)
    {
        if (EVEN(a))
        {
            vli_rshift1(a, num_words);
            vli_modInv_update(u, mod, num_words);
        }
        else if (EVEN(b))
        {
            vli_rshift1(b, num_words);
            vli_modInv_update(v, mod, num_words);
        }
        else if (cmpResult > 0)
        {
            vli_sub(a, a, b, num_words);
            vli_rshift1(a, num_words);
            if (vli_cmp_unsafe(u, v, num_words) < 0)
                vli_add(u, u, mod, num_words);

            vli_sub(u, u, v, num_words);
            vli_modInv_update(u, mod, num_words);
        }
        else
        {
            vli_sub(b, b, a, num_words);
            vli_rshift1(b, num_words);
            if (vli_cmp_unsafe(v, u, num_words) < 0)
                vli_add(v, v, mod, num_words);

            vli_sub(v, v, u, num_words);
            vli_modInv_update(v, mod, num_words);
        }
    }
    vli_set(result, u, num_words);
}


#if CryptoWordSize == 1

void vli_nativeToBytes(uint8_t *bytes, int num_bytes, const uint8_t *native)
{
    for (int8_t i = 0; i < num_bytes; ++i)
        bytes[i] = native[(num_bytes - 1) - i];
}

void vli_bytesToNative(uint8_t *native, const uint8_t *bytes, int num_bytes)
{
    vli_nativeToBytes(native, num_bytes, bytes);
}

#else

void vli_nativeToBytes(uint8_t *bytes, int num_bytes, const word_t *native)
{
    for (int8_t i = 0; i < num_bytes; ++i)
    {
        unsigned b = num_bytes - 1 - i;
        bytes[i] = native[b / CryptoWordSize] >> (8 * (b % CryptoWordSize));
    }
}

void vli_bytesToNative(word_t *native, const uint8_t *bytes, int num_bytes)
{
    vli_clear(native, (num_bytes + (CryptoWordSize - 1)) / CryptoWordSize);
    for (int8_t i = 0; i < num_bytes; ++i)
    {
        unsigned b = num_bytes - 1 - i;
        native[b / CryptoWordSize] |=
            (word_t)bytes[i] << (8 * (b % CryptoWordSize));
    }
}
#endif


/* Returns sign of left - right. */
int8_t vli_cmp_unsafe(const word_t *left, const word_t *right, int8_t num_words)
{
    for (int8_t i = num_words - 1; i >= 0; --i)
    {
        if (left[i] > right[i])
            return 1;
         else if (left[i] < right[i])
            return -1;
    }
    return 0;
}


void vli_set_rng(CryptoECC_RNG rng_function)
{
    g_rng_function = rng_function;
}

CryptoECC_RNG vli_get_rng(void)
{
    return g_rng_function;
}

/* Generates a random integer in the range 0 < random < top.
   Both random and top have num_words words. */
bool generate_random_int(word_t *random, const word_t *top, int8_t num_words)
{
    word_t mask = (word_t)-1;
    word_t tries;
    uint16_t num_bits = vli_numBits(top, num_words);

    if (!g_rng_function)
        return false;

    for (tries = 0; tries < CryptoRNGMaxTries; ++tries)
    {
        if (!g_rng_function((uint8_t *)random, num_words * CryptoWordSize))
            return false;

        random[num_words - 1] &= mask >> ((uint16_t)(num_words * CryptoWordSize *
                                                     8 - num_bits));
        if (!vli_isZero(random, num_words) &&
                vli_cmp(top, random, num_words) == 1)
            return true;
    }
    return false;
}
