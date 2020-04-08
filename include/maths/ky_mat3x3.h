
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
 * @file     ky_mat3x3.h
 * @brief    3x3矩阵类
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
#ifndef MAT3X3_H
#define MAT3X3_H
#include "ky_vec4.h"


/**
        Matrix3 class. Works basicly like the matrix stuff in GLSL.
        But remember that this is a ROW-MATRIX and NOT a COL-MATRIX
        like in OpenGL or GLSL respectively.
    */
template<typename T>
struct ky_matrix3X3_mem
{
    typedef T ElemType;
    enum
    {
        size = 9,
        cols = 3,
        rows = 3
    };

    union
    {
        struct {
            T t00, t01, t02;
            T t10, t11, t12;
            T t20, t21, t22;
        };
        T elemRowCol[3][3];
        T elem[9];
    };
};
template<typename T>
struct ky_matrix3X3 : public ky_matrix3X3_mem<T>
{
    typedef ky_vector3<T> RowType;
    typedef ky_matrix3X3_mem<T> memory;

    /*
        constructors
    */

    /// default constructor
    ky_matrix3X3() {}
    /// Init all elements with the same value
    explicit ky_matrix3X3(T t) {
        for (int64 i = 0; i < memory::size; ++i)
            memory::elem[i] = t;
    }
    /// Init with another Matrix of another type
    template<typename U>
    ky_matrix3X3(const ky_matrix3X3<U>& m) {
        for (int64 i = 0; i < m.size; ++i)
            memory::elem[i] = T(m.elem[i]);
    }
    /// Init from array with equal size
    explicit ky_matrix3X3(const T* t) {
        for (int64 i = 0; i < memory::size; ++i)
            memory::elem[i] = t[i];
    }
    /// Init componentwisely
    ky_matrix3X3(T _t00, T _t01, T _t02,
                 T _t10, T _t11, T _t12,
                 T _t20, T _t21, T _t22) {
        memory::t00 = _t00; memory::t01 = _t01; memory::t02 = _t02;
        memory::t10 = _t10; memory::t11 = _t11; memory::t12 = _t12;
        memory::t20 = _t20; memory::t21 = _t21; memory::t22 = _t22;
    }
    /// Init with three ky_vector3
    ky_matrix3X3(const ky_vector3<T>& v1, const ky_vector3<T>& v2, const ky_vector3<T>& v3) {
        for (int64 i = 0; i < v1.size; ++i)
            memory::elem[0*memory::rows + i] = v1.elem[i];
        for (int64 i = 0; i < v2.size; ++i)
            memory::elem[1*memory::rows + i] = v2.elem[i];
        for (int64 i = 0; i < v3.size; ++i)
            memory::elem[2*memory::rows + i] = v3.elem[i];
    }
    template<typename U>
    ky_matrix3X3(const ky_matrix3X3_mem<U>& v) {
        for (int64 i = 0; i < v.size; ++i)
            memory::elem[i] = T(v.elem[i]);
    }
    ky_matrix3X3(const ky_matrix3X3_mem<T> & v){
        for (int64 i = 0; i < v.size; ++i)
            memory::elem[i] = v.elem[i];
    }

    ky_matrix3X3(const ky_matrix3X3<T> &v)
    {
        for (int64 i = 0; i < v.size; ++i)
            memory::elem[i] = v.elem[i];
    }
    inline ky_matrix3X3<T> &operator = (const ky_matrix3X3<T> &c)
    {
        for (int64 i = 0; i < c.size; ++i)
            memory::elem[i] = c.elem[i];
        return *this;
    }
    /*
        create special matrices
    */
    static const ky_matrix3X3<T> zero;
    static const ky_matrix3X3<T> identity;

    static ky_matrix3X3<T> create_zero() {
        return zero;
    }

    static ky_matrix3X3<T> create_identity() {
        return identity;
    }

    static ky_matrix3X3<T> create_translation(const ky_vector2<T>& v) {
        ky_matrix3X3<T> result = identity;
        result.t02 = v.elem[0];
        result.t12 = v.elem[1];
        return result;
    }

