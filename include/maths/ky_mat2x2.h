
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
 * @file     ky_mat2x2.h
 * @brief    2x2矩阵类
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
#ifndef ky_matrix2X2_H
#define ky_matrix2X2_H

#include "ky_vec4.h"
#include "ky_debug.h"
/*
    FIXME build a SSE-optimized mat4
*/

/**
    ky_matrix2X2 class. Works basicly like the matrix stuff in GLSL.
    But remember that this is a ROW-MATRIX and NOT a COL-MATRIX
    like in OpenGL or GLSL respectively.
*/
template<typename T>
struct ky_matrix2X2_mem
{
    typedef T ElemType;
    enum
    {
        size = 4,
        cols = 2,
        rows = 2
    };

    union
    {
        struct
        {
            T t00, t01;
            T t10, t11;
        };
        T elemRowCol[2][2];
        T elem[4];
    };
};
template<typename T>
struct ky_matrix2X2 : public ky_matrix2X2_mem<T>
{
    typedef ky_vector2<T> RowType;
    typedef ky_matrix2X2_mem<T> memory;

    ky_matrix2X2() {}
    /// Init all elements with the same value
    explicit ky_matrix2X2(T t) {
        for (int64 i = 0; i < memory::size; ++i)
            memory::elem[i] = t;
    }
    /// Init with another Matrix of another type
    template<typename U>
    ky_matrix2X2(const ky_matrix2X2<U>& m) {
        for (int64 i = 0; i < m.size; ++i)
            memory::elem[i] = T(m.elem[i]);
    }
    /// Init from array with equal size
    explicit ky_matrix2X2(const T* t) {
        for (int64 i = 0; i < memory::size; ++i)
            memory::elem[i] = t[i];
    }
    /// Init componentwisely
    ky_matrix2X2(T _t00, T _t01,
                 T _t10, T _t11) {
        memory::t00 = _t00; memory::t01 = _t01;
        memory::t10 = _t10; memory::t11 = _t11;
    }
    /// Init with two Vector2
    ky_matrix2X2(const ky_vector2<T>& v1, const ky_vector2<T>& v2) {
        for (int64 i = 0; i < v1.size; ++i)
            memory::elem[0*memory::rows + i] = v1.elem[i];
        for (int64 i = 0; i < v2.size; ++i)
            memory::elem[1*memory::rows + i] = v2.elem[i];
    }
    ky_matrix2X2(const ky_matrix2X2<T> &v)
    {
        for (int64 i = 0; i < v.size; ++i)
            memory::elem[i] = v.elem[i];
    }

    template<typename U>
    ky_matrix2X2(const ky_matrix2X2_mem<U>& v) {
        for (int64 i = 0; i < v.size; ++i)
            memory::elem[i] = T(v.elem[i]);
    }
    ky_matrix2X2(const ky_matrix2X2_mem<T> & v){
        for (int64 i = 0; i < v.size; ++i)
            memory::elem[i] = v.elem[i];
    }

    inline ky_matrix2X2<T> &operator = (const ky_matrix2X2<T> &c)
    {
        for (int64 i = 0; i < c.size; ++i)
            memory::elem[i] = c.elem[i];
        return *this;
    }


    static const ky_matrix2X2<T> zero;
    static const ky_matrix2X2<T> identity;

    static ky_matrix2X2<T> create_zero() {
        return zero;
    }

    static ky_matrix2X2<T> create_identity() {
        return identity;
    }

    static ky_matrix2X2<T> create_scale(const ky_vector2<T>& v)
    {
        ky_matrix2X2<T> result = identity;
        result.t00 = v.elem[0];
        result.t11 = v.elem[1];
        return result;
    }

    static ky_matrix2X2<T> create_rotation(T angle)
    {
        ky_matrix2X2<T> result = identity;
        result.t00 =  ky_cos(angle);
        result.t01 = -ky_sin(angle);
        result.t10 =  ky_sin(angle);
        result.t11 =  ky_cos(angle);
        return result;
    }

