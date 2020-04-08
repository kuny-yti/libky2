
#include "tools/ky_string.h"
#include "ky_unicode.h"
#include "ky_ucsprivate.h"
#include "ky_number_double.h"
#include "ky_number_integer.h"
#include "ky_debug.h"

/////////////////////////ky_string////////////////////////////////
ky_string::ky_string():
    ky_utf16()
{
}
ky_string::ky_string( const ky_string& str):
    ky_utf16()
{
     *this = str;
}

ky_string::ky_string( const std::string& str):
    ky_utf16()
{
    *this = str;
}

ky_string::ky_string( const std::wstring& str):
    ky_utf16()
{
    *this = str;
}

ky_string::ky_string( const ky_char* str, int len ):
    ky_utf16((uint16*)str, len)
{
}
ky_string::ky_string(const ky_utf16 &array):
    ky_utf16(array)
{

}
ky_string::ky_string( const char* str):
    ky_utf16()
{
    *this = str;
}

ky_string::ky_string( const char*str, int len ):
    ky_utf16()
{
    this->append(str, len);
}
ky_string::ky_string( const wchar_t* str ):
    ky_utf16()
{
    *this = str;
}

ky_string::ky_string( const wchar_t* str, int len ):
    ky_utf16()
{
    this->append(str, len);
}
ky_string::~ky_string()
{
}
std::string ky_string::to_std() const
{
    return std::string((char*)to_utf8().data ());
}
std::wstring ky_string::to_wstd()const
{
    return std::wstring(to_wchar ());
}
/////////////////////return////////////////////////////
ky_char* ky_string::data()
{
    return (ky_char* )ky_utf16::data();
}
const ky_char* ky_string::data() const
{
    return (ky_char* )ky_utf16::data();
}

ky_char& ky_string::operator[](int index)
{
    kyASSERT((int64)index >= length());
    return (*(ky_char*)(data()+ index));
}
ky_char ky_string::operator[](int index) const
{
    kyASSERT((int64)index >= length());
    return (*(ky_char*)(data()+ index));
}
ky_char ky_string::at(int index) const
{
    return this->operator [](index);
}
void ky_string::assign(int index, ky_char c)
{
    kyASSERT((int64)index >= length());
    this->data()[index] = c.unicode();
}

ky_string::operator ky_char* () const
{
    ky_string *take = (ky_string *)this;
    return take->data();
}
ky_string::operator char* () const
{
    return (char*)to_latin1 ();
}
ky_string::operator wchar_t* () const
{
    return to_wchar();
}

////////////////////////is_//////////////////////////////////

int64 ky_string::length() const
{
    return this->size();
}
int ky_string::count() const
{
    return (int)length();
}

bool ky_string::is_latin1() const
{
    for (int64 i = 0; i < length(); ++i)
    {
        if (!this->data()[i].is_letter())
            return false;
    }
    return true;
}
bool ky_string::is_unicode() const
{
    for (int64 i = 0; i < length(); ++i)
    {
        if (!this->data()[i].is_unicode())
            return false;
    }
    return true;
}
bool ky_string::is_number() const
{
    for (int64 i = 0; i < length(); ++i)
    {
        if (!this->data()[i].is_number())
            return false;
    }
    return true;
}
///////////////////////////contains///////////////////////////////////////
bool ky_string::contains(const char* str)const
{
    int fs = find(str);
    return fs >= 0;
}
bool ky_string::contains(const wchar_t* str)const
{
    int fs = find(str);
    return fs >= 0;
}
bool ky_string::contains(const ky_string &str)const
{
    int fs = find(str);
    return fs >= 0;
}
bool ky_string::contains(const std::string &str)const
{
    int fs = find(str);
    return fs >= 0;
}
bool ky_string::contains(const std::wstring &str)const
{
    int fs = find(str);
    return fs >= 0;
}