    static ky_matrix3X3<T> create_scale(const ky_vector3<T>& v) {
        ky_matrix3X3<T> result = identity;
        result.t00 = v.elem[0];
        result.t11 = v.elem[1];
        result.t22 = v.elem[2];
        return result;
    }

    /**
         * Simliar to glRotate butp angle is inem RADIAN AND NOTem DEGREE.
         *
         * @param angle The angle inem radian.
         * @param axis The axis of rotation. Needn't be in unit length.
         */
    static ky_matrix3X3<T> create_rotation(T angle, ky_vector3<T> axis);

    /**
         * inverts the matrix.
         * @return true - if successful -- otherwise false
        */
    inline bool invert(ky_matrix3X3<T>& result) const;

    inline T det() {
        return memory::elem[0]*memory::elem[4]*memory::elem[8] +
                memory::elem[1]*memory::elem[5]*memory::elem[6] +
                memory::elem[2]*memory::elem[3]*memory::elem[7] -
                memory::elem[0]*memory::elem[5]*memory::elem[7] -
                memory::elem[1]*memory::elem[3]*memory::elem[8] -
                memory::elem[2]*memory::elem[4]*memory::elem[6];
    }
    /*
        index operator
    */
    /*
            The C++ standard forbids typenamees with non trivial constructors
            as union members. Thus this hack provides functionality similar
            to the GLSL matrix index operator.
        */
    /// return arbitrary row-vector similar to GLSL. <br>
    /// BUT REMEMBER: GLSL gives you COLS NOT ROWS!
    inline const ky_vector3<T>& operator [] (int64 i) const {
        kyASSERT(i >= memory::size);
        return *((ky_vector3<T>*) memory::elemRowCol[i]);
    }
    /// return arbitrary row-vector similar to GLSL. <br>
    /// BUT REMEMBER: GLSL gives you COLS NOT ROWS!
    inline ky_vector3<T>& operator [] (int64 i) {
        kyASSERT(i >= memory::size);
        return *((ky_vector3<T>*) memory::elemRowCol[i]);
    }
};

/*
        init statics
    */

/// init statics
template<typename T>
const ky_matrix3X3<T> ky_matrix3X3<T>::zero = ky_matrix3X3<T>
        (
            T(0), T(0), T(0),
            T(0), T(0), T(0),
            T(0), T(0), T(0)
            );

template<typename T>
const ky_matrix3X3<T> ky_matrix3X3<T>::identity = ky_matrix3X3<T>
        (
            T(1), T(0), T(0),
            T(0), T(1), T(0),
            T(0), T(0), T(1)
            );

template<typename T>
bool ky_matrix3X3<T>::invert(ky_matrix3X3<T>& result) const
{
    result = ky_matrix3X3<T>::identity;

    T z = memory::elem[0]*(memory::elem[4]*memory::elem[8] - memory::elem[5]*memory::elem[7]) +
            memory::elem[1]*(memory::elem[5]*memory::elem[6] - memory::elem[3]*memory::elem[8]) +
            memory::elem[2]*(memory::elem[3]*memory::elem[7] - memory::elem[4]*memory::elem[6]);

    if( ky_fabs(z) < 0.00001)
        return false;

    //we have a row-matrix:
    result.elem[0] = (memory::elem[4]*memory::elem[8] - memory::elem[5]*memory::elem[7]);
    result.elem[1] = (memory::elem[2]*memory::elem[7] - memory::elem[1]*memory::elem[8]);
    result.elem[2] = (memory::elem[1]*memory::elem[5] - memory::elem[2]*memory::elem[4]);

    result.elem[3] = (memory::elem[5]*memory::elem[6] - memory::elem[3]*memory::elem[8]);
    result.elem[4] = (memory::elem[0]*memory::elem[8] - memory::elem[2]*memory::elem[6]);
    result.elem[5] = (memory::elem[2]*memory::elem[3] - memory::elem[0]*memory::elem[5]);

    result.elem[6] = (memory::elem[3]*memory::elem[7] - memory::elem[4]*memory::elem[6]);
    result.elem[7] = (memory::elem[1]*memory::elem[6] - memory::elem[0]*memory::elem[7]);
    result.elem[8] = (memory::elem[0]*memory::elem[4] - memory::elem[1]*memory::elem[3]);

    result /= z;

    return true;
}

