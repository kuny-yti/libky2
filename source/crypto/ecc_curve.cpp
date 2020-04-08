#include "ecc_curve.h"
#include "vli_math.h"

void vli_modMult_fast(word_t *result, const word_t *left, const word_t *right,
                      ecc_curve *curve)
{
    word_t product[2 * CryptoMaxWord];
    vli_mult(product, left, right, curve->num_words);
#if (CryptoOptimizatLevel > CO_LevelSystem)
    curve->mmod_fast(result, product);
#else
    vli_mmod(result, product, curve->p, curve->num_words);
#endif
}

#if CryptoSquareFunc
void vli_modSquare_fast(word_t *result, const word_t *left, ecc_curve* curve)
{
    word_t product[2 * CryptoMaxWord];
    vli_square(product, left, curve->num_words);
#if (CryptoOptimizatLevel > CO_LevelSystem)
    curve->mmod_fast(result, product);
#else
    vli_mmod(result, product, curve->p, curve->num_words);
#endif
}

#else
void vli_modSquare_fast(word_t *result, const word_t *left, ecc_curve* curve)
{
    vli_modMult_fast(result, left, left, curve);
}
#endif

/* ------ Point operations ------ */
#include "curve_specific.inl"


/* Point multiplication algorithm using Montgomery's ladder with co-Z coordinates.
From http://eprint.iacr.org/2011/338.pdf
*/

/* Modify (x1, y1) => (x1 * z^2, y1 * z^3) */
void apply_z(word_t * X1, word_t * Y1, const word_t * const Z,
                    ecc_curve* curve)
{
    word_t t1[CryptoMaxWord];

    vli_modSquare_fast(t1, Z, curve);    /* z^2 */
    vli_modMult_fast(X1, X1, t1, curve); /* x1 * z^2 */
    vli_modMult_fast(t1, t1, Z, curve);  /* z^3 */
    vli_modMult_fast(Y1, Y1, t1, curve); /* y1 * z^3 */
}

/* P = (x1, y1) => 2P, (x2, y2) => P' */
void XYcZ_initial_double(word_t * X1, word_t * Y1, word_t * X2, word_t * Y2,
                                const word_t * const initial_Z, ecc_curve* curve)
{
    word_t z[CryptoMaxWord];
    int8_t num_words = curve->num_words;
    if (initial_Z)
        vli_set(z, initial_Z, num_words);
     else
    {
        vli_clear(z, num_words);
        z[0] = 1;
    }

    vli_set(X2, X1, num_words);
    vli_set(Y2, Y1, num_words);

    apply_z(X1, Y1, z, curve);
    curve->double_jacobian(X1, Y1, z, curve);
    apply_z(X2, Y2, z, curve);
}

/* Input P = (x1, y1, Z), Q = (x2, y2, Z)
   Output P' = (x1', y1', Z3), P + Q = (x3, y3, Z3)
   or P => P', Q => P + Q
*/
void XYcZ_add(word_t * X1, word_t * Y1, word_t * X2, word_t * Y2,
                     ecc_curve* curve)
{
    /* t1 = X1, t2 = Y1, t3 = X2, t4 = Y2 */
    word_t t5[CryptoMaxWord];
    int8_t num_words = curve->num_words;

    vli_modSub(t5, X2, X1, curve->p, num_words); /* t5 = x2 - x1 */
    vli_modSquare_fast(t5, t5, curve);           /* t5 = (x2 - x1)^2 = A */
    vli_modMult_fast(X1, X1, t5, curve);         /* t1 = x1*A = B */
    vli_modMult_fast(X2, X2, t5, curve);         /* t3 = x2*A = C */
    vli_modSub(Y2, Y2, Y1, curve->p, num_words); /* t4 = y2 - y1 */
    vli_modSquare_fast(t5, Y2, curve);           /* t5 = (y2 - y1)^2 = D */

    vli_modSub(t5, t5, X1, curve->p, num_words); /* t5 = D - B */
    vli_modSub(t5, t5, X2, curve->p, num_words); /* t5 = D - B - C = x3 */
    vli_modSub(X2, X2, X1, curve->p, num_words); /* t3 = C - B */
    vli_modMult_fast(Y1, Y1, X2, curve);         /* t2 = y1*(C - B) */
    vli_modSub(X2, X1, t5, curve->p, num_words); /* t3 = B - x3 */
    vli_modMult_fast(Y2, Y2, X2, curve);         /* t4 = (y2 - y1)*(B - x3) */
    vli_modSub(Y2, Y2, Y1, curve->p, num_words); /* t4 = y3 */

    vli_set(X2, t5, num_words);
}