//////////////////////////append///////////////////////////////
ky_string& ky_string::append( const char c)
{
    uint16 tmp = 0;
    ::from_latin1(&c, 1, &tmp);
    ky_utf16::append(&tmp, 1);
    return *this;
}
ky_string& ky_string::append( const wchar_t c)
{
    uint16 tmp = 0;
    if (!kyHasUTF32)
        ::from_utf16(reinterpret_cast<const uint16 *>(&c), 1, &tmp);
    else
        ::from_ucs4(reinterpret_cast<const uint32 *>(&c), 1, &tmp);
    ky_utf16::append(&tmp, 1);
    return *this;
}
ky_string& ky_string::append( const ky_char& c)
{
    ky_utf16::append((uint16*)&c, 1);
    return *this;
}
ky_string& ky_string::append( const char*str, int len)
{
    if (str == 0)
        return *this;
    if (len == -1)
        len = strlen(str);
    uint16 tmp[len+1];
    ::from_latin1(str, len, tmp);
    ky_utf16::append(tmp, len);
    return *this;
}
ky_string& ky_string::append( const ky_char* str, int len)
{
    if (str == NULL)
        return *this;
    ky_utf16::append((uint16*)str, len);
    return *this;
}
ky_string& ky_string::append( const wchar_t* str, int len)
{
    if (str == NULL)
        return *this;
    if (len == -1)
        len = wcslen(str);
    uint16 tmp[len+1];
    if (!kyHasUTF32)
        ::from_utf16(reinterpret_cast<const uint16 *>(str), len, tmp);
    else
        ::from_ucs4(reinterpret_cast<const uint32 *>(str), len, tmp);

    ky_utf16::append(tmp, len);
    return *this;
}
ky_string& ky_string::append( const ky_string& rhs)
{
    if (!rhs.length())
        return *this;

    ky_utf16::append(rhs);
    return *this;
}
ky_string& ky_string::append( const std::string& rhs)
{
    return append(rhs.data(), rhs.length());
}
ky_string& ky_string::append( const std::wstring& rhs)
{
    return append(rhs.data(), rhs.length());
}

/////////////////////////insert////////////////////////////////
ky_string& ky_string::insert(int pos,  const char*str, int len)
{
    if (pos < 0 || !str || !(*str))
        return *this;
    if (len == -1)
        len = strlen(str);
    uint16 tmp[len+1] ;
    ::from_latin1(str, len, tmp);
    ky_utf16::insert(pos, tmp, len);
    return *this;
}

ky_string& ky_string::insert(int pos,  const ky_char*str, int len)
{
    if (pos < 0 || len <= 0 || !str)
        return *this;

    ky_utf16::insert(pos, (uint16*)str, len);
    return *this;
}

ky_string& ky_string::insert(int pos,  const wchar_t*str, int len)
{
    if (len == -1)
        len = wcslen(str);

    uint16 tmp[len+1] ;
    if (!kyHasUTF32)
        ::from_utf16(reinterpret_cast<const uint16 *>(str), len, tmp);
    else
        ::from_ucs4(reinterpret_cast<const uint *>(str), len, tmp);

    return insert(pos, (ky_char*)tmp, len);
}

ky_string& ky_string::insert(int pos,  const ky_string& str)
{
    return insert(pos, str.data(), str.length());
}

ky_string& ky_string::insert(int pos,  const std::string&str)
{
    return insert(pos, str.data(), str.length());
}

ky_string& ky_string::insert(int pos,  const std::wstring& str)
{
    return insert(pos, str.data(), str.length());
}

///////////////////////remove//////////////////////////////////
ky_string& ky_string::remove(int pos, int len)
{
    ky_utf16::remove(pos, len);
    return *this;
}
ky_string& ky_string::remove(const char*str, int len)
{
    if (str != NULL)
    {
        if (len == -1)
            len = strlen(str);
        int i = find(str);
        if (i >= 0)
            remove(i, len);
    }
    return *this;
}
ky_string& ky_string::remove(const ky_char* str, int len)
{
    if (str != NULL)
    {
        int i = find(ky_string(str, len));
        if (i >= 0)
            remove(i, len);
    }
    return *this;
}
ky_string& ky_string::remove(const wchar_t*str, int len)
{
    if (str)
    {
        if (len == -1)
            len = wcslen(str);
        int i = find(str);
        if (i >= 0)
            remove(i, len);
    }
    return *this;
}
ky_string& ky_string::remove(const ky_string &str)
{
    if (str.length())
    {
        int i = find(str);
        if (i >= 0)
            remove(i, str.length());
    }
    return *this;
}
ky_string& ky_string::remove(const std::string &str)
{
    return remove(str.data(), str.length());
}
ky_string& ky_string::remove(const std::wstring &str)
{
    return remove(str.data(), str.length());
}

