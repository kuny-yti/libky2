
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
 * @file     ky_vec2.h
 * @brief    2维向量
 *
 * @author   kunyang
 * @email    kunyang.yk@gmail.com
 * @version  1.1.1.1
 * @date     2014/04/09
 * @license  GNU General Public License (GPL)
 *
 * Change History :
 *    Date    |  Version  |  Author  |   Description
 * 2014/04/09 | 1.0.0.1   | kunyang  | 创建文件
 * 2016/11/10 | 1.1.0.1   | kunyang  | 分离运算为体外
 * 2018/02/23 | 1.1.1.1   | kunyang  | 内存分布调整
 */
#ifndef ky_VEC2_H
#define ky_VEC2_H

#include "ky_maths.h"
#include "tools/ky_stream.h"
#include "tools/ky_variant.h"

template<typename T>
struct ky_vector2_mem
{
    typedef T ElemType;

    enum
    {
        size = 2
    };

    union
    {
        struct
        {
            T x;
            T y;
        };
        struct {
            T s;
            T t;
        };
        struct {
            T r;
            T g;
        };
        struct {
            T width;
            T height;
        };
        T elem[size];
    };
};

template<typename T>
struct ky_vector2 : public ky_vector2_mem<T>
{
    typedef ky_vector2_mem<T> memory;

    ky_vector2() {}
    explicit ky_vector2(T v) {
        for (int64 i = 0; i < memory::size; ++i)
            memory::elem[i] = v;
    }
    explicit ky_vector2(const T* v) {
        for (int64 i = 0; i < memory::size; ++i)
            memory::elem[i] = v[i];
    }
    ky_vector2(T t1, T t2) {
        memory::elem[0] = t1;
        memory::elem[1] = t2;
    }

    template<typename U>
    ky_vector2(const ky_vector2<U>& v) {
        for (int64 i = 0; i < v.size; ++i)
            memory::elem[i] = T(v.elem[i]);
    }
    ky_vector2(const ky_vector2<T> & v){
        for (int64 i = 0; i < v.size; ++i)
            memory::elem[i] = v.elem[i];
    }

    template<typename U>
    ky_vector2(const ky_vector2_mem<U>& v) {
        for (int64 i = 0; i < v.size; ++i)
            memory::elem[i] = T(v.elem[i]);
    }
    ky_vector2(const ky_vector2_mem<T> & v){
        for (int64 i = 0; i < v.size; ++i)
            memory::elem[i] = v.elem[i];
    }

    inline ky_vector2<T> &operator = (const ky_vector2<T> &v){
        for (int64 i = 0; i < v.size; ++i)
            memory::elem[i] = v.elem[i];
        return *this;
    }

    inline const T& operator [] (int64 index) const {
        return memory::elem[index];
    }

    inline T& operator [] (int64 index) {
        return memory::elem[index];
    }

    static ky_vector2<T> zero;
};

template<typename T>
ky_vector2<T> ky_vector2<T>::zero = ky_vector2<T>(T(0), T(0));

typedef ky_vector2<float>   ky_vector2f;
typedef ky_vector2<real>  ky_vector2d;

/*
 * 像GLSL中的类型
 */
typedef ky_vector2<float>  vec2;
typedef ky_vector2<real>   dvec2; // GLSL目前不支持，但保留关键字
typedef ky_vector2<int>    ivec2;
typedef ky_vector2<int64>  svec2;
typedef ky_vector2<bool>   bvec2;

typedef ky_vector2<int>   isize;
typedef ky_vector2<float> fsize;
typedef ky_vector2<real>  dsize;

typedef ky_vector2<int>   ipoint;
typedef ky_vector2<float> fpoint;
typedef ky_vector2<real>  dpoint;

/*
 * 数学运算
 */
template<typename T> inline ky_vector2<T> operator + (const ky_vector2<T>& v1, const ky_vector2<T>& v2)
{
    ky_vector2<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = v1.elem[i] + v2.elem[i];
    return vRes;
}


