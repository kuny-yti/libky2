
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
 * @file     ky_bitset.h
 * @brief    位操作类最大支持64位
 *       1.位操作支持bin宏实现的ky_binary对象.
 *       2.支持运算符操作
 *
 * @author   kunyang
 * @email    kunyang.yk@gmail.com
 * @version  1.0.3.0
 * @date     2016/04/11
 * @license  GNU General Public License (GPL)
 *
 * Change History :
 *    Date    |  Version  |  Author  |   Description
 * 2016/04/11 | 1.0.0.1   | kunyang  | 创建文件实现ky_bitset
 * 2016/04/12 | 1.0.1.0   | kunyang  | 修改内部操作位时使用BIT方法
 * 2017/01/07 | 1.0.2.0   | kunyang  | 实现基本的运算符操作
 * 2017/04/29 | 1.0.3.0   | kunyang  | 实现bin宏模式赋值
 */
#ifndef ky_BITSET_H
#define ky_BITSET_H

#include "ky_define.h"
#include "ky_string.h"
#include "ky_binary.h"

template <int>
struct __bit_pick   {enum {shift = 0, bit = 0,  mod = 0};};
template <>
struct __bit_pick<1>{enum {shift = 3, bit = 8,  mod = 0x07};};
template <>
struct __bit_pick<2>{enum {shift = 4, bit = 16, mod = 0x0f};};
template <>
struct __bit_pick<4>{enum {shift = 5, bit = 32, mod = 0x1f};};
template <>
struct __bit_pick<8>{enum {shift = 6, bit = 64, mod = 0x3f};};

template <int Count, typename T = u32>
struct __bit_bridge
{
    volatile u8 i;
    T           v:Count;

    __bit_bridge(){i = 0; v = 0;}

    inline __bit_bridge &operator = (bool b);
    inline T operator ()()const{return v;}
    inline operator T()const{return v;}
    inline operator bool() const {return v & ((1 << i) & ~(0));}
    inline bool operator == (bool b)const;
    inline bool operator != (bool b)const;
};

/*!
 * @brief The ky_bitset class
 * @class ky_bitset
 */
template <int Count, typename T = u32>
class ky_bitset
{
public:
    enum {bits = Count, size = Count};
    enum Bitset
    {
        False = 0,
        True = 1,

        Low = 0,
        High = 1,

        Off = 0,
        On = 1,

        No = 0,
        Yes = 1
    } ;

private:
    enum
    {
        __sbit = __bit_pick<sizeof(T)>::bit,
        __shift = __bit_pick<sizeof(T)>::shift,
        __smod = __bit_pick<sizeof(T)>::mod,
        __srema  = bits & __smod,
        __sgroup = bits >> __shift,

        __group = !!__srema ? __sgroup+1 : __sgroup,
        __rema = __sgroup ? __sbit : __srema
    };
    typedef __bit_bridge<__rema, T> __bridge_t;

public:
    //!
    //! \brief ky_bitset
    //! \param v
    //!
    explicit ky_bitset(T v = 0);
    //!
    //! \brief ky_bitset
    //! \param v
    //!
    ky_bitset(eBinarys v);
    //!
    //! \brief ky_bitset
    //! \param v
    //!
    ky_bitset(const ky_binary &v);
    //!
    //! \brief ky_bitset
    //! \param rhs
    //!
    ky_bitset(const ky_bitset &rhs);
    //!
    //! \brief operator =
    //! \param rhs
    //! \return
    //!
    inline ky_bitset &operator = (const ky_bitset &rhs);
    //!
    //! \brief operator =
    //! \param rhs
    //! \return
    //!
    inline ky_bitset &operator = (const eBinarys &rhs);
    //!
    //! \brief operator =
    //! \param rhs
    //! \return
    //!
    inline ky_bitset &operator = (const ky_binary &rhs);

    //!
    //! \brief operator 重载按位与、或、非、异或运算
    //! \param rhs
    //! \return  返回一个运算结果
    //!
    inline ky_bitset operator & (const ky_bitset &rhs)const;
    inline ky_bitset operator | (const ky_bitset &rhs)const;
    inline ky_bitset operator ~ ()const;
    inline ky_bitset operator ^ (const ky_bitset &rhs)const;

    //!
    //! \brief operator 重载按位与、或、非、异或运算,并改变自身值
    //! \param rhs
    //! \return
    //!
    inline ky_bitset &operator &= (const ky_bitset &rhs);
    inline ky_bitset &operator |= (const ky_bitset &rhs);
    inline ky_bitset &operator ~ ();
    inline ky_bitset &operator ^= (const ky_bitset &rhs);

    //!
    //! \brief operator 重载左右移位操作
    //! \param rhs
    //! \return
    //!
    inline ky_bitset operator << (uint b)const;
    inline ky_bitset operator >> (uint b)const;
    inline ky_bitset &operator <<= (uint b);
    inline ky_bitset &operator >>= (uint b);

    //!
    //! \brief operator 重载逻辑运算
    //! \param rhs
    //! \return
    //!
    inline bool operator || (const ky_bitset &rhs)const;
    inline bool operator && (const ky_bitset &rhs)const;
    inline bool operator ! ()const;

    inline bool operator == (const ky_bitset &rhs);
    inline bool operator != (const ky_bitset &rhs);

    //!
    //! \brief operator []  利用下标方法修改指定位的值
    //! \param bit
    //! \return
    //!
    inline __bridge_t &operator [](uint bit);
    inline __bridge_t operator [](uint bit)const;

    inline T value()const{return mu[0].v;}
    inline T operator ()(){return value();}
    inline operator T(){return value();}

    //!
    //! \brief extract 提取指定位的值
    //! \param lo_idx 最低位索引
    //! \param hi_idx 最高位索引
    //! \return
    //!
    template <int nCount>
    inline ky_bitset<nCount, T> extract (uint lo_idx, uint hi_idx)const;
    inline T extract(uint lo_idx, uint hi_idx)const;

    //!
    //! \brief bit_mask 包含最低num位的掩码
    //! \param num 位数
    //! \return
    //!
    template <typename U>
    static U bit_mask(uint num);
    //!
    //! \brief bit_extract 提取指定位的值
    //! \param lo_idx 最低位索引
    //! \param hi_idx 最高位索引
    //! \return
    //!
    template<typename U>
    static U bit_extract(U num, uint lo_idx, uint hi_idx);

public:
    //!
    //! \brief set 所有位置为１
    //!
    inline void set();
    //!
    //! \brief set 将bit位置为１
    //!
    inline void set(uint bit) ;
    //!
    //! \brief set 所有位置为0
    //!
    inline void unset();
    //!
    //! \brief set 将bit位置为0
    //!
    inline void unset(uint bit);

    //!
    //! \brief any 返回是否有有效位
    //! \return
    //!
    inline bool any()const;
    //!
    //! \brief none 返回是否全为无效位
    //! \return
    //!
    inline bool none()const;
    //!
    //! \brief count 返回有效位数
    //! \return
    //!
    inline int count()const;

    //!
    //! \brief flip 所有位翻转
    //!
    inline void flip();
    //!
    //! \brief flip 将bit位翻转
    //!
    inline void flip(uint bit);

    //!
    //! \brief swap 交换
    //! \param b
    //!
    inline void swap(ky_bitset &b);

    friend void ky_swap(ky_bitset &a, ky_bitset &b) {a.swap(b);}

    static void __convert_array (const __bridge_t *mu, u8 *arr) ;
    static void __convert_bridge (__bridge_t *mu, u8 *arr, int idx = 0);
    __bridge_t mu[__group];
};

#include "ky_bitset.inl"
#endif)
