#ifndef ECC_CURVE_H
#define ECC_CURVE_H
#include "crypto.h"

struct ecc_curve
{
    int8_t num_words;
    int8_t num_bytes;
    int16_t num_n_bits;
    word_t p[CryptoMaxWord];
    word_t n[CryptoMaxWord];
    word_t G[CryptoMaxWord * 2];
    word_t b[CryptoMaxWord];

    void (*double_jacobian)(word_t * X1, word_t * Y1, word_t * Z1,
                            ecc_curve *curve);

    void (*mod_sqrt)(word_t *a, ecc_curve *curve);

    void (*x_side)(word_t *result, const word_t *x, ecc_curve *curve);

    void (*mmod_fast)(word_t *result, word_t *product);

};

#if CryptoSupportSecp160R1
ecc_curve* ecc_secp160r1();
#endif
#if CryptoSupportSecp192R1
ecc_curve* ecc_secp192r1();
#endif
#if CryptoSupportSecp224R1
ecc_curve* ecc_secp224r1();
#endif
#if CryptoSupportSecp256R1
ecc_curve* ecc_secp256r1();
#endif
#if CryptoSupportSecp256K1
ecc_curve* ecc_secp256k1();
#endif

/* Returns 1 if 'point' is the point at infinity, 0 otherwise. */
#define EccPoint_isZero(point, curve) vli_isZero((point), (curve)->num_words * 2)

/* Calculates result = product (mod curve->p), where product is up to
   2 * curve->num_words long. */
void vli_mmod_fast(word_t *result, word_t *product, struct ecc_curve* curve);

/* Computes result = (left * right) % curve->p. */
void vli_modMult_fast(word_t *result, const word_t *left, const word_t *right,
                      struct ecc_curve *curve);

/* Computes result = left^2 % curve->p. */
void vli_modSquare_fast(word_t *result, const word_t *left, struct ecc_curve *curve);

#if CryptoSupportCompressed
/* Calculates a = sqrt(a) (mod curve->p) */
void vli_mod_sqrt(word_t *a, struct ecc_curve *curve);
#endif

unsigned curve_num_words(struct ecc_curve * curve);
unsigned curve_num_bytes(struct ecc_curve * curve);
unsigned curve_num_bits(struct ecc_curve * curve);
unsigned curve_num_n_words(struct ecc_curve * curve);
unsigned curve_num_n_bytes(struct ecc_curve * curve);
unsigned curve_num_n_bits(struct ecc_curve * curve);

const word_t *curve_p(struct ecc_curve * curve);
const word_t *curve_n(struct ecc_curve * curve);
const word_t *curve_G(struct ecc_curve * curve);
const word_t *curve_b(struct ecc_curve * curve);

bool valid_point(const word_t *point, struct ecc_curve * curve);

/* Multiplies a point by a scalar. Points are represented by the X coordinate followed by
   the Y coordinate in the same array, both coordinates are curve->num_words long. Note
   that scalar must be curve->num_n_words long (NOT curve->num_words). */
void point_mult(word_t *result, const word_t *point, const word_t *scalar,
                struct ecc_curve * curve);

/* Modify (x1, y1) => (x1 * z^2, y1 * z^3) */
void apply_z(word_t * X1, word_t * Y1, const word_t * const Z,
                    ecc_curve* curve);
/* P = (x1, y1) => 2P, (x2, y2) => P' */
void XYcZ_initial_double(word_t * X1, word_t * Y1, word_t * X2, word_t * Y2,
                                const word_t * const initial_Z, ecc_curve* curve);

/* Input P = (x1, y1, Z), Q = (x2, y2, Z)
   Output P' = (x1', y1', Z3), P + Q = (x3, y3, Z3)
   or P => P', Q => P + Q
*/
void XYcZ_add(word_t * X1, word_t * Y1, word_t * X2, word_t * Y2,
                     ecc_curve* curve);

/* Input P = (x1, y1, Z), Q = (x2, y2, Z)
   Output P + Q = (x3, y3, Z3), P - Q = (x3', y3', Z3)
   or P => P - Q, Q => P + Q
*/
void XYcZ_addC(word_t * X1, word_t * Y1, word_t * X2, word_t * Y2,
                      ecc_curve* curve);

/* result may overlap point. */
void EccPoint_mult(word_t * result, const word_t * point,
                          const word_t * scalar, const word_t * initial_Z,
                          int16_t num_bits, ecc_curve* curve);

word_t regularize_k(const word_t * const k, word_t *k0, word_t *k1,
                           ecc_curve* curve);

word_t EccPoint_compute_public_key(word_t *result, word_t *private_key,
                                          ecc_curve* curve);
#endif // ECC_CURVE_H
