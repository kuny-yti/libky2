
#include "tools/ky_string.h"
#include "unicode_table.h"
#include <algorithm>

#define FLAG(x) (1 << (x))

typedef enum  {
    Char_Null = 0x0000,
    Char_Tabulation = 0x0009,
    Char_LineFeed = 0x000a,
    Char_CarriageReturn = 0x000d,
    Char_Space = 0x0020,
    Char_Nbsp = 0x00a0,
    Char_SoftHyphen = 0x00ad,
    Char_ReplacementCharacter = 0xfffd,
    Char_ObjectReplacementCharacter = 0xfffc,
    Char_ByteOrderMark = 0xfeff,
    Char_ByteOrderSwapped = 0xfffe,
    Char_ParagraphSeparator = 0x2029,
    Char_LineSeparator = 0x2028,
    Char_LastValidCodePoint = 0x10ffff
}eSpecialChars;

typedef enum
{
    Category_MarkNonSpacing,          //   Mn
    Category_MarkSpacingCombining,    //   Mc
    Category_MarkEnclosing,           //   Me

    Category_NumberDecimalDigit,      //   Nd
    Category_NumberLetter,            //   Nl
    Category_NumberOther,             //   No

    Category_SeparatorSpace,          //   Zs
    Category_SeparatorLine,           //   Zl
    Category_SeparatorParagraph,      //   Zp

    Category_OtherControl,            //   Cc
    Category_OtherFormat,             //   Cf
    Category_OtherSurrogate,          //   Cs
    Category_OtherPrivateUse,         //   Co
    Category_OtherNotAssigned,        //   Cn

    Category_LetterUppercase,         //   Lu
    Category_LetterLowercase,         //   Ll
    Category_LetterTitlecase,         //   Lt
    Category_LetterModifier,          //   Lm
    Category_LetterOther,             //   Lo

    Category_PunctuationConnector,    //   Pc
    Category_PunctuationDash,         //   Pd
    Category_PunctuationOpen,         //   Ps
    Category_PunctuationClose,        //   Pe
    Category_PunctuationInitialQuote, //   Pi
    Category_PunctuationFinalQuote,   //   Pf
    Category_PunctuationOther,        //   Po

    Category_SymbolMath,              //   Sm
    Category_SymbolCurrency,          //   Sc
    Category_SymbolModifier,          //   Sk
    Category_SymbolOther              //   So
}eCategorys;

typedef enum
{
    Script_Unknown,
    Script_Inherited,
    Script_Common,

    Script_Latin,
    Script_Greek,
    Script_Cyrillic,
    Script_Armenian,
    Script_Hebrew,
    Script_Arabic,
    Script_Syriac,
    Script_Thaana,
    Script_Devanagari,
    Script_Bengali,
    Script_Gurmukhi,
    Script_Gujarati,
    Script_Oriya,
    Script_Tamil,
    Script_Telugu,
    Script_Kannada,
    Script_Malayalam,
    Script_Sinhala,
    Script_Thai,
    Script_Lao,
    Script_Tibetan,
    Script_Myanmar,
    Script_Georgian,
    Script_Hangul,
    Script_Ethiopic,
    Script_Cherokee,
    Script_CanadianAboriginal,
    Script_Ogham,
    Script_Runic,
    Script_Khmer,
    Script_Mongolian,
    Script_Hiragana,
    Script_Katakana,
    Script_Bopomofo,
    Script_Han,
    Script_Yi,
    Script_OldItalic,
    Script_Gothic,
    Script_Deseret,
    Script_Tagalog,
    Script_Hanunoo,
    Script_Buhid,
    Script_Tagbanwa,
    Script_Coptic,

    // Unicode 4.0 additions
    Script_Limbu,
    Script_TaiLe,
    Script_LinearB,
    Script_Ugaritic,
    Script_Shavian,
    Script_Osmanya,
    Script_Cypriot,
    Script_Braille,

    // Unicode 4.1 additions
    Script_Buginese,
    Script_NewTaiLue,
    Script_Glagolitic,
    Script_Tifinagh,
    Script_SylotiNagri,
    Script_OldPersian,
    Script_Kharoshthi,

    // Unicode 5.0 additions
    Script_Balinese,
    Script_Cuneiform,
    Script_Phoenician,
    Script_PhagsPa,
    Script_Nko,

    // Unicode 5.1 additions
    Script_Sundanese,
    Script_Lepcha,
    Script_OlChiki,
    Script_Vai,
    Script_Saurashtra,
    Script_KayahLi,
    Script_Rejang,
    Script_Lycian,
    Script_Carian,
    Script_Lydian,
    Script_Cham,

    // Unicode 5.2 additions
    Script_TaiTham,
    Script_TaiViet,
    Script_Avestan,
    Script_EgyptianHieroglyphs,
    Script_Samaritan,
    Script_Lisu,
    Script_Bamum,
    Script_Javanese,
    Script_MeeteiMayek,
    Script_ImperialAramaic,
    Script_OldSouthArabian,
    Script_InscriptionalParthian,
    Script_InscriptionalPahlavi,
    Script_OldTurkic,
    Script_Kaithi,

    // Unicode 6.0 additions
    Script_Batak,
    Script_Brahmi,
    Script_Mandaic,

    // Unicode 6.1 additions
    Script_Chakma,
    Script_MeroiticCursive,
    Script_MeroiticHieroglyphs,
    Script_Miao,
    Script_Sharada,
    Script_SoraSompeng,
    Script_Takri,

    ScriptCount
}eScripts;

