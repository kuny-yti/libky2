
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
 * @file     ky_maths.h
 * @brief    数学基础库
 *
 * @author   kunyang
 * @email    kunyang.yk@gmail.com
 * @version  1.2.0.1
 * @date     2014/04/09
 * @license  GNU General Public License (GPL)
 *
 * Change History :
 *    Date    |  Version  |  Author  |   Description
 * 2014/04/09 | 1.0.0.1   | kunyang  | 创建文件
 * 2018/03/13 | 1.1.0.1   | kunyang  | 添加快速sqrt运算
 * 2020/02/11 | 1.2.0.1   | kunyang  | 添加取整函数
 */
#ifndef KY_MATHS_H
#define KY_MATHS_H

#include "ky_define.h"
#include <math.h>
#include <float.h>
#include <ctime>

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

#ifdef PI
#undef PI
#endif

const float  kyFloatEps    = 1.0e-7f;
const float  kyFloatEpsSQ  = 1.0e-14f;
const float  kyFloatPi     = 3.14159265358979323846f;
const float  kyPiDiv2f     = 1.57079632679489661923f;
const float  kyPif         = kyFloatPi;
const real   kyDoubleEps   = 1.0e-14;
const real   kyDoubleEpsSQ = 1.0e-28;
const real   kyDoublePi    = 3.14159265358979323846;
const real   kyPiDiv2      = 1.57079632679489661923;
const real   kyPi          = kyDoublePi;
const real   kyEpsilon     = 2.2204460492503131e-16;

const real   kyTwoPi       =  6.28318530717958647693;
const float  kyTwoPif      =  6.28318530717958647693f;

const real   kySqrtEps     = 1.490116119385000000e-8;

/// convert a radian to degree
/// 弧度转换角度
inline float ky_rad2deg(float f)
{
    static const float rdf = 57.2957795133;
    return rdf * f;
}

/// convert a radian to degree
/// 弧度转换角度
inline real ky_rad2deg(real d )
{
    static const real rdr = 57.2957795133;
    return rdr * d;
}

/// convert a degree to radian
///  角度转换弧度
inline float ky_deg2rad(float f)
{
    static const float drf = 0.01745329252;
    return drf * f;
}

/// convert a degree to radian
/// 角度转换弧度
inline real ky_deg2rad(real d )
{
    static const float drr = 0.01745329252;
    return drr * d;
}

inline float ky_abs(float x)
{
    if (x < 0)
        x = 0 - x;
    return x;
}
//!
//! \brief cfabs fabs函数
//! \param x
//! \return
//!
inline float ky_fabs (float x)
{
    int tmp = *(int *)&x;
    tmp &= 0x7FFFFFFF;
    return *(float *)&tmp;
}

inline float ky_sin(float x)
{
    const float B = 1.2732395447;
    const float C = -0.4052847346;
    const float P = 0.2310792853;//0.225;
    float y = B * x + C * x * ky_fabs(x);
    y = P * (y * ky_fabs(y) - y) + y;
    return y;
}

inline float ky_cos(float x)
{
    const float Q = 1.5707963268;
    x += Q;

    if(x > kyPif)
        x -= kyTwoPif;
    return( ky_sin(x));
}
inline float ky_tan(float x)
{
    return std::tan(x);
}
inline float ky_atan(float x)
{
    return std::atan(x);
}
inline float ky_pow(float a, int b)
{
    float r = a;
    if (b > 0)
    {
      while(--b)
         r *= a;

    }
    else if (b < 0)
    {
        while(++b)
            r *= a;
         r = 1.0 / r;
    }
    else
        r=0;
    return r;
}

//!
//! \brief rsqrt 逆sqrt函数
//! \param x
//! \return
//!
inline float ky_rsqrt (float x)
{
    const float xhalf = 0.5f * x;
    int i = *(int*)&x;
    i = 0x5f3759df - (i >> 1);
    x = *(float*)&i;
    x = x * (1.5f - xhalf * x * x);
    // 更精确
    x = x * (1.5f - xhalf * x * x);
    //x = x * (1.5f - xhalf * x * x);

    return x;
}
//!
//! \brief csqrt sqrt函数
//! \param x
//! \return
//!
inline float ky_sqrt (float x)
{
    return 1.f / ky_rsqrt(x);
}

