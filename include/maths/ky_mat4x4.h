
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
 * @file     ky_mat4x4.h
 * @brief    4x4矩阵类
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
#ifndef MAT4X4_H
#define MAT4X4_H
#include "ky_vec4.h"
#include "ky_debug.h"

template<typename T>
struct ky_matrix4X4_mem
{
    typedef T ElemType;
    enum
    {
        size = 16,
        cols = 4,
        rows = 4
    };

    union
    {
        struct {
            T t00, t01, t02, t03;
            T t10, t11, t12, t13;
            T t20, t21, t22, t23;
            T t30, t31, t32, t33;
        };
        T elemRowCol[4][4];
        T elem[16];
    };
};

template<typename T>
struct ky_matrix4X4 : public ky_matrix4X4_mem<T>
{
    typedef ky_vector4<T> RowType;
    typedef ky_matrix4X4_mem<T> memory;

    ky_matrix4X4() {}
    explicit ky_matrix4X4(T t)
    {
        for (int64 i = 0; i < memory::size; ++i)
            memory::elem[i] = t;
    }
    template<typename U>
    ky_matrix4X4(const ky_matrix4X4<U>& m)
    {
        for (int64 i = 0; i < m.size; ++i)
            memory::elem[i] = T(m.elem[i]);
    }
    explicit ky_matrix4X4(const T* t)
    {
        for (int64 i = 0; i < memory::size; ++i)
            memory::elem[i] = t[i];
    }

    ky_matrix4X4(T _t00, T _t01, T _t02, T _t03,
                 T _t10, T _t11, T _t12, T _t13,
                 T _t20, T _t21, T _t22, T _t23,
                 T _t30, T _t31, T _t32, T _t33)
    {
        memory::t00 = _t00; memory::t01 = _t01; memory::t02 = _t02; memory::t03 = _t03;
        memory::t10 = _t10; memory::t11 = _t11; memory::t12 = _t12; memory::t13 = _t13;
        memory::t20 = _t20; memory::t21 = _t21; memory::t22 = _t22; memory::t23 = _t23;
        memory::t30 = _t30; memory::t31 = _t31; memory::t32 = _t32; memory::t33 = _t33;
    }

    ky_matrix4X4(const ky_vector4<T>& v1, const ky_vector4<T>& v2,
                 const ky_vector4<T>& v3, const ky_vector4<T>& v4)
    {
        for (int64 i = 0; i < v1.size; ++i)
            memory::elem[0*memory::rows + i] = v1.elem[i];
        for (int64 i = 0; i < v2.size; ++i)
            memory::elem[1*memory::rows + i] = v2.elem[i];
        for (int64 i = 0; i < v3.size; ++i)
            memory::elem[2*memory::rows + i] = v3.elem[i];
        for (int64 i = 0; i < v4.size; ++i)
            memory::elem[3*memory::rows + i] = v4.elem[i];
    }

    template<typename U>
    ky_matrix4X4(const ky_matrix4X4_mem<U>& v)
    {
        for (int64 i = 0; i < v.size; ++i)
            memory::elem[i] = T(v.elem[i]);
    }
    ky_matrix4X4(const ky_matrix4X4_mem<T> & v)
    {
        for (int64 i = 0; i < v.size; ++i)
            memory::elem[i] = v.elem[i];
    }
    ky_matrix4X4(const ky_matrix4X4<T> &v)
    {
        for (int64 i = 0; i < v.size; ++i)
            memory::elem[i] = v.elem[i];
    }
    inline ky_matrix4X4<T> &operator = (const ky_matrix4X4<T> &c)
    {
        for (int64 i = 0; i < c.size; ++i)
            memory::elem[i] = c.elem[i];
        return *this;
    }

    inline const ky_vector4<T>& operator [] (int64 i) const
    {
        kyASSERT(i >= memory::size);
        return *((ky_vector4<T>*) memory::elemRowCol[i]);
    }
    inline ky_vector4<T>& operator [] (int64 i)
    {
        kyASSERT(i >= memory::size);
        return *((ky_vector4<T>*) memory::elemRowCol[i]);
    }

    //!
    //! \brief get_scaling 获取缩放
    //! \return
    //!
    inline ky_vector3<T> get_scaling() const;

