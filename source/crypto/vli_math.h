#ifndef VLI_MATH_H
#define VLI_MATH_H

#include "crypto.h"

//!
typedef int (*CryptoECC_RNG)(uint8_t *dest, unsigned size);


void vli_muladd(word_t a, word_t b, word_t *r0, word_t *r1, word_t *r2);

#if CryptoNativeLittleEndian
void bcopy(uint8_t *dst, const uint8_t *src, unsigned num_bytes);
#endif
//!
void vli_clear(word_t *vli, int8_t num_words);

/* Constant-time comparison to zero - secure way to compare long integers */
/* Returns 1 if vli == 0, 0 otherwise. */
word_t vli_isZero(const word_t *vli, int8_t num_words);

/* Returns nonzero if bit 'bit' of vli is set. */
word_t vli_testBit(const word_t *vli, int16_t bit);

/* Counts the number of bits required to represent vli. */
int16_t vli_numBits(const word_t *vli, const int8_t max_words);

/* Sets dest = src. */
void vli_set(word_t *dest, const word_t *src, int8_t num_words);

/* Constant-time comparison function - secure way to compare long integers */
/* Returns one if left == right, zero otherwise */
word_t vli_equal(const word_t *left, const word_t *right, int8_t num_words);

/* Constant-time comparison function - secure way to compare long integers */
/* Returns sign of left - right, in constant time. */
int8_t vli_cmp(const word_t *left, const word_t *right, int8_t num_words);

/* Computes vli = vli >> 1. */
void vli_rshift1(word_t *vli, int8_t num_words);

/* Computes result = left + right, returning carry. Can modify in place. */
word_t vli_add(word_t *result, const word_t *left, const word_t *right,
               int8_t num_words);

/* Computes result = left - right, returning borrow. Can modify in place. */
word_t vli_sub(word_t *result, const word_t *left, const word_t *right,
               int8_t num_words);

/* Computes result = left * right. Result must be 2 * num_words long. */
void vli_mult(word_t *result, const word_t *left, const word_t *right,
              int8_t num_words);

/* Computes result = left^2. Result must be 2 * num_words long. */
void vli_square(word_t *result, const word_t *left, int8_t num_words);

/* Computes result = (left + right) % mod.
   Assumes that left < mod and right < mod, and that result does not overlap mod. */
void vli_modAdd(word_t *result, const word_t *left, const word_t *right,
                const word_t *mod, int8_t num_words);

/* Computes result = (left - right) % mod.
   Assumes that left < mod and right < mod, and that result does not overlap mod. */
void vli_modSub(word_t *result, const word_t *left, const word_t *right,
                const word_t *mod, int8_t num_words);

/* Computes result = product % mod, where product is 2N words long.
   Currently only designed to work for mod == curve->p or curve_n. */
void vli_mmod(word_t *result, word_t *product, const word_t *mod,
              int8_t num_words);

/* Computes result = (left * right) % mod.
   Currently only designed to work for mod == curve->p or curve_n. */
void vli_modMult(word_t *result, const word_t *left, const word_t *right,
                 const word_t *mod, int8_t num_words);

/* Computes result = left^2 % mod.
   Currently only designed to work for mod == curve->p or curve_n. */
void vli_modSquare(word_t *result, const word_t *left, const word_t *mod,
                   int8_t num_words);

/* Computes result = (1 / input) % mod.*/
void vli_modInv(word_t *result, const word_t *input, const word_t *mod,
                int8_t num_words);


/* Converts an integer in uECC native format to big-endian bytes. */
void vli_nativeToBytes(uint8_t *bytes, int num_bytes, const word_t *native);
/* Converts big-endian bytes to an integer in uECC native format. */
void vli_bytesToNative(word_t *native, const uint8_t *bytes, int num_bytes);

/* Returns sign of left - right. */
int8_t vli_cmp_unsafe(const word_t *left, const word_t *right, int8_t num_words);


void vli_set_rng(CryptoECC_RNG rng_function);

CryptoECC_RNG vli_get_rng(void);

/* Generates a random integer in the range 0 < random < top.
   Both random and top have num_words words. */
bool generate_random_int(word_t *random, const word_t *top,
                         int8_t num_words);

#endif // VLI_MATH_H