typedef enum
{
    NoDecomposition,
    Canonical,
    Font,
    NoBreak,
    Initial,
    Medial,
    Final,
    Isolated,
    Circle,
    Super,
    Sub,
    Vertical,
    Wide,
    Narrow,
    Small,
    Square,
    Compat,
    Fraction
}eDecompositions;

typedef enum  {
    Joining_None,
    Joining_Causing,
    Joining_Dual,
    Joining_Right,
    Joining_Left,
    Joining_Transparent
}eJoiningTypes;

typedef enum
{
    Combining_BelowLeftAttached       = 200,
    Combining_BelowAttached           = 202,
    Combining_BelowRightAttached      = 204,
    Combining_LeftAttached            = 208,
    Combining_RightAttached           = 210,
    Combining_AboveLeftAttached       = 212,
    Combining_AboveAttached           = 214,
    Combining_AboveRightAttached      = 216,

    Combining_BelowLeft               = 218,
    Combining_Below                   = 220,
    Combining_BelowRight              = 222,
    Combining_Left                    = 224,
    Combining_Right                   = 226,
    Combining_AboveLeft               = 228,
    Combining_Above                   = 230,
    Combining_AboveRight              = 232,

    Combining_DoubleBelow             = 233,
    Combining_DoubleAbove             = 234,
    Combining_IotaSubscript           = 240
}eCombiningClasss;

typedef enum  {
    Unicode_Unassigned,
    Unicode_11,
    Unicode_20,
    Unicode_212,
    Unicode_30,
    Unicode_31,
    Unicode_32,
    Unicode_40,
    Unicode_41,
    Unicode_50,
    Unicode_51,
    Unicode_52,
    Unicode_60,
    Unicode_61,
    Unicode_62,
    Unicode_63
}eUnicodeVersions;

typedef enum
{
    DirL, DirR, DirEN, DirES, DirET, DirAN, DirCS, DirB, DirS, DirWS, DirON,
    DirLRE, DirLRO, DirAL, DirRLE, DirRLO, DirPDF, DirNSM, DirBN,
    DirLRI, DirRLI, DirFSI, DirPDI
}eDirections;