    //!
    //! \brief get_rotational 获取旋转矩阵
    //! \return
    //!
    inline ky_matrix4X4<T> get_rotational() const;

    //!
    //! \brief invert 逆矩阵运算
    //! \param result
    //! \return
    //!
    inline bool get_invert(ky_matrix4X4<T>& result) const;

    //!
    //! \brief create_zero
    //! \return
    //!
    static ky_matrix4X4<T> create_zero() {return zero;}
    //!
    //! \brief create_identity
    //! \return
    //!
    static ky_matrix4X4<T> create_identity() {return identity;}

    //!
    //! \brief create_translation 创建移动矩阵
    //! \param v
    //! \return
    //!
    static ky_matrix4X4<T> create_translation(const ky_vector3<T>& v);

    //!
    //! \brief create_scale 创建缩放矩阵
    //! \param v
    //! \return
    //!
    static ky_matrix4X4<T> create_scale(const ky_vector3<T>& v);

    //!
    //! \brief create_rotation 创建旋转矩阵
    //! \param angle 角度(弧度单位)
    //! \param axis 旋转轴
    //! \return
    //!
    static ky_matrix4X4<T> create_rotation(T angle, ky_vector3<T> axis);

    //!
    //! \brief create_lookAt 创建观看矩阵
    //! \param eye 眼睛位置
    //! \param focus 观看焦点
    //! \param up   向上矢量
    //! \return
    //!
    static ky_matrix4X4<T> create_lookAt(const ky_vector3<T>& eye, const ky_vector3<T>& focus, const ky_vector3<T>& up);

    //!
    //! \brief create_frustum 创建视锥投影矩阵
    //! \param left
    //! \param right
    //! \param top
    //! \param bottom
    //! \param pnear
    //! \param pfar
    //! \return
    //!
    static ky_matrix4X4<T> create_frustum(T left, T right, T top, T bottom, T pnear, T pfar);

    //!
    //! \brief create_perspective 创建透视投影矩阵
    //! \param fov   视角 (弧度)
    //! \param aspect 宽高比
    //! \param pnear 近裁剪
    //! \param pfar 远裁剪
    //! \return
    //!
    static ky_matrix4X4<T> create_perspective(T fov, T aspect, T pnear, T pfar);

    //!
    //! \brief create_ortho 创建正交投影矩阵
    //! \param left
    //! \param right
    //! \param top
    //! \param bottom
    //! \param pnear
    //! \param pfar
    //! \return
    //!
    static ky_matrix4X4<T> create_ortho(T left, T right, T top, T bottom, T pnear, T pfar);


    static const ky_matrix4X4<T> zero;
    static const ky_matrix4X4<T> identity;
};

typedef ky_matrix4X4<float>  ky_mat4X4f;
typedef ky_matrix4X4<real>   ky_mat4X4d;

typedef ky_mat4X4f           ky_matrix;
typedef ky_mat4X4f           mat4;
typedef ky_mat4X4f           fmat4;
typedef ky_mat4X4d           dmat4;

typedef ky_mat4X4f           ky_xform;

template<typename T>
const ky_matrix4X4<T> ky_matrix4X4<T>::zero = ky_matrix4X4<T>
        (
            T(0), T(0), T(0), T(0),
            T(0), T(0), T(0), T(0),
            T(0), T(0), T(0), T(0),
            T(0), T(0), T(0), T(0)
            );

template<typename T>
const ky_matrix4X4<T> ky_matrix4X4<T>::identity = ky_matrix4X4<T>
        (
            T(1), T(0), T(0), T(0),
            T(0), T(1), T(0), T(0),
            T(0), T(0), T(1), T(0),
            T(0), T(0), T(0), T(1)
            );


template<typename T>
inline ky_vector3<T> ky_matrix4X4<T>::get_scaling() const
{
    ky_vector3<T> result(T(0));
    result.elem[0] = memory::t00;
    result.elem[1] = memory::t11;
    result.elem[2] = memory::t22;
    return result;
}

