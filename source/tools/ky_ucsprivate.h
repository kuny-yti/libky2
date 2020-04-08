
#include "tools/ky_string.h"
#include "ky_unicode.h"

void from_latin1(const char *str, int size, uint16 *buf)
{
    if (!str && !buf)
        return ;
    if (size < 0)
    {
        size = 0;
        while (str[size] != 0)
            ++size;
    }

#ifdef kyHAS_SSE2
    const char *e = str + size;
    intptr offset = 0;

    for ( ; str + offset + 15 < e; offset += 16)
    {
        const __m128i chunk = _mm_loadu_si128((const __m128i*)(str + offset)); // load
#ifdef kyHAS_AVX2
        // zero extend to an YMM register
        const __m256i extended = _mm256_cvtepu8_epi16(chunk);

        // store
        _mm256_storeu_si256((__m256i*)(dst + offset), extended);
#else
        const __m128i nullMask = _mm_set1_epi32(0);

        // unpack the first 8 bytes, padding with zeros
        const __m128i firstHalf = _mm_unpacklo_epi8(chunk, nullMask);
        _mm_storeu_si128((__m128i*)(buf + offset), firstHalf); // store

        // unpack the last 8 bytes, padding with zeros
        const __m128i secondHalf = _mm_unpackhi_epi8 (chunk, nullMask);
        _mm_storeu_si128((__m128i*)(buf + offset + 8), secondHalf); // store
#endif
    }

    size = size % 16;
    buf += offset;
    str += offset;
#endif

    while (size--)
        *buf++ = (uchar)*str++;
}
void from_utf16(const uint16 *ucode, int size, uint16 *buf)
{
    if (!ucode && !buf)
        return ;
    if (size < 0)
    {
        size = 0;
        while (ucode[size] != 0)
            ++size;
    }
    uint32 offset = 0;
    uint16 *dst = buf;

    for (offset = 0; offset < (uint)size; ++offset)
        dst[offset] = ucode[offset];
}
void from_ucs4(const uint *ucode, int size,  uint16 *buf)
{
    if (!ucode && !buf)
        return ;
    if (size < 0) {
        size = 0;
        while (ucode[size] != 0)
            ++size;
    }
    uint32 offset = 0;
    uint16 *dst = buf;

    for (offset = 0; offset < (uint)size; ++size)
        dst[offset] = (uint16)(ucode[offset] & 0xffff);
}

int toUcs4_helper(const uint16 *uc, int length, uint *out)
{
    if (!uc && !out)
        return 0;
    if (length < 0) {
        length = 0;
        while (uc[length] != 0)
            ++length;
    }

    int count = 0;
    for (count = 0; count < length; ++count)
        out[count] = uc[count];

    return count;
}

#ifdef kyHAS_SSE2
static inline __m128i mergeQuestionMarks(__m128i chunk)
{
    const __m128i questionMark = _mm_set1_epi16('?');

# ifdef kyHAS_SSE4_2
    const __m128i rangeMatch = _mm_cvtsi32_si128(0xffff0100);
    const __m128i offLimitMask = _mm_cmpestrm(rangeMatch, 2, chunk, 8,
            _SIDD_UWORD_OPS | _SIDD_CMP_RANGES | _SIDD_UNIT_MASK);

    // replace the non-Latin 1 characters in the chunk with question marks
    chunk = _mm_blendv_epi8(chunk, questionMark, offLimitMask);
# else
    const __m128i signedBitOffset = _mm_set1_epi16(short(0x8000));
    const __m128i thresholdMask = _mm_set1_epi16(short(0xff + 0x8000));

    const __m128i signedChunk = _mm_add_epi16(chunk, signedBitOffset);
    const __m128i offLimitMask = _mm_cmpgt_epi16(signedChunk, thresholdMask);

#  ifdef kyHAS_SSE4_1
    chunk = _mm_blendv_epi8(chunk, questionMark, offLimitMask);
#  else
    const __m128i offLimitQuestionMark = _mm_and_si128(offLimitMask, questionMark);

    const __m128i correctBytes = _mm_andnot_si128(offLimitMask, chunk);

    chunk = _mm_or_si128(correctBytes, offLimitQuestionMark);
#  endif
# endif
    return chunk;
}
#endif
static void to_latin1(uchar *dst, const uint16 *src, int length)
{
#ifdef kyHAS_SSE2
    uchar *e = dst + length;
    intptr offset = 0;

    for ( ; dst + offset + 15 < e; offset += 16)
    {
        __m128i chunk1 = _mm_loadu_si128((const __m128i*)(src + offset)); // load
        chunk1 = mergeQuestionMarks(chunk1);

        __m128i chunk2 = _mm_loadu_si128((const __m128i*)(src + offset + 8)); // load
        chunk2 = mergeQuestionMarks(chunk2);

        const __m128i result = _mm_packus_epi16(chunk1, chunk2);
        _mm_storeu_si128((__m128i*)(dst + offset), result); // store
    }

    length = length % 16;
    dst += offset;
    src += offset;
#endif

    while (length--)
    {
        *dst++ = (*src > 0xff) ? '?' : (uchar) *src;
        ++src;
    }
}
static void to_wchar(wchar_t *dst, const uint16 *src, int length)
{
    int64 rc = 0;
    uint error_status = 0;
    uint error_mask = 0xFFFFFFFF;
    uint error_code_point = 0xFFFD;
    const char* p1 = 0;
    const uint16* pc = 0;
    if (!dst && !src)
        return ;

    int utf8len = (length+1) * sizeof(wchar_t);
    uchar utf8[utf8len+1] ;
    rc = ky_unicode::convert_UTF16_UTF8(false, src, length, utf8, utf8len,
                                &error_status, error_mask, error_code_point, &pc);

    if ( error_status )
        return ;
    if ( rc > 0 && rc <= (int64)utf8len )
    {
        utf8[rc] = 0;
        utf8len = rc;
    }
    else
    {
        utf8[utf8len-1] = 0;
        rc = 0;
    }

    rc = ky_unicode::convert_UTF8_WideChar(false,
                                        (char*)utf8, utf8len,
                                        dst, length,
                                &error_status, error_mask, error_code_point, &p1);

    if ( error_status )
        return ;

    if ( rc > 0 && rc <= (int64)length )
        dst[rc] = 0;
    else
    {
        dst[length-1] = 0;
        rc = 0;
    }
}