////////////////////////////replace////////////////////////////////////
int ky_string::replace( int pos, int len , const ky_string &str)
{
    ky_utf16::replace(pos, len, str);
    return contains(str) ? pos : -1;
}
int ky_string::replace( const char* token1, const char* token2 )
{
    int i = find(token1);
    if (i >= 0)
        return replace(i, strlen(token1), ky_string(token2));
    return -1;
}

int ky_string::replace( const wchar_t* token1, const wchar_t* token2 )
{
    int i = find(token1);
    if (i >= 0)
        return replace(i, wcslen(token1), ky_string(token2));
    return -1;
}
int ky_string::replace( const ky_string &token1, const ky_string &token2 )
{
    int i = find(token1);
    if (i >= 0)
        return replace(i, token1.length(), token2);
    return -1;
}
int ky_string::replace( const std::string &token1, const std::string &token2 )
{
    int i = find(token1);
    if (i >= 0)
        return replace(i, token1.length(), ky_string(token2));
    return -1;
}
int ky_string::replace( const std::wstring &token1, const std::wstring &token2 )
{
    int i = find(token1);
    if (i >= 0)
        return replace(i, token1.length(), ky_string(token2));
    return -1;
}
/////////////////////////extract////////////////////////////////
//! count 负数时根据pos位置向前取，此时顺序不为逆
ky_string ky_string::extract( int pos, int count)const
{
    return ky_utf16::extract(pos, count);
}
//! 根据pos位置将后面的全部提取
ky_string ky_string::extract( int pos )const
{
    return  ky_utf16::extract(pos);
}
//! 从开头提取count个字符
ky_string ky_string::start( int count)const
{
    return ky_utf16::start(count);
}
//! 从结尾提取count个字符，此时不为逆
ky_string ky_string::end( int count)const
{
    return ky_utf16::end (count);
}
ky_string ky_string::trimmed()const
{
    const ky_char *dh = data();
    ky_char *dc = (ky_char *)dh;
    ky_char *de = dc + (count()-1);
    while ((dc != de) && dc->is_space ())
        dc++;
    while ((dc != de) && (de->is_space () || de->unicode() == 0))
        de--;

    return ky_string(dc, (de - dc)+1);
}

/////////////////////////find/////////////////////////////////////////
int ky_string::find(const char* str) const
{
    return find(ky_string(str));
}
int ky_string::find(const wchar_t*str) const
{
    return find(ky_string(str));
}
int ky_string::find(const ky_string& str) const
{
    return findString((ky_char*)data(),length(), (ky_char*)str.data (), str.length ());
}
int ky_string::find(const std::string &str) const
{
    return find(ky_string(str));
}
int ky_string::find(const std::wstring &str) const
{
    return find(ky_string(str));
}

//////////////////////////////////////////////////////////////
ky_string_list ky_string::split(const char*str)const
{
    return split(ky_string(str));
}
ky_string_list ky_string::split(const wchar_t*str)const
{
    return split(ky_string(str));
}

ky_string_list ky_string::split(const ky_string&rhs)const
{
    ky_string_list substr;
    int i = 0;
    int back_i = 0;
    int len = length();
    while ((i = findString((ky_char*)data()+back_i, len-back_i, (ky_char*)rhs.data(), rhs.length())) != -1)
    {
        substr.append(ky_string(data()+back_i, i));

        back_i += i + rhs.length ();
    }
    if (len)
        substr.append(ky_string(data()+back_i, len - back_i));
    return substr;
}
ky_string_list ky_string::split(const std::string &str)const
{
    return split(ky_string(str));
}
ky_string_list ky_string::split(const std::wstring &str)const
{
    return split(ky_string(str));
}

///////////////////////split_path//////////////////////////////

///////////////////////////compare////////////////////////
int ky_string::compare(const char*str)const
{
    return compare(ky_string(str));
}
int ky_string::compare(const wchar_t*str)const
{
    return compare(ky_string(str));
}
int ky_string::compare(const ky_string&str)const
{
    return compare_helper(data(), length(), str.data(), str.length());
}
int ky_string::compare(const std::string &str)const
{
    return compare(ky_string(str));
}
int ky_string::compare(const std::wstring &str)const
{
    return compare(ky_string(str));
}

