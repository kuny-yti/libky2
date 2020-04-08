
/**
 * Basic tool library
 * Copyright (C) 2014 kunyang kunyang.yk@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * You should have received a copy of the GNU General Public License.
 * If not, see <http://www.gnu.org/licenses/>.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file     ky_string.h
 * @brief    字符串类
 *           1.国际字符集存储
 *           2.数字-字符串转换
 *           3.字符集转换
 *
 * @author   kunyang
 * @email    kunyang.yk@gmail.com
 * @version  1.3.3.1
 * @date     2013/08/20
 * @license  GNU General Public License (GPL)
 *
 * Change History :
 *    Date    |  Version  |  Author  |   Description
 * 2013/08/20 | 1.0.0.1   | kunyang  | 创建文件
 * 2014/02/21 | 1.0.1.1   | kunyang  | 修改宽字符转换
 * 2014/05/03 | 1.1.0.1   | kunyang  | 将字符存储修改为int16类型并加入字符类
 * 2015/10/13 | 1.2.0.1   | kunyang  | 重构字符串类，并加入国际字符编码
 * 2016/03/11 | 1.2.1.1   | kunyang  | 使用utf8格式作为字符串存储方式，实现字符串截取功能
 * 2016/06/01 | 1.2.2.1   | kunyang  | 修复字符串删除内存出错
 * 2016/08/21 | 1.3.0.1   | kunyang  | 加入数字和字符串相互转换
 * 2017/02/25 | 1.3.1.1   | kunyang  | 加入字符串包含功能函数，和字符串修剪功能
 * 2018/05/17 | 1.3.2.1   | kunyang  | 修复linux系统下宽字符集转换错误
 * 2019/05/07 | 1.3.3.1   | kunyang  | 修复trimmed函数错误
 */
#ifndef KY_STRING_H
#define KY_STRING_H

#include "ky_define.h"
#include "ky_list.h"
#include "ky_array.h"
#include "ky_byte.h"
#include "ky_variant.h"

class ky_string;

typedef enum
{
    UTF_UnSet=0, // 不是UTF BOM
    UTF_Not,     // 不是UTF 编码
    UTF_8,       // UTF-8 本机 CPU 字节序
    UTF_16,      // UTF-16 本机 CPU 字节序
    UTF_16be,    // UTF-16 大端字节序
    UTF_16le,    // UTF-16 小端字节序
    UTF_32,      // UTF-32 本机 CPU 字节序
    UTF_32be,    // UTF-32 大端字节序
    UTF_32le     // UTF-32 小端字节序
}eUnicodeEncodings;

class ky_char
{
public:
    static ky_char from_latin1(char c);

public:
    ky_char();
    ky_char( const ky_char &);
    ky_char( char);
    ky_char( wchar_t);
    ky_char(uint16);

    ~ky_char();

    //!
    //! \brief digit
    //! \return
    //!
    int digit() const ;
    //!
    //! \brief to_latin1
    //! \return
    //!
    char to_latin1() const;
    //!
    //! \brief to_lower
    //! \return
    //!
    ky_char to_lower() const ;
    //!
    //! \brief to_upper
    //! \return
    //!
    ky_char to_upper() const ;
    //!
    //! \brief to_title_case
    //! \return
    //!
    ky_char to_title_case() const ;
    //!
    //! \brief to_case_folded
    //! \return
    //!
    ky_char to_case_folded() const ;

    //!
    //! \brief operator char wchar_t uint16 作为字符
    //!
    operator char () const;
    operator wchar_t () const;
    operator uint16 ()const;
    operator uint16 &();

    //!
    //! \brief unicode
    //! \return
    //!
    uint16 unicode()const;
    uint16 &unicode() ;