static inline eCategorys category(uint32 ucs4)
{
    if (ucs4 > Char_LastValidCodePoint)
        return Category_OtherNotAssigned;
    return (eCategorys) unicode_table::getProp(ucs4)->category;
}
static inline eDirections direction(uint32 ucs4)
{
    if (ucs4 > Char_LastValidCodePoint)
        return DirL;
    return (eDirections) unicode_table::getProp(ucs4)->direction;
}
static inline eJoiningTypes joiningType(uint32 ucs4)
{
    if (ucs4 > Char_LastValidCodePoint)
        return Joining_None;
    return eJoiningTypes(unicode_table::getProp(ucs4)->joining);
}
static inline bool hasMirrored(uint32 ucs4)
{
    if (ucs4 > Char_LastValidCodePoint)
        return false;
    return unicode_table::getProp(ucs4)->mirrorDiff != 0;
}
static inline uint32 mirroredChar(uint32 ucs4)
{
    if (ucs4 > Char_LastValidCodePoint)
        return ucs4;
    return ucs4 + unicode_table::getProp(ucs4)->mirrorDiff;
}

static inline bool isLetter_helper(uint32 ucs4)
{
    if (ucs4 > Char_LastValidCodePoint)
        return false;
    const int test = FLAG(Category_LetterUppercase) |
                     FLAG(Category_LetterLowercase) |
                     FLAG(Category_LetterTitlecase) |
                     FLAG(Category_LetterModifier) |
                     FLAG(Category_LetterOther);
    long ro = 1;
    ro = ro << (unicode_table::getProp(ucs4)->category);
    return ro & test;
}
static inline bool isNumber_helper(uint32 ucs4)
{
    if (ucs4 > Char_LastValidCodePoint)
        return false;
    const int test = FLAG(Category_NumberDecimalDigit) |
                     FLAG(Category_NumberLetter) |
                     FLAG(Category_NumberOther);
    long ro = 1;
    ro = ro << (unicode_table::getProp(ucs4)->category);
    return ro & test;
}
static inline bool isLetterOrNumber_helper(uint32 ucs4)
{
    if (ucs4 > Char_LastValidCodePoint)
        return false;
    const int test = FLAG(Category_LetterUppercase) |
                     FLAG(Category_LetterLowercase) |
                     FLAG(Category_LetterTitlecase) |
                     FLAG(Category_LetterModifier) |
                     FLAG(Category_LetterOther) |
                     FLAG(Category_NumberDecimalDigit) |
                     FLAG(Category_NumberLetter) |
                     FLAG(Category_NumberOther);
    long ro = 1;
    ro = ro << (unicode_table::getProp(ucs4)->category);
    return ro & test;
}
static inline bool isSpace_helper(uint32 ucs4)
{
    if (ucs4 > Char_LastValidCodePoint)
        return false;
    const int test = FLAG(Category_SeparatorSpace) |
                     FLAG(Category_SeparatorLine) |
                     FLAG(Category_SeparatorParagraph);
    long ro = 1;
    ro = ro << (unicode_table::getProp(ucs4)->category);
    return ro & test;
}

static inline bool isNonCharacter(uint32 ucs4) {
    return ucs4 >= 0xfdd0 && (ucs4 <= 0xfdef || (ucs4 & 0xfffe) == 0xfffe);
}
static inline bool isHighSurrogate(uint32 ucs4) {
    return ((ucs4 & 0xfffffc00) == 0xd800);
}
static inline bool isLowSurrogate(uint32 ucs4) {
    return ((ucs4 & 0xfffffc00) == 0xdc00);
}
static inline bool isSurrogate(uint32 ucs4) {
    return (ucs4 - 0xd800u < 2048u);
}
static inline bool requiresSurrogates(uint32 ucs4) {
    return (ucs4 >= 0x10000);
}
static uint32 surrogateToUcs4(uint16 high, uint16 low) {
    return (uint32(high)<<10) + low - 0x35fdc00;
}