typedef ky_matrix3X3<float>  ky_mat3X3f;
typedef ky_matrix3X3<real>   ky_mat3X3d;
typedef ky_mat3X3f           mat3;
typedef ky_mat3X3f           fmat3;
typedef ky_mat3X3d           dmat3;

/*
        createRotation for Matrix3<T> and Matrix4<T>
    */

template<typename T>
ky_matrix3X3<T> ky_matrix3X3<T>::create_rotation(T angle, ky_vector3<T> axis)
{
    axis = ky_normalize(axis);
    T s = ky_sin(angle);
    T c = ky_cos(angle);
    T x = axis.x;
    T y = axis.y;
    T z = axis.z;

    ky_matrix3X3<T> result
            (
                x*x*(1-c) + c,      x*y*(1-c) - z*s,    x*z*(1-c) + y*s,
                x*y*(1-c) + z*s,    y*y*(1-c) + c,      y*z*(1-c) - x*s,
                x*z*(1-c) - y*s,    y*z*(1-c) + x*s,    z*z*(1-c) + c
             );

    return result;
}

/*
        Prepare to implement 3 * 5 * 7 = 105 operators and a couple of functions
        the lazy way with evil voodoo macro magic
    */

template<typename T> inline ky_matrix3X3<T> ky_transpose(const ky_matrix3X3<T>& m)
{
    ky_matrix3X3<T> mRes;
    for (int64 row = 0; row < m.rows; ++row)
        for (int64 col = 0; col < m.cols; ++col)
            mRes.elemRowCol[col][row] = m.elemRowCol[row][col];
    return mRes;
}

/*
        Similar to GLSL make an exception with the
        Matrix-Matrix, Vector-Matrix and Matrix-Vector multiplication.
        These are "correct" operations and do not go componentwisely
    */
template<typename T>
inline typename ky_matrix3X3<T>::RowType operator * (const ky_matrix3X3<T>& m,
                                                     const typename ky_matrix3X3<T>::RowType& v)
{
    typename ky_matrix3X3<T>::RowType vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes[i] = dot(m[i], v);
    return vRes;
}

template<typename T>
inline typename ky_matrix3X3<T>::RowType operator * (const typename ky_matrix3X3<T>::RowType& v,
                                                     const ky_matrix3X3<T>& m)
{
    typename ky_matrix3X3<T>::RowType vRes;
    ky_matrix3X3<T> transposed = ky_transpose(m);
    for (int64 i = 0; i < v.size; ++i)
        vRes[i] = dot(v, transposed[i]);
    return vRes;
}

template<typename T>
inline ky_matrix3X3<T> operator * (const ky_matrix3X3<T>& m1, const ky_matrix3X3<T>& m2)
{
    ky_matrix3X3<T> mRes;
    ky_matrix3X3<T> transposed = ky_transpose(m2);
    for (int64 row = 0; row < m1.rows; ++row)
        for (int64 col = 0; col < m1.cols; ++col)
            mRes.elemRowCol[row][col] =
                    dot(m1[row], transposed[col]);
    return mRes;
}

template<typename T>
inline ky_matrix3X3<T>& operator *= (ky_matrix3X3<T>& m1, const ky_matrix3X3<T>& m2)
{
    ky_matrix3X3<T> temp = m1;
    m1 = temp * m2;
    return m1;
}

template<typename T>
inline ky_matrix3X3<T> ky_matrix_comp_mult (const ky_matrix3X3<T>& v1, const ky_matrix3X3<T>& v2)
{
    ky_matrix3X3<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = v1.elem[i] * v2.elem[i];
    return vRes;
}

//--------------------------------------------------------------

template<typename T>
inline ky_matrix3X3<T> operator - (const ky_matrix3X3<T>& v)
{
    ky_matrix3X3<T> vRes(v);
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = -v.elem[i];
    return vRes;
}

template<typename T>
inline ky_matrix3X3<T> ky_min(const ky_matrix3X3<T>& v1, const ky_matrix3X3<T>& v2)
{
    ky_matrix3X3<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = ky_min(v1.elem[i], v2.elem[i]);
    return vRes;
}