#include <stdarg.h>
//////////////////////////////////////////////////////
#define MAX_MSG_LENGTH 2048
ky_string& ky_string::format( const char*sFormat, ...)
{
    char sMessage[MAX_MSG_LENGTH];
    va_list args;

    /* put formatted message in sMessage */
    ky_memory::zero (sMessage,sizeof(sMessage),'\0');
    if (sFormat)
    {
        va_start(args, sFormat);
        vsnprintf(sMessage, MAX_MSG_LENGTH-1, sFormat, args);
        sMessage[MAX_MSG_LENGTH-1] = '\0';
        va_end(args);
    }

    return this->append(sMessage, strlen(sMessage));
}
ky_string& ky_string::format( const wchar_t*sFormat, ...)
{
    wchar_t sMessage[MAX_MSG_LENGTH];
    va_list args;

    /* put formatted message in sMessage */
    ky_memory::zero(sMessage,sizeof(sMessage),'\0');
    if (sFormat) {
        va_start(args, sFormat);
        vswprintf(sMessage, MAX_MSG_LENGTH-1, sFormat, args);
        sMessage[MAX_MSG_LENGTH-1] = '\0';
        va_end(args);
    }
    return this->append(sMessage, wcslen(sMessage));
}
ky_string ky_string::formats( const char*sFormat, ...)
{
    char sMessage[MAX_MSG_LENGTH];
    va_list args;

    /* put formatted message in sMessage */
    ky_memory::zero(sMessage, sizeof(sMessage), '\0');
    if (sFormat)
    {
        va_start(args, sFormat);
        vsnprintf(sMessage, MAX_MSG_LENGTH-1, sFormat, args);
        sMessage[MAX_MSG_LENGTH-1] = '\0';
        va_end(args);
    }
    return ky_string(sMessage);
}
ky_string ky_string::formats( const wchar_t*sFormat, ...)
{
    wchar_t sMessage[MAX_MSG_LENGTH] = {'\0'};
    va_list args;

    /* put formatted message in sMessage */
    ky_memory::zero(sMessage,sizeof(sMessage), '\0');
    if (sFormat) {
        va_start(args, sFormat);
        vswprintf(sMessage, MAX_MSG_LENGTH-1, sFormat, args);
        sMessage[MAX_MSG_LENGTH-1] = '\0';
        va_end(args);
    }
    return ky_string(sMessage);
}
#undef MAX_MSG_LENGTH
/////////////////////////////////////////
ky_string& ky_string::operator=(const char*str)
{
    clear ();
    append (str);
    return *this;

}
ky_string& ky_string::operator=(const wchar_t* str)
{
    clear ();
    append (str);
    return *this;
}

ky_string& ky_string::operator=(const ky_string& str)
{
    ky_utf16::operator = (str);
    return *this;
}
ky_string& ky_string::operator=(const std::string &str)
{
    clear ();
    append (str.data (), str.length ());
    return *this;
}
ky_string& ky_string::operator=(const std::wstring &str)
{
    clear ();
    append (str.data (), str.length ());
    return *this;
}
///////////////////////////////////////////////////////
ky_string ky_string::operator+(const char c) const
{
    return *this + ky_string(&c, 1);
}
ky_string ky_string::operator+(const wchar_t c) const
{
    return *this + ky_string(&c, 1);
}
ky_string ky_string::operator+(const char*str) const
{
    return *this + ky_string(str);
}
ky_string ky_string::operator+(const wchar_t*str) const
{
    return *this + ky_string(str);
}