static inline uint16 highSurrogate(uint32 ucs4) {
    return uint16((ucs4>>10) + 0xd7c0);
}
static inline uint16 lowSurrogate(uint32 ucs4) {
    return uint16(ucs4%0x400 + 0xdc00);
}
static inline bool isSpace(uint32 ucs4)
{
    // note that [0x09..0x0d] + 0x85 are exceptional Cc-s and must be handled explicitly
    return ucs4 == 0x20 || (ucs4 <= 0x0d && ucs4 >= 0x09)
            || (ucs4 > 127 && (ucs4 == 0x85 || ucs4 == 0xa0 || isSpace_helper(ucs4)));
}
static inline bool isLetter(uint32 ucs4)
{
    return (ucs4 >= 'A' && ucs4 <= 'z' && (ucs4 >= 'a' || ucs4 <= 'Z'))
            || (ucs4 > 127 && isLetter_helper(ucs4));
}
static inline bool isNumber(uint32 ucs4)
{
    return (ucs4 <= '9' && ucs4 >= '0') || (ucs4 > 127 && isNumber_helper(ucs4));
}
static inline bool isLetterOrNumber(uint32 ucs4)
{
    return (ucs4 >= 'A' && ucs4 <= 'z' && (ucs4 >= 'a' || ucs4 <= 'Z'))
            || (ucs4 >= '0' && ucs4 <= '9')
            || (ucs4 > 127 && isLetterOrNumber_helper(ucs4));
}
static inline bool isDigit(uint32 ucs4)
{
    return (ucs4 <= '9' && ucs4 >= '0') || (ucs4 > 127 && category(ucs4)
                                            == Category_NumberDecimalDigit);
}
static inline bool isLower(uint32 ucs4)
{
    return (ucs4 <= 'z' && ucs4 >= 'a') || (ucs4 > 127 && category(ucs4)
                                            == Category_LetterLowercase);
}
static inline bool isUpper(uint32 ucs4)
{
    return (ucs4 <= 'Z' && ucs4 >= 'A') || (ucs4 > 127 && category(ucs4)
                                            == Category_LetterUppercase);
}
static inline bool isTitleCase(uint32 ucs4)
{
    return ucs4 > 127 && category(ucs4) == Category_LetterTitlecase;
}
static inline bool isPrint(uint32 ucs4)
{
    if (ucs4 > Char_LastValidCodePoint)
        return false;
    const int test = FLAG(Category_OtherControl) |
                     FLAG(Category_OtherFormat) |
                     FLAG(Category_OtherSurrogate) |
                     FLAG(Category_OtherPrivateUse) |
                     FLAG(Category_OtherNotAssigned);
    long ro = 1;
    ro = ro << (unicode_table::getProp(ucs4)->category);
    return !(ro & test);
}
static inline bool isMark(uint32 ucs4)
{
    if (ucs4 > Char_LastValidCodePoint)
        return false;
    const int test = FLAG(Category_MarkNonSpacing) |
                     FLAG(Category_MarkSpacingCombining) |
                     FLAG(Category_MarkEnclosing);
    long ro = 1;
    ro = ro << (unicode_table::getProp(ucs4)->category);
    return ro & test;
}
static inline bool isPunct(uint32 ucs4)
{
    if (ucs4 > Char_LastValidCodePoint)
        return false;
    const int test = FLAG(Category_PunctuationConnector) |
                     FLAG(Category_PunctuationDash) |
                     FLAG(Category_PunctuationOpen) |
                     FLAG(Category_PunctuationClose) |
                     FLAG(Category_PunctuationInitialQuote) |
                     FLAG(Category_PunctuationFinalQuote) |
                     FLAG(Category_PunctuationOther);
    long ro = 1;
    ro = ro << (unicode_table::getProp(ucs4)->category);
    return ro & test;
}
static inline bool isSymbol(uint32 ucs4)
{
    if (ucs4 > Char_LastValidCodePoint)
        return false;
    const int test = FLAG(Category_SymbolMath) |
                     FLAG(Category_SymbolCurrency) |
                     FLAG(Category_SymbolModifier) |
                     FLAG(Category_SymbolOther);
    long ro = 1;
    ro = ro << (unicode_table::getProp(ucs4)->category);
    return ro & test;
}
static inline int digitValue(uint32 ucs4)
{
    if (ucs4 > Char_LastValidCodePoint)
        return -1;
    return unicode_table::getProp(ucs4)->digitValue;
}
enum {
    Hangul_SBase = 0xac00,
    Hangul_LBase = 0x1100,
    Hangul_VBase = 0x1161,
    Hangul_TBase = 0x11a7,
    Hangul_LCount = 19,
    Hangul_VCount = 21,
    Hangul_TCount = 28,
    Hangul_NCount = Hangul_VCount * Hangul_TCount,
    Hangul_SCount = Hangul_LCount * Hangul_NCount
};
static inline const uint16 * decompositionHelper (uint32 ucs4, int *length,
                                           int *tag, unsigned short *buffer)
{
    if (ucs4 >= Hangul_SBase && ucs4 < Hangul_SBase + Hangul_SCount)
    {
        // compute Hangul syllable decomposition as per UAX #15
        const uint32 SIndex = ucs4 - Hangul_SBase;
        buffer[0] = Hangul_LBase + SIndex / Hangul_NCount; // L
        buffer[1] = Hangul_VBase + (SIndex % Hangul_NCount) / Hangul_TCount; // V
        buffer[2] = Hangul_TBase + SIndex % Hangul_TCount; // T
        *length = buffer[2] == Hangul_TBase ? 2 : 3;
        *tag = Canonical;
        return buffer;
    }

    const unsigned short index = GET_DECOMPOSITION_INDEX(ucs4);
    if (index == 0xffff) {
        *length = 0;
        *tag = NoDecomposition;
        return 0;
    }

    const unsigned short *decomposition = uc_decomposition_map+index;
    *tag = (*decomposition) & 0xff;
    *length = (*decomposition) >> 8;
    return decomposition+1;
}
static inline ky_string decomposition(uint32 ucs4)
{
    unsigned short buffer[3];
    int length;
    int tag;
    const uint16 *d = decompositionHelper(ucs4, &length, &tag, buffer);
    return ky_string((ky_char *)d, length);
}
static inline eDecompositions decompositionTag(uint32 ucs4)
{
    if (ucs4 >= Hangul_SBase && ucs4 < Hangul_SBase + Hangul_SCount)
        return Canonical;
    const uint16 index = GET_DECOMPOSITION_INDEX(ucs4);
    if (index == 0xffff)
        return NoDecomposition;
    return (eDecompositions)(uc_decomposition_map[index] & 0xff);
}
static inline uint8 combiningClass(uint32 ucs4)
{
    if (ucs4 > Char_LastValidCodePoint)
        return 0;
    return (uint8) unicode_table::getProp(ucs4)->combiningClass;
}
static inline eScripts script(uint32 ucs4)
{
    if (ucs4 > Char_LastValidCodePoint)
        return Script_Unknown;
    return (eScripts) unicode_table::getProp(ucs4)->script;
}
static inline eUnicodeVersions unicodeVersion(uint32 ucs4)
{
    if (ucs4 > Char_LastValidCodePoint)
        return Unicode_Unassigned;
    return (eUnicodeVersions) unicode_table::getProp(ucs4)->unicodeVersion;
}
static inline eUnicodeVersions currentUnicodeVersion()
{
    return UNICODE_DATA_VERSION;
}
template <typename T>
static inline T toLowerCase_helper(T uc)
{
    const Properties *p = unicode_table::getProp(uc);
    if (p->lowerCaseSpecial) {
        const uint16 *specialCase = specialCaseMap + p->lowerCaseDiff;
        return (*specialCase == 1) ? specialCase[1] : uc;
    }
    return uc + p->lowerCaseDiff;
}
template <typename T>
static inline T toUpperCase_helper(T uc)
{
    const Properties *p = unicode_table::getProp(uc);
    if (p->upperCaseSpecial) {
        const uint16 *specialCase = specialCaseMap + p->upperCaseDiff;
        return (*specialCase == 1) ? specialCase[1] : uc;
    }
    return uc + p->upperCaseDiff;
}
template <typename T>
static inline T toTitleCase_helper(T uc)
{
    const Properties *p = unicode_table::getProp(uc);
    if (p->titleCaseSpecial) {
        const uint16 *specialCase = specialCaseMap + p->titleCaseDiff;
        return (*specialCase == 1) ? specialCase[1] : uc;
    }
    return uc + p->titleCaseDiff;
}
template <typename T>
static inline T toCaseFolded_helper(T uc)
{
    const Properties *p = unicode_table::getProp(uc);
    if (p->caseFoldSpecial) {
        const uint16 *specialCase = specialCaseMap + p->caseFoldDiff;
        return (*specialCase == 1) ? specialCase[1] : uc;
    }
    return uc + p->caseFoldDiff;
}
static inline uint32 toLower(uint32 ucs4)
{
    if (ucs4 > Char_LastValidCodePoint)
        return ucs4;
     return toLowerCase_helper<uint32>(ucs4);
}
static inline uint32 toUpper(uint32 ucs4)
{
    if (ucs4 > Char_LastValidCodePoint)
        return ucs4;
    return toUpperCase_helper<uint32>(ucs4);
}
static inline uint32 toTitleCase(uint32 ucs4)
{
    if (ucs4 > Char_LastValidCodePoint)
        return ucs4;
    return toTitleCase_helper<uint32>(ucs4);
}
static inline uint32 foldCase(const uint16 *ch, const uint16 *start)
{
    uint16 c = *ch;
    if (ky_char(c).is_low_surrogate() && ch > start &&
            ky_char(*(ch - 1)).is_high_surrogate())
        c = surrogateToUcs4(*(ch - 1), c);
    return toCaseFolded_helper<uint32>(c);
}
static inline uint32 foldCase(uint32 ch, uint32 &last)
{
    uint16 c = ch & 0XFFFF;
    if (ky_char(c).is_low_surrogate() && ky_char((uint16)last).is_high_surrogate())
        c = surrogateToUcs4(last, c);
    last = ch;
    return toCaseFolded_helper<uint32>(c);
}
static inline uint16 foldCase(uint16 ch)
{
    return toCaseFolded_helper<uint16>(ch);
}
static inline uint32 toCaseFolded(uint32 ucs4)
{
    if (ucs4 > Char_LastValidCodePoint)
        return ucs4;
    return toCaseFolded_helper<uint32>(ucs4);
}
static inline void decomposeHelper(ky_string *str, bool canonical,
                            eUnicodeVersions version, int from)
{
    int length;
    int tag;
    unsigned short buffer[3];

    ky_string &s = *str;

    const unsigned short *utf16 = reinterpret_cast<unsigned short *>(s.data());
    const unsigned short *uc = utf16 + s.length();
    while (uc != utf16 + from) {
        uint32 ucs4 = *(--uc);
        if (ky_char(uint16(ucs4&0XFFFF)).is_low_surrogate() && uc != utf16) {
            uint16 high = *(uc - 1);
            if (ky_char(high).is_high_surrogate()) {
                --uc;
                ucs4 = surrogateToUcs4(high, ucs4);
            }
        }

        if (unicodeVersion(ucs4) > version)
            continue;

        const unsigned short *d = decompositionHelper(ucs4, &length, &tag, buffer);
        if (!d || (canonical && tag != Canonical))
            continue;

        int pos = uc - utf16;
        //s.replace(pos, requiresSurrogates(ucs4) ? 2 : 1,
                  //reinterpret_cast<const ky_char *>(d), length);
        // since the replace invalidates the pointers and we do decomposition recursive
        utf16 = reinterpret_cast<uint16 *>(s.data());
        uc = utf16 + pos + length;
    }
}