template<typename T>
inline ky_matrix3X3<T> ky_max(const ky_matrix3X3<T>& v1, const ky_matrix3X3<T>& v2)
{
    ky_matrix3X3<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = ky_max(v1.elem[i], v2.elem[i]);
    return vRes;
}

template<typename T>
inline ky_matrix3X3<T> ky_floor(const ky_matrix3X3<T>& v)
{
    ky_matrix3X3<T> result;
    for (int64 i = 0; i < v.size; ++i)
        result[i] = ky_floor(v.elem[i]);
    return result;
}

template<typename T>
inline ky_matrix3X3<T> ky_ceil(const ky_matrix3X3<T>& v)
{
    ky_matrix3X3<T> result;
    for (int64 i = 0; i < v.size; ++i)
        result[i] = ky_ceil(v.elem[i]);
    return result;
}

template<typename T>
inline ky_matrix3X3<T> ky_min(const ky_matrix3X3<T>& v, T t)
{
    ky_matrix3X3<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = ky_min(v.elem[i], t);
    return vRes;
}

template<typename T>
inline ky_matrix3X3<T> ky_max(const ky_matrix3X3<T>& v, T t)
{
    ky_matrix3X3<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = ky_max(v.elem[i], t);
    return vRes;
}

template<typename T>
inline T ky_min(const ky_matrix3X3<T>& v)
{
    T res = v.elem[0];
    for (int64 i = 1; i < v.size; ++i)
        res = ky_min(v.elem[i], res);
    return res;
}

template<typename T>
inline T ky_max(const ky_matrix3X3<T>& v)
{
    T res = v.elem[0];
    for (int64 i = 1; i < v.size; ++i)
        res = ky_max(v.elem[i], res);
    return res;
}

template<typename T>
inline ky_matrix3X3<T> ky_clamp(const ky_matrix3X3<T>& v, const ky_matrix3X3<T>& minVal,
                                const ky_matrix3X3<T>& maxVal)
{
    return ky_min( ky_max(v, minVal), maxVal );
}

template<typename T>
inline ky_matrix3X3<T> ky_clamp(const ky_matrix3X3<T>& v, T minVal, T maxVal)
{
    return ky_min( ky_max(v, minVal), maxVal );
}

template<typename T>
inline T ky_hadd(const ky_matrix3X3<T>& v)
{
    T result = v.elem[0];
    for (int64 i = 1; i < v.size; ++i)
        result += v.elem[i];
    return result;
}

template<typename T>
inline T ky_hsub(const ky_matrix3X3<T>& v)
{
    T result = v.elem[0];
    for (int64 i = 1; i < v.size; ++i)
        result -= v.elem[i];
    return result;
}

template<typename T>
inline T ky_hmul(const ky_matrix3X3<T>& v)
{
    T result = v.elem[0];
    for (int64 i = 1; i < v.size; ++i)
        result *= v.elem[i];
    return result;
}

template<typename T>
inline T ky_hdiv(const ky_matrix3X3<T>& v)
{
    T result = v.elem[0];
    for (int64 i = 1; i < v.size; ++i)
        result /= v.elem[i];
    return result;
}

template<typename T>
inline T ky_hmod(const ky_matrix3X3<T>& v)
{
    T result = v.elem[0];
    for (int64 i = 1; i < v.size; ++i)
        result %= v.elem[i];
    return result;
}

template<typename T>
inline bool ky_hand(const ky_matrix3X3<T>& v)
{
    bool result = v.elem[0] && v.elem[1];
    for (int64 i = 2; i < v.size; ++i)
        result &= v.elem[i];
    return result;
}

template<typename T>
inline bool ky_hor(const ky_matrix3X3<T>& v)
{
    T result = v.elem[0] || v.elem[1];
    for (int64 i = 2; i < v.size; ++i)
        result |= v.elem[i];
    return result;
}

template<typename T>
inline bool ky_hxor(const ky_matrix3X3<T>& v)
{
    T result = v.elem[0] ^ v.elem[1];
    for (int64 i = 2; i < v.size; ++i)
        result ^= v.elem[i];
    return result;
}