    //! 判断是否是空子符'\0'
    bool is_valid()const;
    //!
    //! \brief is_letter_number 是否字母或数字
    //! \return
    //!
    bool is_letter_number() const;
    //!
    //! \brief is_title_case
    //! \return
    //!
    bool is_title_case() const;
    //!
    //! \brief is_non_char
    //! \return
    //!
    bool is_non_char() const ;
    //!
    //! \brief is_high_surrogate
    //! \return
    //!
    bool is_high_surrogate() const ;
    //!
    //! \brief is_low_surrogate
    //! \return
    //!
    bool is_low_surrogate() const ;
    //!
    //! \brief is_surrogate
    //! \return
    //!
    bool is_surrogate() const ;
    //!
    //! \brief is_digit 是否是十进制数字（'0' - '9'）
    //! \return
    //!
    bool is_digit() const;
    //!
    //! \brief is_number 是否是数字，包括正负号、小数点等
    //! \return
    //!
    bool is_number() const;
    //!
    //! \brief is_lower 是否小写字母
    //! \return
    //!
    bool is_lower() const;
    //!
    //! \brief is_upper  是否大写字母
    //! \return
    //!
    bool is_upper() const;
    //!
    //! \brief is_letter  是否字母
    //! \return
    //!
    bool is_letter() const;
    //!
    //! \brief is_space 是否分隔符，包括空格等
    //! \return
    //!
    bool is_space() const;
    //!
    //! \brief is_punct
    //! \return
    //!
    bool is_punct() const;
    //!
    //! \brief is_print 是否是可打印字符
    //! \return
    //!
    bool is_print() const;
    //!
    //! \brief is_symbol 是否是字符
    //! \return
    //!
    bool is_symbol() const;
    //!
    //! \brief is_mark
    //! \return
    //!
    bool is_mark()const;
    //!
    //! \brief is_unicode
    //! \return
    //!
    bool is_unicode() const;

    ky_char& operator=(uint16);
    ky_char& operator=(char);
    ky_char& operator=(wchar_t);
    ky_char& operator=(const ky_char &);

    bool operator==(char) const;
    bool operator==(wchar_t)const ;
    bool operator==(const ky_char &)const ;

    bool operator!=(const ky_char&)const ;
    bool operator!=(wchar_t)const ;
    bool operator!=(char)const ;

    //!
    //! \brief lower 小写
    //!
    void lower();
    //!
    //! \brief upper 大写
    //!
    void upper();
    //!
    //! \brief reverse 取反
    //!
    void reverse();

private:
    uint16  data;
};


typedef ky_list<ky_string> ky_string_list;
typedef ky_array<u8> ky_utf8;
typedef ky_array<u16> ky_utf16;
typedef ky_array<u32> ky_utf32;

class ky_string : public ky_utf16
{
public:
    ky_string();
    virtual ~ky_string();

    ky_string( const ky_string& );
    ky_string( const std::string& );
    ky_string( const std::wstring& );
    ky_string( const ky_char*, int /*length*/ );
    ky_string( const ky_utf16 &array);
    ky_string( const char* );
    ky_string( const char*, int /*length*/ );
    ky_string( const wchar_t* src );
    ky_string( const wchar_t* src, int length );

    std::string to_std()const;
    std::wstring to_wstd()const;

public:
    //!
    //! \brief data  作为字符串
    //! \return
    //!
    ky_char* data();
    const ky_char* data() const;

    operator ky_char* () const;
    operator char* () const;
    operator wchar_t* () const;

    //!
    //! \brief operator [] 字符访问重载
    //! \return
    //!
    ky_char& operator[](int);
    ky_char operator[](int) const;
    //!
    //! \brief at 字符访问
    //! \return
    //!
    ky_char at(int) const;
    void assign(int, ky_char);

public:
    //!
    //! \brief length 字符串长度
    //! \return
    //!
    int64 length() const;
    //!
    //! \brief count 字符串数(等同length())
    //! \return
    //!
    int count() const;

