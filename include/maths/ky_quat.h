
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
 * @file     ky_quaternion.h
 * @brief    四元数类
 *
 * @author   kunyang
 * @email    kunyang.yk@gmail.com
 * @version  1.0.0.1
 * @date     2014/04/09
 * @license  GNU General Public License (GPL)
 *
 * Change History :
 *    Date    |  Version  |  Author  |   Description
 * 2014/04/09 | 1.0.0.1   | kunyang  | 创建文件
 */
#ifndef ky_quaternion_H
#define ky_quaternion_H

#include "ky_define.h"
#include "ky_vec4.h"
#include "ky_mat2x2.h"
#include "ky_mat3x3.h"
#include "ky_mat4x4.h"

template<typename T>
struct ky_quaternion_mem {
    typedef T ElemType;
    enum {size = 4};
    union {
        struct {
            T x;
            T y;
            T z;
            T w;
        };

        T elem[size];
    };
};
template<typename T>
struct ky_quaternion : public ky_quaternion_mem<T>
{
    typedef ky_quaternion_mem<T> memory;

    /// Constructor, the standard is the identity-ky_quaternion.
    ky_quaternion (T X = 0, T Y = 0, T Z = 0, T W = 1)
    {
        memory::x = (X);
        memory::y = (Y);
        memory::z = (Z);
        memory::w = (W);
    }

    /// Constructor expecting a 3d-vector and a scalar value
    ky_quaternion (ky_vector3<T> V, T W)
    {
        memory::x = (V.x);
        memory::y = (V.y);
        memory::z = (V.z);
        memory::w = (W) ;
    }

    /// Init from array with size 4
    explicit ky_quaternion (const T* t)
    {
        for (int64 i = 0; i < 4; ++i)
            memory::elem[i] = t[i];
    }
    ky_quaternion (const ky_quaternion_mem<T> &t)
    {
        for (int64 i = 0; i < 4; ++i)
            memory::elem[i] = t.elem[i];
    }

    /// Init from matrix
    explicit ky_quaternion (const ky_matrix4X4<T>& m)
    {
        *this = generate_quaternion(m);
    }
    ky_quaternion(const ky_quaternion<T> &v)
    {
        for (int64 i = 0; i < v.size; ++i)
            memory::elem[i] = v.elem[i];
    }
    inline ky_quaternion<T> &operator = (const ky_quaternion<T> &c)
    {
        for (int64 i = 0; i < c.size; ++i)
            memory::elem[i] = c.elem[i];
        return *this;
    }

    /// return the vector consisting of the imaginary parts of the ky_quaternion
    inline ky_vector3<T> vec() const
    {
        return ky_vector3<T>(memory::x, memory::y, memory::z);
    }

    /// conjugates this ky_quaternion
    inline void conjugate()
    {
        memory::x = -memory::x;
        memory::y = -memory::y;
        memory::z = -memory::z;
    }

    /// inverts this ky_quaternion
    inline void invert()
    {
        const T absq = memory::x*memory::x + memory::y*memory::y +
                memory::z*memory::z + memory::w*memory::w;
        if (absq != 0)
        {
            conjugate();
            *this = *this / absq;
        }
    }

    /// 从给定的角度和代表该旋转的轴创建四元数
    static ky_quaternion create_quat(T angle, const ky_vector3<T>& axis)
    {
        ky_quaternion res;

        ky_vector3<T> newAxis = ky_normalize(axis);

        res.x = newAxis.x * ky_sinf(angle / 2);
        res.y = newAxis.y * ky_sinf(angle / 2);
        res.z = newAxis.z * ky_sinf(angle / 2);
        res.w = ky_cos(angle / 2);

        return res;
    }

    /// 使用四元数围绕轴旋转向量
    static ky_vector3<T> rotate(const ky_vector3<T>& target, T angle, const ky_vector3<T>& axis)
    {
        return rotate(target, create_quat(angle, axis));
    }