template<typename T>
inline bool operator == (const ky_matrix3X3<T>& v1, const ky_matrix3X3<T>& v2)
{
    bool result = true;
    for (int64 i = 0; i < v1.size; ++i)
        result &= (v1.elem[i] == v2.elem[i]);
    return result;
}

template<typename T>
inline bool operator != (const ky_matrix3X3<T>& v1, const ky_matrix3X3<T>& v2)
{
    return !(v1 == v2);
}

template<typename T>
inline ky_matrix3X3<bool> ky_less_than (const ky_matrix3X3<T>& v1, const ky_matrix3X3<T>& v2)
{
    ky_matrix3X3<bool> vRes;
    for (int64 i = 0; i < vRes.size; ++i)
        vRes.elem[i] = (v1.elem[i] < v2.elem[i]);
    return vRes;
}

template<typename T>
inline ky_matrix3X3<bool> ky_less_than_equal (const ky_matrix3X3<T>& v1, const ky_matrix3X3<T>& v2)
{
    ky_matrix3X3<bool> vRes;
    for (int64 i = 0; i < vRes.size; ++i)
        vRes.elem[i] = (v1.elem[i] <= v2.elem[i]);
    return vRes;
}

template<typename T>
inline ky_matrix3X3<bool> ky_greater_than (const ky_matrix3X3<T>& v1, const ky_matrix3X3<T>& v2)
{
    ky_matrix3X3<bool> vRes;
    for (int64 i = 0; i < vRes.size; ++i)
        vRes.elem[i] = (v1.elem[i] > v2.elem[i]);
    return vRes;
}

template<typename T>
inline ky_matrix3X3<bool> ky_greater_than_equal (const ky_matrix3X3<T>& v1, const ky_matrix3X3<T>& v2)
{
    ky_matrix3X3<bool> vRes;
    for (int64 i = 0; i < vRes.size; ++i)
        vRes.elem[i] = (v1.elem[i] >= v2.elem[i]);
    return vRes;
}

template<typename T>
inline ky_matrix3X3<bool> ky_equal (const ky_matrix3X3<T>& v1, const ky_matrix3X3<T>& v2)
{
    ky_matrix3X3<bool> vRes;
    for (int64 i = 0; i < vRes.size; ++i)
        vRes.elem[i] = (v1.elem[i] == v2.elem[i]);
    return vRes;
}

template<typename T>
inline ky_matrix3X3<bool> ky_not_equal (const ky_matrix3X3<T>& v1, const ky_matrix3X3<T>& v2)
{
    ky_matrix3X3<bool> vRes;
    for (int64 i = 0; i < vRes.size; ++i)
        vRes.elem[i] = (v1.elem[i] != v2.elem[i]);
    return vRes;
}

//------------------------------------------------------
template<typename T>
inline ky_matrix3X3<T> operator + (const ky_matrix3X3<T>& v1, const ky_matrix3X3<T>& v2)
{
    ky_matrix3X3<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = v1.elem[i] + v2.elem[i];
    return vRes;
}

template<typename T>
inline ky_matrix3X3<T> operator + (const ky_matrix3X3<T>& v, T t)
{
    ky_matrix3X3<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = v.elem[i] + t;
    return vRes;
}

template<typename T>
inline ky_matrix3X3<T> operator + (T t, const ky_matrix3X3<T>& v)
{
    ky_matrix3X3<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = t + v.elem[i];
    return vRes;
}

template<typename T>
inline ky_matrix3X3<T>& operator += (ky_matrix3X3<T>& v1, const ky_matrix3X3<T>& v2)
{
    for (int64 i = 0; i < v1.size; ++i)
        v1.elem[i] += v2.elem[i];
    return v1;
}

template<typename T>
inline ky_matrix3X3<T>& operator += (ky_matrix3X3<T>& v, T t)
{
    for (int64 i = 0; i < v.size; ++i)
        v.elem[i] += t;
    return v;
}

//------------------------------------------------------
template<typename T>
inline ky_matrix3X3<T> operator - (const ky_matrix3X3<T>& v1, const ky_matrix3X3<T>& v2)
{
    ky_matrix3X3<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = v1.elem[i] - v2.elem[i];
    return vRes;
}

