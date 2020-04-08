
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
 * @file     ky_algorlthm.h
 * @brief    基本算法定义和实现
 *       1.数据类型的大小判断
 *       2.常用排序算法
 *       3.基本数据类型的交换
 *       3.一些哈希算法实现(散列)
 *
 * @author   kunyang
 * @email    kunyang.yk@gmail.com
 * @version  1.2.1.1
 * @date     2015/04/15
 * @license  GNU General Public License (GPL)
 *
 * Change History :
 *    Date    |  Version  |  Author  |   Description
 * 2015/04/15 | 1.0.0.1   | kunyang  | 创建文件加入数据类型的交换
 * 2015/08/22 | 1.0.1.0   | kunyang  | 修改根据编译器来交换64位整形类型
 * 2016/02/11 | 1.1.0.1   | kunyang  | 加入散列计算算法
 * 2016/06/04 | 1.2.0.1   | kunyang  | 加入排序算法
 * 2016/11/22 | 1.2.1.1   | kunyang  | 修改排序算法并加入归并排序算法
 */
#ifndef ky_ALGORLTHM_H
#define ky_ALGORLTHM_H

#include "ky_define.h"

//!
//! \brief ky_min
//! \param a
//! \param b
//! \return
//!
template <typename T>
inline const T & ky_min(const T & a, const T & b) { return (a < b) ? a : b; }

//!
//! \brief ky_max
//! \param a
//! \param b
//! \return
//!
template <typename T>
inline const T & ky_max(const T & a, const T & b) { return (a < b) ? b : a; }

//!
//! \brief ky_bound
//! \param min
//! \param val
//! \param max
//! \return
//!
template <typename T>
inline const T & ky_bound(const T & min, const T & val, const T & max)
{
    return ky_max(min, ky_min(max, val));
}

#define ky_swap16(x) \
    (((x) & 0x00ff) << 8 | \
    ( (x) & 0xff00) >> 8)

#define ky_swap32(x) \
    (((x) & 0x000000ff) << 24 | \
    ( (x) & 0x0000ff00) << 8 | \
    ( (x) & 0x00ff0000) >> 8 | \
    ( (x) & 0xff000000) >> 24)

#define ky_swap64(x) \
    ((((x) & 0x00000000000000ff) << 56) | \
    (( (x) & 0x000000000000ff00) << 40) | \
    (( (x) & 0x0000000000ff0000) << 24) | \
    (( (x) & 0x00000000ff000000) << 8)  | \
    (( (x) & 0x000000ff00000000) >> 8)  | \
    (( (x) & 0x0000ff0000000000) >> 24) | \
    (( (x) & 0x00ff000000000000) >> 40) | \
    (( (x) & 0xff00000000000000) >> 56))

template <typename T>
T ky_endian_swap(T &v)
{
    if (sizeof(T) == 2)
        v = ky_swap16(v);
    else if (sizeof(T) == 4)
        v = ky_swap32(v);
    else if (sizeof(T) == 8)
        v = ky_swap64(v);
    return v;
}
#define __RegularTypeSwap(T) inline void ky_swap(T & a, T & b){T tmp = a; a = b; b = tmp;}
__RegularTypeSwap(bool)
__RegularTypeSwap(bool_t)

__RegularTypeSwap(i8)
__RegularTypeSwap(i16)
__RegularTypeSwap(i32)
__RegularTypeSwap(i64)
__RegularTypeSwap(f32)

__RegularTypeSwap(u8)
__RegularTypeSwap(u16)
__RegularTypeSwap(u32)
__RegularTypeSwap(u64)
__RegularTypeSwap(f64)

__RegularTypeSwap(f16)

__RegularTypeSwap(wchar_t)

__RegularTypeSwap(bool*)
__RegularTypeSwap(bool_t*)

__RegularTypeSwap(i8*)
__RegularTypeSwap(i16*)
__RegularTypeSwap(i32*)
__RegularTypeSwap(i64*)
__RegularTypeSwap(f32*)

__RegularTypeSwap(u8*)
__RegularTypeSwap(u16*)
__RegularTypeSwap(u32*)
__RegularTypeSwap(u64*)
__RegularTypeSwap(f64*)

__RegularTypeSwap(f16*)

__RegularTypeSwap(wchar_t*)

#  if !kyCompilerIsGNUC && !kyCompilerIsCLANG
__RegularTypeSwap(int64)
__RegularTypeSwap(uint64)
__RegularTypeSwap(int64*)
__RegularTypeSwap(uint64*)
#  endif
#undef __RegularTypeSwap

namespace __sort__
{
    //!
    //! @brief insert 数组型插入排序
    //!
    //! @param a 是数组的开始地址
    //! @param length 是数组的长度
    //!
    template<typename T>
    void insert( T *a, int64 length);

    //!
    //! \brief insert 容器型插入排序
    //! \param begin
    //! \param end
    //!
    template<typename container_iterator>
    void insert (container_iterator begin, container_iterator end);