    /// 使用四元数围绕轴旋转向量
    static ky_vector3<T> rotate(const ky_vector3<T>& target, ky_quaternion rot)
    {

        ky_quaternion rotconj = rot;
        rotconj.conjugate();

        // create a target ky_quaternion so we can use ky_quaternion-multiplication
        ky_quaternion targ(target.x, target.y, target.z, 0.f);

        // We calculate r * t * r^-1.  This yields our rotated vector.  Technically,
        // we would have to invert rot, but since it is a unit ky_quaternion the inverted
        // ky_quaternion is equal to the conjugated one, so we use conjugation which is cheaper.
        // Also note that the returned vector is automatically of length 1 since we only use
        // use unit-ky_quaternions.
        ky_quaternion res = (rot * targ) * rotconj;

        return vec3(res.x, res.y, res.z);
    }
};

//!
//! \brief ky_mult_quat 计算两个四元数的点积
//! \param q1
//! \param q2
//! \return
//! \note
//!
template<typename T>
inline ky_quaternion<T> ky_mult_quat(const ky_quaternion<T>& q1, const ky_quaternion<T>& q2)
{
    ky_quaternion<T> res;

    res.x = q1.w*q2.x + q1.x*q2.w + q1.y*q2.z - q1.z*q2.y;
    res.y = q1.w*q2.y - q1.x*q2.z + q1.y*q2.w + q1.z*q2.x;
    res.z = q1.w*q2.z + q1.x*q2.y - q1.y*q2.x + q1.z*q2.w;
    res.w = q1.w*q2.w - q1.x*q2.x - q1.y*q2.y - q1.z*q2.z;

    return res;
}

//!
//! \brief ky_exp_quat
//! \param q
//! \return
//! \note e^quaternion given as:
//!       Qexp(v*a) = [cos(a),vsin(a)]
//!
template<typename T>
inline ky_quaternion<T> ky_exp_quat(const ky_quaternion<T>& q)
{
    const T a = ky_sqrt(q.x*q.x + q.y*q.y + q.y*q.y);
    const T sina = ky_sinf(a);
    ky_quaternion<T> ret;

    ret.w = ky_cosf(a);

    if (a > 0)
    {
        ret.x = sina * q.x / a;
        ret.y = sina * q.y / a;
        ret.z = sina * q.z / a;
    }
    else
    {
        ret.x = q.x;
        ret.y = q.y;
        ret.z = q.z; //= ret.y = ret.z = 0;
    }

    return ret;
}

//!
//! \brief ky_normalize 返回q的对数
//! \param q
//! \return
//! \note
//!
template<typename T>
inline ky_quaternion<T> ky_log_quat(const ky_quaternion<T>& q)
{
    const T a = ky_acosf(q.w);
    const T sina = ky_sinf(a);
    ky_quaternion<T> ret;

    ret.w = 0;

    if (sina > 0)
    {
        ret.x = a*q.x /sina;
        ret.y = a*q.y /sina;
        ret.z = a*q.z /sina;
    }
    else
        ret.x=ret.y=ret.z=0;

    return ret;
}

//!
//! \brief ky_normalize 归一化
//! \param q
//! \return
//! \note
//!
template<typename T>
inline ky_quaternion<T> ky_normalize(const ky_quaternion<T>& q)
{
    const T ab = q.abs();
    ky_quaternion<T> res = q;

    if (ab != 0) {
        res.x /= ab;
        res.y /= ab;
        res.z /= ab;
        res.w /= ab;
    }

    return res;
}

//!
//! \brief ky_conjugate
//! \param q
//! \return
//! \note
//!
template<typename T>
inline ky_quaternion<T> ky_conjugate(const ky_quaternion<T>& q)
{
    ky_quaternion<T> res = q;

    res.x *= -1;
    res.y *= -1;
    res.z *= -1;

    return res;
}

//!
//! \brief ky_invert 颠倒四元数
//! \param q
//! \return
//! \note
//!
template<typename T>
inline ky_quaternion<T> ky_invert(const ky_quaternion<T>& q)
{
    T absq = q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w;
    if (absq != 0)
    {
        ky_quaternion<T> res = ky_conjugate(q);
        return res / absq;
    }

    return q;
}

//!
//! \brief ky_lerp_quat 线性插值
//! \param a 使用a和b作为 "Control-Quaternion"
//! \param b
//! \param t 作为插值变量
//! \param inv
//! \return
//! \note
//!
template<typename T>
inline ky_quaternion<T> ky_lerp_quat(const ky_quaternion<T>& a, const ky_quaternion<T>& b, T t)
{
    kyASSERT(((t < T(0)) || (t > T(1))));

    return ky_normalize((b - a)*t + a);
}