template<typename T>
inline ky_matrix3X3<T> operator - (const ky_matrix3X3<T>& v, T t)
{
    ky_matrix3X3<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = v.elem[i] - t;
    return vRes;
}

template<typename T>
inline ky_matrix3X3<T> operator - (T t, const ky_matrix3X3<T>& v)
{
    ky_matrix3X3<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = t - v.elem[i];
    return vRes;
}

template<typename T>
inline ky_matrix3X3<T>& operator -= (ky_matrix3X3<T>& v1, const ky_matrix3X3<T>& v2)
{
    for (int64 i = 0; i < v1.size; ++i)
        v1.elem[i] -= v2.elem[i];
    return v1;
}

template<typename T>
inline ky_matrix3X3<T>& operator -= (ky_matrix3X3<T>& v, T t)
{
    for (int64 i = 0; i < v.size; ++i)
        v.elem[i] -= t;
    return v;
}

//------------------------------------------------------

template<typename T>
inline ky_matrix3X3<T> operator * (const ky_matrix3X3<T>& v, T t)
{
    ky_matrix3X3<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = v.elem[i] * t;
    return vRes;
}

template<typename T>
inline ky_matrix3X3<T> operator * (T t, const ky_matrix3X3<T>& v)
{
    ky_matrix3X3<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = t * v.elem[i];
    return vRes;
}

template<typename T>
inline ky_matrix3X3<T>& operator *= (ky_matrix3X3<T>& v, T t)
{
    for (int64 i = 0; i < v.size; ++i)
        v.elem[i] *= t;
    return v;
}

//------------------------------------------------------
template<typename T>
inline ky_matrix3X3<T> operator / (const ky_matrix3X3<T>& v1, const ky_matrix3X3<T>& v2)
{
    ky_matrix3X3<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = v1.elem[i] / v2.elem[i];
    return vRes;
}

template<typename T>
inline ky_matrix3X3<T> operator / (const ky_matrix3X3<T>& v, T t)
{
    ky_matrix3X3<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = v.elem[i] / t;
    return vRes;
}

template<typename T>
inline ky_matrix3X3<T> operator / (T t, const ky_matrix3X3<T>& v)
{
    ky_matrix3X3<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = t / v.elem[i];
    return vRes;
}

template<typename T>
inline ky_matrix3X3<T>& operator /= (ky_matrix3X3<T>& v1, const ky_matrix3X3<T>& v2)
{
    for (int64 i = 0; i < v1.size; ++i)
        v1.elem[i] /= v2.elem[i];
    return v1;
}

template<typename T>
inline ky_matrix3X3<T>& operator /= (ky_matrix3X3<T>& v, T t)
{
    for (int64 i = 0; i < v.size; ++i)
        v.elem[i] /= t;
    return v;
}

//------------------------------------------------------
template<typename T>
inline ky_matrix3X3<T> operator & (const ky_matrix3X3<T>& v1, const ky_matrix3X3<T>& v2)
{
    ky_matrix3X3<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = v1.elem[i] & v2.elem[i];
    return vRes;
}

template<typename T>
inline ky_matrix3X3<T> operator & (const ky_matrix3X3<T>& v, T t)
{
    ky_matrix3X3<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = v.elem[i] & t;
    return vRes;
}

template<typename T>
inline ky_matrix3X3<T> operator & (T t, const ky_matrix3X3<T>& v)
{
    ky_matrix3X3<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = t & v.elem[i];
    return vRes;
}

template<typename T>
inline ky_matrix3X3<T>& operator &= (ky_matrix3X3<T>& v1, const ky_matrix3X3<T>& v2)
{
    for (int64 i = 0; i < v1.size; ++i)
        v1.elem[i] &= v2.elem[i];
    return v1;
}

template<typename T>
inline ky_matrix3X3<T>& operator &= (ky_matrix3X3<T>& v, T t)
{
    for (int64 i = 0; i < v.size; ++i)
        v.elem[i] &= t;
    return v;
}

//------------------------------------------------------
template<typename T>
inline ky_matrix3X3<T> operator | (const ky_matrix3X3<T>& v1, const ky_matrix3X3<T>& v2)
{
    ky_matrix3X3<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = v1.elem[i] | v2.elem[i];
    return vRes;
}