/* Input P = (x1, y1, Z), Q = (x2, y2, Z)
   Output P + Q = (x3, y3, Z3), P - Q = (x3', y3', Z3)
   or P => P - Q, Q => P + Q
*/
void XYcZ_addC(word_t * X1, word_t * Y1, word_t * X2, word_t * Y2,
                      ecc_curve* curve)
{
    /* t1 = X1, t2 = Y1, t3 = X2, t4 = Y2 */
    word_t t5[CryptoMaxWord];
    word_t t6[CryptoMaxWord];
    word_t t7[CryptoMaxWord];
    int8_t num_words = curve->num_words;

    vli_modSub(t5, X2, X1, curve->p, num_words); /* t5 = x2 - x1 */
    vli_modSquare_fast(t5, t5, curve);           /* t5 = (x2 - x1)^2 = A */
    vli_modMult_fast(X1, X1, t5, curve);         /* t1 = x1*A = B */
    vli_modMult_fast(X2, X2, t5, curve);         /* t3 = x2*A = C */
    vli_modAdd(t5, Y2, Y1, curve->p, num_words); /* t5 = y2 + y1 */
    vli_modSub(Y2, Y2, Y1, curve->p, num_words); /* t4 = y2 - y1 */

    vli_modSub(t6, X2, X1, curve->p, num_words); /* t6 = C - B */
    vli_modMult_fast(Y1, Y1, t6, curve);         /* t2 = y1 * (C - B) = E */
    vli_modAdd(t6, X1, X2, curve->p, num_words); /* t6 = B + C */
    vli_modSquare_fast(X2, Y2, curve);           /* t3 = (y2 - y1)^2 = D */
    vli_modSub(X2, X2, t6, curve->p, num_words); /* t3 = D - (B + C) = x3 */

    vli_modSub(t7, X1, X2, curve->p, num_words); /* t7 = B - x3 */
    vli_modMult_fast(Y2, Y2, t7, curve);         /* t4 = (y2 - y1)*(B - x3) */
    vli_modSub(Y2, Y2, Y1, curve->p, num_words); /* t4 = (y2 - y1)*(B - x3) - E = y3 */

    vli_modSquare_fast(t7, t5, curve);           /* t7 = (y2 + y1)^2 = F */
    vli_modSub(t7, t7, t6, curve->p, num_words); /* t7 = F - (B + C) = x3' */
    vli_modSub(t6, t7, X1, curve->p, num_words); /* t6 = x3' - B */
    vli_modMult_fast(t6, t6, t5, curve);         /* t6 = (y2+y1)*(x3' - B) */
    vli_modSub(Y1, t6, Y1, curve->p, num_words); /* t2 = (y2+y1)*(x3' - B) - E = y3' */

    vli_set(X1, t7, num_words);
}

/* result may overlap point. */
void EccPoint_mult(word_t * result, const word_t * point,
                          const word_t * scalar, const word_t * initial_Z,
                          int16_t num_bits, ecc_curve* curve)
{
    /* R0 and R1 */
    word_t Rx[2][CryptoMaxWord];
    word_t Ry[2][CryptoMaxWord];
    word_t z[CryptoMaxWord];
    int16_t i;
    word_t nb;
    int8_t num_words = curve->num_words;

    vli_set(Rx[1], point, num_words);
    vli_set(Ry[1], point + num_words, num_words);

    XYcZ_initial_double(Rx[1], Ry[1], Rx[0], Ry[0], initial_Z, curve);

    for (i = num_bits - 2; i > 0; --i)
    {
        nb = !vli_testBit(scalar, i);
        XYcZ_addC(Rx[1 - nb], Ry[1 - nb], Rx[nb], Ry[nb], curve);
        XYcZ_add(Rx[nb], Ry[nb], Rx[1 - nb], Ry[1 - nb], curve);
    }

    nb = !vli_testBit(scalar, 0);
    XYcZ_addC(Rx[1 - nb], Ry[1 - nb], Rx[nb], Ry[nb], curve);

    /* Find final 1/Z value. */
    vli_modSub(z, Rx[1], Rx[0], curve->p, num_words); /* X1 - X0 */
    vli_modMult_fast(z, z, Ry[1 - nb], curve);        /* Yb * (X1 - X0) */
    vli_modMult_fast(z, z, point, curve);             /* xP * Yb * (X1 - X0) */
    vli_modInv(z, z, curve->p, num_words);            /* 1 / (xP * Yb * (X1 - X0)) */
    /* yP / (xP * Yb * (X1 - X0)) */
    vli_modMult_fast(z, z, point + num_words, curve);
    vli_modMult_fast(z, z, Rx[1 - nb], curve); /* Xb * yP / (xP * Yb * (X1 - X0)) */
    /* End 1/Z calculation */

    XYcZ_add(Rx[nb], Ry[nb], Rx[1 - nb], Ry[1 - nb], curve);
    apply_z(Rx[0], Ry[0], z, curve);

    vli_set(result, Rx[0], num_words);
    vli_set(result + num_words, Ry[0], num_words);
}

