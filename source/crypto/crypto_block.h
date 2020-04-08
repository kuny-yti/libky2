#ifndef CRYPTO_BLOCK_H
#define CRYPTO_BLOCK_H

#include "crypto/ky_crypto.h"
#include "crypto.h"

#include "crypto_aes.h"
#include "crypto_blowfish.h"
#include "crypto_rc4.h"
#include "crypto_rc6.h"
#include "crypto_rsa.h"

struct crypto_block
{
    virtual ~crypto_block(){}
    //!
    //! \brief Electronic codebook encrypt
    //!             cleartext
    //!                 |
    //! key -- block cipher encrypt  ... ...
    //!                 |
    //!             ciphertext
    ky_byte encrypt_ecb(const ky_byte &cleartext);
    //!
    //! \brief Electronic codebook decrypt
    //!             ciphertext
    //!                 |
    //! key -- block cipher decrypt  ... ...
    //!                 |
    //!             cleartext
    ky_byte decrypt_ecb(const ky_byte &ciphertext);

    //!
    //! \brief Cipher-block chaining encrypt
    //!                cleartext                  cleartext
    //!                    |                          |
    //!    IV(iv) ------- xor              |-------- xor
    //!                    |               |          |
    //!  key ----- block cipher encrypt    |  block cipher encrypt  ... ...
    //!                    |---------------|          |-------------
    //!                    |                          |
    //!               ciphertext                  ciphertext
    ky_byte encrypt_cbc(const ky_byte &cleartext, const uint8* iv);
    //!
    //! \brief Cipher-block chaining decrypt
    //!                ciphertext                  ciphertext
    //!                     |------------|             |------------
    //!                     |            |             |
    //!  key ---- block cipher decrypt   |    block cipher decrypt  ... ...
    //!                     |            |             |
    //!    IV(iv) -------- xor           |----------- xor
    //!                     |                          |
    //!                 cleartext                  cleartext
    ky_byte decrypt_cbc(const ky_byte &ciphertext, const uint8* iv);

    //!
    //! \brief Cipher feedback encrypt
    //!                IV(iv)         |-----------------|
    //!                  |            |                 |
    //!  key --- block cipher encrypt | key --- block cipher encrypt  ... ...
    //!                  |            |                 |
    //!  cleartext ---- xor           | cleartext ---- xor
    //!                  |------------|                 |-------------
    //!                  |                              |
    //!             ciphertext                     ciphertext
    ky_byte encrypt_cfb(const ky_byte &cleartext, const uint8* iv);
    //!
    //! \brief Cipher feedback decrypt
    //!                IV(iv)         |-----------------|             |----
    //!                  |            |                 |             |
    //!  key --- block cipher encrypt | key --- block cipher encrypt  |     ... ...
    //!                  |            |                 |             |
    //!                 xor ----- ciphertext           xor ----- ciphertext
    //!                  |                              |
    //!              cleartext                      cleartext
    ky_byte decrypt_cfb(const ky_byte &ciphertext, const uint8* iv);

    //!
    //! \brief Output feedback encrypt
    //!                IV(iv)         |-----------------|
    //!                  |            |                 |
    //!  key --- block cipher encrypt | key --- block cipher encrypt  ... ...
    //!                  |------------|                 |-------------
    //!  cleartext ---- xor              cleartext --- xor
    //!                  |                              |
    //!              ciphertext                     ciphertext
    ky_byte encrypt_ofb(const ky_byte &cleartext, const uint8* iv);
    //!
    //! \brief Output feedback decrypt
    //!                IV(iv)         |-----------------|
    //!                  |            |                 |
    //!  key --- block cipher encrypt | key --- block cipher encrypt  ... ...
    //!                  |------------|                 |-------------
    //!  ciphertext --- xor              ciphertext -- xor
    //!                  |                              |
    //!              cleartext                     cleartext
    ky_byte decrypt_ofb(const ky_byte &ciphertext, const uint8* iv);

    //!
    //! \brief Counter mode encrypt
    //!                IV(iv)(var)                   IV(iv)(var++)
    //!                  |                             |
    //!  key --- block cipher encrypt  key --- block cipher encrypt  ... ...
    //!                  |                             |
    //!  cleartext ---- xor            cleartext ---- xor
    //!                  |                             |
    //!             ciphertext                     ciphertext
    ky_byte encrypt_ctr(const ky_byte &cleartext, const uint8* iv);
    ky_byte decrypt_ctr(const ky_byte &ciphertext, const uint8* iv);

    uint8                 byte;
    icrypto              *priv;
};


#endif // CRYPTO_AES_H