    /*
        index operators
    */
    /*
            The C++ standard forbids typenamees with non trivial constructors
            as union members. Thus this hack provides functionality similar
            to the GLSL matrix index operator.
        */
    /// return arbitrary row-vector similar to GLSL. <br>
    /// BUT REMEMBER: GLSL gives you COLS NOT ROWS!
    inline const ky_vector2<T>& operator [] (int64 i) const
    {
        kyASSERT(i >= memory::size);
        return *((ky_vector2<T>*) memory::elemRowCol[i]);
    }
    /// return arbitrary row-vector similar to GLSL. <br>
    /// BUT REMEMBER: GLSL gives you COLS NOT ROWS!
    inline ky_vector2<T>& operator [] (int64 i)
    {
        kyASSERT(i >= memory::size);
        return *((ky_vector2<T>*) memory::elemRowCol[i]);
    }
};

template<typename T>
const ky_matrix2X2<T> ky_matrix2X2<T>::zero = ky_matrix2X2<T>
        (
            T(0), T(0),
            T(0), T(0)
        );

template<typename T>
const ky_matrix2X2<T> ky_matrix2X2<T>::identity = ky_matrix2X2<T>
        (
            T(1), T(0),
            T(0), T(1)
         );

/*
        typedefs for easy usage
    */

typedef ky_matrix2X2<float>  ky_mat2X2f;
typedef ky_matrix2X2<real>   ky_mat2X2d;

/*
        types like in GLSL
    */
typedef ky_mat2X2f        fmat2;
typedef ky_mat2X2d        dmat2;
typedef fmat2             mat2;

/*
        Prepare to implement 3 * 5 * 7 = 105 operators and a couple of functions
        the lazy way with evil voodoo macro magic
    */