struct UCS2Pair {
    uint16 u1;
    uint16 u2;
};

inline bool operator<(const UCS2Pair &ligature1, const UCS2Pair &ligature2)
{ return ligature1.u1 < ligature2.u1; }
inline bool operator<(uint16 u1, const UCS2Pair &ligature)
{ return u1 < ligature.u1; }
inline bool operator<(const UCS2Pair &ligature, uint16 u1)
{ return ligature.u1 < u1; }

struct UCS2SurrogatePair {
    UCS2Pair p1;
    UCS2Pair p2;
};

inline bool operator<(const UCS2SurrogatePair &ligature1, const UCS2SurrogatePair &ligature2)
{
    return surrogateToUcs4(ligature1.p1.u1, ligature1.p1.u2) <
            surrogateToUcs4(ligature2.p1.u1, ligature2.p1.u2);
}
inline bool operator<(uint32 u1, const UCS2SurrogatePair &ligature)
{
    return u1 < surrogateToUcs4(ligature.p1.u1, ligature.p1.u2);
}
inline bool operator<(const UCS2SurrogatePair &ligature, uint32 u1)
{
    return surrogateToUcs4(ligature.p1.u1, ligature.p1.u2) < u1;
}

static uint32 inline ligatureHelper(uint32 u1, uint32 u2)
{
    if (u1 >= Hangul_LBase && u1 <= Hangul_SBase + Hangul_SCount)
    {
        // compute Hangul syllable composition as per UAX #15
        // hangul L-V pair
        const uint32 LIndex = u1 - Hangul_LBase;
        if (LIndex < Hangul_LCount) {
            const uint32 VIndex = u2 - Hangul_VBase;
            if (VIndex < Hangul_VCount)
                return Hangul_SBase + (LIndex * Hangul_VCount + VIndex) * Hangul_TCount;
        }
        // hangul LV-T pair
        const uint32 SIndex = u1 - Hangul_SBase;
        if (SIndex < Hangul_SCount && (SIndex % Hangul_TCount) == 0) {
            const uint32 TIndex = u2 - Hangul_TBase;
            if (TIndex <= Hangul_TCount)
                return u1 + TIndex;
        }
    }

    const unsigned short index = GET_LIGATURE_INDEX(u2);
    if (index == 0xffff)
        return 0;
    const unsigned short *ligatures = uc_ligature_map+index;
    uint16 length = *ligatures++;
    if (requiresSurrogates(u1))
    {
        const UCS2SurrogatePair *data = reinterpret_cast<const UCS2SurrogatePair *>(ligatures);
        const UCS2SurrogatePair *r = std::lower_bound(data, data + length, u1);
        if (r != data + length && surrogateToUcs4(r->p1.u1, r->p1.u2) == u1)
            return surrogateToUcs4(r->p2.u1, r->p2.u2);
    } else {
        const UCS2Pair *data = reinterpret_cast<const UCS2Pair *>(ligatures);
        const UCS2Pair *r = std::lower_bound(data, data + length, uint16(u1));
        if (r != data + length && r->u1 == uint16(u1))
            return r->u2;
    }

    return 0;
}