template<typename T> inline ky_vector2<T> operator + (const ky_vector2<T>& v, T t)
{
    ky_vector2<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = v.elem[i] + t;
    return vRes;
}
template<typename T> inline ky_vector2<T> operator + (T t, const ky_vector2<T>& v)
{
    ky_vector2<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = t + v.elem[i];
    return vRes;
}
template<typename T> inline ky_vector2<T>& operator += (ky_vector2<T>& v1, const ky_vector2<T>& v2)
{
    for (int64 i = 0; i < v1.size; ++i)
        v1.elem[i] += v2.elem[i];
    return v1;
}
template<typename T> inline ky_vector2<T>& operator += (ky_vector2<T>& v, T t)
{
    for (int64 i = 0; i < v.size; ++i)
        v.elem[i] += t;
    return v;
}

template<typename T> inline ky_vector2<T> operator - (const ky_vector2<T>& v1, const ky_vector2<T>& v2)
{
    ky_vector2<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = v1.elem[i] - v2.elem[i];
    return vRes;
}


template<typename T> inline ky_vector2<T> operator - (const ky_vector2<T>& v, T t)
{
    ky_vector2<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = v.elem[i] - t;
    return vRes;
}
template<typename T> inline ky_vector2<T> operator - (T t, const ky_vector2<T>& v)
{
    ky_vector2<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = t - v.elem[i];
    return vRes;
}
template<typename T> inline ky_vector2<T>& operator -= (ky_vector2<T>& v1, const ky_vector2<T>& v2)
{
    for (int64 i = 0; i < v1.size; ++i)
        v1.elem[i] -= v2.elem[i];
    return v1;
}
template<typename T> inline ky_vector2<T>& operator -= (ky_vector2<T>& v, T t)
{
    for (int64 i = 0; i < v.size; ++i)
        v.elem[i] -= t;
    return v;
}

template<typename T> inline ky_vector2<T> operator * (const ky_vector2<T>& v1, const ky_vector2<T>& v2)
{
    ky_vector2<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = v1.elem[i] * v2.elem[i];
    return vRes;
}


template<typename T> inline ky_vector2<T> operator * (const ky_vector2<T>& v, T t)
{
    ky_vector2<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = v.elem[i] * t;
    return vRes;
}
template<typename T> inline ky_vector2<T> operator * (T t, const ky_vector2<T>& v)
{
    ky_vector2<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = t * v.elem[i];
    return vRes;
}
template<typename T> inline ky_vector2<T>& operator *= (ky_vector2<T>& v1, const ky_vector2<T>& v2)
{
    for (int64 i = 0; i < v1.size; ++i)
        v1.elem[i] *= v2.elem[i];
    return v1;
}
template<typename T> inline ky_vector2<T>& operator *= (ky_vector2<T>& v, T t)
{
    for (int64 i = 0; i < v.size; ++i)
        v.elem[i] *= t;
    return v;
}

template<typename T> inline ky_vector2<T> operator / (const ky_vector2<T>& v1, const ky_vector2<T>& v2)
{
    ky_vector2<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = v1.elem[i] / v2.elem[i];
    return vRes;
}


template<typename T> inline ky_vector2<T> operator / (const ky_vector2<T>& v, T t)
{
    ky_vector2<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = v.elem[i] / t;
    return vRes;
}
template<typename T> inline ky_vector2<T> operator / (T t, const ky_vector2<T>& v)
{
    ky_vector2<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = t / v.elem[i];
    return vRes;
}
template<typename T> inline ky_vector2<T>& operator /= (ky_vector2<T>& v1, const ky_vector2<T>& v2)
{
    for (int64 i = 0; i < v1.size; ++i)
        v1.elem[i] /= v2.elem[i];
    return v1;
}
template<typename T> inline ky_vector2<T>& operator /= (ky_vector2<T>& v, T t)
{
    for (int64 i = 0; i < v.size; ++i)
        v.elem[i] /= t;
    return v;
}

template<typename T> inline ky_vector2<T> operator % (const ky_vector2<T>& v1, const ky_vector2<T>& v2)
{
    ky_vector2<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = v1.elem[i] % v2.elem[i];
    return vRes;
}


template<typename T> inline ky_vector2<T> operator % (const ky_vector2<T>& v, T t)
{
    ky_vector2<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = v.elem[i] % t;
    return vRes;
}
template<typename T> inline ky_vector2<T> operator % (T t, const ky_vector2<T>& v)
{
    ky_vector2<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = t % v.elem[i];
    return vRes;
}
template<typename T> inline ky_vector2<T>& operator %= (ky_vector2<T>& v1, const ky_vector2<T>& v2)
{
    for (int64 i = 0; i < v1.size; ++i)
        v1.elem[i] %= v2.elem[i];
    return v1;
}
template<typename T> inline ky_vector2<T>& operator %= (ky_vector2<T>& v, T t)
{
    for (int64 i = 0; i < v.size; ++i)
        v.elem[i] %= t;
    return v;
}

