#include "ky_random.h"
#include "ky_datetime.h"

#define NSUM 25
static real inl_rand(real *r)
{
    real x = 0.0;
    if (!r)
    {
        uint64 v = ky_datetime::nanosec();
        x = *((real*)&v);
    }
    else
        x = *r;
    real base = 256.0;
    real a = 17.0;
    real b = 139.0;
    real temp1 = a * x + b;
    real temp2 = (int)(temp1 / base);
    real temp3 = temp1 - temp2 * base;
    x = temp3;
    return x / base;
}
real ky_random::rand(real seed)
{
    return inl_rand(&seed);
}
real ky_random::normality(real u, real t, real seed)
{
    real total = 0.0;
    for (int i = 0; i < NSUM; i++)
          total += inl_rand(&seed);
    total -= NSUM / 2.0;
    return u + t * total / ky_sqrt(NSUM / 12);
}
real ky_random::gauss(real seed)
{
     real x = 0;
     for(int32 i = 0; i < NSUM; i++)
         x += inl_rand(&seed) / RAND_MAX;

     x -= NSUM / 2.0;
     x /= ky_sqrt(NSUM / 12.0);
     return x;
}

real ky_random::box_muller(real seed)
{
    static real U, V;
    static int phase = 0;
    real z = 0.0;

    if(phase == 0)
    {
        U = inl_rand(&seed) / (RAND_MAX + 1.0);
        V = inl_rand(&seed) / (RAND_MAX + 1.0);
        z = ky_sqrt(-2.0 * ky_log(U))* ky_sin(2.0 * PI * V);
    }
    else
    {
        z = ky_sqrt(-2.0 * ky_log(U)) * ky_cos(2.0 * PI * V);
    }

    phase = 1 - phase;
    return z;
}
real ky_random::marsaglia(real seed)
{
    static real V1, V2, S;
    static int phase = 0;
    real X = 0.0;

    if(phase == 0)
    {
        do
        {
            real U1 = inl_rand(&seed) / RAND_MAX;
            real U2 = inl_rand(&seed) / RAND_MAX;

            V1 = 2 * U1 - 1;
            V2 = 2 * U2 - 1;
            S  = V1 * V1 + V2 * V2;
        }
        while( S >= 1 || S ==0);

        X = V1 * ky_sqrt (-2 * ky_log(S) / S);
    }
    else
    {
        X = V2 * ky_sqrt(-2 * ky_log(S) / S);
    }

    phase = 1 - phase;
    return X;
}
