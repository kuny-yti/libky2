
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
 * @file     ky_bigint.h
 * @brief    大整数类
 *       1.支持一元运算符
 *       2.支持四则运算
 *       3.支持比较
 *       4.支持最大公约数计算
 *       5.支持最小公倍数计算
 *       6.支持平方根计算
 *
 * @author   kunyang
 * @email    kunyang.yk@gmail.com
 * @version  1.0.0.1
 * @date     2019/02/13
 * @license  GNU General Public License (GPL)
 *
 * Change History :
 *    Date    |  Version  |  Author  |   Description
 * 2019/02/13 | 1.0.0.1   | kunyang  | 创建文件
 */
#ifndef KY_BIGINT_H
#define KY_BIGINT_H
#include "ky_define.h"
#include "tools/ky_string.h"

typedef struct _bigint_
{
    ky_byte value;
    int8    sign;

    _bigint_();
    _bigint_(const _bigint_ &);
    _bigint_(const longlong &);
    _bigint_(const ky_string &);

    _bigint_& operator=(const _bigint_&);
    _bigint_& operator=(const longlong&);
    _bigint_& operator=(const ky_string&);

    //!
    //! \brief operator + - 一元运算符操作
    //! \return
    //!
    _bigint_ operator+() const;
    _bigint_ operator-() const;

    //!
    //! \brief operator ++ 前置和后置自增、自减运算
    //! \return
    //!
    _bigint_& operator++();
    _bigint_& operator--();
    _bigint_ operator++(int);
    _bigint_ operator--(int);

    //!
    //! \brief operator + - * / % 四则运算和模运算
    //! \return
    //!
    _bigint_ operator+(const _bigint_ &) const;
    _bigint_ operator-(const _bigint_ &) const;
    _bigint_ operator*(const _bigint_ &) const;
    _bigint_ operator/(const _bigint_ &) const;
    _bigint_ operator%(const _bigint_ &) const;

    _bigint_ operator+(const longlong &) const;
    _bigint_ operator-(const longlong &) const;
    _bigint_ operator*(const longlong &) const;
    _bigint_ operator/(const longlong &) const;
    _bigint_ operator%(const longlong &) const;

    _bigint_ operator+(const ky_string &) const;
    _bigint_ operator-(const ky_string &) const;
    _bigint_ operator*(const ky_string &) const;
    _bigint_ operator/(const ky_string &) const;
    _bigint_ operator%(const ky_string &) const;


    inline _bigint_& operator+=(const _bigint_ &n){*this=*this+n; return *this;}
    inline _bigint_& operator-=(const _bigint_ &n){*this=*this-n; return *this;}
    inline _bigint_& operator*=(const _bigint_ &n){*this=*this*n; return *this;}
    inline _bigint_& operator/=(const _bigint_ &n){*this=*this/n; return *this;}
    inline _bigint_& operator%=(const _bigint_ &n){*this=*this%n; return *this;}

    inline _bigint_& operator+=(const longlong &n){*this=*this+n; return *this;}
    inline _bigint_& operator-=(const longlong &n){*this=*this-n; return *this;}
    inline _bigint_& operator*=(const longlong &n){*this=*this*n; return *this;}
    inline _bigint_& operator/=(const longlong &n){*this=*this/n; return *this;}
    inline _bigint_& operator%=(const longlong &n){*this=*this%n; return *this;}

    inline _bigint_& operator+=(const ky_string &n){*this=*this+n; return *this;}
    inline _bigint_& operator-=(const ky_string &n){*this=*this-n; return *this;}
    inline _bigint_& operator*=(const ky_string &n){*this=*this*n; return *this;}
    inline _bigint_& operator/=(const ky_string &n){*this=*this/n; return *this;}
    inline _bigint_& operator%=(const ky_string &n){*this=*this%n; return *this;}

    //!
    //! \brief operator  比较运算
    //! \return
    //!
    bool operator<(const _bigint_&) const;
    bool operator>(const _bigint_&) const;
    bool operator<=(const _bigint_&) const;
    bool operator>=(const _bigint_&) const;
    bool operator==(const _bigint_&) const;
    bool operator!=(const _bigint_&) const;
    bool operator<(const longlong&) const;
    bool operator>(const longlong&) const;
    bool operator<=(const longlong&) const;
    bool operator>=(const longlong&) const;
    bool operator==(const longlong&) const;
    bool operator!=(const longlong&) const;
    bool operator<(const ky_string&) const;
    bool operator>(const ky_string&) const;
    bool operator<=(const ky_string&) const;
    bool operator>=(const ky_string&) const;
    bool operator==(const ky_string&) const;
    bool operator!=(const ky_string&) const;

    //!
    //! \brief to_string 转换为字符串
    //! \return
    //!
    ky_string to_string() const;
    //!
    //! \brief to_int 转换为整形
    //! \return
    //!
    int to_int() const;
    long to_long() const;
    longlong to_longlong() const;

    //!
    //! \brief compare 比较大小
    //! \return >0 比v大，<0 比v小，=0 相等
    //!
    int compare(const _bigint_ &v) const;
    //!
    //! \brief pow10 返回等于10 ^ exp的ky_bigint
    //! \param d
    //! \return
    //! \note 指数应该是非负整数
    //!
    _bigint_ & pow10(int32 const &d);

    //!
    //! \brief trimmed 修剪前导零
    //!
    void trimmed();
}ky_bigint;

//!
//! \brief abs 求绝对值
//! \return
//!
_bigint_ ky_abs(const _bigint_ &d);

//!
//! \brief pow 返回等于base ^ exp的ky_bigint
//! \param base
//! \param exp
//! \return
//!
_bigint_ ky_pow(const _bigint_ &base, const int32 &exp);
//!
//! \brief sqrt 使用Newton方法返回ky_bigint的正整数平方根。
//! \param num
//! \return
//! \note 输入必须是非负的。
//!
_bigint_ ky_sqrt(const _bigint_& d);

//!
//! \brief ky_gcd 使用Euclid算法返回两个ky_bigint的最大公约数(GCD, a.k.a. HCF)
//! \param num1
//! \param num2
//! \return
//!
_bigint_ ky_gcd(const _bigint_ &num1, const _bigint_ &num2);

//!
//! \brief ky_lcm 返回两个ky_bigint的最小公倍数 (LCM)
//! \param num1
//! \param num2
//! \return
//!
_bigint_ ky_lcm(const _bigint_& num1, const _bigint_& num2);


#endif