//!
//! \brief log2 log2函数
//! \param val
//! \return
//!
inline int ky_log2 (int val)
{
    int answer;
    answer = 0;
    while ((val >>= 1) != 0)
        answer++;
    return answer;
}

inline float ky_log(float val)
{
    return std::log(val);
}
/*
    sign
*/
inline float ky_sign(float f)
{
    return f == 0.f ? 0.f : f / ky_fabs(f);
}

inline real ky_sign(real d)
{
    return d == 0.0 ? 0.0 : d / ky_fabs(d);
}

inline int ky_sign(int i)
{
    return i == 0 ? 0 : i / ky_fabs(i);
}

inline int ky_isign(float f)
{
    return static_cast<int>(ky_sign(f));
}

inline int ky_isign(real d)
{
    return static_cast<int>(ky_sign(d));
}

/*
    floor
*/
inline int ky_ifloor(float f)
{
    return static_cast<int>(ky_floor(f));
}

inline int ky_ifloor(real d)
{
    return static_cast<int>(ky_floor(d));
}

/*
    ceil
*/
inline int ky_iceil(float f)
{
    return static_cast<int>(ky_ceil(f));
}

inline int ky_iceil(real d)
{
    return static_cast<int>(ky_ceil(d));
}

/*
    fix
*/

inline float ky_fix(float f)
{
    return ky_fabs(f) * ky_sign(f);
}

inline double ky_fix(real d)
{
    return ky_fabs(d) * ky_sign(d);
}

inline int ky_ifix(float f)
{
    return static_cast<int>(ky_fix(f));
}

inline int ky_ifix(real d)
{
    return static_cast<int>(ky_fix(d));
}

/*
   fmod
 */
template<typename T, typename U>
inline T ky_fmod(T x, U y)
{
    return std::fmod(x, y);
}

/*
    round
*/
inline float ky_round(float f)
{
    return ky_floor(f + 0.5f);
}

inline double ky_round(real d)
{
    return ky_floor(d + 0.5);
}

inline int ky_iround(float f)
{
    return static_cast<int>(ky_round(f));
}

inline int ky_iround(real d)
{
    return static_cast<int>(ky_round(d));
}

/*
    clamp
*/
inline float ky_clamp(float f, float min, float max)
{
    return ky_min( ky_max(f, min), max );
}

inline double ky_clamp(real f, real min, real max)
{
    return ky_min(ky_max(f, min), max);
}

/// Clamps \p f to range [\p min, \p max].
inline int ky_clamp(int f, int min, int max)
{
    return ky_min(ky_max(f, min), max);
}




/*
    floating point checks
 */

/// Returns false, if \p f is not a number (NaN).
inline bool ky_isNumber(float f) {
    // according to the IEEE-754 floating-point standard
    // comparisons with NaN always fail
    return f == f;
}

/// Returns false, if \p d is not a number (NaN).
inline bool ky_isNumber(real d) {
    // according to the IEEE-754 floating-point standard
    // comparisons with NaN always fail
    return d == d;
}

/// Returns true, if \p value with the type T is not a number (NaN).
template<typename T>
inline bool ky_isNaN(const T& value) {
    return (value != value);
}

/// Returns false, if \p f is NaN or infinity.
inline bool ky_isFiniteNumber(float f) {
    return (f <= FLT_MAX && f >= -FLT_MAX);
}

/// Returns false, if \p d is NaN or infinity.
inline bool ky_isFiniteNumber(real d) {
    return (d <= DBL_MAX && d >= -DBL_MAX);
}

