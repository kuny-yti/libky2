#include "crypto_rsa.h"

#include <cmath>
#include <time.h>
#include <stdio.h>

static int64 gcd(int64 a, int64 b)
{
    int64 c;
    while ( a != 0 )
    {
        c = a;
        a = b % a;
        b = c;
    }
    return b;
}

static int64 ExtEuclid(int64 a, int64 b)
{
    int64 x = 0, y = 1, u = 1, v = 0, gcd = b, m, n, q, r;
    while (a != 0)
    {
        q = gcd / a;
        r = gcd % a;

        m = x - u * q;
        n = y - v * q;

        gcd = a;

        a = r;
        x = u;
        y = v;
        u = m;
        v = n;
    }
    return y;
}

static int64 rsa_modExp(int64 b, int64 e, int64 m)
{
    if (b < 0 || e < 0 || m <= 0)
        return -1;

    b = b % m;
    if (e == 0)
        return 1;
    if (e == 1)
        return b;
    if ( e % 2 == 0)
        return (rsa_modExp (b * b % m, e/2, m) % m );

    if( e % 2 == 1)
        return ( b * rsa_modExp(b, (e-1), m) % m );
    return 0;
}

static bool IsPrime(int num)
{
    if(num < 2)
        return false;
    if(num == 2)
        return true;

    if(num % 2 == 0)
        return false;

    for(int i = 3; i <= ::sqrt(num); ++i)
    {
        if(num % i == 0)
            return false;
    }
    return true;
}
static int64* GenPrime(int64 count, int base)
{
    static int64* prime_table = 0;
    static int64  table_count = 0;
    if (table_count != count)
    {
        if (prime_table)
            ::free(prime_table);

        //srand(time(NULL));
        prime_table = (int64*)::kyMalloc(count * sizeof(int64));
        table_count = count;
    }

    //const int base = (rand() % 10000); // 10000内原因是linux64位时出错
    const int bx = base % 10000;
    for (int64 i = 0, inx = 0; inx < count; ++i)
    {
        if(IsPrime(bx + i))
            prime_table[inx++] = bx + i;
    }
    return prime_table;
}

crypto_rsa::crypto_rsa(const int prime_base, const int64 prime_count)
{
    count = prime_count;
    primes = 0;
    primes = GenPrime(count, prime_base);
}
crypto_rsa::~crypto_rsa()
{
    if (primes)
        kyFree(primes);
}

void crypto_rsa::set_public(const key &pp)
{
    pubkey = pp;

}
void crypto_rsa::set_private(const key &pp)
{
    prikey = pp;
}

crypto_rsa::key crypto_rsa::key_public()const
{
    return pubkey;
}
crypto_rsa::key crypto_rsa::key_private()const
{
    return prikey;
}

void crypto_rsa::generate()
{
    int64 p = 0;
    int64 q = 0;

    int64 e = powl(2, 8) + 1;
    int64 d = 0;
    int64 max = 0;
    int64 phi_max = 0;

    srand(time(NULL));

    do{
        int a = (double)rand() * (count +1) / (RAND_MAX +1.0);
        int b = (double)rand() * (count +1) / (RAND_MAX +1.0);

        p = primes [a];
        q = primes [b];

        max = p * q;
        phi_max = (p -1) * (q -1);
    }
    while(!(p && q) || (p == q) || (gcd(phi_max, e) != 1));

    d = ExtEuclid(phi_max, e);
    while(d < 0)
        d = d + phi_max;

    pubkey.modulus() = max;
    pubkey.exponent() = e;

    prikey.modulus() = max;
    prikey.exponent() = d;
}


ky_byte crypto_rsa::encrypt(const ky_byte &cleartext)
{
    ky_byte encrypted;
    encrypted.resize(sizeof(int64) * (cleartext.size()));

    for(int32 i = 0; i < cleartext.size(); i++)
    {
        int64 cp = rsa_modExp(cleartext[i], pubkey.exponent(), pubkey.modulus());
        uint8 *tmp = (uint8*)(&cp);
        for (int8 x = 0; x < sizeof(int64); ++x)
            encrypted[(int32)(i * sizeof(int64)) + x] = tmp[x];
    }

    return encrypted;
}

ky_byte crypto_rsa::decrypt(const ky_byte &ciphertext)
{
    if (ciphertext.size() % sizeof(int64))
        return ky_byte();

    ky_byte decrypted;
    decrypted.resize(ciphertext.size() / sizeof(int64));

    for (int32 i = 0; i < ciphertext.size() / sizeof(int64); i++)
    {
        int64 cp = ((int64*)ciphertext.data())[i];
        decrypted[i] = (uint8)rsa_modExp(cp, prikey.exponent(), prikey.modulus());
    }

    return decrypted;
}