ky_string ky_string::operator+(const ky_string&rhs) const
{
    ky_string out(*this);
    out.append(rhs);
    return out;
}
ky_string ky_string::operator+(const std::string&str) const
{
    return *this + ky_string(str);
}
ky_string ky_string::operator+(const std::wstring&str) const
{
    return *this + ky_string(str);
}
bool ky_string::operator < (const ky_string&str)const
{
    return this->compare(str) < 0;
}
bool ky_string::operator > (const ky_string&str)const
{
    return this->compare(str) > 0;
}
////////////////////////to_//////////////////////////////////////////////
char* ky_string::to_latin1()const
{
    ky_string *fake = (ky_string*) this;
    fake->s_latin1.resize(this->length()+1, ky_array<char>::Type());
    ::to_latin1((uchar*)s_latin1.data(), (uint16*)this->data(), this->length());
    return (char*)s_latin1.data();
}
wchar_t* ky_string::to_wchar()const
{
    ky_string *fake = (ky_string*) this;
    fake->s_unicode.resize(this->length()+1, ky_array<wchar_t>::Type());

    if (kyHasUTF32)
        ::to_wchar(fake->s_unicode.data(), (uint16*)this->data(),
                   this->length());
    else
        ky_memory::copy(fake->s_unicode.data(),
                        this->data(), this->bytecount());

    return fake->s_unicode.data();
}
ky_utf8 ky_string::to_utf8()const
{
    return to_utf8s((uint16*)this->data(), length());
}
ky_utf16 ky_string::to_utf16()const
{
    return *this;
}
ky_utf32 ky_string::to_utf32()const
{
    return to_utf32s((uint16*)this->data(), length());
}
ky_byte ky_string::to_byte()const
{
    ky_byte outs;
    outs.resize(sizeOf * length());
    ky_memory::copy (outs.data(), this->data(), length()*sizeOf);
    return outs;
}
void ky_string::upper()
{
    for (int64 i = 0; i < length(); ++i)
        this->data()[i].upper();
}
void ky_string::lower()
{
    for (int64 i = 0; i < length(); ++i)
        this->data()[i].lower();
}
void ky_string::reverse()
{
    for (int64 i = 0; i < length(); ++i)
        this->data()[i].reverse();
}
//////////////////////////////////////////////////////////////////
bool ky_string::from_latin1(const char*str)
{
    *this = ky_string(str, -1);
    return true;
}
bool ky_string::from_wchar(const wchar_t*str)
{
    *this = ky_string(str, -1);
    return true;
}
bool ky_string::from_utf8(const uint8 *utf8, int len)
{
    uint16 utf16[len+1];
    int64 rc = 0;
    uint error_status = 0;
    uint error_mask = 0xFFFFFFFF;
    uint error_code_point = 0xFFFD;
    const uchar* p1 = 0;
    if (!utf8 || len < 1)
        return false;

    rc = ky_unicode::convert_UTF8_UTF16(false, utf8, len, utf16, len,
                                &error_status, error_mask, error_code_point, &p1);

    if (error_status)
        return false;

    if (rc > 0 && rc <= (uint)len)
        utf16[rc] = 0;
    else
    {
        utf16[len-1] = 0;
        rc = 0;
        return false;
    }
    *this = ky_string((ky_char*)utf16, rc);
    return true;
}
bool ky_string::from_utf16(const uint16*utf16, int len)
{
    *this = ky_string((ky_char*)utf16, len);
    return true;
}
bool ky_string::from_utf32(const uint32*utf32, int len )
{
    int utf16len = len*(sizeof(uint)/ sizeof(uint16));
    uint16 utf16[utf16len+1];
    int64 rc = 0;
    uint error_status = 0;
    uint error_mask = 0xFFFFFFFF;
    uint error_code_point = 0xFFFD;
    const uint* p1 = 0;
    if (!utf32)
        return false;

    rc = ky_unicode::convert_UTF32_UTF16(false, utf32, len, utf16, utf16len,
                                &error_status, error_mask, error_code_point, &p1);

    if ( error_status )
        return false;

    if ( rc > 0 && rc <= (uint)utf16len )
        utf16[rc] = 0;
    else
    {
        utf16[len-1] = 0;
        rc = 0;
        return false;
    }
    *this = ky_string((ky_char*)utf16, len);
    return true;
}