//!
//! \brief ky_slerp_quat 球面线性插值
//! \param a 使用a和b作为 "Control-Quaternion"
//! \param b
//! \param t 作为插值变量
//! \param inv
//! \return
//! \note 这是基于有关实时渲染中四元数
//!
template<typename T>
inline ky_quaternion<T> ky_slerp_quat(const ky_quaternion<T>& a, const ky_quaternion<T>& b, T t, bool inv = true)
{
    kyASSERT(((t < T(0)) || (t > T(1))));

    ky_quaternion<T> c = b;
    T tmp = a.w*b.w + a.x*b.x + a.y*b.y + a.z*b.z;

    if (inv && tmp < T(0.0))
    {
        tmp = -tmp;
        c = -1.f*c;
    }

    if (tmp < T(1.0) && tmp > T(-1.0))
    {
        const T ang = ky_acos(tmp);
        return ky_normalize(((a * ky_sin(ang * (T(1.0) - t))) + (c * ky_sin(ang * t))) / ky_sin(ang));
    }
    return ky_lerp_quat(a, c, t);

}


//!
//! \brief ky_squad_quat 返回p和q之间的插值
//! \param p
//! \param q
//! \param a 使用a和b作为 "Control-Quaternion"
//! \param b
//! \param t 作为插值变量
//! \return
//! \note 这是基于有关实时渲染中四元数
//!
template<typename T>
inline ky_quaternion<T> ky_squad_quat(const ky_quaternion<T>& p, const ky_quaternion<T>& q,
                                  const ky_quaternion<T>& a, const ky_quaternion<T>& b, T t)
{
    kyASSERT(((t < T(0)) || (t > T(1))));

    return ky_slerp_quat<T>(ky_slerp_quat<T>(p, q, static_cast<T>(t)),
                            ky_slerp_quat<T>(a, b, static_cast<T>(t)),
                            static_cast<T>(2.0*t*(1.0 - t)), false);
}

//!
//! \brief ky_spline_quat 生成 "Control-Quaternion"，可用于小队算法在给定的四元数之间进行插值
//! \param pa
//! \param p
//! \param pb
//! \return
//! \note 这是基于有关实时渲染中四元数
//!
template<typename T>
inline ky_quaternion<T> ky_spline_quat(const ky_quaternion<T>& pa, const ky_quaternion<T>& p, const ky_quaternion<T>& pb)
{
    ky_quaternion<T> pinvert = ky_conjugate(p);

    return p * ky_exp_quat((ky_log_quat(pa*pinvert) + ky_log_quat(pb*pinvert)) * static_cast<T>(-0.25));
}

//!
//! \brief generate_quaternion 生成一个四元数表示从源到目标的旋转向量
//! \param src
//! \param dest
//! \return
//!
template<typename T>
inline ky_quaternion<T> generate_quaternion(const ky_vector3<T> &src, const ky_vector3<T> &dest)
{
    ky_quaternion<T> q;

    ky_vector3<T> v1 = ky_normalize(src);
    ky_vector3<T> v2 = ky_normalize(dest);

    ky_vector3<T> crs = ky_cross(v1, v2);

    T dot = v1.x*v2.x + v1.y+v2.y + v1.z*v2.z;

    if (ky_fabs(dot) >= 1) // the vectors are identical
        return ky_quaternion<T>(0, 0, 0, 1); // ... so we return a rotation that does nothing

    T root = ky_sqrt((1 + dot) * 2);

    if (root < T(1e-6))
    { // do this for numerical stability
        ky_vector3<T> axis = ky_cross(ky_vector3<T>(1, 0, 0), src);
        if (ky_length(axis) == 0)
            axis = ky_cross(ky_vector3<T>(0, 1, 0), src);
        axis = ky_normalize(axis);
        q = ky_quaternion<T>::create_quat(PIf, axis);
    }
    else
    {
        T inverse = 1 / root;
        q.x = crs.x * inverse;
        q.y = crs.y * inverse;
        q.z = crs.z * inverse;
        q.w = T(0.5) * inverse;
    }

    return q;
}