static inline void composeHelper(ky_string *str, eUnicodeVersions version, int from)
{
    ky_string &s = *str;

    if (from < 0 || s.length() - from < 2)
        return;

    uint32 stcode = 0; // starter code point
    int starter = -1; // starter position
    int next = -1; // to prevent i == next
    int lastCombining = 255; // to prevent combining > lastCombining

    int pos = from;
    while ((int64)pos < s.length()) {
        int i = pos;
        uint32 uc = s.at(pos).unicode();
        if (ky_char(uint16(uc&0XFFFF)).is_high_surrogate() && (int64)pos < s.length()-1) {
            uint16 low = s.at(pos+1).unicode();
            if (ky_char(low).is_low_surrogate()) {
                uc = surrogateToUcs4(uc, low);
                ++pos;
            }
        }

        const Properties *p = unicode_table::getProp(uc);
        if (p->unicodeVersion > version) {
            starter = -1;
            next = -1; // to prevent i == next
            lastCombining = 255; // to prevent combining > lastCombining
            ++pos;
            continue;
        }

        int combining = p->combiningClass;
        if ((i == next || combining > lastCombining) && starter >= from) {
            // allowed to form ligature with S
            uint32 ligature = ligatureHelper(stcode, uc);
            if (ligature) {
                stcode = ligature;
                ky_char *d = s.data();
                // ligatureHelper() never changes planes
                if (requiresSurrogates(ligature)) {
                    d[starter] = highSurrogate(ligature);
                    d[starter + 1] = lowSurrogate(ligature);
                    //s.remove(i, 2);
                } else {
                    d[starter] = (uint16)(ligature&0XFFFF);
                    //s.remove(i, 1);
                }
                continue;
            }
        }
        if (combining == 0) {
            starter = i;
            stcode = uc;
            next = pos + 1;
        }
        lastCombining = combining;

        ++pos;
    }
}