template<typename T>
inline ky_matrix4X4<T> ky_matrix4X4<T>::get_rotational() const
{
    ky_matrix4X4<T> result(T(0));
    result.t00 = memory::t00; result.t01  = memory::t01; result.t02 = memory::t02;
    result.t10 = memory::t10; result.t11  = memory::t11; result.t12 = memory::t12;
    result.t20 = memory::t20; result.t21  = memory::t21; result.t22 = memory::t22;
    result.t33 = T(1);
    return result;
}
template<typename T>
bool ky_matrix4X4<T>::get_invert(ky_matrix4X4<T>& result) const
{
    T t;
    ky_matrix4X4<T> tmp = *this;
    result = ky_matrix4X4<T>::identity;

    for (int64 i = 0; i < 4; ++i)
    {
        // Look for largest element in column
        int64 swap = i;
        for (int64 j = i + 1; j < 4; ++j)
        {
            if (ky_fabs(tmp.elemRowCol[j][i]) > ky_fabs(tmp.elemRowCol[i][i]))
                swap = j;
        }

        if (swap != i)
        {
            // Swap rows.
            for (int64 k = 0; k < 4; ++k)
            {
                t = tmp.elemRowCol[i][k];
                tmp.elemRowCol[i][k] = tmp.elemRowCol[swap][k];
                tmp.elemRowCol[swap][k] = t;

                t = result.elem[i*4+k];
                result.elem[i*4+k] = result.elem[swap*4+k];
                result.elem[swap*4+k] = t;
            }
        }

        if (tmp.elemRowCol[i][i] == 0)
        {
            // The matrix is singular
            return false;
        }

        t = tmp.elemRowCol[i][i];
        for (int64 k = 0; k < 4; k++)
        {
            tmp.elemRowCol[i][k] /= t;
            result.elem[i*4+k] /= t;
        }
        for (int64 j = 0; j < 4; j++)
        {
            if (j != i) {
                t = tmp.elemRowCol[j][i];

                for (int64 k = 0; k < 4; k++) {
                    tmp.elemRowCol[j][k] -= tmp.elemRowCol[i][k]*t;
                    result.elem[j*4+k] -= result.elem[i*4+k]*t;
                }
            }
        }
    }
    return true;
}

template<typename T>
ky_matrix4X4<T> ky_matrix4X4<T>::create_translation(const ky_vector3<T>& v)
{
    ky_matrix4X4<T> result = identity;
    result.t03 = v.elem[0];
    result.t13 = v.elem[1];
    result.t23 = v.elem[2];
    return result;
}
template<typename T>
ky_matrix4X4<T> ky_matrix4X4<T>::create_scale(const ky_vector3<T>& v)
{
    ky_matrix4X4<T> result = identity;
    result.t00 = v.elem[0];
    result.t11 = v.elem[1];
    result.t22 = v.elem[2];
    return result;
}
template<typename T>
ky_matrix4X4<T> ky_matrix4X4<T>::create_rotation(T angle, ky_vector3<T> axis)
{
    axis = ky_normalize(axis);
    T s = ky_sin(angle);
    T c = ky_cos(angle);
    T x = axis.x;
    T y = axis.y;
    T z = axis.z;

    ky_matrix4X4<T> result
            (
                x*x*(1-c) + c,      x*y*(1-c) - z*s,    x*z*(1-c) + y*s, T(0),
                x*y*(1-c) + z*s,    y*y*(1-c) + c,      y*z*(1-c) - x*s, T(0),
                x*z*(1-c) - y*s,    y*z*(1-c) + x*s,    z*z*(1-c) + c,   T(0),
                T(0),               T(0),               T(0),            T(1)
                );

    return result;
}

template<typename T>
ky_matrix4X4<T> ky_matrix4X4<T>::create_lookAt(const ky_vector3<T>& eye, const ky_vector3<T>& focus, const ky_vector3<T>& up) {

    ky_vector3<T> look   = ky_normalize(focus - eye); // orientation
    ky_vector3<T> strafe = ky_normalize(ky_cross(look, ky_normalize(up)));
    ky_vector3<T> up2    = ky_cross(strafe, look);

    ky_matrix4X4<T> m(ky_vector4<T>( strafe, T(0) ),
                      ky_vector4<T>( up2, T(0) ),
                      ky_vector4<T>( -look, T(0) ),
                      ky_vector4<T>( T(0), T(0), T(0), T(1) ));

    return m * ky_matrix4X4<T>::create_translation(-eye);
}

