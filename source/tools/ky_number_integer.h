#ifndef KY_NUMBER_INTEGER_H
#define KY_NUMBER_INTEGER_H

#include "ky_define.h".h"

enum
{
    AsciiSpaceMask = (1 << (' ' - 1)) |
                     (1 << ('\t' - 1)) |   // 9: HT - horizontal tab
                     (1 << ('\n' - 1)) |   // 10: LF - line feed
                     (1 << ('\v' - 1)) |   // 11: VT - vertical tab
                     (1 << ('\f' - 1)) |   // 12: FF - form feed
                     (1 << ('\r' - 1))     // 13: CR - carriage return
};

static bool is_space_ascii(u8 c)
{
    return c >= 1U && c <= 32U && (u32(AsciiSpaceMask) >> u32(c - 1)) & 1U;
}

static char* uint2str(u64 l, int base)
{
    static char buff[65]; // length of MAX_ULLONG in base 2
    char *p = buff + 65;

    if (l == 0)
    {
        *(--p) = '0';
    }
    else if (base != 10)
    {
        while (l != 0)
        {
            const int c = l % base;
            --p;

            if (c < 10)
                *p = '0' + c;
            else
                *p = c - 10 + 'a';

            l /= base;
        }
    }
    else
    {
        while (l != 0)
        {
            const int c = l % base;

            *(--p) = '0' + c;

            l /= base;
        }
    }

    p[65 - (p - buff)] = '\0';
    return p;
}
static char* int2str(i64 l, int base)
{
    return uint2str(l < 0 ? -l : l, base);
}

static u64 str2uint(const char *nptr, const char **endptr, int base)
{
    u64 acc;
    u64 qbase, cutoff;
    int any, cutlim;

    const char *s = nptr;
    u8 c;
    do
    {
        c = *s++;
    } while (is_space_ascii(c));

    if (c == '-')
    {
        if (endptr != 0)
            *endptr = s - 1;
        return 0;
    }
    else
    {
        if (c == '+')
            c = *s++;
    }

    if ((base == 0 || base == 16) &&
        (c == '0') && ((*s == 'x') || (*s == 'X')))
    {
        c = s[1];
        s += 2;
        base = 16;
    }

    if (base == 0)
        base = (c == '0') ? 8 : 10;
    qbase = unsigned(base);
    cutoff = u64(ULLONG_MAX) / qbase;
    cutlim = u64(ULLONG_MAX) % qbase;
    for (acc = 0, any = 0;; c = *s++)
    {
        if (!::isascii(c))
            break;

        if (::isdigit(c))
            c -= '0';
        else if (::isalpha(c))
            c -= ::isupper(c) ? 'A' - 10 : 'a' - 10;
        else
            break;

        if (c >= base)
            break;

        if ((any < 0) || (acc > cutoff) || ((acc == cutoff) && (c > cutlim)))
            any = -1;
        else
        {
            any = 1;
            acc *= qbase;
            acc += c;
        }
    }

    if (any == 0)
    {

    }
    else if (any < 0)
    {
        acc = ULLONG_MAX;
    }

    if (endptr != 0)
        *endptr = (any ? s - 1 : nptr);
    return acc;
}
static i64 str2int(const char *nptr, const char **endptr, int base)
{
    u64 acc;
    u8  c;
    u64 qbase, cutoff;
    int neg, any, cutlim;

    const char *s = nptr;
    do
    {
        c = *s++;
    }
    while (is_space_ascii(c));

    if (c == '-')
    {
        neg = 1;
        c = *s++;
    }
    else
    {
        neg = 0;
        if (c == '+')
            c = *s++;
    }

    if (((base == 0) || (base == 16)) &&
        (c == '0') && ((*s == 'x') || (*s == 'X')))
    {
        c = s[1];
        s += 2;
        base = 16;
    }
    if (base == 0)
        base = c == '0' ? 8 : 10;

    qbase = unsigned(base);
    cutoff = neg ? u64(0-(LLONG_MIN + LLONG_MAX)) + LLONG_MAX : LLONG_MAX;
    cutlim = cutoff % qbase;
    cutoff /= qbase;

    for (acc = 0, any = 0;; c = *s++)
    {
        if (!::isascii(c))
            break;

        if (::isdigit(c))
            c -= '0';
        else if (::isalpha(c))
            c -= ::isupper(c) ? 'A' - 10 : 'a' - 10;
        else
            break;

        if (c >= base)
            break;

        if ((any < 0) || (acc > cutoff) || ((acc == cutoff) && (c > cutlim)))
            any = -1;
        else
        {
            any = 1;
            acc *= qbase;
            acc += c;
        }
    }

    if (any < 0)
    {
        acc = neg ? LLONG_MIN : LLONG_MAX;
    }
    else if (neg)
    {
        acc = (~acc) + 1;
    }

    if (endptr != 0)
        *endptr = (any >= 0 ? s - 1 : nptr);

    return acc;
}

template <typename T>
static T str2int_helper(const char *data, int len, int base)
{
    len = len;
    i64 val = str2int(data, 0, base);
    if (T(val) != val)
        val = 0;

    return T(val);
}
template <typename T>
static T str2uint_helper(const char *data, int len, int base)
{
    len = len;
    u64 val = str2uint(data, 0, base);
    if (T(val) != val)
        val = 0;

    return T(val);
}

#endif // ky_NUMBER