    //!
    //! \brief is_latin1 是否拉丁字符串
    //! \return
    //!
    bool is_latin1() const;
    //!
    //! \brief is_unicode 是否国际字符集
    //! \return
    //!
    bool is_unicode() const;
    //!
    //! \brief is_number 是否数字字符串
    //! \return
    //!
    bool is_number() const;


public:
    //!
    //! \brief contains 包含指定字符串
    //! \param
    //! \return
    //!
    bool contains(const char* )const;
    bool contains(const wchar_t* )const;
    bool contains(const ky_string &)const;
    bool contains(const std::string &)const;
    bool contains(const std::wstring &)const;

public:
    //!
    //! \brief append 字符串添加
    //! \param
    //! \return
    //!
    ky_string& append( const char);
    ky_string& append( const wchar_t);
    ky_string& append( const ky_char &c);
    ky_string& append( const char*, int =-1);
    ky_string& append( const ky_char*, int );
    ky_string& append( const wchar_t*, int =-1);
    ky_string& append( const ky_string& );
    ky_string& append( const std::string& );
    ky_string& append( const std::wstring& );

public:
    //!
    //! \brief insert 字符串插入
    //! \param pos
    //! \param
    //! \param
    //! \return
    //!
    ky_string& insert(int pos,  const char*, int =-1);
    ky_string& insert(int pos,  const ky_char*, int );
    ky_string& insert(int pos,  const wchar_t*, int =-1);
    ky_string& insert(int pos,  const ky_string&);
    ky_string& insert(int pos,  const std::string&);
    ky_string& insert(int pos,  const std::wstring&);

public:
    //!
    //! \brief remove 字符串删除
    //! \param pos
    //! \param len
    //! \return
    //!
    ky_string& remove(int pos, int len);
    ky_string& remove(const char*, int =-1);
    ky_string& remove(const ky_char*, int );
    ky_string& remove(const wchar_t*, int =-1);
    ky_string& remove(const ky_string &str);
    ky_string& remove(const std::string &str);
    ky_string& remove(const std::wstring &str);

public:
    //!
    //! \brief replace 字符串替换
    //! \param pos
    //! \param len
    //! \param token
    //! \return
    //!
    int replace( int pos, int len, const ky_string &token );
    int replace( const char* token1, const char* token2 );
    int replace( const wchar_t* token1, const wchar_t* token2 );
    int replace( const ky_string &token1, const ky_string &token2 );
    int replace( const std::string &token1, const std::string &token2 );
    int replace( const std::wstring &token1, const std::wstring &token2 );

public:
    //!
    //! \brief extract 字符串提取
    //! \param pos
    //! \param count 负数时根据pos位置向前取，此时顺序不为逆
    //! \return
    //!
    ky_string extract( int pos, int count)const;
    //!
    //! \brief extract  根据pos位置将后面的全部提取
    //! \param pos
    //! \return
    //!
    ky_string extract( int pos )const ;
    //!
    //! \brief start  从开头提取count个字符
    //! \param count
    //! \return
    //!
    ky_string start( int count)const;
    //!
    //! \brief end  从结尾提取count个字符，此时不为逆
    //! \param count
    //! \return
    //!
    ky_string end( int count) const ;
    //!
    //! \brief trimmed  修饰字符串
    //! \return
    //!
    ky_string trimmed() const;

public:
    //!
    //! \brief find 字符串查找
    //! \param
    //! \return
    //!
    int find(const char*) const;
    int find(const wchar_t*) const;
    int find(const ky_string&) const;
    int find(const std::string &) const;
    int find(const std::wstring &) const;

public:
    //!
    //! \brief split 字符串分割
    //! \param
    //! \return
    //!
    ky_string_list split(const char*)const;
    ky_string_list split(const wchar_t*)const;
    ky_string_list split(const ky_string&)const;
    ky_string_list split(const std::string &)const;
    ky_string_list split(const std::wstring &)const;

public:
    //!
    //! \brief compare 字符串比较
    //! \param
    //! \return
    //!
   int compare(const char*)const;
   int compare(const wchar_t*)const;
   int compare(const ky_string&)const;
   int compare(const std::string &)const;
   int compare(const std::wstring &)const;

public:
   //!
   //! \brief format 格式输入
   //! \param
   //! \param ...
   //! \return
   //!
   ky_string& format( const char*, ...) ;
   ky_string& format( const wchar_t*, ...);

   static ky_string formats( const char*, ...) ;
   static ky_string formats( const wchar_t*, ...);

   //! ? and * 通配符
public:


public:
   //!
   //! \brief operator = 重载 =
   //! \param
   //! \return
   //!
   ky_string& operator=(const char*);
   ky_string& operator=(const wchar_t* );
   ky_string& operator=(const ky_string&);
   ky_string& operator=(const std::string &);
   ky_string& operator=(const std::wstring &);

public:
   //!
   //! \brief operator == 重载 ==
   //! \param str
   //! \return
   //!
   bool operator==(const char*str)const {return compare(str) == 0;}
   bool operator==(const wchar_t* str)const {return compare(str) == 0;}
   bool operator==(const ky_string&str)const {return compare(str) == 0;}
   bool operator==(const std::string &str)const {return compare(str) == 0;}
   bool operator==(const std::wstring &str)const {return compare(str) == 0;}

public:
   //!
   //! \brief operator != 重载 !=
   //! \param str
   //! \return
   //!
   bool operator!=(const char*str)const {return !((*this) == str);}
   bool operator!=(const wchar_t* str)const{return !((*this) == str);}
   bool operator!=(const ky_string&str)const{return !((*this) == str);}
   bool operator!=(const std::string &str)const{return !((*this) == str);}
   bool operator!=(const std::wstring &str)const{return !((*this) == str);}

public:
   //!
   //! \brief operator +  重载 +
   //! \param
   //! \return
   //!
   ky_string operator+(const char) const;
   ky_string operator+(const wchar_t) const;
   ky_string operator+(const char*) const;
   ky_string operator+(const wchar_t*) const;
   ky_string operator+(const ky_string&) const;
   ky_string operator+(const std::string&) const;
   ky_string operator+(const std::wstring&) const;

public:
   //!
   //! \brief operator += 重载 +=
   //! \param c
   //! \return
   //!
   const ky_string& operator+=(const char c){return append(c);}
   const ky_string& operator+=(const wchar_t c){return append(c);}
   const ky_string& operator+=(const char*str){return append(str);}
   const ky_string& operator+=(const wchar_t*str){return append(str);}
   const ky_string& operator+=(const ky_string&str){return append(str);}
   const ky_string& operator+=(const std::string&str){return append(str);}
   const ky_string& operator+=(const std::wstring&str){return append(str);}

public:
   //!
   //! \brief operator < 重载 <
   //! \param str
   //! \return
   //!
   bool operator < (const ky_string&str)const;
   bool operator > (const ky_string&str)const;

public:
   //!
   //! \brief to_latin1 转换字符串为拉丁文
   //! \return
   //!
   char* to_latin1()const;
   //!
   //! \brief to_wchar 转换字符串为宽字符串
   //! \return
   //!
   wchar_t* to_wchar()const;
   //!
   //! \brief to_utf8
   //! \return
   //!
   ky_utf8 to_utf8()const;
   //!
   //! \brief to_utf16
   //! \return
   //!
   ky_utf16 to_utf16()const;
   //!
   //! \brief to_utf32
   //! \return
   //!
   ky_utf32 to_utf32()const;
   //!
   //! \brief to_byte
   //! \return
   //!
   ky_byte to_byte()const;

