
#include "tools/ky_string.h"
#include "ky_ucs.h"

namespace ky_char_p {

uchar combining_class(const ky_char &data)
{
    return combiningClass(data.unicode ());
}
ky_char mirrored_char(const ky_char &data)
{
    return (ushort)(mirroredChar(data.unicode ())&0xFFFF);
}
bool has_mirrored(const ky_char &data)
{
    return hasMirrored(data.unicode ());
}
ky_string decomposition(const ky_char &data)
{
    return ::decomposition(data.unicode ());
}
eDecompositions decomposition_tag(const ky_char &data)
{
    return decompositionTag(data.unicode ());
}

eCategorys category(const ky_char &data)
{
    return ::category(data.unicode ());
}
eDirections direction(const ky_char &data)
{
    return ::direction(data.unicode ());
}
eJoiningTypes joining_type(const ky_char &data)
{
    return joiningType(data.unicode ());
}
eScripts script(const ky_char &data)
{
    return ::script(data.unicode ());
}
eUnicodeVersions unicode_version(const ky_char &data)
{
    return unicodeVersion(data.unicode ());
}
}

ky_char::ky_char():
    data(0)
{

}
ky_char::ky_char( const ky_char &c):
    data(c.data)
{
}
ky_char::ky_char( char c):
    data(from_latin1(c).data)
{
}
ky_char::ky_char( wchar_t c):
    data(uint16(c))
{
}
ky_char::ky_char( ushort c):
    data(c)
{
}

ky_char::~ky_char()
{

}

ky_char::operator char () const
{
    return to_latin1();
}
ky_char::operator wchar_t () const
{
    return wchar_t(data);
}
ky_char::operator ushort &()
{
    return data;
}
ky_char::operator ushort ()const
{
    return data;
}
ushort ky_char::unicode()const
{
    return data;
}
ushort &ky_char::unicode()
{
    return data;
}

ky_char& ky_char::operator=(ushort c)
{
    data = c;
    return *this;
}
ky_char& ky_char::operator=(char c)
{
    data = from_latin1(c).data ;
    return *this;
}
ky_char& ky_char::operator=(wchar_t c)
{
    data = ushort(c);
    return *this;
}
ky_char& ky_char::operator=(const ky_char &c)
{
    data = c.data;
    return *this;
}

bool ky_char::operator==(char c) const
{
    return ky_char(c).data == data;
}
bool ky_char::operator==(wchar_t c)const
{
    return data == ky_char(c).data;
}
bool ky_char::operator==(const ky_char &c)const
{
    return data == c.data;
}

bool ky_char::operator!=(const ky_char& c)const
{
    return !((*this) == c);
}
bool ky_char::operator!=(wchar_t c)const
{
    return !((*this) == c);
}
bool ky_char::operator!=(char c)const
{
    return !((*this) == c);
}

bool ky_char::is_letter_number() const
{
    return isLetterOrNumber(data);
}
bool ky_char::is_title_case() const
{
    return isTitleCase(data);
}
bool ky_char::is_non_char() const
{
    return isNonCharacter(data);
}
bool ky_char::is_high_surrogate() const
{
    return isHighSurrogate(data);
}
bool ky_char::is_low_surrogate() const
{
    return isLowSurrogate(data);
}
bool ky_char::is_surrogate() const
{
    return isSurrogate(data);
}
bool ky_char::is_letter() const
{
    return isLetter(data);
}
bool ky_char::is_unicode() const
{
    return !is_letter();
}
bool ky_char::is_upper() const
{
    return isUpper(data);
}
bool ky_char::is_symbol() const
{
    return isSymbol(data);
}
bool ky_char::is_mark()const
{
    return isMark(data);
}
bool ky_char::is_punct() const
{
    return isPunct(data);
}
bool ky_char::is_print() const
{
    return isPrint(data);
}
bool ky_char::is_space() const
{
    return isSpace(data);
}
bool ky_char::is_digit() const
{
    return isDigit(data);
}
bool ky_char::is_number() const
{
    return isNumber(data);
}
bool ky_char::is_lower() const
{
    return isLower(data);
}
bool ky_char::is_valid()const
{
    return data == 0;
}

void ky_char::lower()
{
    data = toLower(data);
}
void ky_char::upper()
{
    data = toUpper(data);
}
void ky_char::reverse()
{
    if (!is_letter())
        return ;

    if (is_lower())
        upper();
    else
        lower();
}

int ky_char::digit() const
{
    return digitValue(data);
}
char ky_char::to_latin1()const
{
    return data > 0xff ? '\0' : char(data);
}
ky_char ky_char::to_lower() const
{
    return ky_char((ushort)(toLower(data) &0XFFFF));
}
ky_char ky_char::to_upper() const
{
    return (ushort)(toUpper(data)&0xFFFF);
}
ky_char ky_char::to_title_case() const
{
    return (ushort)(toTitleCase(data)&0xFFFF);
}
ky_char ky_char::to_case_folded() const
{
    return (ushort)(toCaseFolded(data)&0xFFFF);
}
ky_char ky_char::from_latin1(char c)
{
    return ky_char(ushort(uchar(c)));
}