    //!
    //! @brief insert2 数组型2路插入排序
    //!
    //! @param a 是数组的开始地址
    //! @param length 是数组的长度
    //!
    template<typename T>
    void insert2(T* a, int64 alength);

    //!
    //! @brief quick 数组型快速排序
    //!
    //! @param begin 是数组的开始地址
    //! @param end 是数组的结束地址
    //!
    template<typename T>
    void quick(T *begin, T *end);

    //!
    //! @brief quick 容器型快速排序
    //!
    //! @param begin 是容器开始迭代
    //! @param end 是容器结束迭代
    //!
    template<typename container_iterator>
    void quick(container_iterator begin, container_iterator end);

    //!
    //! @brief heap 堆排序算法
    //!
    //! @param H是待调整的堆数组
    //! @param length是数组的长度
    //!
    template <typename T>
    void heap(T *H, int64 length);

    //!
    //! @brief bubble 冒泡排序算法
    //!
    //! @param H是待调整的堆数组
    //! @param length是数组的长度
    //!
    template <typename T>
    void bubble (T *r, int64 length);
    //!
    //! @brief merge 归并排序算法
    //!
    //! @param arr 是待调整的堆数组
    //! @param length是数组的长度
    //!
    template <typename T>
    void merge(T *arr, int64 length);
}

//!
//! \brief ky_isort
//! \param a
//! \param length
//!
template<typename T>
inline void ky_isort( T *a, int64 length){__sort__::insert(a, length);}

//!
//! \brief ky_isort
//! \param begin
//! \param end
//!
template<typename container_iterator>
inline void ky_isort(container_iterator begin, container_iterator end){__sort__::insert(begin, end);}

//!
//! \brief ky_isort2
//! \param a
//! \param alength
//!
template<typename T>
inline void ky_isort2(T* a, int64 alength){__sort__::insert2(a, alength);}

//!
//! \brief ky_qsort
//! \param begin
//! \param end
//!
template<typename T>
inline void ky_qsort(T *begin, T *end){__sort__::quick(begin, end);}

//!
//! \brief ky_qsort
//! \param begin
//! \param end
//!
template<typename container_iterator>
inline void ky_qsort(container_iterator begin, container_iterator end){__sort__::quick(begin, end);}

//!
//! \brief ky_hsort
//! \param H
//! \param length
//!
template<typename T>
inline void ky_hsort(T *H, int64 length){__sort__::heap(H, length);}

//!
//! \brief ky_bsort
//! \param H
//! \param length
//!
template<typename T>
inline void ky_bsort(T *H, int64 length){__sort__::bubble(H, length);}

//!
//! \brief ky_msort
//! \param H
//! \param length
//!
template<typename T>
inline void ky_msort(T *H, int64 length){__sort__::merge(H, length);}

namespace __hash__
{
    // RS Hash Function
    u32 RS (char *str, i64 len);
    // JS Hash Function
    u32 JS(char *str, i64 len);
    // P. J. Weinberger Hash Function
    u32 PJW(char *str, i64 len);
    // ELF Hash Function
    u32 ELF(char *str, i64 len);
    // BKDR Hash Function
    u32 BKDR(char *str, i64 len, uint seed = 131);
    // SDBM Hash Function
    u32 SDBM(char *str, i64 len);
    // DJB Hash Function
    u32 DJB(char *str, i64 len);
    // AP Hash Function
    u32 AP(char *str, i64 len);
    /*DEKHash*/
    u32 DEK(char *str, i64 len);
    /*BPHash*/
    u32 BP(char *str, i64 len);
    /*FNVHash*/
    u32 FNV(char *str, i64 len);
    u32 FNV64(char *str, i64 len);
    u32 H64(u64 v);
}

inline u32 ky_hash_f(const i8 & v){return (u32)v;}
inline u32 ky_hash_f(const u8 & v){return (u32)v;}
inline u32 ky_hash_f(const i16 & v){return (u32)v;}
inline u32 ky_hash_f(const u16 & v){return (u32)v;}
inline u32 ky_hash_f(const i32 & v){return (u32)v;}
inline u32 ky_hash_f(const u32 & v){return (u32)v;}
inline u32 ky_hash_f(const wchar_t & v){return (u32)v;}
inline u32 ky_hash_f(const i64 & v){return __hash__::H64 (v);}
inline u32 ky_hash_f(const u64 & v){return __hash__::H64 (v);}
inline u32 ky_hash_f(f32 v){union{f32 tv;u64 v64;};tv = v; return __hash__::H64(v64);}
inline u32 ky_hash_f(f64 v){union{f64 tv;u64 v64;};tv = v; return __hash__::H64(v64);}
inline u32 ky_hash_f(char *v, int len = -1){if (len < 0) len = ::strlen (v);return __hash__::DJB(v, len);}

#include "ky_algorlthm.inl"

#endif //ky_ALGORLTHM_H