template<typename T> inline ky_vector2<T> operator & (const ky_vector2<T>& v1, const ky_vector2<T>& v2)
{
    ky_vector2<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = v1.elem[i] & v2.elem[i];
    return vRes;
}


template<typename T> inline ky_vector2<T> operator & (const ky_vector2<T>& v, T t)
{
    ky_vector2<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = v.elem[i] & t;
    return vRes;
}
template<typename T> inline ky_vector2<T> operator & (T t, const ky_vector2<T>& v)
{
    ky_vector2<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = t & v.elem[i];
    return vRes;
}
template<typename T> inline ky_vector2<T>& operator &= (ky_vector2<T>& v1, const ky_vector2<T>& v2)
{
    for (int64 i = 0; i < v1.size; ++i)
        v1.elem[i] &= v2.elem[i];
    return v1;
}
template<typename T> inline ky_vector2<T>& operator &= (ky_vector2<T>& v, T t)
{
    for (int64 i = 0; i < v.size; ++i)
        v.elem[i] &= t;
    return v;
}

template<typename T> inline ky_vector2<T> operator | (const ky_vector2<T>& v1, const ky_vector2<T>& v2)
{
    ky_vector2<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = v1.elem[i] | v2.elem[i];
    return vRes;
}

template<typename T> inline ky_vector2<T> operator | (const ky_vector2<T>& v, T t)
{
    ky_vector2<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = v.elem[i] | t;
    return vRes;
}
template<typename T> inline ky_vector2<T> operator | (T t, const ky_vector2<T>& v)
{
    ky_vector2<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = t | v.elem[i];
    return vRes;
}
template<typename T> inline ky_vector2<T>& operator |= (ky_vector2<T>& v1, const ky_vector2<T>& v2)
{
    for (int64 i = 0; i < v1.size; ++i)
        v1.elem[i] |= v2.elem[i];
    return v1;
}
template<typename T> inline ky_vector2<T>& operator |= (ky_vector2<T>& v, T t)
{
    for (int64 i = 0; i < v.size; ++i)
        v.elem[i] |= t;
    return v;
}

template<typename T> inline ky_vector2<T> operator ^ (const ky_vector2<T>& v1, const ky_vector2<T>& v2)
{
    ky_vector2<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = v1.elem[i] ^ v2.elem[i];
    return vRes;
}


template<typename T> inline ky_vector2<T> operator ^ (const ky_vector2<T>& v, T t)
{
    ky_vector2<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = v.elem[i] ^ t;
    return vRes;
}
template<typename T> inline ky_vector2<T> operator ^ (T t, const ky_vector2<T>& v)
{
    ky_vector2<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = t ^ v.elem[i];
    return vRes;
}
template<typename T> inline ky_vector2<T>& operator ^= (ky_vector2<T>& v1, const ky_vector2<T>& v2)
{
    for (int64 i = 0; i < v1.size; ++i)
        v1.elem[i] ^= v2.elem[i];
    return v1;
}
template<typename T> inline ky_vector2<T>& operator ^= (ky_vector2<T>& v, T t)
{
    for (int64 i = 0; i < v.size; ++i)
        v.elem[i] ^= t;
    return v;
}

template<typename T> inline bool operator > (const ky_vector2<T>& v1, const ky_vector2<T>& v2)
{
    bool result = true;
    for (int64 i = 0; i < v1.size; ++i)
        result &= (v1.elem[i] > v2.elem[i]);
    return result;
}
template<typename T> inline bool operator < (const ky_vector2<T>& v1, const ky_vector2<T>& v2)
{
    bool result = true;
    for (int64 i = 0; i < v1.size; ++i)
        result &= (v1.elem[i] < v2.elem[i]);
    return result;
}

/*
 * 关系运算符
 */
template<typename T>
inline ky_vector2<bool> ky_less_than (const ky_vector2<T>& v1, const ky_vector2<T>& v2)
{
    ky_vector2<bool> vRes;
    for (int64 i = 0; i < vRes.size; ++i)
        vRes.elem[i] = (v1.elem[i] < v2.elem[i]);
    return vRes;
}