/*
//! Unicode字符不区分大小写的比较
static int ucstricmp(const uint16 *a, const uint16 *ae,
                     const uint16 *b, const uint16 *be)
{
    if (a == b)
        return (ae - be);
    if (a == 0)
        return 1;
    if (b == 0)
        return -1;

    const uint16 *e = ae;
    if (be - b < ae - a)
        e = a + (be - b);

    while (a < e)
    {
        int diff = *a - *b;
        if ((diff))
            return diff;
        ++a;
        ++b;
    }
    if (a == ae) {
        if (b == be)
            return 0;
        return -1;
    }
    return 1;
}
//! 不区分大小写比较一个Unicode字符串和一个latin1字符串
static int ucstricmp(const uint16 *a, const uint16 *ae,
                     const uchar *b, const uchar *be)
{
    if (a == 0)
    {
        if (b == 0)
            return 0;
        return 1;
    }
    if (b == 0)
        return -1;

    const uint16 *e = ae;
    if (be - b < ae - a)
        e = a + (be - b);

    while (a < e) {
        int diff = *a - *b;
        if ((diff))
            return diff;
        ++a;
        ++b;
    }
    if (a == ae) {
        if (b == be)
            return 0;
        return -1;
    }
    return 1;
}


//! Unicode区分大小写的比较一个Unicode字符串和一个latin1字符串
static int ucstrncmp(const ky_char *a, const uchar *c, int l)
{
    const uint16 *uc = reinterpret_cast<const uint16 *>(a);
    const uint16 *e = uc + l;

#if kyHAS_SSE2
    __m128i nullmask = _mm_setzero_si128();
    uint32 offset = 0;

    // we're going to read uc[offset..offset+15] (32 bytes)
    // and c[offset..offset+15] (16 bytes)
    for ( ; uc + offset + 15 < e; offset += 16) {
        // similar to fromLatin1_helper:
        // load 16 bytes of Latin 1 data
        __m128i chunk = _mm_loadu_si128((__m128i*)(c + offset));

#  if kyHAS_AVX2
        // expand Latin 1 data via zero extension
        __m256i ldata = _mm256_cvtepu8_epi16(chunk);

        // load UTF-16 data and compare
        __m256i ucdata = _mm256_loadu_si256((__m256i*)(uc + offset));
        __m256i result = _mm256_cmpeq_epi16(ldata, ucdata);

        uint mask = ~_mm256_movemask_epi8(result);
#  else
        // expand via unpacking
        __m128i firstHalf = _mm_unpacklo_epi8(chunk, nullmask);
        __m128i secondHalf = _mm_unpackhi_epi8(chunk, nullmask);

        // load UTF-16 data and compare
        __m128i ucdata1 = _mm_loadu_si128((__m128i*)(uc + offset));
        __m128i ucdata2 = _mm_loadu_si128((__m128i*)(uc + offset + 8));
        __m128i result1 = _mm_cmpeq_epi16(firstHalf, ucdata1);
        __m128i result2 = _mm_cmpeq_epi16(secondHalf, ucdata2);

        uint mask = ~(_mm_movemask_epi8(result1) | _mm_movemask_epi8(result2) << 16);
#  endif
        if (mask) {
            // found a different character
            uint idx = uint(_bit_scan_forward(mask));
            return uc[offset + idx / 2] - c[offset + idx / 2];
        }
    }

#  ifdef Q_PROCESSOR_X86_64
    enum { MaxTailLength = 7 };
    // we'll read uc[offset..offset+7] (16 bytes) and c[offset..offset+7] (8 bytes)
    if (uc + offset + 7 < e) {
        // same, but we're using an 8-byte load
        __m128i chunk = _mm_cvtsi64_si128(*(long long *)(c + offset));
        __m128i secondHalf = _mm_unpacklo_epi8(chunk, nullmask);

        __m128i ucdata = _mm_loadu_si128((__m128i*)(uc + offset));
        __m128i result = _mm_cmpeq_epi16(secondHalf, ucdata);
        uint mask = ~_mm_movemask_epi8(result);
        if (uint16(mask)) {
            // found a different character
            uint idx = uint(_bit_scan_forward(mask));
            return uc[offset + idx / 2] - c[offset + idx / 2];
        }

        // still matched
        offset += 8;
    }
#  else
    // 32-bit, we can't do MOVQ to load 8 bytes
    enum { MaxTailLength = 15 };
#  endif

    // reset uc and c
    uc += offset;
    c += offset;

#endif

    while (uc < e) {
        int diff = *uc - *c;
        if (diff)
            return diff;
        uc++, c++;
    }

    return 0;
}
*/
//! Unicode区分大小写的比较两个相同大小的字符串
static int ucstrncmp(const ky_char *a, const ky_char *b, int l)
{

    if (!l)
        return 0;

    union {
        const ky_char *w;
        const uint32 *d;
        uint32 value;
    } sa, sb;
    sa.w = a;
    sb.w = b;

    // check alignment
    if ((sa.value & 2) == (sb.value & 2))
    {
        // both addresses have the same alignment
        if (sa.value & 2) {
            // both addresses are not aligned to 4-bytes boundaries
            // compare the first character
            if (*sa.w != *sb.w)
                return sa.w->unicode() - sb.w->unicode();
            --l;
            ++sa.w;
            ++sb.w;

            // now both addresses are 4-bytes aligned
        }

        // both addresses are 4-bytes aligned
        // do a fast 32-bit comparison
        const uint32 *e = sa.d + (l >> 1);
        for ( ; sa.d != e; ++sa.d, ++sb.d) {
            if (*sa.d != *sb.d) {
                if (*sa.w != *sb.w)
                    return sa.w->unicode() - sb.w->unicode();
                return sa.w[1].unicode() - sb.w[1].unicode();
            }
        }

        // do we have a tail?
        return (l & 1) ? sa.w->unicode() - sb.w->unicode() : 0;
    } else {
        // one of the addresses isn't 4-byte aligned but the other is
        const ky_char *e = sa.w + l;
        for ( ; sa.w != e; ++sa.w, ++sb.w) {
            if (*sa.w != *sb.w)
                return sa.w->unicode() - sb.w->unicode();
        }
    }
    return 0;
}
//! Unicode区分大小写的比较
static int ucstrcmp(const ky_char *a, int alen, const ky_char *b, int blen)
{
    if (a == b && alen == blen)
        return 0;
    int l = ky_min(alen, blen);
    int cmp = ucstrncmp(a, b, l);
    return cmp ? cmp : (alen-blen);
}
/*
//! Unicode字符串不区分大小写的比较两个同样大小的
static int ucstrnicmp(const uint16 *a, const uint16 *b, int l)
{
    return ucstricmp(a, a + l, b, b + l);
}

static bool memEquals(const uint16 *a, const uint16 *b, int length)
{
    if (a == b || !length)
        return true;

    return ucstrncmp(reinterpret_cast<const ky_char *>(a),
                     reinterpret_cast<const ky_char *>(b), length) == 0;
}

static int ucstrcmp(const ky_char *a, int alen, const uchar *b, int blen)
{
    int l = ky_min(alen, blen);
    int cmp = ucstrncmp(a, b, l);
    return cmp ? cmp : (alen-blen);
}
*/
static int compare_helper(const ky_char *data1, int length1,
                   const ky_char *data2, int length2)
{
    return ucstrcmp(data1, length1, data2, length2);
    /*const uint16 *s1 = reinterpret_cast<const uint16 *>(data1);
    const uint16 *s2 = reinterpret_cast<const uint16 *>(data2);
    return ucstricmp(s1, s1 + length1, s2, s2 + length2);*/
}
int findChar(ky_char *str,int slen, const ky_char &ch)
{
    for (int i = 0; i < slen; ++i)
    {
        if (str[i] == ch)
            return i;
    }
    return -1;
}
int findString(ky_char *str,int slen, ky_char* dst, int dlen)
{
    int bpos = -1;
    if (!dst)
        return -1;
    if ((bpos = findChar (str, slen, dst[0])) != -1)
        for (int i = 1; i < dlen && ((bpos+i) < slen); ++i)
            bpos = str[bpos+i] == dst[i] ? bpos : -1;
    return bpos;
}
