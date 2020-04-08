
/**
 * Basic tool library
 * Copyright (C) 2014 kunyang kunyang.yk@gmail.com
 *
 * @file     ky_binary.h
 * @brief    定义二进制型的枚举
 *       1.枚举出8位内的所有二进制名称.
 *       2.使用宏将二进制型转化为字符串供系统数据类型转化为有效数据
 *
 * @author   kunyang
 * @email    kunyang.yk@gmail.com
 * @version  1.1.0.1
 * @date     2015/03/08
 * @license  GNU General Public License (GPL)
 *
 * Change History :
 *    Date    |  Version  |  Author  |   Description
 * 2015/03/08 | 1.0.0.1   | kunyang  | 创建文件
 * 2015/03/09 | 1.0.1.0   | kunyang  | 将枚举类型完善到4bit
 * 2015/03/10 | 1.0.2.0   | kunyang  | 将枚举类型完善到8bit
 * 2016/05/20 | 1.1.0.1   | kunyang  | 修改枚举8bit类型建立ky_binary类，并用宏bin来生成二进制类
 */

#ifndef ky_BINARY_H
#define ky_BINARY_H

typedef enum
{
    b00000000 = 0,
    b0000000 = 0,
    b000000 = 0,
    b00000 = 0,
    b0000 = 0,
    b000 = 0,
    b00 = 0,
    b0 = 0,
    b00000001 = 1,
    b0000001 = 1,
    b000001 = 1,
    b00001 = 1,
    b0001 = 1,
    b001 = 1,
    b01 = 1,
    b1 = 1,
    b00000010 = 2,
    b0000010 = 2,
    b000010 = 2,
    b00010 = 2,
    b0010 = 2,
    b010 = 2,
    b10 = 2,
    b00000011 = 3,
    b0000011 = 3,
    b000011 = 3,
    b00011 = 3,
    b0011 = 3,
    b011 = 3,
    b11 = 3,
    b00000100 = 4,
    b0000100 = 4,
    b000100 = 4,
    b00100 = 4,
    b0100 = 4,
    b100 = 4,
    b00000101 = 5,
    b0000101 = 5,
    b000101 = 5,
    b00101 = 5,
    b0101 = 5,
    b101 = 5,
    b00000110 = 6,
    b0000110 = 6,
    b000110 = 6,
    b00110 = 6,
    b0110 = 6,
    b110 = 6,
    b00000111 = 7,
    b0000111 = 7,
    b000111 = 7,
    b00111 = 7,
    b0111 = 7,
    b111 = 7,
    b00001000 = 8,
    b0001000 = 8,
    b001000 = 8,
    b01000 = 8,
    b1000 = 8,
    b00001001 = 9,
    b0001001 = 9,
    b001001 = 9,
    b01001 = 9,
    b1001 = 9,
    b00001010 = 10,
    b0001010 = 10,
    b001010 = 10,
    b01010 = 10,
    b1010 = 10,
    b00001011 = 11,
    b0001011 = 11,
    b001011 = 11,
    b01011 = 11,
    b1011 = 11,
    b00001100 = 12,
    b0001100 = 12,
    b001100 = 12,
    b01100 = 12,
    b1100 = 12,
    b00001101 = 13,
    b0001101 = 13,
    b001101 = 13,
    b01101 = 13,
    b1101 = 13,
    b00001110 = 14,
    b0001110 = 14,
    b001110 = 14,
    b01110 = 14,
    b1110 = 14,
    b00001111 = 15,
    b0001111 = 15,
    b001111 = 15,
    b01111 = 15,
    b1111 = 15,
    b00010000 = 16,
    b0010000 = 16,
    b010000 = 16,
    b10000 = 16,
    b00010001 = 17,
    b0010001 = 17,
    b010001 = 17,
    b10001 = 17,
    b00010010 = 18,
    b0010010 = 18,
    b010010 = 18,
    b10010 = 18,
    b00010011 = 19,
    b0010011 = 19,
    b010011 = 19,
    b10011 = 19,
    b00010100 = 20,
    b0010100 = 20,
    b010100 = 20,
    b10100 = 20,
    b00010101 = 21,
    b0010101 = 21,
    b010101 = 21,
    b10101 = 21,
    b00010110 = 22,
    b0010110 = 22,
    b010110 = 22,
    b10110 = 22,
    b00010111 = 23,
    b0010111 = 23,
    b010111 = 23,
    b10111 = 23,
    b00011000 = 24,
    b0011000 = 24,
    b011000 = 24,
    b11000 = 24,
    b00011001 = 25,
    b0011001 = 25,
    b011001 = 25,
    b11001 = 25,
    b00011010 = 26,
    b0011010 = 26,
    b011010 = 26,
    b11010 = 26,
    b00011011 = 27,
    b0011011 = 27,
    b011011 = 27,
    b11011 = 27,
    b00011100 = 28,
    b0011100 = 28,
    b011100 = 28,
    b11100 = 28,
    b00011101 = 29,
    b0011101 = 29,
    b011101 = 29,
    b11101 = 29,
    b00011110 = 30,
    b0011110 = 30,
    b011110 = 30,
    b11110 = 30,
    b00011111 = 31,
    b0011111 = 31,
    b011111 = 31,
    b11111 = 31,
    b00100000 = 32,
    b0100000 = 32,
    b100000 = 32,
    b00100001 = 33,
    b0100001 = 33,
    b100001 = 33,
    b00100010 = 34,
    b0100010 = 34,
    b100010 = 34,
    b00100011 = 35,
    b0100011 = 35,
    b100011 = 35,
    b00100100 = 36,
    b0100100 = 36,
    b100100 = 36,
    b00100101 = 37,
    b0100101 = 37,
    b100101 = 37,
    b00100110 = 38,
    b0100110 = 38,
    b100110 = 38,
    b00100111 = 39,
    b0100111 = 39,
    b100111 = 39,
    b00101000 = 40,
    b0101000 = 40,
    b101000 = 40,
    b00101001 = 41,
    b0101001 = 41,
    b101001 = 41,
    b00101010 = 42,
    b0101010 = 42,
    b101010 = 42,
    b00101011 = 43,
    b0101011 = 43,
    b101011 = 43,
    b00101100 = 44,
    b0101100 = 44,
    b101100 = 44,
    b00101101 = 45,
    b0101101 = 45,
    b101101 = 45,
    b00101110 = 46,
    b0101110 = 46,
    b101110 = 46,
    b00101111 = 47,
    b0101111 = 47,
    b101111 = 47,
    b00110000 = 48,
    b0110000 = 48,
    b110000 = 48,
    b00110001 = 49,
    b0110001 = 49,
    b110001 = 49,
    b00110010 = 50,
    b0110010 = 50,
    b110010 = 50,
    b00110011 = 51,
    b0110011 = 51,
    b110011 = 51,
    b00110100 = 52,
    b0110100 = 52,
    b110100 = 52,
    b00110101 = 53,
    b0110101 = 53,
    b110101 = 53,
    b00110110 = 54,
    b0110110 = 54,
    b110110 = 54,
    b00110111 = 55,
    b0110111 = 55,
    b110111 = 55,
    b00111000 = 56,
    b0111000 = 56,
    b111000 = 56,
    b00111001 = 57,
    b0111001 = 57,
    b111001 = 57,
    b00111010 = 58,
    b0111010 = 58,
    b111010 = 58,
    b00111011 = 59,
    b0111011 = 59,
    b111011 = 59,
    b00111100 = 60,
    b0111100 = 60,
    b111100 = 60,
    b00111101 = 61,
    b0111101 = 61,
    b111101 = 61,
    b00111110 = 62,
    b0111110 = 62,
    b111110 = 62,
    b00111111 = 63,
    b0111111 = 63,
    b111111 = 63,
    b01000000 = 64,
    b1000000 = 64,
    b01000001 = 65,
    b1000001 = 65,
    b01000010 = 66,
    b1000010 = 66,
    b01000011 = 67,
    b1000011 = 67,
    b01000100 = 68,
    b1000100 = 68,
    b01000101 = 69,
    b1000101 = 69,
    b01000110 = 70,
    b1000110 = 70,
    b01000111 = 71,
    b1000111 = 71,
    b01001000 = 72,
    b1001000 = 72,
    b01001001 = 73,
    b1001001 = 73,
    b01001010 = 74,
    b1001010 = 74,
    b01001011 = 75,
    b1001011 = 75,
    b01001100 = 76,
    b1001100 = 76,
    b01001101 = 77,
    b1001101 = 77,
    b01001110 = 78,
    b1001110 = 78,
    b01001111 = 79,
    b1001111 = 79,
    b01010000 = 80,
    b1010000 = 80,
    b01010001 = 81,
    b1010001 = 81,
    b01010010 = 82,
    b1010010 = 82,
    b01010011 = 83,
    b1010011 = 83,
    b01010100 = 84,
    b1010100 = 84,
    b01010101 = 85,
    b1010101 = 85,
    b01010110 = 86,
    b1010110 = 86,
    b01010111 = 87,
    b1010111 = 87,
    b01011000 = 88,
    b1011000 = 88,
    b01011001 = 89,
    b1011001 = 89,
    b01011010 = 90,
    b1011010 = 90,
    b01011011 = 91,
    b1011011 = 91,
    b01011100 = 92,
    b1011100 = 92,
    b01011101 = 93,
    b1011101 = 93,
    b01011110 = 94,
    b1011110 = 94,
    b01011111 = 95,
    b1011111 = 95,
    b01100000 = 96,
    b1100000 = 96,
    b01100001 = 97,
    b1100001 = 97,
    b01100010 = 98,
    b1100010 = 98,
    b01100011 = 99,
    b1100011 = 99,
    b01100100 = 100,
    b1100100 = 100,
    b01100101 = 101,
    b1100101 = 101,
    b01100110 = 102,
    b1100110 = 102,
    b01100111 = 103,
    b1100111 = 103,
    b01101000 = 104,
    b1101000 = 104,
    b01101001 = 105,
    b1101001 = 105,
    b01101010 = 106,
    b1101010 = 106,
    b01101011 = 107,
    b1101011 = 107,
    b01101100 = 108,
    b1101100 = 108,
    b01101101 = 109,
    b1101101 = 109,
    b01101110 = 110,
    b1101110 = 110,
    b01101111 = 111,
    b1101111 = 111,
    b01110000 = 112,
    b1110000 = 112,
    b01110001 = 113,
    b1110001 = 113,
    b01110010 = 114,
    b1110010 = 114,
    b01110011 = 115,
    b1110011 = 115,
    b01110100 = 116,
    b1110100 = 116,
    b01110101 = 117,
    b1110101 = 117,
    b01110110 = 118,
    b1110110 = 118,
    b01110111 = 119,
    b1110111 = 119,
    b01111000 = 120,
    b1111000 = 120,
    b01111001 = 121,
    b1111001 = 121,
    b01111010 = 122,
    b1111010 = 122,
    b01111011 = 123,
    b1111011 = 123,
    b01111100 = 124,
    b1111100 = 124,
    b01111101 = 125,
    b1111101 = 125,
    b01111110 = 126,
    b1111110 = 126,
    b01111111 = 127,
    b1111111 = 127,
    b10000000 = 128,
    b10000001 = 129,
    b10000010 = 130,
    b10000011 = 131,
    b10000100 = 132,
    b10000101 = 133,
    b10000110 = 134,
    b10000111 = 135,
    b10001000 = 136,
    b10001001 = 137,
    b10001010 = 138,
    b10001011 = 139,
    b10001100 = 140,
    b10001101 = 141,
    b10001110 = 142,
    b10001111 = 143,
    b10010000 = 144,
    b10010001 = 145,
    b10010010 = 146,
    b10010011 = 147,
    b10010100 = 148,
    b10010101 = 149,
    b10010110 = 150,
    b10010111 = 151,
    b10011000 = 152,
    b10011001 = 153,
    b10011010 = 154,
    b10011011 = 155,
    b10011100 = 156,
    b10011101 = 157,
    b10011110 = 158,
    b10011111 = 159,
    b10100000 = 160,
    b10100001 = 161,
    b10100010 = 162,
    b10100011 = 163,
    b10100100 = 164,
    b10100101 = 165,
    b10100110 = 166,
    b10100111 = 167,
    b10101000 = 168,
    b10101001 = 169,
    b10101010 = 170,
    b10101011 = 171,
    b10101100 = 172,
    b10101101 = 173,
    b10101110 = 174,
    b10101111 = 175,
    b10110000 = 176,
    b10110001 = 177,
    b10110010 = 178,
    b10110011 = 179,
    b10110100 = 180,
    b10110101 = 181,
    b10110110 = 182,
    b10110111 = 183,
    b10111000 = 184,
    b10111001 = 185,
    b10111010 = 186,
    b10111011 = 187,
    b10111100 = 188,
    b10111101 = 189,
    b10111110 = 190,
    b10111111 = 191,
    b11000000 = 192,
    b11000001 = 193,
    b11000010 = 194,
    b11000011 = 195,
    b11000100 = 196,
    b11000101 = 197,
    b11000110 = 198,
    b11000111 = 199,
    b11001000 = 200,
    b11001001 = 201,
    b11001010 = 202,
    b11001011 = 203,
    b11001100 = 204,
    b11001101 = 205,
    b11001110 = 206,
    b11001111 = 207,
    b11010000 = 208,
    b11010001 = 209,
    b11010010 = 210,
    b11010011 = 211,
    b11010100 = 212,
    b11010101 = 213,
    b11010110 = 214,
    b11010111 = 215,
    b11011000 = 216,
    b11011001 = 217,
    b11011010 = 218,
    b11011011 = 219,
    b11011100 = 220,
    b11011101 = 221,
    b11011110 = 222,
    b11011111 = 223,
    b11100000 = 224,
    b11100001 = 225,
    b11100010 = 226,
    b11100011 = 227,
    b11100100 = 228,
    b11100101 = 229,
    b11100110 = 230,
    b11100111 = 231,
    b11101000 = 232,
    b11101001 = 233,
    b11101010 = 234,
    b11101011 = 235,
    b11101100 = 236,
    b11101101 = 237,
    b11101110 = 238,
    b11101111 = 239,
    b11110000 = 240,
    b11110001 = 241,
    b11110010 = 242,
    b11110011 = 243,
    b11110100 = 244,
    b11110101 = 245,
    b11110110 = 246,
    b11110111 = 247,
    b11111000 = 248,
    b11111001 = 249,
    b11111010 = 250,
    b11111011 = 251,
    b11111100 = 252,
    b11111101 = 253,
    b11111110 = 254,
    b11111111 = 255,
}eBinarys;

struct ky_binary
{
    const char * const v;
    explicit ky_binary(const char * c):v(c){}
};
#define bin(v) ky_binary("b"#v)

#endif // ky_BINARY_H