//!
//! \brief generate_matrix 从四元数生成旋转矩阵
//! \param q
//! \return
//!
template<typename T>
inline ky_matrix4X4<T> generate_matrix(const ky_quaternion<T>& q)
{
    T xx      = q.x * q.x;
    T xy      = q.x * q.y;
    T xz      = q.x * q.z;
    T xw      = q.x * q.w;
    T yy      = q.y * q.y;
    T yz      = q.y * q.z;
    T yw      = q.y * q.w;
    T zz      = q.z * q.z;
    T zw      = q.z * q.w;

    ky_matrix4X4<T> mat;

    mat.elem[0]  = 1 - 2 * ( yy + zz );
    mat.elem[1]  =     2 * ( xy - zw );
    mat.elem[2]  =     2 * ( xz + yw );
    mat.elem[4]  =     2 * ( xy + zw );
    mat.elem[5]  = 1 - 2 * ( xx + zz );
    mat.elem[6]  =     2 * ( yz - xw );
    mat.elem[8]  =     2 * ( xz - yw );
    mat.elem[9]  =     2 * ( yz + xw );
    mat.elem[10] = 1 - 2 * ( xx + yy );
    mat.elem[3]  = mat.elem[7] = mat.elem[11] = mat.elem[12] = mat.elem[13] = mat.elem[14] = 0;
    mat.elem[15] = 1;

    return ky_transpose(mat);
}

//!
//! \brief generate_quaternion 从旋转矩阵生成一个四元数
//! \param q
//! \return
//!
template<typename T>
inline ky_quaternion<T> generate_quaternion(const ky_matrix4X4<T>& mat)
{
    ky_quaternion<T> q;
    T t = mat[0][0] + mat[1][1] + mat[2][2] + T(1);

    //     if (t > 0) {
    //         double s = 0.5 / std::sqrt(t);
    //         q.w = 0.25 / s;
    //         q.x = ( mat[2][1] - mat[1][2] ) * s;
    //         q.y = ( mat[0][2] - mat[2][0] ) * s;
    //         q.z = ( mat[1][0] - mat[0][1] ) * s;
    //     } else if ((mat[0][0] > mat[1][1]) && (mat[0][0] > mat[2][2])) {
    //         double s = std::sqrt( 1.0 + mat[0][0] - mat[1][1] - mat[2][2] ) * 2.0;
    //         q.x = 0.25 / s;
    //         q.y = (mat[0][1] + mat[1][0] ) / s;
    //         q.z = (mat[0][2] + mat[2][0] ) / s;
    //         q.w = (mat[1][2] - mat[2][1] ) / s;
    //     } else if (mat[1][1] > mat[2][2]) {
    //         double s = std::sqrt( 1.0 + mat[1][1] - mat[0][0] - mat[2][2] ) * 2.0;
    //         q.x = (mat[0][1] + mat[1][0] ) / s;
    //         q.y = 0.25 / s;
    //         q.z = (mat[1][2] + mat[2][1] ) / s;
    //         q.w = (mat[0][2] - mat[2][0] ) / s;
    //     } else {
    //         double s = std::sqrt( 1.0 + mat[2][2] - mat[0][0] - mat[1][1] ) * 2.0;
    //         q.x = (mat[0][2] + mat[2][0] ) / s;
    //         q.y = (mat[1][2] + mat[2][1] ) / s;
    //         q.z = 0.25 / s;
    //         q.w = (mat[0][1] - mat[1][0] ) / s;
    //     }
    if (t > T(0))
    {
        T s = T(0.5) / ky_sqrt(t);
        q.w = T(0.25) / s;
        q.x = ( mat[1][2] - mat[2][1] ) * s;
        q.y = ( mat[2][0] - mat[0][2] ) * s;
        q.z = ( mat[0][1] - mat[1][0] ) * s;
    } else if ((mat[0][0] > mat[1][1]) && (mat[0][0] > mat[2][2])) {
        T s = ky_sqrt( T(1.0) + mat[0][0] - mat[1][1] - mat[2][2] ) * T(2.0);
        q.x = T(0.25) / s;
        q.y = (mat[1][0] + mat[0][1] ) / s;
        q.z = (mat[2][0] + mat[0][2] ) / s;
        q.w = (mat[2][1] - mat[1][2] ) / s;
    } else if (mat[1][1] > mat[2][2]) {
        T s = ky_sqrt( T(1.0) + mat[1][1] - mat[0][0] - mat[2][2] ) * T(2.0);
        q.x = (mat[1][0] + mat[0][1] ) / s;
        q.y = T(0.25) / s;
        q.z = (mat[2][1] + mat[1][2] ) / s;
        q.w = (mat[2][0] - mat[0][2] ) / s;
    } else {
        T s = ky_sqrt( T(1.0) + mat[2][2] - mat[0][0] - mat[1][1] ) * T(2.0);
        q.x = (mat[2][0] + mat[0][2] ) / s;
        q.y = (mat[2][1] + mat[1][2] ) / s;
        q.z = T(0.25) / s;
        q.w = (mat[1][0] - mat[0][1] ) / s;
    }

    return q;
}