static inline void canonicalOrderHelper(ky_string *str, eUnicodeVersions version, int from)
{
    ky_string &s = *str;
    const int l = s.length()-1;

    uint32 u1, u2;
    uint16 c1, c2;

    int pos = from;
    while (pos < l) {
        int p2 = pos+1;
        u1 = s.at(pos).unicode();
        if (ky_char((uint16)(u1&0XFFFF)).is_high_surrogate()) {
            uint16 low = s.at(p2).unicode();
            if (ky_char(low).is_low_surrogate()) {
                u1 = surrogateToUcs4(u1, low);
                if (p2 >= l)
                    break;
                ++p2;
            }
        }
        c1 = 0;

    advance:
        u2 = s.at(p2).unicode();
        if (ky_char(uint16(u2&0XFFFF)).is_high_surrogate() && p2 < l) {
            uint16 low = s.at(p2+1).unicode();
            if (ky_char(low).is_low_surrogate()) {
                u2 = surrogateToUcs4(u2, low);
                ++p2;
            }
        }

        c2 = 0;
        {
            const Properties *p = unicode_table::getProp(u2);
            if (p->unicodeVersion <= version)
                c2 = p->combiningClass;
        }
        if (c2 == 0) {
            pos = p2+1;
            continue;
        }

        if (c1 == 0) {
            const Properties *p = unicode_table::getProp(u1);
            if (p->unicodeVersion <= version)
                c1 = p->combiningClass;
        }

        if (c1 > c2) {
            ky_char *uc = s.data();
            int p = pos;
            // exchange characters
            if (!requiresSurrogates(u2)) {
                uc[p++] = (uint16)(u2&0XFF);
            } else {
                uc[p++] = highSurrogate(u2);
                uc[p++] = lowSurrogate(u2);
            }
            if (!requiresSurrogates(u1)) {
                uc[p++] = (uint16)(u1&0XFF);
            } else {
                uc[p++] = highSurrogate(u1);
                uc[p++] = lowSurrogate(u1);
            }
            if (pos > 0)
                --pos;
            if (pos > 0 && s.at(pos).is_low_surrogate())
                --pos;
        } else {
            ++pos;
            if (requiresSurrogates(u1))
                ++pos;

            u1 = u2;
            c1 = c2; // != 0
            p2 = pos + 1;
            if (requiresSurrogates(u1))
                ++p2;
            if (p2 > l)
                break;

            goto advance;
        }
    }
}