template<typename T>
ky_matrix4X4<T> ky_matrix4X4<T>::create_frustum(T left, T right, T bottom, T top, T pnear, T pfar)
{
    const T width = right - left;
    const T invheight = top - bottom;
    const T clip = pnear - pfar;
    ky_matrix4X4<T> m(
                T(2)*pnear/(width), T(0),                    (right+left)/(width),     T(0),
                T(0),               T(2)*pnear/(top-bottom), (top+bottom)/(invheight), T(0),
                T(0),               T(0),                    (pnear + pfar)/clip,     (T(2)*pfar*pnear) / clip,
                T(0),               T(0),                    -T(1),                    T(0)
                );

    return m;
}

template<typename T>
ky_matrix4X4<T> ky_matrix4X4<T>::create_perspective(T fov, T aspect, T pnear, T pfar)
{
    const T half_fovy = T(1) / ky_tan( fov/T(2) );
    ky_matrix4X4<T> m(
                half_fovy / aspect, T(0),         T(0),                      T(0),
                T(0),               half_fovy,    T(0),                      T(0),
                T(0),               T(0), (pnear+pfar)/(pnear-pfar), (T(2)*pfar*pnear)/(pnear-pfar),
                T(0),               T(0), -T(1),                     T(0)
                );

    return m;
}

template<typename T>
ky_matrix4X4<T> ky_matrix4X4<T>::create_ortho(T left, T right, T top, T bottom, T pnear, T pfar)
{
    const T width = right - left;
    const T invheight = top - bottom;
    const T clip = pfar - pnear;
    ky_matrix4X4<T> m(
                T(2)/width, T(0),             T(0),         -(left+right)/width,
                T(0),         T(2)/(invheight), T(0),         -(top+bottom)/invheight,
                T(0),         T(0),             T(-2)/(clip), -(pfar + pnear)/clip,
                T(0),         T(0),             T(0),         T(1)
                );

    return m;
}

//!
//! \brief ky_transpose 矩阵转置
//!
template<typename T>
inline ky_matrix4X4<T> ky_transpose(const ky_matrix4X4<T>& m)
{
    ky_matrix4X4<T> mRes;
    for (int64 row = 0; row < m.rows; ++row)
        for (int64 col = 0; col < m.cols; ++col)
            mRes.elemRowCol[col][row] = m.elemRowCol[row][col];
    return mRes;
}

template<typename T>
inline ky_matrix4X4<T> ky_matrix_comp_mult (const ky_matrix4X4<T>& v1, const ky_matrix4X4<T>& v2)
{
    ky_matrix4X4<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = v1.elem[i] * v2.elem[i];
    return vRes;
}


template<typename T>
inline ky_matrix4X4<T> ky_min(const ky_matrix4X4<T>& v1, const ky_matrix4X4<T>& v2)
{
    ky_matrix4X4<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = ky_min(v1.elem[i], v2.elem[i]);
    return vRes;
}

template<typename T>
inline ky_matrix4X4<T> ky_max(const ky_matrix4X4<T>& v1, const ky_matrix4X4<T>& v2)
{
    ky_matrix4X4<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = ky_max(v1.elem[i], v2.elem[i]);
    return vRes;
}

template<typename T>
inline ky_matrix4X4<T> ky_floor(const ky_matrix4X4<T>& v)
{
    ky_matrix4X4<T> result;
    for (int64 i = 0; i < v.size; ++i)
        result[i] = ky_floor(v.elem[i]);
    return result;
}

template<typename T>
inline ky_matrix4X4<T> ky_ceil(const ky_matrix4X4<T>& v)
{
    ky_matrix4X4<T> result;
    for (int64 i = 0; i < v.size; ++i)
        result[i] = ky_ceil(v.elem[i]);
    return result;
}

template<typename T>
inline ky_matrix4X4<T> ky_min(const ky_matrix4X4<T>& v, T t)
{
    ky_matrix4X4<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = ky_min(v.elem[i], t);
    return vRes;
}

template<typename T>
inline ky_matrix4X4<T> ky_max(const ky_matrix4X4<T>& v, T t)
{
    ky_matrix4X4<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = ky_max(v.elem[i], t);
    return vRes;
}

template<typename T>
inline T ky_min(const ky_matrix4X4<T>& v)
{
    T res = v.elem[0];
    for (int64 i = 1; i < v.size; ++i)
        res = ky_min(v.elem[i], res);
    return res;
}