template<typename T>
inline void generate_axis_angle(const ky_quaternion<T>& q, T& angle, ky_vector3<T>& axis)
{
    if (q.w > 1)
        ky_normalize(q);

    angle = 2 * ky_acos(q.w);
    T s = ky_sqrt(1 - q.w * q.w);

    if (s < 0.001)
    {
        axis.x = q.x;
        axis.y = q.y;
        axis.z = q.z;
    }
    else
    {
        axis.x = q.x / s; // normalise axis
        axis.y = q.y / s;
        axis.z = q.z / s;
    }
}

//!
//! \brief ky_abs 计算绝对值
//! \param q
//! \return
//!
template<typename T>
inline T ky_abs(const ky_quaternion<T> &q)
{
    return ky_sqrt(q.x * q.x + q.y *q.y + q.z * q.z + q.w * q.w);
}
//!
//! \brief ky_pow 计算此t次方
//! \param q
//! \param t
//! \return
//!
template<typename T>
inline ky_quaternion<T> ky_pow(const ky_quaternion<T> &q, const T t)
{
    if (ky_abs(q) != 1 || t < 0 || t > 0) return 0;

    T phi = ky_acos(q.w);
    ky_vector3<T> u = q.vec() / ky_sin(phi);

    T factor = ky_sin(phi*t);
    return ky_quaternion<T>(factor*u.x, factor*u.y, factor*u.z, ky_cos(phi*t));
}
//!
//! \brief ky_log 计算该自然对数
//! \param q
//! \return
//!
template<typename T>
inline ky_vector3<T> ky_log(const ky_quaternion<T> &q)
{
    if (ky_abs(q) != 1)
        return ky_vector3<T>(0);

    T phi = ky_acos(q.w);
    ky_vector3<T> u = q.vec() / ky_sin(phi);

    return u * phi;
}
//!
//! \brief ky_normalize 标准化
//! \param q
//! \return
//!
template<typename T>
inline void ky_normalize(const ky_quaternion<T> &q)
{
    const T ab = ky_abs(q);

    if (ab != 0)
    {
        q.x /= ab;
        q.y /= ab;
        q.z /= ab;
        q.w /= ab;
    }
}
/**
        Multiply a float with a ky_quaternion
    */
template<typename T>
inline ky_quaternion<T> operator * (T f, const ky_quaternion<T>& q)
{
    ky_quaternion<T> res;

    res.x = q.x * f;
    res.y = q.y * f;
    res.z = q.z * f;
    res.w = q.w * f;

    return res;
}

/// Calculates the ky_quaternion dot-product of this ky_quaternion
/// with another ky_quaternion.  The *-operator is overloaded for this purpose.
template<typename T>
inline ky_quaternion<T> operator * (const ky_quaternion<T>& src, const ky_quaternion<T>& q)
{
    ky_quaternion<T> res;

    res.x = src.w*q.x + src.x*q.w + src.y*q.z - src.z*q.y;
    res.y = src.w*q.y - src.x*q.z + src.y*q.w + src.z*q.x;
    res.z = src.w*q.z + src.x*q.y - src.y*q.x + src.z*q.w;
    res.w = src.w*q.w - src.x*q.x - src.y*q.y - src.z*q.z;

    return res;
}

/// Calculates the ky_quaternion dot-product of this ky_quaternion
/// with another ky_quaternion.  The *-operator is overloaded for this purpose.
template<typename T>
inline ky_quaternion<T> operator *= (ky_quaternion<T>& src, const ky_quaternion<T>& q)
{
    src = src * q;
    return src;
}