/////////////////////////////////////////////////////////////////
ky_utf8 ky_string::to_utf8s(uint16* utf16, int len)
{
    static uchar tempbuf[6] = {0};
    int f16_byte = 0;
    if (len <= 0)
        return ky_utf8();
    for (int i = 0; i < len; ++i)
    {
        int bytes = ky_unicode::encode_UTF8(utf16[i], tempbuf);
        if (f16_byte < bytes)
            f16_byte = bytes;
    }

    ky_utf8 oututf8;
    oututf8.resize(len * f16_byte, '\0');
    int64 rc = 0;
    uint error_status = 0;
    uint error_mask = 0xFFFFFFFF;
    uint error_code_point = 0xFFFD;
    const uint16* p1 = 0;

    if (!utf16)
        return ky_utf8();

    rc = ky_unicode::convert_UTF16_UTF8(false, utf16, len, oututf8.data(), oututf8.size(),
                                &error_status, error_mask, error_code_point, &p1);

    if ( error_status )
        return ky_utf8();

    if ( rc > 0 && rc <= oututf8.size())
        return oututf8;
    return ky_utf8();
}
ky_utf8 ky_string::to_utf8s(uint32* utf32, int len)
{
    static uchar tempbuf[6] = {0};
    int f16_byte = 0;
    if (len <= 0)
        return ky_utf8();
    for (int i = 0; i < len; ++i)
    {
        int bytes = ky_unicode::encode_UTF8(utf32[i], tempbuf);
        if (f16_byte < bytes)
            f16_byte = bytes;
    }
    ky_utf8 oututf8;
    oututf8.resize(len*f16_byte, '\0');
    int64 rc = 0;
    uint error_status = 0;
    uint error_mask = 0xFFFFFFFF;
    uint error_code_point = 0xFFFD;
    const uint* p1 = 0;
    if (!utf32)
        return ky_utf8();

    rc = ky_unicode::convert_UTF32_UTF8(false, utf32, len, oututf8.data(), oututf8.size(),
                                &error_status, error_mask, error_code_point, &p1);

    if ( error_status )
        return ky_utf8();

    if ( rc > 0 && rc <= (uint)oututf8.size() )
        return oututf8;
    return ky_utf8();
}
ky_utf16 ky_string::to_utf16s(uint8 *utf8, int len)
{
    static uint16 tempbuf[2] = {0};
    int f16_byte = 0;
    if (len <= 0)
        return ky_utf16();
    for (int i = 0; i < len; ++i)
    {
        int bytes = ky_unicode::encode_UTF16(utf8[i], tempbuf);
        if (f16_byte < bytes)
            f16_byte = bytes;
    }
    ky_utf16 oututf16;
    oututf16.resize(len*f16_byte, '\0');
    int64 rc = 0;
    uint error_status = 0;
    uint error_mask = 0xFFFFFFFF;
    uint error_code_point = 0xFFFD;
    const uchar* p1 = 0;
    if (!utf8)
        return ky_utf16();

    rc = ky_unicode::convert_UTF8_UTF16(false, utf8, len, oututf16.data(), oututf16.size(),
                                &error_status, error_mask, error_code_point, &p1);

    if ( error_status )
        return ky_utf16();

    if ( rc > 0 && rc <= (uint)oututf16.size() )
        return oututf16;
    return ky_utf16();
}