// returns true if the text is in a desired Normalization Form already; false otherwise.
// sets lastStable to the position of the last stable code point
static inline bool normalizationQuickCheckHelper(ky_string *str, int mode,
                                          int from, int *lastStable)
{
    enum { NFQC_YES = 0, NFQC_NO = 1, NFQC_MAYBE = 3 };

    const uint16 *string = reinterpret_cast<const uint16 *>(str->data());
    int length = str->length();

    // this avoids one out of bounds check in the loop
    while (length > from && isHighSurrogate(string[length - 1]))
        --length;

    uint8 lastCombining = 0;
    for (int i = from; i < length; ++i) {
        int pos = i;
        uint32 uc = string[i];
        if (uc < 0x80) {
            // ASCII characters are stable code points
            lastCombining = 0;
            *lastStable = pos;
            continue;
        }

        if (isHighSurrogate(uc)) {
            uint16 low = string[i + 1];
            if (!isLowSurrogate(low)) {
                // treat surrogate like stable code point
                lastCombining = 0;
                *lastStable = pos;
                continue;
            }
            ++i;
            uc = surrogateToUcs4(uc, low);
        }

        const Properties *p = unicode_table::getProp(uc);

        if (p->combiningClass < lastCombining && p->combiningClass > 0)
            return false;

        const uint8 check = (p->nfQuickCheck >> (mode << 1)) & 0x03;
        if (check != NFQC_YES)
            return false; // ### can we quick check NFQC_MAYBE ?

        lastCombining = p->combiningClass;
        if (lastCombining == 0)
            *lastStable = pos;
    }

    if ((int64)length != str->length()) // low surrogate parts at the end of text
        *lastStable = str->length() - 1;

    return true;
}