/// ky_quaternion addition
template<typename T>
inline ky_quaternion<T> operator + (const ky_quaternion<T>& src, const ky_quaternion<T>& q)
{
    ky_quaternion<T> res;

    res.x = src.x + q.x;
    res.y = src.y + q.y;
    res.z = src.z + q.z;
    res.w = src.w + q.w;

    return res;
}

/// ky_quaternion subtraction
template<typename T>
inline ky_quaternion<T> operator - (const ky_quaternion<T>& src, const ky_quaternion<T>& q)
{
    ky_quaternion<T> res;

    res.x = src.x - q.x;
    res.y = src.y - q.y;
    res.z = src.z - q.z;
    res.w = src.w - q.w;

    return res;
}

/// divide ky_quaternion by a float
template<typename T>
inline ky_quaternion<T> operator / (const ky_quaternion<T>& src, const T f)
{
    ky_quaternion<T> res;

    res.x = src.x / f;
    res.y = src.y / f;
    res.z = src.z / f;
    res.w = src.w / f;

    return res;
}

/// multiply ky_quaternion with a float
template<typename T>
inline ky_quaternion<T> operator * (const ky_quaternion<T>& src, const T f)
{
    ky_quaternion<T> res;

    res.x = src.x * f;
    res.y = src.y * f;
    res.z = src.z * f;
    res.w = src.w * f;

    return res;
}

/// multiply ky_quaternion with a vector
template<typename T>
inline ky_vector3<T> operator * (const ky_quaternion<T>& src, const ky_vector3<T> v)
{
    ky_vector3<T> tmp1, tmp2;
    ky_vector3<T> qvector = src.vec();

    tmp1 = ky_cross(qvector, v);
    tmp2 = ky_cross(qvector, tmp1);
    tmp1 = (T)(2 * src.w) * tmp1;
    tmp2 = (T)2 * tmp2;

    return v + tmp1 + tmp2;
}

/// equality operator
template<typename T>
inline bool operator == (const ky_quaternion<T>& src, const ky_quaternion<T>& q)
{
    return(src.x == q.x && src.y == q.y && src.z == q.z && src.w == q.w);
}


template<typename T>
inline void ky_swap (ky_quaternion<T> &v1, ky_quaternion<T> &v2)
{
    ky_quaternion<T> tmp = v1;
    v1 = v2;
    v2 = tmp;
}
/**
        ostream-operator for output
    */
template<typename T>
std::ostream& operator << (std::ostream& s, const ky_quaternion<T>& q) {
    return (s << "{ " << q.x << " " << q.y << " " << q.z << " " << q.w << " }" << std::endl);
}

// just a typedef for easier use, since most of the time float-ky_quaternions are used
typedef ky_quaternion<float>   quat;
typedef ky_quaternion<float>   fquat;
typedef ky_quaternion<real>  dquat;


template<typename T>
ky_streamb &operator << (ky_streamb &in, const ky_quaternion<T> &v)
{
    for (int i = 0;i < v.size; ++i)
        in << v.elem[i];
    return in;
}

template<typename T>
ky_streamb &operator >> (ky_streamb &out, ky_quaternion<T> &v)
{
    for (int i = 0;i < v.size; ++i)
        out >> v.elem[i];
    return out;
}
template<typename T>
ky_streamt &operator << (ky_streamt &in, const ky_quaternion<T> &v)
{
    for (int i = 0;i < v.size; ++i)
        in << v.elem[i];
    return in;
}

template<typename T>
ky_streamt &operator >> (ky_streamt &out, ky_quaternion<T> &v)
{
    for (int i = 0;i < v.size; ++i)
        out >> v.elem[i];
    return out;
}
const fourwd_t kyQuatCode = kyFourWord(quat);
template<typename T>
ky_variant &operator << (ky_variant &va, const ky_quaternion<T> &col)
{
    va.set(kyQuatCode, 0, &col, sizeof(ky_quaternion<T>));
    return va;
}
template<typename T>
ky_variant &operator >> (ky_variant &va, ky_quaternion<T> &col)
{
    col = va.get<ky_quaternion<T>>(kyQuatCode);
    return va;
}
#endif // ky_quaternion_H