template<typename T>
inline T ky_max(const ky_matrix4X4<T>& v)
{
    T res = v.elem[0];
    for (int64 i = 1; i < v.size; ++i)
        res = ky_max(v.elem[i], res);
    return res;
}

template<typename T>
inline ky_matrix4X4<T> ky_clamp(const ky_matrix4X4<T>& v, const ky_matrix4X4<T>& minVal,
                                const ky_matrix4X4<T>& maxVal)
{
    return ky_min( ky_max(v, minVal), maxVal );
}

template<typename T>
inline ky_matrix4X4<T> ky_clamp(const ky_matrix4X4<T>& v, T minVal, T maxVal)
{
    return ky_min( ky_max(v, minVal), maxVal );
}

template<typename T>
inline T ky_hadd(const ky_matrix4X4<T>& v)
{
    T result = v.elem[0];
    for (int64 i = 1; i < v.size; ++i)
        result += v.elem[i];
    return result;
}

template<typename T>
inline T ky_hsub(const ky_matrix4X4<T>& v)
{
    T result = v.elem[0];
    for (int64 i = 1; i < v.size; ++i)
        result -= v.elem[i];
    return result;
}

template<typename T>
inline T ky_hmul(const ky_matrix4X4<T>& v)
{
    T result = v.elem[0];
    for (int64 i = 1; i < v.size; ++i)
        result *= v.elem[i];
    return result;
}

template<typename T>
inline T ky_hdiv(const ky_matrix4X4<T>& v)
{
    T result = v.elem[0];
    for (int64 i = 1; i < v.size; ++i)
        result /= v.elem[i];
    return result;
}

template<typename T>
inline T ky_hmod(const ky_matrix4X4<T>& v)
{
    T result = v.elem[0];
    for (int64 i = 1; i < v.size; ++i)
        result %= v.elem[i];
    return result;
}

template<typename T>
inline bool ky_hand(const ky_matrix4X4<T>& v)
{
    bool result = v.elem[0] && v.elem[1];
    for (int64 i = 2; i < v.size; ++i)
        result &= v.elem[i];
    return result;
}

template<typename T>
inline bool ky_hor(const ky_matrix4X4<T>& v)
{
    T result = v.elem[0] || v.elem[1];
    for (int64 i = 2; i < v.size; ++i)
        result |= v.elem[i];
    return result;
}

template<typename T>
inline bool ky_hxor(const ky_matrix4X4<T>& v)
{
    T result = v.elem[0] ^ v.elem[1];
    for (int64 i = 2; i < v.size; ++i)
        result ^= v.elem[i];
    return result;
}

//!
//! \brief ky_less_than 返回两矩阵小于
//! \param v1
//! \param v2
//! \return
//!
template<typename T>
inline ky_matrix4X4<bool> ky_less_than (const ky_matrix4X4<T>& v1, const ky_matrix4X4<T>& v2)
{
    ky_matrix4X4<bool> vRes;
    for (int64 i = 0; i < vRes.size; ++i)
        vRes.elem[i] = (v1.elem[i] < v2.elem[i]);
    return vRes;
}
//!
//! \brief ky_less_than_equal 返回两矩阵小于等于
//! \param v1
//! \param v2
//! \return
//!
template<typename T>
inline ky_matrix4X4<bool> ky_less_than_equal (const ky_matrix4X4<T>& v1, const ky_matrix4X4<T>& v2)
{
    ky_matrix4X4<bool> vRes;
    for (int64 i = 0; i < vRes.size; ++i)
        vRes.elem[i] = (v1.elem[i] <= v2.elem[i]);
    return vRes;
}
//!
//! \brief ky_greater_than 返回两矩阵大于
//! \param v1
//! \param v2
//! \return
//!
template<typename T>
inline ky_matrix4X4<bool> ky_greater_than (const ky_matrix4X4<T>& v1, const ky_matrix4X4<T>& v2)
{
    ky_matrix4X4<bool> vRes;
    for (int64 i = 0; i < vRes.size; ++i)
        vRes.elem[i] = (v1.elem[i] > v2.elem[i]);
    return vRes;
}
//!
//! \brief ky_greater_than_equal 返回两矩阵大于等于
//! \param v1
//! \param v2
//! \return
//!
template<typename T>
inline ky_matrix4X4<bool> ky_greater_than_equal (const ky_matrix4X4<T>& v1, const ky_matrix4X4<T>& v2)
{
    ky_matrix4X4<bool> vRes;
    for (int64 i = 0; i < vRes.size; ++i)
        vRes.elem[i] = (v1.elem[i] >= v2.elem[i]);
    return vRes;
}
//!
//! \brief ky_equal 返回两矩阵等于
//! \param v1
//! \param v2
//! \return
//!
template<typename T>
inline ky_matrix4X4<bool> ky_equal (const ky_matrix4X4<T>& v1, const ky_matrix4X4<T>& v2)
{
    ky_matrix4X4<bool> vRes;
    for (int64 i = 0; i < vRes.size; ++i)
        vRes.elem[i] = (v1.elem[i] == v2.elem[i]);
    return vRes;
}

