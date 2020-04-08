#include "ky_algorlthm.h"

namespace __hash_
{
    // RS Hash Function
    u32 RS (char *str, i64 len)
    {
        u32 b = 378551 ,a =63689 ;
        u32 hash = 0 ;

        for (i64 i = 0; i < len; ++i)
        {
            hash = hash * a + str[i];
            a *= b;
        }

        return hash;
    }
    // JS Hash Function
    u32 JS(char *str, i64 len)
    {
        u32 hash = 1315423911 ;

        for (i64 i = 0; i < len; ++i)
            hash ^= ((hash << 5) + (str[i]) + (hash >> 2));

        return hash;
    }
    // P. J. Weinberger Hash Function
    u32 PJW(char *str, i64 len)
    {
        u32 BitsInUnignedInt = (u32)(sizeof(uint) *8);
        u32 ThreeQuarters = (u32)((BitsInUnignedInt *3) /4);
        u32 OneEighth = (u32)(BitsInUnignedInt /8);
        u32 HighBits = (u32)(0xFFFFFFFF) << (BitsInUnignedInt - OneEighth);
        u32 hash = 0 ;
        u32 test = 0 ;

        for (i64 i = 0; i < len; ++i)
        {
            hash = (hash << OneEighth) + (str[i]);
            if((test = hash & HighBits) != 0)
                hash = ((hash ^ (test >> ThreeQuarters)) & (~HighBits));
        }

        return hash;
    }
    // ELF Hash Function
    u32 ELF(char *str, i64 len)
    {
        u32 hash = 0 ;
        u32 x = 0 ;

        for (i64 i = 0; i < len; ++i)
        {
            hash = (hash << 4) + str[i];
            if((x = hash & 0xF0000000L) != 0)
            {
                hash ^= (x >> 24);
                hash &= ~x ;
            }
        }

        return hash;
    }

    // BKDR Hash Function
    u32 BKDR(char *str, i64 len, uint seed)
    {
        //uint seed = 131 ;// 31 131 1313 13131 131313 etc..
        u32 hash = 0 ;

        for (i64 i = 0; i < len; ++i)
            hash = hash * seed + str[i];

        return hash;
    }

    // SDBM Hash Function
    u32 SDBM(char *str, i64 len)
    {
        u32 hash=0 ;

        for (i64 i = 0; i < len; ++i)
            hash = str[i] + (hash << 6) + (hash << 16) - hash ;

        return hash;
    }

    // DJB Hash Function
    u32 DJB(char *str, i64 len)
    {
        u32 hash = 5381 ;

        for (i64 i = 0; i < len; ++i)
            hash = (hash + (hash << 5)) + str[i];

        return hash;
    }

    // AP Hash Function
    u32 AP(char *str, i64 len)
    {
        u32 hash = 0 ;
        for(i64 i = 0; i < len; i++)
        {
            if((i & 1) == 0)
                hash ^= ((hash << 7) ^ str[i] ^ (hash >> 3));
            else
                hash ^= (~((hash << 11) ^ str[i] ^ (hash >> 5)));
        }

        return hash;
    }
    /*DEKHash*/
    u32 DEK(char *str, i64 len)
    {
        u32 hash = len;
        for(i64 i = 0; i < len; i++)
            hash = ((hash << 5) ^ (hash >> 27)) ^ str[i];
        return hash;
    }
    /*BPHash*/
    u32 BP(char *str, i64 len)
    {
        u32 hash = 0;
        for(i64 i = 0;i < len; i++)
            hash = hash << 7 ^ str[i];
        return hash;
    }
    /*FNVHash*/
    u32 FNV(char *str, i64 len)
    {
        u32 fnv_prime = 0x811C9DC5;
        for(i64 i = 0; i < len; i++)
        {
            fnv_prime ^= str[i];
            fnv_prime *= 0x01000193;
        }
        return fnv_prime;
    }
    u64 FNV64(char* str, size_t len)
    {
        u64 fnv_prime = 0xcbf29ce484222325;
        for(i64 i = 0; i < len; i++)
        {
            fnv_prime ^= str[i];
            fnv_prime *= 0x100000001b3;
        }
        return fnv_prime;
    }
    u32 H64(u64 v)
    {
        v = (~v) + (v << 18); // v = (v << 18) - v - 1;
        v = v ^ (v >> 31);
        v = v * 21; // v = (v + (v << 2)) + (v << 4);
        v = v ^ (v >> 11);
        v = v + (v << 6);
        v = v ^ (v >> 22);
        return (u32) v;
    }
}