template<typename T>
inline ky_vector2<bool> ky_less_than_equal (const ky_vector2<T>& v1, const ky_vector2<T>& v2)
{
    ky_vector2<bool> vRes;
    for (int64 i = 0; i < vRes.size; ++i)
        vRes.elem[i] = (v1.elem[i] <= v2.elem[i]);
    return vRes;
}

template<typename T>
inline ky_vector2<bool> ky_greater_than (const ky_vector2<T>& v1, const ky_vector2<T>& v2)
{
    ky_vector2<bool> vRes;
    for (int64 i = 0; i < vRes.size; ++i)
        vRes.elem[i] = (v1.elem[i] > v2.elem[i]);
    return vRes;
}

template<typename T>
inline ky_vector2<bool> ky_greater_than_equal (const ky_vector2<T>& v1, const ky_vector2<T>& v2)
{
    ky_vector2<bool> vRes;
    for (int64 i = 0; i < vRes.size; ++i)
        vRes.elem[i] = (v1.elem[i] >= v2.elem[i]);
    return vRes;
}
template<typename T>
inline ky_vector2<bool> ky_equal (const ky_vector2<T>& v1, const ky_vector2<T>& v2)
{
    ky_vector2<bool> vRes;
    for (int64 i = 0; i < vRes.size; ++i)
        vRes.elem[i] = (v1.elem[i] == v2.elem[i]);
    return vRes;
}

template<typename T>
inline ky_vector2<bool> ky_not_equal (const ky_vector2<T>& v1, const ky_vector2<T>& v2)
{
    ky_vector2<bool> vRes;
    for (int64 i = 0; i < vRes.size; ++i)
        vRes.elem[i] = (v1.elem[i] != v2.elem[i]);
    return vRes;
}

template<typename T>
inline bool operator == (const ky_vector2<T>& v1, const ky_vector2<T>& v2)
{
    bool result = true;
    for (int64 i = 0; i < v1.size; ++i)
        result &= (v1.elem[i] == v2.elem[i]);
    return result;
}

template<typename T>
inline bool operator != (const ky_vector2<T>& v1, const ky_vector2<T>& v2)
{
    return !(v1 == v2);
}


/*
 * 其他向量功能
 */
template<typename T> inline T ky_dot(const ky_vector2<T>& v1, const ky_vector2<T>& v2)
{
    T result = 0;
    for (int64 i = 0; i < v1.size; ++i)
        result += v1.elem[i] * v2.elem[i];
    return result;
}

template<typename T> inline T ky_length_sq(const ky_vector2<T>& v)
{
    T result = 0;
    for (int64 i = 0; i < v.size; ++i)
        result += (v.elem[i] * v.elem[i]);
    return result;
}

template<typename T> inline T ky_length(const ky_vector2<T>& v)
{
    return ky_sqrt(ky_length_sq(v));
}

template<typename T> inline ky_vector2<T> ky_normalize(const ky_vector2<T>& v)
{
    ky_vector2<T> vRes(v);
    T recipLength = 1 / ky_length(v);
    vRes *= recipLength;
    return vRes;
}

template<typename T> inline T ky_distance(const ky_vector2<T>& v1, const ky_vector2<T>& v2)
{
    return ky_length(v2 - v1);
}

template<typename T> inline T ky_distance_sq(const ky_vector2<T>& v1, const ky_vector2<T>& v2)
{
    return ky_length_sq(v2 - v1);
}

template<typename T> inline ky_vector2<T> operator - (const ky_vector2<T>& v)
{
    ky_vector2<T> vRes(v);
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = -v.elem[i];
    return vRes;
}

template<typename T> inline ky_vector2<T> ky_min(const ky_vector2<T>& v1, const ky_vector2<T>& v2)
{
    ky_vector2<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = ky_min(v1.elem[i], v2.elem[i]);
    return vRes;
}

template<typename T> inline ky_vector2<T> ky_max(const ky_vector2<T>& v1, const ky_vector2<T>& v2)
{
    ky_vector2<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = ky_max(v1.elem[i], v2.elem[i]);
    return vRes;
}

template<typename T> inline ky_vector2<T> ky_min(const ky_vector2<T>& v, T t)
{
    ky_vector2<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = ky_min(v.elem[i], t);
    return vRes;
}

template<typename T> inline ky_vector2<T> ky_max(const ky_vector2<T>& v, T t)
{
    ky_vector2<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = ky_max(v.elem[i], t);
    return vRes;
}