//!
//! \brief ky_not_equal 返回两矩阵不等
//! \param v1
//! \param v2
//! \return
//!
template<typename T>
inline ky_matrix4X4<bool> ky_not_equal (const ky_matrix4X4<T>& v1, const ky_matrix4X4<T>& v2)
{
    ky_matrix4X4<bool> vRes;
    for (int64 i = 0; i < vRes.size; ++i)
        vRes.elem[i] = (v1.elem[i] != v2.elem[i]);
    return vRes;
}

template<typename T>
inline bool operator == (const ky_matrix4X4<T>& v1, const ky_matrix4X4<T>& v2)
{
    bool result = true;
    for (int64 i = 0; i < v1.size; ++i)
        result &= (v1.elem[i] == v2.elem[i]);
    return result;
}

template<typename T>
inline bool operator != (const ky_matrix4X4<T>& v1, const ky_matrix4X4<T>& v2)
{
    return !(v1 == v2);
}

template<typename T>
inline ky_matrix4X4<T> operator - (const ky_matrix4X4<T>& v)
{
    ky_matrix4X4<T> vRes(v);
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = -v.elem[i];
    return vRes;
}

template<typename T>
inline typename ky_matrix4X4<T>::RowType operator * (const ky_matrix4X4<T>& m,
                                                     const typename ky_matrix4X4<T>::RowType& v)
{
    typename ky_matrix4X4<T>::RowType vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes[i] = dot(m[i], v);
    return vRes;
}

template<typename T>
inline typename ky_matrix4X4<T>::RowType operator * (const typename ky_matrix4X4<T>::RowType& v,
                                                     const ky_matrix4X4<T>& m)
{
    typename ky_matrix4X4<T>::RowType vRes;
    ky_matrix4X4<T> transposed = ky_transpose(m);
    for (int64 i = 0; i < v.size; ++i)
        vRes[i] = dot(v, transposed[i]);
    return vRes;
}

template<typename T>
inline ky_matrix4X4<T> operator * (const ky_matrix4X4<T>& m1, const ky_matrix4X4<T>& m2)
{
    ky_matrix4X4<T> mRes;
    ky_matrix4X4<T> transposed = ky_transpose(m2);
    for (int64 row = 0; row < m1.rows; ++row)
        for (int64 col = 0; col < m1.cols; ++col)
            mRes.elemRowCol[row][col] =
                    dot(m1[row], transposed[col]);
    return mRes;
}

template<typename T>
inline ky_matrix4X4<T>& operator *= (ky_matrix4X4<T>& m1, const ky_matrix4X4<T>& m2)
{
    ky_matrix4X4<T> temp = m1;
    m1 = temp * m2;
    return m1;
}

template<typename T>
inline ky_matrix4X4<T> operator + (const ky_matrix4X4<T>& v1, const ky_matrix4X4<T>& v2)
{
    ky_matrix4X4<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = v1.elem[i] + v2.elem[i];
    return vRes;
}

template<typename T>
inline ky_matrix4X4<T> operator + (const ky_matrix4X4<T>& v, T t)
{
    ky_matrix4X4<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = v.elem[i] + t;
    return vRes;
}

template<typename T>
inline ky_matrix4X4<T> operator + (T t, const ky_matrix4X4<T>& v)
{
    ky_matrix4X4<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = t + v.elem[i];
    return vRes;
}

template<typename T>
inline ky_matrix4X4<T>& operator += (ky_matrix4X4<T>& v1, const ky_matrix4X4<T>& v2)
{
    for (int64 i = 0; i < v1.size; ++i)
        v1.elem[i] += v2.elem[i];
    return v1;
}