template<typename T>
inline ky_matrix3X3<T> operator | (const ky_matrix3X3<T>& v, T t)
{
    ky_matrix3X3<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = v.elem[i] | t;
    return vRes;
}

template<typename T>
inline ky_matrix3X3<T> operator | (T t, const ky_matrix3X3<T>& v)
{
    ky_matrix3X3<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = t | v.elem[i];
    return vRes;
}

template<typename T>
inline ky_matrix3X3<T>& operator |= (ky_matrix3X3<T>& v1, const ky_matrix3X3<T>& v2)
{
    for (int64 i = 0; i < v1.size; ++i)
        v1.elem[i] |= v2.elem[i];
    return v1;
}

template<typename T>
inline ky_matrix3X3<T>& operator |= (ky_matrix3X3<T>& v, T t)
{
    for (int64 i = 0; i < v.size; ++i)
        v.elem[i] |= t;
    return v;
}

//------------------------------------------------------
template<typename T>
inline ky_matrix3X3<T> operator ^ (const ky_matrix3X3<T>& v1, const ky_matrix3X3<T>& v2)
{
    ky_matrix3X3<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = v1.elem[i] ^ v2.elem[i];
    return vRes;
}

template<typename T>
inline ky_matrix3X3<T> operator ^ (const ky_matrix3X3<T>& v, T t)
{
    ky_matrix3X3<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = v.elem[i] ^ t;
    return vRes;
}

template<typename T>
inline ky_matrix3X3<T> operator ^ (T t, const ky_matrix3X3<T>& v)
{
    ky_matrix3X3<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = t ^ v.elem[i];
    return vRes;
}

template<typename T>
inline ky_matrix3X3<T>& operator ^= (ky_matrix3X3<T>& v1, const ky_matrix3X3<T>& v2)
{
    for (int64 i = 0; i < v1.size; ++i)
        v1.elem[i] ^= v2.elem[i];
    return v1;
}

template<typename T>
inline ky_matrix3X3<T>& operator ^= (ky_matrix3X3<T>& v, T t)
{
    for (int64 i = 0; i < v.size; ++i)
        v.elem[i] ^= t;
    return v;
}


/// ostream-operator
template<typename T>
std::ostream& operator << (std::ostream& s, const ky_matrix3X3<T>& m) {
    return (s
            << "| " << m.elem[0] << " " << m.elem[1] << " " << m.elem[2] << " |" << std::endl
                                 << "| " << m.elem[3] << " " << m.elem[4] << " " << m.elem[5] << " |" << std::endl
                                 << "| " << m.elem[6] << " " << m.elem[7] << " " << m.elem[8] << " |" << std::endl);
}

template<typename T>
ky_streamb &operator << (ky_streamb &in, const ky_matrix3X3<T> &v)
{
    for (int i = 0; i < v.size; ++i)
        in << v.elem[i];
    return in;
}

template<typename T>
ky_streamb &operator >> (ky_streamb &out, ky_matrix3X3<T> &v)
{
    for (int i = 0; i < v.size; ++i)
        out >> v.elem[i];
    return out;
}
template<typename T>
ky_streamt &operator << (ky_streamt &in, const ky_matrix3X3<T> &v)
{
    for (int i = 0; i < v.size; ++i)
        in << v.elem[i];
    return in;
}
template<typename T>
ky_streamt &operator >> (ky_streamt &out, ky_matrix3X3<T> &v)
{
    for (int i = 0; i < v.size; ++i)
        out >> v.elem[i];
    return out;
}
const fourwd_t kyMat3Code = kyFourWord(mat3);
template<typename T>
ky_variant &operator << (ky_variant &va, const ky_matrix3X3<T> &col)
{
    va.set(kyMat3Code, 0, &col, sizeof(ky_matrix3X3<T>));
    return va;
}
template<typename T>
ky_variant &operator >> (ky_variant &va, ky_matrix3X3<T> &col)
{
    col = va.get<ky_matrix3X3<T>>(kyMat3Code);
    return va;
}
#endif // MAT3X3_H