template<typename T> inline T ky_min(const ky_vector2<T>& v)
{
    T res = v.elem[0];
    for (int64 i = 1; i < v.size; ++i)
        res = ky_min(v.elem[i], res);
    return res;
}

template<typename T> inline T ky_max(const ky_vector2<T>& v)
{
    T res = v.elem[0];
    for (int64 i = 1; i < v.size; ++i)
        res = ky_max(v.elem[i], res);
    return res;
}

template<typename T> inline ky_vector2<T> ky_clamp(const ky_vector2<T>& v, const ky_vector2<T>& minVal, const ky_vector2<T>& maxVal)
{
    return ky_min( ky_max(v, minVal), maxVal );
}

template<typename T> inline ky_vector2<T> ky_clamp(const ky_vector2<T>& v, T minVal, T maxVal)
{
    return ky_min( ky_max(v, minVal), maxVal );
}

template<typename T> inline ky_vector2<T> ky_ceil(const ky_vector2<T>& v)
{
    ky_vector2<T> result;
    for (int64 i = 0; i < v.size; ++i)
        result[i] = ky_ceil(v.elem[i]);
    return result;
}

template<typename T> inline ky_vector2<T> ky_floor(const ky_vector2<T>& v)
{
    ky_vector2<T> result;
    for (int64 i = 0; i < v.size; ++i)
        result[i] = ky_floor(v.elem[i]);
    return result;
}

template<typename T> inline ky_vector2<T> ky_fix(const ky_vector2<T>& v)
{
    ky_vector2<T> result;
    for (int64 i = 0; i < v.size; ++i)
        result[i] = ky_fix(v.elem[i]);
    return result;
}

template<typename T> inline ky_vector2<T> ky_round(const ky_vector2<T>& v)
{
    ky_vector2<T> result;
    for (int64 i = 0; i < v.size; ++i)
        result[i] = ky_round(v.elem[i]);
    return result;
}

template<typename T> inline ky_vector2<T> ky_sign(const ky_vector2<T>& v) {
    ky_vector2<T> result;
    for (int64 i = 0; i < v.size; ++i)
        result[i] = ky_sign(v.elem[i]);
    return result;
}

template<typename T> inline ky_vector2<int> ky_iceil(const ky_vector2<T>& v)
{
    ky_vector2<int> result;
    for (int64 i = 0; i < v.size; ++i)
        result[i] = ky_iceil(v.elem[i]);
    return result;
}

template<typename T> inline ky_vector2<int> ky_ifloor(const ky_vector2<T>& v)
{
    ky_vector2<int> result;
    for (int64 i = 0; i < v.size; ++i)
        result[i] = ky_ifloor(v.elem[i]);
    return result;
}

template<typename T> inline ky_vector2<int> ky_iround(const ky_vector2<T>& v)
{
    ky_vector2<int> result;
    for (int64 i = 0; i < v.size; ++i)
        result[i] = ky_iround(v.elem[i]);
    return result;
}

template<typename T> inline ky_vector2<int> ky_ifix(const ky_vector2<T>& v)
{
    ky_vector2<int> result;
    for (int64 i = 0; i < v.size; ++i)
        result[i] = ky_ifix(v.elem[i]);
    return result;
}

template<typename T> inline ky_vector2<int> ky_isign(const ky_vector2<T>& v)
{
    ky_vector2<int> result;
    for (int64 i = 0; i < v.size; ++i)
        result[i] = ky_isign(v.elem[i]);
    return result;
}

template<typename T> inline ky_vector2<T> ky_mix(const ky_vector2<T>& v, const ky_vector2<T>& v2, T t)
{
    ky_vector2<T> result;
    for (int64 i = 0; i < v.size; ++i)
        result.elem[i] = (v.elem[i]*(1-t)) + (v2.elem[i]*t);
    return result;
}

/*
 * horizontally working functions
 */
template<typename T> inline T ky_hadd(const ky_vector2<T>& v)
{
    T result = v.elem[0];
    for (int64 i = 1; i < v.size; ++i)
        result += v.elem[i];
    return result;
}

template<typename T> inline T ky_hsub(const ky_vector2<T>& v)
{
    T result = v.elem[0];
    for (int64 i = 1; i < v.size; ++i)
        result -= v.elem[i];
    return result;
}