template<typename T>
inline ky_matrix2X2<T> ky_transpose(const ky_matrix2X2<T>& m)
{
    ky_matrix2X2<T> mRes;
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
inline typename ky_matrix2X2<T>::RowType operator * (const ky_matrix2X2<T>& m,
                                                     const typename ky_matrix2X2<T>::RowType& v)
{
    typename ky_matrix2X2<T>::RowType vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes[i] = dot(m[i], v);
    return vRes;
}

template<typename T>
inline typename ky_matrix2X2<T>::RowType operator * (const typename ky_matrix2X2<T>::RowType& v,
                                                     const ky_matrix2X2<T>& m)
{
    typename ky_matrix2X2<T>::RowType vRes;
    ky_matrix2X2<T> transposed = ky_transpose(m);
    for (int64 i = 0; i < v.size; ++i)
        vRes[i] = dot(v, transposed[i]);
    return vRes;
}

template<typename T>
inline ky_matrix2X2<T> operator * (const ky_matrix2X2<T>& m1, const ky_matrix2X2<T>& m2)
{
    ky_matrix2X2<T> mRes;
    ky_matrix2X2<T> transposed = ky_transpose(m2);
    for (int64 row = 0; row < m1.rows; ++row)
        for (int64 col = 0; col < m1.cols; ++col)
            mRes.elemRowCol[row][col] =
                    dot(m1[row], transposed[col]);
    return mRes;
}

template<typename T>
inline ky_matrix2X2<T>& operator *= (ky_matrix2X2<T>& m1, const ky_matrix2X2<T>& m2)
{
    ky_matrix2X2<T> temp = m1;
    m1 = temp * m2;
    return m1;
}

template<typename T>
inline ky_matrix2X2<T> ky_matrix_comp_mult (const ky_matrix2X2<T>& v1, const ky_matrix2X2<T>& v2)
{
    ky_matrix2X2<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = v1.elem[i] * v2.elem[i];
    return vRes;
}

/*
        Implementation of ky_matrix2X2<T> operators
    */

//--------------------------------------------------------------

template<typename T> inline ky_matrix2X2<T> operator - (const ky_matrix2X2<T>& v)
{
    ky_matrix2X2<T> vRes(v);
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = -v.elem[i];
    return vRes;
}

template<typename T>
inline ky_matrix2X2<T> ky_min(const ky_matrix2X2<T>& v1, const ky_matrix2X2<T>& v2)
{
    ky_matrix2X2<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = ky_min(v1.elem[i], v2.elem[i]);
    return vRes;
}

template<typename T>
inline ky_matrix2X2<T> ky_max(const ky_matrix2X2<T>& v1, const ky_matrix2X2<T>& v2)
{
    ky_matrix2X2<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = ky_max(v1.elem[i], v2.elem[i]);
    return vRes;
}

template<typename T>
inline ky_matrix2X2<T> ky_floor(const ky_matrix2X2<T>& v)
{
    ky_matrix2X2<T> result;
    for (int64 i = 0; i < v.size; ++i)
        result[i] = ky_floor(v.elem[i]);
    return result;
}

template<typename T>
inline ky_matrix2X2<T> ky_ceil(const ky_matrix2X2<T>& v)
{
    ky_matrix2X2<T> result;
    for (int64 i = 0; i < v.size; ++i)
        result[i] = ky_ceil(v.elem[i]);
    return result;
}

template<typename T>
inline ky_matrix2X2<T> ky_min(const ky_matrix2X2<T>& v, T t)
{
    ky_matrix2X2<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = ky_min(v.elem[i], t);
    return vRes;
}

template<typename T>
inline ky_matrix2X2<T> ky_max(const ky_matrix2X2<T>& v, T t)
{
    ky_matrix2X2<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = ky_max(v.elem[i], t);
    return vRes;
}

template<typename T>
inline T ky_min(const ky_matrix2X2<T>& v)
{
    T res = v.elem[0];
    for (int64 i = 1; i < v.size; ++i)
        res = ky_min(v.elem[i], res);
    return res;
}

template<typename T>
inline T ky_max(const ky_matrix2X2<T>& v)
{
    T res = v.elem[0];
    for (int64 i = 1; i < v.size; ++i)
        res = ky_max(v.elem[i], res);
    return res;
}

template<typename T>
inline ky_matrix2X2<T> ky_clamp(const ky_matrix2X2<T>& v, const ky_matrix2X2<T>& minVal,
                                const ky_matrix2X2<T>& maxVal)
{
    return ky_min( ky_max(v, minVal), maxVal );
}

template<typename T>
inline ky_matrix2X2<T> ky_clamp(const ky_matrix2X2<T>& v, T minVal, T maxVal)
{
    return ky_min( ky_max(v, minVal), maxVal );
}

template<typename T>
inline T ky_hadd(const ky_matrix2X2<T>& v)
{
    T result = v.elem[0];
    for (int64 i = 1; i < v.size; ++i)
        result += v.elem[i];
    return result;
}

template<typename T>
inline T ky_hsub(const ky_matrix2X2<T>& v)
{
    T result = v.elem[0];
    for (int64 i = 1; i < v.size; ++i)
        result -= v.elem[i];
    return result;
}

template<typename T>
inline T ky_hmul(const ky_matrix2X2<T>& v)
{
    T result = v.elem[0];
    for (int64 i = 1; i < v.size; ++i)
        result *= v.elem[i];
    return result;
}

template<typename T>
inline T ky_hdiv(const ky_matrix2X2<T>& v)
{
    T result = v.elem[0];
    for (int64 i = 1; i < v.size; ++i)
        result /= v.elem[i];
    return result;
}

template<typename T> inline T ky_hmod(const ky_matrix2X2<T>& v)
{
    T result = v.elem[0];
    for (int64 i = 1; i < v.size; ++i)
        result %= v.elem[i];
    return result;
}

template<typename T>
inline bool ky_hand(const ky_matrix2X2<T>& v)
{
    bool result = v.elem[0] && v.elem[1];
    for (int64 i = 2; i < v.size; ++i)
        result &= v.elem[i];
    return result;
}

template<typename T>
inline bool ky_hor(const ky_matrix2X2<T>& v)
{
    T result = v.elem[0] || v.elem[1];
    for (int64 i = 2; i < v.size; ++i)
        result |= v.elem[i];
    return result;
}

template<typename T>
inline bool ky_hxor(const ky_matrix2X2<T>& v)
{
    T result = v.elem[0] ^ v.elem[1];
    for (int64 i = 2; i < v.size; ++i)
        result ^= v.elem[i];
    return result;
}

template<typename T>
inline bool operator == (const ky_matrix2X2<T>& v1, const ky_matrix2X2<T>& v2)
{
    bool result = true;
    for (int64 i = 0; i < v1.size; ++i)
        result &= (v1.elem[i] == v2.elem[i]);
    return result;
}

template<typename T>
inline bool operator != (const ky_matrix2X2<T>& v1, const ky_matrix2X2<T>& v2)
{
    return !(v1 == v2);
}

template<typename T>
inline ky_matrix2X2<bool> ky_less_than (const ky_matrix2X2<T>& v1, const ky_matrix2X2<T>& v2)
{
    ky_matrix2X2<bool> vRes;
    for (int64 i = 0; i < vRes.size; ++i)
        vRes.elem[i] = (v1.elem[i] < v2.elem[i]);
    return vRes;
}

template<typename T>
inline ky_matrix2X2<bool> ky_less_than_equal (const ky_matrix2X2<T>& v1, const ky_matrix2X2<T>& v2)
{
    ky_matrix2X2<bool> vRes;
    for (int64 i = 0; i < vRes.size; ++i)
        vRes.elem[i] = (v1.elem[i] <= v2.elem[i]);
    return vRes;
}

template<typename T>
inline ky_matrix2X2<bool> ky_greater_than (const ky_matrix2X2<T>& v1, const ky_matrix2X2<T>& v2)
{
    ky_matrix2X2<bool> vRes;
    for (int64 i = 0; i < vRes.size; ++i)
        vRes.elem[i] = (v1.elem[i] > v2.elem[i]);
    return vRes;
}

template<typename T>
inline ky_matrix2X2<bool> ky_greater_than_equal (const ky_matrix2X2<T>& v1, const ky_matrix2X2<T>& v2)
{
    ky_matrix2X2<bool> vRes;
    for (int64 i = 0; i < vRes.size; ++i)
        vRes.elem[i] = (v1.elem[i] >= v2.elem[i]);
    return vRes;
}

template<typename T>
inline ky_matrix2X2<bool> ky_equal (const ky_matrix2X2<T>& v1, const ky_matrix2X2<T>& v2)
{
    ky_matrix2X2<bool> vRes;
    for (int64 i = 0; i < vRes.size; ++i)
        vRes.elem[i] = (v1.elem[i] == v2.elem[i]);
    return vRes;
}

template<typename T>
inline ky_matrix2X2<bool> ky_not_equal (const ky_matrix2X2<T>& v1, const ky_matrix2X2<T>& v2)
{
    ky_matrix2X2<bool> vRes;
    for (int64 i = 0; i < vRes.size; ++i)
        vRes.elem[i] = (v1.elem[i] != v2.elem[i]);
    return vRes;
}

//------------------------------------------------------
template<typename T>
inline ky_matrix2X2<T> operator + (const ky_matrix2X2<T>& v1, const ky_matrix2X2<T>& v2)
{
    ky_matrix2X2<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = v1.elem[i] + v2.elem[i];
    return vRes;
}

template<typename T>
inline ky_matrix2X2<T> operator + (const ky_matrix2X2<T>& v, T t)
{
    ky_matrix2X2<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = v.elem[i] + t;
    return vRes;
}

template<typename T>
inline ky_matrix2X2<T> operator + (T t, const ky_matrix2X2<T>& v)
{
    ky_matrix2X2<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = t + v.elem[i];
    return vRes;
}

template<typename T>
inline ky_matrix2X2<T>& operator += (ky_matrix2X2<T>& v1, const ky_matrix2X2<T>& v2)
{
    for (int64 i = 0; i < v1.size; ++i)
        v1.elem[i] += v2.elem[i];
    return v1;
}

template<typename T>
inline ky_matrix2X2<T>& operator += (ky_matrix2X2<T>& v, T t)
{
    for (int64 i = 0; i < v.size; ++i)
        v.elem[i] += t;
    return v;
}

//------------------------------------------------------
template<typename T>
inline ky_matrix2X2<T> operator - (const ky_matrix2X2<T>& v1, const ky_matrix2X2<T>& v2)
{
    ky_matrix2X2<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = v1.elem[i] - v2.elem[i];
    return vRes;
}

template<typename T>
inline ky_matrix2X2<T> operator - (const ky_matrix2X2<T>& v, T t)
{
    ky_matrix2X2<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = v.elem[i] - t;
    return vRes;
}

template<typename T>
inline ky_matrix2X2<T> operator - (T t, const ky_matrix2X2<T>& v)
{
    ky_matrix2X2<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = t - v.elem[i];
    return vRes;
}

template<typename T>
inline ky_matrix2X2<T>& operator -= (ky_matrix2X2<T>& v1, const ky_matrix2X2<T>& v2)
{
    for (int64 i = 0; i < v1.size; ++i)
        v1.elem[i] -= v2.elem[i];
    return v1;
}

template<typename T>
inline ky_matrix2X2<T>& operator -= (ky_matrix2X2<T>& v, T t)
{
    for (int64 i = 0; i < v.size; ++i)
        v.elem[i] -= t;
    return v;
}

//------------------------------------------------------


template<typename T>
inline ky_matrix2X2<T> operator * (const ky_matrix2X2<T>& v, T t)
{
    ky_matrix2X2<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = v.elem[i] * t;
    return vRes;
}

template<typename T>
inline ky_matrix2X2<T> operator * (T t, const ky_matrix2X2<T>& v)
{
    ky_matrix2X2<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = t * v.elem[i];
    return vRes;
}

template<typename T>
inline ky_matrix2X2<T>& operator *= (ky_matrix2X2<T>& v, T t)
{
    for (int64 i = 0; i < v.size; ++i)
        v.elem[i] *= t;
    return v;
}

//------------------------------------------------------
template<typename T>
inline ky_matrix2X2<T> operator / (const ky_matrix2X2<T>& v1, const ky_matrix2X2<T>& v2)
{
    ky_matrix2X2<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = v1.elem[i] / v2.elem[i];
    return vRes;
}

template<typename T>
inline ky_matrix2X2<T> operator / (const ky_matrix2X2<T>& v, T t)
{
    ky_matrix2X2<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = v.elem[i] / t;
    return vRes;
}

template<typename T>
inline ky_matrix2X2<T> operator / (T t, const ky_matrix2X2<T>& v)
{
    ky_matrix2X2<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = t / v.elem[i];
    return vRes;
}

template<typename T>
inline ky_matrix2X2<T>& operator /= (ky_matrix2X2<T>& v1, const ky_matrix2X2<T>& v2)
{
    for (int64 i = 0; i < v1.size; ++i)
        v1.elem[i] /= v2.elem[i];
    return v1;
}

template<typename T>
inline ky_matrix2X2<T>& operator /= (ky_matrix2X2<T>& v, T t)
{
    for (int64 i = 0; i < v.size; ++i)
        v.elem[i] /= t;
    return v;
}

//------------------------------------------------------
template<typename T>
inline ky_matrix2X2<T> operator & (const ky_matrix2X2<T>& v1, const ky_matrix2X2<T>& v2)
{
    ky_matrix2X2<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = v1.elem[i] & v2.elem[i];
    return vRes;
}

template<typename T>
inline ky_matrix2X2<T> operator & (const ky_matrix2X2<T>& v, T t)
{
    ky_matrix2X2<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = v.elem[i] & t;
    return vRes;
}

template<typename T>
inline ky_matrix2X2<T> operator & (T t, const ky_matrix2X2<T>& v)
{
    ky_matrix2X2<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = t & v.elem[i];
    return vRes;
}

template<typename T>
inline ky_matrix2X2<T>& operator &= (ky_matrix2X2<T>& v1, const ky_matrix2X2<T>& v2)
{
    for (int64 i = 0; i < v1.size; ++i)
        v1.elem[i] &= v2.elem[i];
    return v1;
}

template<typename T>
inline ky_matrix2X2<T>& operator &= (ky_matrix2X2<T>& v, T t)
{
    for (int64 i = 0; i < v.size; ++i)
        v.elem[i] &= t;
    return v;
}

//------------------------------------------------------
template<typename T>
inline ky_matrix2X2<T> operator | (const ky_matrix2X2<T>& v1, const ky_matrix2X2<T>& v2)
{
    ky_matrix2X2<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = v1.elem[i] | v2.elem[i];
    return vRes;
}

template<typename T>
inline ky_matrix2X2<T> operator | (const ky_matrix2X2<T>& v, T t)
{
    ky_matrix2X2<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = v.elem[i] | t;
    return vRes;
}

template<typename T>
inline ky_matrix2X2<T> operator | (T t, const ky_matrix2X2<T>& v)
{
    ky_matrix2X2<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = t | v.elem[i];
    return vRes;
}

template<typename T>
inline ky_matrix2X2<T>& operator |= (ky_matrix2X2<T>& v1, const ky_matrix2X2<T>& v2)
{
    for (int64 i = 0; i < v1.size; ++i)
        v1.elem[i] |= v2.elem[i];
    return v1;
}

template<typename T>
inline ky_matrix2X2<T>& operator |= (ky_matrix2X2<T>& v, T t)
{
    for (int64 i = 0; i < v.size; ++i)
        v.elem[i] |= t;
    return v;
}

//------------------------------------------------------
template<typename T>
inline ky_matrix2X2<T> operator ^ (const ky_matrix2X2<T>& v1, const ky_matrix2X2<T>& v2)
{
    ky_matrix2X2<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = v1.elem[i] ^ v2.elem[i];
    return vRes;
}

template<typename T>
inline ky_matrix2X2<T> operator ^ (const ky_matrix2X2<T>& v, T t)
{
    ky_matrix2X2<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = v.elem[i] ^ t;
    return vRes;
}

template<typename T>
inline ky_matrix2X2<T> operator ^ (T t, const ky_matrix2X2<T>& v)
{
    ky_matrix2X2<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = t ^ v.elem[i];
    return vRes;
}

template<typename T>
inline ky_matrix2X2<T>& operator ^= (ky_matrix2X2<T>& v1, const ky_matrix2X2<T>& v2)
{
    for (int64 i = 0; i < v1.size; ++i)
        v1.elem[i] ^= v2.elem[i];
    return v1;
}

template<typename T>
inline ky_matrix2X2<T>& operator ^= (ky_matrix2X2<T>& v, T t)
{
    for (int64 i = 0; i < v.size; ++i)
        v.elem[i] ^= t;
    return v;
}



/*
        ostream operators
    */

/// ostream-operator
template<typename T>
std::ostream& operator << (std::ostream& s, const ky_matrix2X2<T>& m) {
    return (s
            << "| " << m.elem[0] << " " << m.elem[1] << " |" << std::endl
                                 << "| " << m.elem[2] << " " << m.elem[3] << " |" << std::endl);
}


template<typename T>
ky_streamb &operator << (ky_streamb &in, const ky_matrix2X2<T> &v)
{
    for (int i = 0; i < v.size; ++i)
        in << v.elem[i];
    return in;
}

template<typename T>
ky_streamb &operator >> (ky_streamb &out, ky_matrix2X2<T> &v)
{
    for (int i = 0; i < v.size; ++i)
        out >> v.elem[i];
    return out;
}
template<typename T>
ky_streamt &operator << (ky_streamt &in, const ky_matrix2X2<T> &v)
{
    for (int i = 0; i < v.size; ++i)
        in << v.elem[i];
    return in;
}
template<typename T>
ky_streamt &operator >> (ky_streamt &out, ky_matrix2X2<T> &v)
{
    for (int i = 0; i < v.size; ++i)
        out >> v.elem[i];
    return out;
}
const fourwd_t kyMat2Code = kyFourWord(mat2);
template<typename T>
ky_variant &operator << (ky_variant &va, const ky_matrix2X2<T> &col)
{
    va.set(kyMat2Code, 0, &col, sizeof(ky_matrix2X2<T>));
    return va;
}
template<typename T>
ky_variant &operator >> (ky_variant &va, ky_matrix2X2<T> &col)
{
    col = va.get<ky_matrix2X2<T>>(kyMat2Code);
    return va;
}
#endif // MATRIX2_H