   //!
   //! \brief upper
   //!
   void upper();
   //!
   //! \brief lower
   //!
   void lower();
   //!
   //! \brief reverse
   //!
   void reverse();

public:
   //!
   //! \brief from_latin1
   //! \return
   //!
   bool from_latin1(const char*);
   bool from_wchar(const wchar_t*);
   bool from_utf8(const uint8*, int );
   bool from_utf16(const uint16*, int );
   bool from_utf32(const uint32*, int );

   //!
   //! \brief to_utf8s
   //! \return
   //!
   static ky_utf8 to_utf8s(uint16*, int) ;
   static ky_utf8 to_utf8s(uint32*, int) ;
   static ky_utf16 to_utf16s(uint8*, int) ;
   static ky_utf16 to_utf16s(uint32 *, int) ;
   static ky_utf32 to_utf32s(uint8*, int) ;
   static ky_utf32 to_utf32s(uint16*, int) ;

   static ky_string from(const void *, int);
   static bool has_unicode(void*, int);
   static bool has_ascii(void*, int);
   static bool has_utf8(void*, int);
   static bool has_utf16(void*, int);
   static bool has_utf32(void*, int);

public:
   //!
   //! \brief from  数字输入
   //! \param
   //! \param radix 代表需要存储为什么进制
   //! \param prec 代表精度
   //! \return
   //!
   ky_string &from(bool, int radix = 10);
   ky_string &from(char, int radix = 10);
   ky_string &from(uchar, int radix = 10);
   ky_string &from(short, int radix = 10);
   ky_string &from(uint16, int radix = 10);
   ky_string &from(int, int radix = 10);
   ky_string &from(uint, int radix = 10);
   ky_string &from(long, int radix = 10);
   ky_string &from(ulong, int radix = 10);
   ky_string &from(longlong, int radix = 10);
   ky_string &from(ulonglong, int radix = 10);
   ky_string &from(float, int prec=10);
   ky_string &from(real, int prec=10);

   template<typename T>
   static ky_string number(T v, int radix = 10){ky_string str; str.from (v, radix);return str;}

public:
   //!
   //! \brief to_? 数字输出
   //! \param radix 代表存储的什么进制
   //! \return
   //!
   bool      to_bool(int radix = 10)const;
   char      to_char(int radix = 10)const;
   uchar     to_uchar(int radix = 10)const;
   short     to_short(int radix = 10)const;
   ushort    to_ushort(int radix = 10)const;
   int       to_int(int radix = 10)const;
   uint      to_uint(int radix = 10)const;
   long      to_long(int radix = 10)const;
   ulong     to_ulong(int radix = 10)const;
   longlong  to_longlong(int radix = 10)const;
   ulonglong to_ulonglong(int radix = 10)const;
   float     to_float()const;
   real      to_real()const;

private:
   ky_array<char> s_latin1;
   ky_array<wchar_t> s_unicode;
};

const fourwd_t kyStringCode = kyFourWord(strx);
#include "ky_stream.h"
ky_stream &operator << (ky_stream &in, const ky_string &v);
ky_stream &operator >> (ky_stream &out, ky_string &v);

#endif

