#ifndef UNICODE_H
#define UNICODE_H

#include "tools/ky_string.h"

class ky_unicode
{
public:
    struct ErrorParameters
    {
      /*
      如果发成错误有 error_status 标示错误.

      Error types:
        1: 输入参数错误， 这个错误不能被掩蔽

        2: 转换后的字符串大于输出缓冲区

        4: 当解析UTF-8或UTF-32时，两个连续的编码序列值形成了一个有效的UTF-16对
            if 0 != (4 & m_error_mask).

        8: 一个超长的UTF-8编码序列中遇到的超长UTF-8序列的值是一个有效的Unicode代码点。
            if 0 != (8 & m_error_mask).

       16: 一个非法的UTF-8，UTF-16或UTF-32编码序列发生或无效的Unicode代码点值造成的解码UTF-8序列。
           if 0 != (16 & m_error_mask).
      */
      uint32 m_error_status;

      /*
      If 0 != (error_mask & 4), then type 4 errors are masked.
      If 0 != (error_mask & 8), then type 8 errors are masked.
      If 0 != (error_mask & 16) and m_error_code_point is a valid unicode
      code point value, then type 16 errors are masked.
      */
      uint32 m_error_mask;

      /*
      Unicode code point value to use in when masking type 16 errors.
      If 0 == (error_mask & 16), then this parameter is ignored.
      0xFFFD is a popular choice for the m_error_code_point value.
      */
      uint32 m_error_code_point;
    };

public:
    static int encode_UTF8(uint32 u, uint8 sUTF8[6]);

    static int decode_UTF8(const uint8* sUTF8, int sUTF8_count, ErrorParameters* e,
                           uint32* unicode_code_point );

    static int encode_UTF16(uint32 unicode_code_point, uint16 sUTF16[2] );

    static int decode_UTF16(const uint16* sUTF16, int sUTF16_count, ErrorParameters* e,
                            uint32* unicode_code_point);

    static int decode_swap_byte_UTF16(const uint16* sUTF16, int sUTF16_count, ErrorParameters* e,
                                      uint32* unicode_code_point);

    //!
    //! \brief UTF8转换到UTF16-UTF32
    //!
    static int convert_UTF8_UTF16(int bTestByteOrder,
                                  const uint8* sUTF8, int sUTF8_count,
                                  uint16* sUTF16, int sUTF16_count,
                                  uint32* error_status, uint32 error_mask, uint32 error_code_point,
                                  const uint8** sNextUTF8);
    static int convert_UTF8_UTF32(int bTestByteOrder,
                                  const uint8* sUTF8, int sUTF8_count,
                                  uint32* sUTF32 , int sUTF32_count,
                                  uint32* error_status , uint32 error_mask, uint32 error_code_point,
                                  const uint8** sNextUTF8 );

    //!
    //! \brief UTF16转换到UTF8-UTF32
    //!
    static int convert_UTF16_UTF8(int bTestByteOrder,
                                  const uint16* sUTF16, int sUTF16_count,
                                  uint8* sUTF8, int sUTF8_count,
                                  uint32* error_status, uint32 error_mask, uint32 error_code_point,
                                  const uint16** sNextUTF16 );
    static int convert_UTF16_UTF32(int bTestByteOrder,
                                   const uint16* sUTF16, int sUTF16_count,
                                   uint32* sUTF32, int sUTF32_count,
                                   uint32* error_status, uint32 error_mask, uint32 error_code_point,
                                   const uint16** sNextUTF16 );

    //!
    //! \brief UTF32转换到UTF8-UTF16-UTF32
    //!
    static int convert_UTF32_UTF8(int bTestByteOrder,
                                  const uint32* sUTF32, int sUTF32_count,
                                  uint8* sUTF8, int sUTF8_count,
                                  uint32* error_status, uint32 error_mask, uint32 error_code_point,
                                  const uint32** sNextUTF32);
    static int convert_UTF32_UTF16(int bTestByteOrder,
                                   const uint32* sUTF32, int sUTF32_count,
                                   uint16* sUTF16, int sUTF16_count,
                                   uint32* error_status, uint32 error_mask, uint32 error_code_point,
                                   const uint32** sNextUTF32);
    static int convert_UTF32_UTF32(int bTestByteOrder,
                                   const uint32* sInputUTF32, int sInputUTF32_count,
                                   uint32* sOuputUTF32,  int sOutputUTF32_count,
                                   uint32* error_status, uint32 error_mask, uint32 error_code_point,
                                   const uint32** sNextInputUTF32);

    //!
    //! \brief WideChar转换到UTF8-UTF32
    //!
    static int convert_WideChar_UTF8(int bTestByteOrder,
                                     const wchar_t* sWideChar, int sWideChar_count,
                                     char* sUTF8, int sUTF8_count,
                                     uint32* error_status, uint32 error_mask, uint32 error_code_point,
                                     const wchar_t** sNextWideChar);
    static int convert_WideChar_UTF32(int bTestByteOrder,
                                      const wchar_t* sWideChar, int sWideChar_count,
                                      uint32* sUTF32, int sUTF32_count,
                                      uint32* error_status,  uint32 error_mask, uint32 error_code_point,
                                      const wchar_t** sNextWideChar);

    //!
    //! \brief UTF8转换到WideChar
    //!
    static int convert_UTF8_WideChar(int bTestByteOrder,
                                     const char* sUTF8, int sUTF8_count,
                                     wchar_t* sWideChar, int sWideChar_count,
                                     uint32* error_status, uint32 error_mask, uint32 error_code_point,
                                     const char** sNextUTF8);

    static eUnicodeEncodings utf_code(const void* buffer, int64 sizeof_buffer);
private:
    //! 本机UTF的字节序
    static eUnicodeEncodings native_CPU_UTF16();
    static eUnicodeEncodings native_CPU_UTF32();

    //! 返回 buffer 的字节序
    static eUnicodeEncodings UTF_byte_order_mark(const void* buffer, int64 sizeof_buffer);

    //! 返回 u 是否为有效编码
    static bool is_valid_code_point( uint32 u);

    static int decode_UTF32(const uint32* sUTF32, int sUTF32_count, ErrorParameters* e,
                            uint32* unicode_code_point );
    static int decode_swap_byte_UTF32(const uint32* sUTF32, int sUTF32_count, ErrorParameters* e,
                                      uint32* unicode_code_point );
};

#endif // UNICODE_H