template<typename T>
inline ky_matrix4X4<T>& operator += (ky_matrix4X4<T>& v, T t)
{
    for (int64 i = 0; i < v.size; ++i)
        v.elem[i] += t;
    return v;
}

template<typename T>
inline ky_matrix4X4<T> operator - (const ky_matrix4X4<T>& v1, const ky_matrix4X4<T>& v2)
{
    ky_matrix4X4<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = v1.elem[i] - v2.elem[i];
    return vRes;
}

template<typename T>
inline ky_matrix4X4<T> operator - (const ky_matrix4X4<T>& v, T t)
{
    ky_matrix4X4<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = v.elem[i] - t;
    return vRes;
}

template<typename T>
inline ky_matrix4X4<T> operator - (T t, const ky_matrix4X4<T>& v)
{
    ky_matrix4X4<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = t - v.elem[i];
    return vRes;
}

template<typename T>
inline ky_matrix4X4<T>& operator -= (ky_matrix4X4<T>& v1, const ky_matrix4X4<T>& v2)
{
    for (int64 i = 0; i < v1.size; ++i)
        v1.elem[i] -= v2.elem[i];
    return v1;
}

template<typename T>
inline ky_matrix4X4<T>& operator -= (ky_matrix4X4<T>& v, T t)
{
    for (int64 i = 0; i < v.size; ++i)
        v.elem[i] -= t;
    return v;
}

template<typename T>
inline ky_matrix4X4<T> operator * (const ky_matrix4X4<T>& v, T t)
{
    ky_matrix4X4<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = v.elem[i] * t;
    return vRes;
}

template<typename T>
inline ky_matrix4X4<T> operator * (T t, const ky_matrix4X4<T>& v)
{
    ky_matrix4X4<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = t * v.elem[i];
    return vRes;
}

template<typename T>
inline ky_matrix4X4<T>& operator *= (ky_matrix4X4<T>& v, T t)
{
    for (int64 i = 0; i < v.size; ++i)
        v.elem[i] *= t;
    return v;
}

template<typename T>
inline ky_matrix4X4<T> operator / (const ky_matrix4X4<T>& v1, const ky_matrix4X4<T>& v2)
{
    ky_matrix4X4<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = v1.elem[i] / v2.elem[i];
    return vRes;
}

template<typename T>
inline ky_matrix4X4<T> operator / (const ky_matrix4X4<T>& v, T t)
{
    ky_matrix4X4<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = v.elem[i] / t;
    return vRes;
}

template<typename T>
inline ky_matrix4X4<T> operator / (T t, const ky_matrix4X4<T>& v)
{
    ky_matrix4X4<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = t / v.elem[i];
    return vRes;
}

template<typename T>
inline ky_matrix4X4<T>& operator /= (ky_matrix4X4<T>& v1, const ky_matrix4X4<T>& v2)
{
    for (int64 i = 0; i < v1.size; ++i)
        v1.elem[i] /= v2.elem[i];
    return v1;
}

template<typename T>
inline ky_matrix4X4<T>& operator /= (ky_matrix4X4<T>& v, T t)
{
    for (int64 i = 0; i < v.size; ++i)
        v.elem[i] /= t;
    return v;
}

template<typename T>
inline ky_matrix4X4<T> operator & (const ky_matrix4X4<T>& v1, const ky_matrix4X4<T>& v2)
{
    ky_matrix4X4<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = v1.elem[i] & v2.elem[i];
    return vRes;
}

template<typename T>
inline ky_matrix4X4<T> operator & (const ky_matrix4X4<T>& v, T t)
{
    ky_matrix4X4<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = v.elem[i] & t;
    return vRes;
}

template<typename T>
inline ky_matrix4X4<T> operator & (T t, const ky_matrix4X4<T>& v)
{
    ky_matrix4X4<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = t & v.elem[i];
    return vRes;
}

template<typename T>
inline ky_matrix4X4<T>& operator &= (ky_matrix4X4<T>& v1, const ky_matrix4X4<T>& v2)
{
    for (int64 i = 0; i < v1.size; ++i)
        v1.elem[i] &= v2.elem[i];
    return v1;
}