ky_utf16 ky_string::to_utf16s(uint32* utf32, int len)
{
    static uint16 tempbuf[2] = {0};
    int f16_byte = 0;
    if (len <= 0)
        return ky_utf16();
    for (int i = 0; i < len; ++i)
    {
        int bytes = ky_unicode::encode_UTF16(utf32[i], tempbuf);
        if (f16_byte < bytes)
            f16_byte = bytes;
    }
    ky_utf16 oututf16;
    oututf16.resize(len*f16_byte, '\0');
    int64 rc = 0;
    uint error_status = 0;
    uint error_mask = 0xFFFFFFFF;
    uint error_code_point = 0xFFFD;
    const uint* p1 = 0;
    if (!utf32)
        return ky_utf16();

    rc = ky_unicode::convert_UTF32_UTF16(false, utf32, len, oututf16.data(), oututf16.size(),
                                &error_status, error_mask, error_code_point, &p1);

    if ( error_status )
        return ky_utf16();

    if ( rc > 0 && rc <= (uint)oututf16.size() )
        return oututf16;
    return ky_utf16();
}
ky_utf32 ky_string::to_utf32s(uint8 *utf8, int len)
{
    ky_utf32 oututf32;
    if (len <= 0)
        return ky_utf32();
    oututf32.resize(len, '\0');
    int64 rc = 0;
    uint error_status = 0;
    uint error_mask = 0xFFFFFFFF;
    uint error_code_point = 0xFFFD;
    const uchar* p1 = 0;
    if (!utf8)
        return ky_utf32();

    rc = ky_unicode::convert_UTF8_UTF32(false, utf8, len, oututf32.data(), oututf32.size(),
                                &error_status, error_mask, error_code_point, &p1);

    if ( error_status )
        return ky_utf32();

    if ( rc > 0 && rc <= (uint)oututf32.size() )
        return oututf32;
    return ky_utf32();
}
ky_utf32 ky_string::to_utf32s(uint16* utf16, int len)
{
    ky_utf32 oututf32;
    if (len <= 0)
        return ky_utf32();
    oututf32.resize(len, '\0');
    int64 rc = 0;
    uint error_status = 0;
    uint error_mask = 0xFFFFFFFF;
    uint error_code_point = 0xFFFD;
    const uint16* p1 = 0;
    if (!utf16)
        return ky_utf32();

    rc = ky_unicode::convert_UTF16_UTF32(false, utf16, len, oututf32.data(), oututf32.size(),
                                &error_status, error_mask, error_code_point, &p1);

    if ( error_status )
        return ky_utf32();

    if ( rc > 0 && rc <= (uint)oututf32.size() )
        return oututf32;
    return ky_utf32();
}
ky_string ky_string::from(const void *buf, int len)
{
    ky_string out;
    switch ((int)ky_unicode::utf_code(buf, len))
    {
    case UTF_8:
        if (!out.from_utf8((const uint8*)buf, len))
            return ky_string();
        break;
    case UTF_16be:
    case UTF_16le:
        if (!out.from_utf16((const uint16*)buf, len))
            return ky_string();
        break;
    case UTF_32be:
    case UTF_32le:
        if (!out.from_utf32((const uint32*)buf, len))
            return ky_string();
        break;
    default:
        out = ky_string((char*)buf, len);
        break;
    }
    return out;
}
bool ky_string::has_unicode(void* buf, int len)
{
    return UTF_UnSet != ky_unicode::utf_code(buf, len);
}
bool ky_string::has_ascii(void* buf, int len)
{
    return UTF_UnSet == ky_unicode::utf_code(buf, len);
}
bool ky_string::has_utf8(void* buf, int len)
{
    return UTF_8 == ky_unicode::utf_code(buf, len);
}
bool ky_string::has_utf16(void* buf, int len)
{
    eUnicodeEncodings encod = ky_unicode::utf_code(buf, len);
    return UTF_16le == encod || UTF_16be == encod;
}
bool ky_string::has_utf32(void* buf, int len)
{
    eUnicodeEncodings encod = ky_unicode::utf_code(buf, len);
    return UTF_32le == encod || UTF_32be == encod;
}