word_t regularize_k(const word_t * const k, word_t *k0, word_t *k1,
                           ecc_curve* curve)
{
    int8_t num_n_words = BITS_TO_WORDS(curve->num_n_bits);
    int16_t num_n_bits = curve->num_n_bits;
    word_t carry = vli_add(k0, k, curve->n, num_n_words) ||
            (num_n_bits < ((int16_t)num_n_words * CryptoWordSize * 8) &&
             vli_testBit(k0, num_n_bits));
    vli_add(k1, k0, curve->n, num_n_words);
    return carry;
}

word_t EccPoint_compute_public_key(word_t *result, word_t *private_key,
                                          ecc_curve* curve)
{
    word_t tmp1[CryptoMaxWord];
    word_t tmp2[CryptoMaxWord];
    word_t *p2[2] = {tmp1, tmp2};
    word_t carry;

    /* Regularize the bitcount for the private key so that attackers cannot use a side channel
       attack to learn the number of leading zeros. */
    carry = regularize_k(private_key, tmp1, tmp2, curve);

    EccPoint_mult(result, curve->G, p2[!carry], 0, curve->num_n_bits + 1, curve);

    if (EccPoint_isZero(result, curve))
        return 0;
    return 1;
}


unsigned curve_num_words(ecc_curve * curve) {
    return curve->num_words;
}

unsigned curve_num_bytes(ecc_curve * curve) {
    return curve->num_bytes;
}

unsigned curve_num_bits(ecc_curve * curve) {
    return curve->num_bytes * 8;
}

unsigned curve_num_n_words(ecc_curve * curve) {
    return BITS_TO_WORDS(curve->num_n_bits);
}

unsigned curve_num_n_bytes(ecc_curve * curve) {
    return BITS_TO_BYTES(curve->num_n_bits);
}

unsigned curve_num_n_bits(ecc_curve * curve) {
    return curve->num_n_bits;
}

const word_t *curve_p(ecc_curve * curve) {
    return curve->p;
}

const word_t *curve_n(ecc_curve * curve) {
    return curve->n;
}

const word_t *curve_G(ecc_curve * curve) {
    return curve->G;
}

const word_t *curve_b(ecc_curve * curve) {
    return curve->b;
}

#if CryptoSupportCompressed
void vli_mod_sqrt(word_t *a, ecc_curve * curve)
{
    curve->mod_sqrt(a, curve);
}
#endif

void vli_mmod_fast(word_t *result, word_t *product, ecc_curve * curve)
{
#if (CryptoOptimizatLevel > CO_LevelSystem)
    curve->mmod_fast(result, product);
#else
    vli_mmod(result, product, curve->p, curve->num_words);
#endif
}

void point_mult(word_t *result, const word_t *point, const word_t *scalar,
                ecc_curve * curve)
{
    word_t tmp1[CryptoMaxWord];
    word_t tmp2[CryptoMaxWord];
    word_t *p2[2] = {tmp1, tmp2};
    word_t carry = regularize_k(scalar, tmp1, tmp2, curve);

    EccPoint_mult(result, point, p2[!carry], 0, curve->num_n_bits + 1, curve);
}

bool valid_point(const word_t *point, ecc_curve * curve)
{
    word_t tmp1[CryptoMaxWord];
    word_t tmp2[CryptoMaxWord];
    int8_t num_words = curve->num_words;

    /* The point at infinity is invalid. */
    if (EccPoint_isZero(point, curve))
        return false;

    /* x and y must be smaller than p. */
    if (vli_cmp_unsafe(curve->p, point, num_words) != 1 ||
            vli_cmp_unsafe(curve->p, point + num_words, num_words) != 1)
        return false;

    vli_modSquare_fast(tmp1, point + num_words, curve);
    curve->x_side(tmp2, point, curve); /* tmp2 = x^3 + ax + b */

    /* Make sure that y^2 == x^3 + ax + b */
    return !!(vli_equal(tmp1, tmp2, num_words));
}