inline float ky_fmap(float value, float inputMin, float inputMax, float outputMin,
                 float outputMax, bool clamp= false)
{
    #ifndef FLT_EPSILON
    #define FLT_EPSILON	__FLT_EPSILON__
    #endif
    if (fabs(inputMin - inputMax) < FLT_EPSILON){
        return outputMin;
    } else {
        float outVal = ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin);

        if( clamp ){
            if(outputMax < outputMin){
                if( outVal < outputMax )outVal = outputMax;
                else if( outVal > outputMin )outVal = outputMin;
            }else{
                if( outVal > outputMax )outVal = outputMax;
                else if( outVal < outputMin )outVal = outputMin;
            }
        }
        return outVal;
    }
}
//!
//! \brief ky_irand 取count个随机int型数组
//! \param count
//! \return
//!
template <typename T>
inline T* ky_rand(T *buf, int64 len)
{
    srand((uint)time(NULL));
    for (int64 i = 0; i < len; ++i)
    {
        if (typeid(T) == typeid(float))
            buf[i] = rand() / float(RAND_MAX);
        else if (typeid(T) == typeid(real))
            buf[i] = rand() / real(RAND_MAX);
        else
            buf[i] = rand();
    }
    return buf;
}
//!
//! \brief ky_irand 取count个以a开始的n为范围内的随机int型数组
//! \param count
//! \return
//!
template <typename T>
inline T* ky_rand(T*buf, int64 len, int a, int n)
{
    srand((uint)time(NULL));
    n = n == 0 ? 1 : n;
    for (int64 i = 0; i < len; ++i)
    {
        if (typeid(T) == typeid(float))
            buf[i] = a+ rand() %n / float(RAND_MAX);
        else if (typeid(T) == typeid(real))
            buf[i] = a+ rand() %n / real(RAND_MAX);
        else
            buf[i] = a+ rand() %n;
    }
    return buf;
}

//!
//! \brief ky_isPow2 给定数字是否为2的幂。
//! \param n
//! \return
//!
template<typename T>
inline bool ky_isPow2(T n)
{
    if(n == 0)
        return false;
    return (n & (n-1)) == 0;
}

//!
//! ky_ceil_log2 以2为底的对数上限（四舍五入）(ceil(log2(x)))
//!
//! \param x (unsigned integer)
//! \return
//! \note 如果输入为零，则为零。
//!
template<typename T>
inline uint64 ky_ceil_log2(T x)
{
    T bit = 1;
    uint64 log = 0;
    while(bit < x && bit != 0)
    {
        log++;
        bit *= 2;
    }

    return log;
}

//!
//! ky_floor_log2 以2为底的对数下限（四舍五入）(floor(log2(f)))
//!
//! \param x 必须 > 0，否则结果不确定。
//! \return
//! \note 快速，使用FPU标准化硬件
//!
inline int ky_floor_log2(const float x)
{
    uint32 i = 0;
    ::memcpy(&i, &x, sizeof(float));
    const uint32 biased_exp = (i >> 23) & 0xFF;
    return (int)biased_exp - 127;
}

//!
//! ky_round_up_pow2 向上取整下一个2的幂(四舍五入)
//!
//! \param x
//! \return
//!
template<typename T>
inline T ky_round_up_pow2(T x)
{
    return T(1) << ky_ceil_log2(x);
}

//!
//! ky_round_down_pow2 向下取整下一个2的幂(四舍五入)
//!
//! \param x
//! \return
//!
template<typename T>
inline T ky_round_down_pow2(T x)
{
    return T(1) << ky_floor_log2(x);
}

//!
//! ky_round_up 向上取下一个整数的倍数
//!
//! \param x 四舍五入的数
//! \param multiple 必须是2的幂
//! \return
//!
template<typename T>
inline T ky_round_up(T n, T multiple)
{
    kyASSERT(ky_isPow2(multiple));
    const T result = (n + multiple-1) & ~(multiple-1);
    kyASSERT(n <= result && result < n+multiple);
    return result;
}

//!
//! ky_round_down 向下取下一个整数的倍数
//!
//! \param x 四舍五入的数
//! \param multiple 必须是2的幂
//! \return
//!
template<typename T>
inline T ky_round_down(T n, T multiple)
{
    kyASSERT(ky_isPow2(multiple));
    const T result = n & ~(multiple-1);
    kyASSERT(result <= n && n < result+multiple);
    return result;
}

#endif