template<typename T>
inline ky_matrix4X4<T>& operator &= (ky_matrix4X4<T>& v, T t)
{
    for (int64 i = 0; i < v.size; ++i)
        v.elem[i] &= t;
    return v;
}

template<typename T>
inline ky_matrix4X4<T> operator | (const ky_matrix4X4<T>& v1, const ky_matrix4X4<T>& v2)
{
    ky_matrix4X4<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = v1.elem[i] | v2.elem[i];
    return vRes;
}

template<typename T>
inline ky_matrix4X4<T> operator | (const ky_matrix4X4<T>& v, T t)
{
    ky_matrix4X4<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = v.elem[i] | t;
    return vRes;
}

template<typename T>
inline ky_matrix4X4<T> operator | (T t, const ky_matrix4X4<T>& v)
{
    ky_matrix4X4<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = t | v.elem[i];
    return vRes;
}

template<typename T>
inline ky_matrix4X4<T>& operator |= (ky_matrix4X4<T>& v1, const ky_matrix4X4<T>& v2)
{
    for (int64 i = 0; i < v1.size; ++i)
        v1.elem[i] |= v2.elem[i];
    return v1;
}

template<typename T>
inline ky_matrix4X4<T>& operator |= (ky_matrix4X4<T>& v, T t)
{
    for (int64 i = 0; i < v.size; ++i)
        v.elem[i] |= t;
    return v;
}

template<typename T>
inline ky_matrix4X4<T> operator ^ (const ky_matrix4X4<T>& v1, const ky_matrix4X4<T>& v2)
{
    ky_matrix4X4<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = v1.elem[i] ^ v2.elem[i];
    return vRes;
}

template<typename T>
inline ky_matrix4X4<T> operator ^ (const ky_matrix4X4<T>& v, T t)
{
    ky_matrix4X4<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = v.elem[i] ^ t;
    return vRes;
}

template<typename T>
inline ky_matrix4X4<T> operator ^ (T t, const ky_matrix4X4<T>& v)
{
    ky_matrix4X4<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = t ^ v.elem[i];
    return vRes;
}

template<typename T>
inline ky_matrix4X4<T>& operator ^= (ky_matrix4X4<T>& v1, const ky_matrix4X4<T>& v2)
{
    for (int64 i = 0; i < v1.size; ++i)
        v1.elem[i] ^= v2.elem[i];
    return v1;
}

template<typename T>
inline ky_matrix4X4<T>& operator ^= (ky_matrix4X4<T>& v, T t)
{
    for (int64 i = 0; i < v.size; ++i)
        v.elem[i] ^= t;
    return v;
}

template<typename T>
inline ky_vector3<T> operator * (const ky_matrix4X4<T>& m, const ky_vector3<T>& v)
{
    ky_vector4<T> v4(v, 1);
    v4 = m * v4;
    v4 /= v4.w;
    return ky_vector3<T>(v4.elem);
}


// 数据流处理
template<typename T>
ky_streamb &operator << (ky_streamb &in, const ky_matrix4X4<T> &v)
{
    for (int i = 0; i < v.size; ++i)
        in << v.elem[i];
    return in;
}

template<typename T>
ky_streamb &operator >> (ky_streamb &out, ky_matrix4X4<T> &v)
{
    for (int i = 0; i < v.size; ++i)
        out >> v.elem[i];
    return out;
}
template<typename T>
ky_streamt &operator << (ky_streamt &in, const ky_matrix4X4<T> &v)
{
    for (int i = 0; i < v.size; ++i)
        in << v.elem[i];
    return in;
}
template<typename T>
ky_streamt &operator >> (ky_streamt &out, ky_matrix4X4<T> &v)
{
    for (int i = 0; i < v.size; ++i)
        out >> v.elem[i];
    return out;
}

// 变种数据结构处理
const fourwd_t kyMat4Code = kyFourWord(mat4);
template<typename T>
ky_variant &operator << (ky_variant &va, const ky_matrix4X4<T> &col)
{
    va.set(kyMat4Code, 0, &col, sizeof(ky_matrix4X4<T>));
    return va;
}
template<typename T>
ky_variant &operator >> (ky_variant &va, ky_matrix4X4<T> &col)
{
    col = va.get<ky_matrix4X4<T>>(kyMat4Code);
    return va;
}

#endif // MAT4X4_H