/////////////////////////////////////////////////////////
ky_string &ky_string::from(bool value, int radix )
{
    append(longlong_string(value, radix));
    return *this;
}
ky_string &ky_string::from(char value, int radix )
{
    append(longlong_string(value, radix));
    return *this;
}
ky_string &ky_string::from(uchar value, int radix )
{
    append(ulonglong_string(value, radix));
    return *this;
}
ky_string &ky_string::from(short value, int radix )
{
    append(longlong_string(value, radix));
    return *this;
}
ky_string &ky_string::from(uint16 value, int radix )
{
    append(ulonglong_string(value, radix));
    return *this;
}
ky_string &ky_string::from(int value, int radix )
{
    append(longlong_string(value, radix));
    return *this;
}
ky_string &ky_string::from(uint value, int radix )
{
    append(ulonglong_string(value, radix));
    return *this;
}
ky_string &ky_string::from(long value, int radix )
{
    append(longlong_string(value, radix));
    return *this;
}
ky_string &ky_string::from(ulong value, int radix )
{
    append(ulonglong_string(value, radix));
    return *this;
}
ky_string &ky_string::from(longlong value, int radix )
{
    append(longlong_string(value, radix));
    return *this;
}
ky_string &ky_string::from(ulonglong value, int radix )
{
    append(ulonglong_string(value, radix));
    return *this;
}
ky_string &ky_string::from(float value, int prec)
{
    return from(real(value), prec);
}
ky_string &ky_string::from(real value,  int prec)
{
    int decpt = 0, sign = 0, slen = 0;
    char *retfll = NULL;
    double_string(value, 0, prec, &decpt, &sign, NULL, &retfll, &slen);

    ky_string temp(retfll, slen);
    kyFree (retfll);
    if (decpt != slen)
    {
        if (decpt > 0)
            temp.insert(decpt, ".");
        else if (decpt < 0)
        {
            for (;decpt != 1; decpt+=1)
            {
                if (decpt + 1 == 1)
                    temp.insert (0,"0.");
                else
                    temp.insert (0,"0");
            }
        }
        else
            temp.insert(0, "0.");
    }

    if (sign != 0 )
        temp.insert(0, "-");
    append (temp);
    return *this;
}
bool      ky_string::to_bool(int radix )const
{
    return toIntegral_helper<bool>((char*)to_utf8().data(), length(), radix);
}
char      ky_string::to_char(int radix ) const
{
    return toIntegral_helper<char>((char*)to_utf8().data(), length(), radix);
}
uchar     ky_string::to_uchar(int radix ) const
{
    return toUIntegral_helper<uchar>((char*)to_utf8().data(), length(), radix);
}
short     ky_string::to_short(int radix ) const
{
    return toIntegral_helper<short>((char*)to_utf8().data(), length(), radix);
}
ushort    ky_string::to_ushort(int radix )const
{
    return toUIntegral_helper<ushort>((char*)to_utf8().data(), length(), radix);
}
int       ky_string::to_int(int radix ) const
{
    return toIntegral_helper<int>((char*)to_utf8().data(), length(), radix);
}
uint      ky_string::to_uint(int radix ) const
{
    return toUIntegral_helper<uint>((char*)to_utf8().data(), length(), radix);
}
long      ky_string::to_long(int radix ) const
{
    return toIntegral_helper<long>((char*)to_utf8().data(), length(), radix);
}
ulong     ky_string::to_ulong(int radix ) const
{
    return toUIntegral_helper<ulong>((char*)to_utf8().data(), length(), radix);
}
longlong  ky_string::to_longlong(int radix ) const
{
    return toIntegral_helper<longlong>((char*)to_utf8().data(), length(), radix);
}
ulonglong ky_string::to_ulonglong(int radix ) const
{
    return toUIntegral_helper<ulonglong>((char*)to_utf8().data(), length(), radix);
}
float     ky_string::to_float() const
{
    return string_double((char*)to_utf8().data(), NULL);
}
real      ky_string::to_real() const
{
    return string_double((char*)to_utf8().data(), NULL);
}

// 字符串比较
/*bool ky_string::operator==(const ky_string& rhs) const
{
    if (length() != rhs.length())
        return false;
    return memEquals(dptr->data, rhs.dptr->data, rhs.length());
}*/


ky_streamb &operator << (ky_streamb &in, const ky_string &v)
{
    in << ky_byte((uint8*)v.data(), sizeof(uint16)*v.size());
    return in;
}
ky_streamb &operator >> (ky_streamb &out, ky_string &v)
{
    ky_byte tmp;
    out >> tmp;
    v = ky_string((ky_char*)tmp.data(), tmp.size()/sizeof(uint16));
    return out;
}

ky_streamt & operator << (ky_streamt &in, const ky_string &v)
{
    char * tsp = "Unicode";
    in.put(tsp, strlen(tsp));
    in.put((char*)v.data(), v.bytecount());
    tsp = "Unicode";
    in.put(tsp, strlen(tsp));
    return in;
}
ky_streamt & operator >> (ky_streamt &out, ky_string &v)
{
    int len = 0;
    ky_string tsp = out.take(&len);
    if (ky_string("Unicode") == tsp)
    {
        if (out.array)
        {
            char *dat = (char*)out.array->data();
            for (int64 i = 0; i < out.array->size(); ++i)
            {
                if (ky_string("Unicode") == ky_string(&dat[i], strlen("Unicode")))
                {
                    v = ky_string(dat, i);
                    out.array->remove(0, i);
                    tsp = out.take(&len);
                    break;
                }
            }
        }
    }
    return out;
}