template<typename T> inline T ky_hmul(const ky_vector2<T>& v)
{
    T result = v.elem[0];
    for (int64 i = 1; i < v.size; ++i)
        result *= v.elem[i];
    return result;
}

template<typename T> inline T ky_hdiv(const ky_vector2<T>& v)
{
    T result = v.elem[0];
    for (int64 i = 1; i < v.size; ++i)
        result /= v.elem[i];
    return result;
}

template<typename T> inline T ky_hmod(const ky_vector2<T>& v)
{
    T result = v.elem[0];
    for (int64 i = 1; i < v.size; ++i)
        result %= v.elem[i];
    return result;
}

template<typename T> inline bool ky_hand(const ky_vector2<T>& v)
{
    bool result = v.elem[0] && v.elem[1];
    for (int64 i = 2; i < v.size; ++i)
        result &= v.elem[i];
    return result;
}

template<typename T> inline bool ky_hor(const ky_vector2<T>& v)
{
    T result = v.elem[0] || v.elem[1];
    for (int64 i = 2; i < v.size; ++i)
        result |= v.elem[i];
    return result;
}

template<typename T> inline bool ky_hxor(const ky_vector2<T>& v)
{
    T result = v.elem[0] ^ v.elem[1];
    for (int64 i = 2; i < v.size; ++i)
        result ^= v.elem[i];
    return result;
}


/**
        cross product
    */
/*
        Component-wise absolute value
    */
template<typename T>
inline ky_vector2<T> ky_abs(const ky_vector2<T>& v)
{
    return ky_vector2<T>(ky_fabs(v.x), ky_fabs(v.y));
}

/*
        specialization for length(VectorX<int>)
    */

inline float ky_length(const ky_vector2<int>& v)
{
    return ky_sqrt( float(ky_length_sq(v)) );
}
template<typename T>
inline void ky_swap (ky_vector2<T> &v1, ky_vector2<T> &v2)
{
    ky_vector2<T> tmp = v1;
    v1 = v2;
    v2 = tmp;
}
/*
         stream operators
    */

/// output ky_vector2d to stream
template<typename T>
std::ostream& operator << (std::ostream& s, const ky_vector2<T>& v) {
    return (s << "[" << v.elem[0] << " " << v.elem[1] << "]");
}

// specialication for (unsigned) int, to make it output numbers and not characters

template<>
inline std::ostream& operator << <uchar>(std::ostream& s, const ky_vector2<uchar>& v) {
    return (s << ky_vector2<int>(v));
}

template<>
inline std::ostream& operator << <char>(std::ostream& s, const ky_vector2<char>& v) {
    return (s << ky_vector2<int>(v));
}

/// input ky_vector2d from stream
template<typename T>
std::istream& operator >> (std::istream& s, ky_vector2<T>& v) {
    char ch;

    s >> ch;
    if (ch == '[') {  // format "[ x y ]"
        s >> v[0] >> v[1] >> ch;
        if (ch != ']') {
            s.clear(std::ios::badbit);
        }
    } else { // format "x y"
        s.putback(ch);
        s >> v[0] >> v[1];
    }

    return s;
}


template<typename T>
ky_streamb &operator << (ky_streamb &in, const ky_vector2<T> &v)
{
    for (int i = 0;i < v.size; ++i)
        in << v.elem[i];
    return in;
}

template<typename T>
ky_streamb &operator >> (ky_streamb &out, ky_vector2<T> &v)
{
    for (int i = 0;i < v.size; ++i)
        out >> v.elem[i];
    return out;
}
template<typename T>
ky_streamt &operator << (ky_streamt &in, const ky_vector2<T> &v)
{
    for (int i = 0;i < v.size; ++i)
        in << v.elem[i];
    return in;
}

template<typename T>
ky_streamt &operator >> (ky_streamt &out, ky_vector2<T> &v)
{
    for (int i = 0;i < v.size; ++i)
        out >> v.elem[i];
    return out;
}
const fourwd_t kyVec2Code = kyFourWord(vec2);
template<typename T>
ky_variant &operator << (ky_variant &va, const ky_vector2<T> &col)
{
    va.set(kyVec2Code, 0, &col, sizeof(ky_vector2<T>));
    return va;
}
template<typename T>
ky_variant &operator >> (ky_variant &va, ky_vector2<T> &col)
{
    col = va.get<ky_vector2<T>>(kyVec2Code);
    return va;
}
#endif // VEC2_H
