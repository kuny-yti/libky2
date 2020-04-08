
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
 * @file     ky_vec4.h
 * @brief    4维向量
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
#ifndef KY_VEC4_H
#define KY_VEC4_H

#include "ky_vec3.h"

template<typename T>
struct ky_vector4_mem
{
    typedef T ElemType;

    enum {
        size = 4
    };

    union {
        struct {
            T x;
            T y;
            T z;
            T w;
        };
        struct {
            T s;
            T t;
            T p;
            T q;
        };
        struct {
            T r;
            T g;
            T b;
            T a;
        };
        struct {
            T _x;
            T _y;
            T width;
            T height;
        };

        struct {
            T _xyz[size-1];
            T _w;
        };
        T elem[size];
    };

};
/**
    This is a 4-dimensional Vector typename similar to vec4 of GLSL.
    All standard-operators are overloaded and go componentwisely
    similar to GLSL. For dotproduct use the function dot.
*/
template<typename T>
struct ky_vector4 :public ky_vector4_mem<T>
{
    typedef ky_vector4_mem<T> memory;

    /*
    constructors
*/
    /// default constructor
    ky_vector4() {}
    /// Init all elements with the same value
    explicit ky_vector4(T init) {
        for (int64 i = 0; i < memory::size; ++i)
            memory::elem[i] = init;
    }
    /// Init from array with equal size
    explicit ky_vector4(const T* init) {
        for (int64 i = 0; i < memory::size; ++i)
            memory::elem[i] = init[i];
    }
    /// Init componentwisely
    ky_vector4(T t1, T t2, T t3, T t4) {
        memory::elem[0] = t1;
        memory::elem[1] = t2;
        memory::elem[2] = t3;
        memory::elem[3] = t4;
    }
    /// Init x and y with v1, z and w with v2
    ky_vector4(const ky_vector4<T>& v1, const ky_vector4<T>& v2) {
        memory::elem[0] = v1.elem[0];
        memory::elem[1] = v1.elem[1];
        memory::elem[2] = v2.elem[0];
        memory::elem[3] = v2.elem[1];
    }
    /// Init x and y with vec2, z and w with _z and _w
    ky_vector4(const ky_vector4<T>& vec, T z_, T w_) {
        memory::elem[0] = vec.elem[0];
        memory::elem[1] = vec.elem[1];
        memory::elem[2] = z_;
        memory::elem[3] = w_;
    }
    /// Init x and y with _x and _y, z and w with _z and _w
    ky_vector4(T _x, T _y, const ky_vector4<T>& v) {
        memory::elem[0] = _x;
        memory::elem[1] = _y;
        memory::elem[2] = v.elem[0];
        memory::elem[3] = v.elem[1];
    }
    /// Init x, y, z with v and w with _w
    ky_vector4(const ky_vector3<T>& vec, T w_) {
        memory::elem[0] = vec.elem[0];
        memory::elem[1] = vec.elem[1];
        memory::elem[2] = vec.elem[2];
        memory::elem[3] = w_;
    }
    /// Init x with _x and y, z and w with v
    ky_vector4(T x_, const ky_vector4<T>& v) {
        memory::elem[0] = x_;
        memory::elem[1] = v.elem[0];
        memory::elem[2] = v.elem[1];
        memory::elem[3] = v.elem[2];
    }
    ky_vector4(const ky_vector2<T>& pos, const ky_vector2<T>& size) {
        memory::elem[0] = pos.x;
        memory::elem[1] = pos.y;
        memory::elem[2] = size.width;
        memory::elem[3] = size.height;
    }

    /// Init with another Vector of another type
    template<typename U>
    ky_vector4(const ky_vector4<U>& v) {
        for (int64 i = 0; i < v.size; ++i)
            memory::elem[i] = T(v.elem[i]);
    }
    ky_vector4(const ky_vector4<T> & v){
        for (int64 i = 0; i < v.size; ++i)
            memory::elem[i] = v.elem[i];
    }

    template<typename U>
    ky_vector4(const ky_vector4_mem<U>& v) {
        for (int64 i = 0; i < v.size; ++i)
            memory::elem[i] = T(v.elem[i]);
    }
    ky_vector4(const ky_vector4_mem<T> & v){
        for (int64 i = 0; i < v.size; ++i)
            memory::elem[i] = v.elem[i];
    }

    inline ky_vector4<T> &operator = (const ky_vector4<T> &v){
        for (int64 i = 0; i < v.size; ++i)
            memory::elem[i] = v.elem[i];
        return *this;
    }

    /*
    index operator
*/
    /// Index operator
    inline const T& operator [] (int64 index) const {
        return memory::elem[index];
    }
    /// Index operator
    inline T& operator [] (int64 index) {
        return memory::elem[index];
    }

    /*
    sub-vector getters
*/
    inline const ky_vector2<T>& xy() const {
        return xyz().xy();
    }
    inline const ky_vector2<T>& yz() const {
        return *((ky_vector2<T>*) (memory::elem + 1));
    }
    inline const ky_vector2<T>& zw() const {
        return *((ky_vector2<T>*) (memory::elem + 2));
    }
    inline const ky_vector3<T>& xyz() const {
        return *((ky_vector3<T>*) memory::_xyz);
    }
    inline const ky_vector3<T>& yzw() const {
        return *((ky_vector3<T>*) (memory::elem + 1));
    }
    inline ky_vector2<T>& xy() {
        return xyz().xy();
    }
    inline ky_vector2<T>& yz() {
        return *((ky_vector2<T>*) (memory::elem + 1));
    }
    inline ky_vector2<T>& zw() {
        return *((ky_vector2<T>*) (memory::elem + 2));
    }
    inline ky_vector3<T>& xyz() {
        return *((ky_vector3<T>*) memory::_xyz);
    }
    inline ky_vector3<T>& yzw() {
        return *((ky_vector3<T>*) (memory::elem + 1));
    }
    static ky_vector4<T> zero;
};
/// init statics
template<typename T>
ky_vector4<T> ky_vector4<T>::zero = ky_vector4<T>(T(0), T(0), T(0), T(0));

typedef ky_vector4<float>   ky_vector4f;
typedef ky_vector4<real>    ky_vector4d;

/*
        types like in GLSL
    */
typedef ky_vector4<float>   vec4;
typedef ky_vector4<real>    dvec4; // currently not supported in GLSL but a reserved keyword
typedef ky_vector4<int>     ivec4;
typedef ky_vector4<int64>   svec4;
typedef ky_vector4<bool>    bvec4;

typedef ky_vector4<int>     irect;
typedef ky_vector4<float>   frect;
typedef ky_vector4<real>    drect;


/*
        Prepare to implement 3 * 5 * 8 = 120 operators and dozens of functions
        the lazy way with evil voodoo macro magic
    */
template<typename T> inline ky_vector4<T> operator + (const ky_vector4<T>& v1, const ky_vector4<T>& v2)
{
    ky_vector4<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = v1.elem[i] + v2.elem[i];
    return vRes;
}


template<typename T> inline ky_vector4<T> operator + (const ky_vector4<T>& v, T t)
{
    ky_vector4<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = v.elem[i] + t;
    return vRes;
}
template<typename T> inline ky_vector4<T> operator + (T t, const ky_vector4<T>& v)
{
    ky_vector4<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = t + v.elem[i];
    return vRes;
}
template<typename T> inline ky_vector4<T>& operator += (ky_vector4<T>& v1, const ky_vector4<T>& v2)
{
    for (int64 i = 0; i < v1.size; ++i)
        v1.elem[i] += v2.elem[i];
    return v1;
}
template<typename T> inline ky_vector4<T>& operator += (ky_vector4<T>& v, T t)
{
    for (int64 i = 0; i < v.size; ++i)
        v.elem[i] += t;
    return v;
}

template<typename T> inline ky_vector4<T> operator - (const ky_vector4<T>& v1, const ky_vector4<T>& v2)
{
    ky_vector4<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = v1.elem[i] - v2.elem[i];
    return vRes;
}


template<typename T> inline ky_vector4<T> operator - (const ky_vector4<T>& v, T t)
{
    ky_vector4<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = v.elem[i] - t;
    return vRes;
}
template<typename T> inline ky_vector4<T> operator - (T t, const ky_vector4<T>& v)
{
    ky_vector4<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = t - v.elem[i];
    return vRes;
}
template<typename T> inline ky_vector4<T>& operator -= (ky_vector4<T>& v1, const ky_vector4<T>& v2)
{
    for (int64 i = 0; i < v1.size; ++i)
        v1.elem[i] -= v2.elem[i];
    return v1;
}
template<typename T> inline ky_vector4<T>& operator -= (ky_vector4<T>& v, T t)
{
    for (int64 i = 0; i < v.size; ++i)
        v.elem[i] -= t;
    return v;
}

template<typename T> inline ky_vector4<T> operator * (const ky_vector4<T>& v1, const ky_vector4<T>& v2)
{
    ky_vector4<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = v1.elem[i] * v2.elem[i];
    return vRes;
}


template<typename T> inline ky_vector4<T> operator * (const ky_vector4<T>& v, T t)
{
    ky_vector4<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = v.elem[i] * t;
    return vRes;
}
template<typename T> inline ky_vector4<T> operator * (T t, const ky_vector4<T>& v)
{
    ky_vector4<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = t * v.elem[i];
    return vRes;
}
template<typename T> inline ky_vector4<T>& operator *= (ky_vector4<T>& v1, const ky_vector4<T>& v2)
{
    for (int64 i = 0; i < v1.size; ++i)
        v1.elem[i] *= v2.elem[i];
    return v1;
}
template<typename T> inline ky_vector4<T>& operator *= (ky_vector4<T>& v, T t)
{
    for (int64 i = 0; i < v.size; ++i)
        v.elem[i] *= t;
    return v;
}

template<typename T> inline ky_vector4<T> operator / (const ky_vector4<T>& v1, const ky_vector4<T>& v2)
{
    ky_vector4<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = v1.elem[i] / v2.elem[i];
    return vRes;
}


template<typename T> inline ky_vector4<T> operator / (const ky_vector4<T>& v, T t)
{
    ky_vector4<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = v.elem[i] / t;
    return vRes;
}
template<typename T> inline ky_vector4<T> operator / (T t, const ky_vector4<T>& v)
{
    ky_vector4<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = t / v.elem[i];
    return vRes;
}
template<typename T> inline ky_vector4<T>& operator /= (ky_vector4<T>& v1, const ky_vector4<T>& v2)
{
    for (int64 i = 0; i < v1.size; ++i)
        v1.elem[i] /= v2.elem[i];
    return v1;
}
template<typename T> inline ky_vector4<T>& operator /= (ky_vector4<T>& v, T t)
{
    for (int64 i = 0; i < v.size; ++i)
        v.elem[i] /= t;
    return v;
}

template<typename T> inline ky_vector4<T> operator % (const ky_vector4<T>& v1, const ky_vector4<T>& v2)
{
    ky_vector4<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = v1.elem[i] % v2.elem[i];
    return vRes;
}


template<typename T> inline ky_vector4<T> operator % (const ky_vector4<T>& v, T t)
{
    ky_vector4<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = v.elem[i] % t;
    return vRes;
}
template<typename T> inline ky_vector4<T> operator % (T t, const ky_vector4<T>& v)
{
    ky_vector4<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = t % v.elem[i];
    return vRes;
}
template<typename T> inline ky_vector4<T>& operator %= (ky_vector4<T>& v1, const ky_vector4<T>& v2)
{
    for (int64 i = 0; i < v1.size; ++i)
        v1.elem[i] %= v2.elem[i];
    return v1;
}
template<typename T> inline ky_vector4<T>& operator %= (ky_vector4<T>& v, T t)
{
    for (int64 i = 0; i < v.size; ++i)
        v.elem[i] %= t;
    return v;
}

template<typename T> inline ky_vector4<T> operator & (const ky_vector4<T>& v1, const ky_vector4<T>& v2)
{
    ky_vector4<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = v1.elem[i] & v2.elem[i];
    return vRes;
}


template<typename T> inline ky_vector4<T> operator & (const ky_vector4<T>& v, T t)
{
    ky_vector4<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = v.elem[i] & t;
    return vRes;
}
template<typename T> inline ky_vector4<T> operator & (T t, const ky_vector4<T>& v)
{
    ky_vector4<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = t & v.elem[i];
    return vRes;
}
template<typename T> inline ky_vector4<T>& operator &= (ky_vector4<T>& v1, const ky_vector4<T>& v2)
{
    for (int64 i = 0; i < v1.size; ++i)
        v1.elem[i] &= v2.elem[i];
    return v1;
}
template<typename T> inline ky_vector4<T>& operator &= (ky_vector4<T>& v, T t)
{
    for (int64 i = 0; i < v.size; ++i)
        v.elem[i] &= t;
    return v;
}

template<typename T> inline ky_vector4<T> operator | (const ky_vector4<T>& v1, const ky_vector4<T>& v2)
{
    ky_vector4<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = v1.elem[i] | v2.elem[i];
    return vRes;
}

template<typename T> inline ky_vector4<T> operator | (const ky_vector4<T>& v, T t)
{
    ky_vector4<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = v.elem[i] | t;
    return vRes;
}
template<typename T> inline ky_vector4<T> operator | (T t, const ky_vector4<T>& v)
{
    ky_vector4<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = t | v.elem[i];
    return vRes;
}
template<typename T> inline ky_vector4<T>& operator |= (ky_vector4<T>& v1, const ky_vector4<T>& v2)
{
    for (int64 i = 0; i < v1.size; ++i)
        v1.elem[i] |= v2.elem[i];
    return v1;
}
template<typename T> inline ky_vector4<T>& operator |= (ky_vector4<T>& v, T t)
{
    for (int64 i = 0; i < v.size; ++i)
        v.elem[i] |= t;
    return v;
}

template<typename T> inline ky_vector4<T> operator ^ (const ky_vector4<T>& v1, const ky_vector4<T>& v2)
{
    ky_vector4<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = v1.elem[i] ^ v2.elem[i];
    return vRes;
}


template<typename T> inline ky_vector4<T> operator ^ (const ky_vector4<T>& v, T t)
{
    ky_vector4<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = v.elem[i] ^ t;
    return vRes;
}
template<typename T> inline ky_vector4<T> operator ^ (T t, const ky_vector4<T>& v)
{
    ky_vector4<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = t ^ v.elem[i];
    return vRes;
}
template<typename T> inline ky_vector4<T>& operator ^= (ky_vector4<T>& v1, const ky_vector4<T>& v2)
{
    for (int64 i = 0; i < v1.size; ++i)
        v1.elem[i] ^= v2.elem[i];
    return v1;
}
template<typename T> inline ky_vector4<T>& operator ^= (ky_vector4<T>& v, T t)
{
    for (int64 i = 0; i < v.size; ++i)
        v.elem[i] ^= t;
    return v;
}

/*
        relational operators
    */
template<typename T>
inline ky_vector4<bool> ky_less_than (const ky_vector4<T>& v1, const ky_vector4<T>& v2)
{
    ky_vector4<bool> vRes;
    for (int64 i = 0; i < vRes.size; ++i)
        vRes.elem[i] = (v1.elem[i] < v2.elem[i]);
    return vRes;
}

template<typename T>
inline ky_vector4<bool> ky_less_than_equal (const ky_vector4<T>& v1, const ky_vector4<T>& v2)
{
    ky_vector4<bool> vRes;
    for (int64 i = 0; i < vRes.size; ++i)
        vRes.elem[i] = (v1.elem[i] <= v2.elem[i]);
    return vRes;
}

template<typename T>
inline ky_vector4<bool> ky_greater_than (const ky_vector4<T>& v1, const ky_vector4<T>& v2)
{
    ky_vector4<bool> vRes;
    for (int64 i = 0; i < vRes.size; ++i)
        vRes.elem[i] = (v1.elem[i] > v2.elem[i]);
    return vRes;
}

template<typename T>
inline ky_vector4<bool> ky_greater_than_equal (const ky_vector4<T>& v1, const ky_vector4<T>& v2)
{
    ky_vector4<bool> vRes;
    for (int64 i = 0; i < vRes.size; ++i)
        vRes.elem[i] = (v1.elem[i] >= v2.elem[i]);
    return vRes;
}
template<typename T>
inline ky_vector4<bool> ky_equal (const ky_vector4<T>& v1, const ky_vector4<T>& v2)
{
    ky_vector4<bool> vRes;
    for (int64 i = 0; i < vRes.size; ++i)
        vRes.elem[i] = (v1.elem[i] == v2.elem[i]);
    return vRes;
}

template<typename T>
inline ky_vector4<bool> ky_not_equal (const ky_vector4<T>& v1, const ky_vector4<T>& v2)
{
    ky_vector4<bool> vRes;
    for (int64 i = 0; i < vRes.size; ++i)
        vRes.elem[i] = (v1.elem[i] != v2.elem[i]);
    return vRes;
}

template<typename T>
inline bool operator == (const ky_vector4<T>& v1, const ky_vector4<T>& v2)
{
    bool result = true;
    for (int64 i = 0; i < v1.size; ++i)
        result &= (v1.elem[i] == v2.elem[i]);
    return result;
}

template<typename T>
inline bool operator != (const ky_vector4<T>& v1, const ky_vector4<T>& v2)
{
    return !(v1 == v2);
}

template<typename T> inline bool operator > (const ky_vector4<T>& v1, const ky_vector4<T>& v2)
{
    bool result = true;
    for (int64 i = 0; i < v1.size; ++i)
        result &= (v1.elem[i] > v2.elem[i]);
    return result;
}
template<typename T> inline bool operator < (const ky_vector4<T>& v1, const ky_vector4<T>& v2)
{
    bool result = true;
    for (int64 i = 0; i < v1.size; ++i)
        result &= (v1.elem[i] < v2.elem[i]);
    return result;
}
/*
        further vector functions
    */
template<typename T> inline T ky_dot(const ky_vector4<T>& v1, const ky_vector4<T>& v2)
{
    T result = 0;
    for (int64 i = 0; i < v1.size; ++i)
        result += v1.elem[i] * v2.elem[i];
    return result;
}

template<typename T> inline T ky_length_sq(const ky_vector4<T>& v)
{
    T result = 0;
    for (int64 i = 0; i < v.size; ++i)
        result += (v.elem[i] * v.elem[i]);
    return result;
}

template<typename T> inline T ky_length(const ky_vector4<T>& v)
{
    return ky_sqrt(ky_length_sq(v));
}

template<typename T> inline ky_vector4<T> ky_normalize(const ky_vector4<T>& v)
{
    ky_vector4<T> vRes(v);
    T recipLength = 1 / ky_length(v);
    vRes *= recipLength;
    return vRes;
}

template<typename T> inline T ky_distance(const ky_vector4<T>& v1, const ky_vector4<T>& v2)
{
    return ky_length(v2 - v1);
}

template<typename T> inline T ky_distance_sq(const ky_vector4<T>& v1, const ky_vector4<T>& v2)
{
    return ky_length_sq(v2 - v1);
}

template<typename T> inline ky_vector4<T> operator - (const ky_vector4<T>& v)
{
    ky_vector4<T> vRes(v);
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = -v.elem[i];
    return vRes;
}

template<typename T> inline ky_vector4<T> ky_min(const ky_vector4<T>& v1, const ky_vector4<T>& v2)
{
    ky_vector4<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = ky_min(v1.elem[i], v2.elem[i]);
    return vRes;
}

template<typename T> inline ky_vector4<T> ky_max(const ky_vector4<T>& v1, const ky_vector4<T>& v2)
{
    ky_vector4<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = ky_max(v1.elem[i], v2.elem[i]);
    return vRes;
}

template<typename T> inline ky_vector4<T> ky_min(const ky_vector4<T>& v, T t)
{
    ky_vector4<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = ky_min(v.elem[i], t);
    return vRes;
}

template<typename T> inline ky_vector4<T> ky_max(const ky_vector4<T>& v, T t)
{
    ky_vector4<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = ky_max(v.elem[i], t);
    return vRes;
}

template<typename T> inline T ky_min(const ky_vector4<T>& v)
{
    T res = v.elem[0];
    for (int64 i = 1; i < v.size; ++i)
        res = ky_min(v.elem[i], res);
    return res;
}

template<typename T> inline T ky_max(const ky_vector4<T>& v)
{
    T res = v.elem[0];
    for (int64 i = 1; i < v.size; ++i)
        res = ky_max(v.elem[i], res);
    return res;
}

template<typename T>
inline ky_vector4<T> ky_clamp(const ky_vector4<T>& v, const ky_vector4<T>& minVal,
                              const ky_vector4<T>& maxVal)
{
    return ky_min( ky_max(v, minVal), maxVal );
}

template<typename T> inline ky_vector4<T> ky_clamp(const ky_vector4<T>& v, T minVal, T maxVal)
{
    return ky_min( ky_max(v, minVal), maxVal );
}

template<typename T> inline ky_vector4<T> ky_ceil(const ky_vector4<T>& v)
{
    ky_vector4<T> result;
    for (int64 i = 0; i < v.size; ++i)
        result[i] = ky_ceil(v.elem[i]);
    return result;
}

template<typename T> inline ky_vector4<T> ky_floor(const ky_vector4<T>& v)
{
    ky_vector4<T> result;
    for (int64 i = 0; i < v.size; ++i)
        result[i] = ky_floor(v.elem[i]);
    return result;
}

template<typename T> inline ky_vector4<T> ky_fix(const ky_vector4<T>& v)
{
    ky_vector4<T> result;
    for (int64 i = 0; i < v.size; ++i)
        result[i] = ky_fix(v.elem[i]);
    return result;
}

template<typename T> inline ky_vector4<T> ky_round(const ky_vector4<T>& v)
{
    ky_vector4<T> result;
    for (int64 i = 0; i < v.size; ++i)
        result[i] = ky_round(v.elem[i]);
    return result;
}

template<typename T> inline ky_vector4<T> ky_sign(const ky_vector4<T>& v) {
    ky_vector4<T> result;
    for (int64 i = 0; i < v.size; ++i)
        result[i] = ky_sign(v.elem[i]);
    return result;
}

template<typename T> inline ky_vector4<int> ky_iceil(const ky_vector4<T>& v)
{
    ky_vector4<int> result;
    for (int64 i = 0; i < v.size; ++i)
        result[i] = ky_iceil(v.elem[i]);
    return result;
}

template<typename T> inline ky_vector4<int> ky_ifloor(const ky_vector4<T>& v)
{
    ky_vector4<int> result;
    for (int64 i = 0; i < v.size; ++i)
        result[i] = ky_ifloor(v.elem[i]);
    return result;
}

template<typename T> inline ky_vector4<int> ky_iround(const ky_vector4<T>& v)
{
    ky_vector4<int> result;
    for (int64 i = 0; i < v.size; ++i)
        result[i] = ky_iround(v.elem[i]);
    return result;
}

template<typename T> inline ky_vector4<int> ky_ifix(const ky_vector4<T>& v)
{
    ky_vector4<int> result;
    for (int64 i = 0; i < v.size; ++i)
        result[i] = ky_ifix(v.elem[i]);
    return result;
}

template<typename T> inline ky_vector4<int> ky_isign(const ky_vector4<T>& v)
{
    ky_vector4<int> result;
    for (int64 i = 0; i < v.size; ++i)
        result[i] = ky_isign(v.elem[i]);
    return result;
}

template<typename T> inline ky_vector4<T> ky_mix(const ky_vector4<T>& v, const ky_vector4<T>& v2, T t)
{
    ky_vector4<T> result;
    for (int64 i = 0; i < v.size; ++i)
        result.elem[i] = (v.elem[i]*(1-t)) + (v2.elem[i]*t);
    return result;
}

/*
        horizontally working functions
    */
template<typename T> inline T ky_hadd(const ky_vector4<T>& v)
{
    T result = v.elem[0];
    for (int64 i = 1; i < v.size; ++i)
        result += v.elem[i];
    return result;
}

template<typename T> inline T ky_hsub(const ky_vector4<T>& v)
{
    T result = v.elem[0];
    for (int64 i = 1; i < v.size; ++i)
        result -= v.elem[i];
    return result;
}

template<typename T> inline T ky_hmul(const ky_vector4<T>& v)
{
    T result = v.elem[0];
    for (int64 i = 1; i < v.size; ++i)
        result *= v.elem[i];
    return result;
}

template<typename T> inline T ky_hdiv(const ky_vector4<T>& v)
{
    T result = v.elem[0];
    for (int64 i = 1; i < v.size; ++i)
        result /= v.elem[i];
    return result;
}

template<typename T> inline T ky_hmod(const ky_vector4<T>& v)
{
    T result = v.elem[0];
    for (int64 i = 1; i < v.size; ++i)
        result %= v.elem[i];
    return result;
}

template<typename T> inline bool ky_hand(const ky_vector4<T>& v)
{
    bool result = v.elem[0] && v.elem[1];
    for (int64 i = 2; i < v.size; ++i)
        result &= v.elem[i];
    return result;
}

template<typename T> inline bool ky_hor(const ky_vector4<T>& v)
{
    T result = v.elem[0] || v.elem[1];
    for (int64 i = 2; i < v.size; ++i)
        result |= v.elem[i];
    return result;
}

template<typename T> inline bool ky_hxor(const ky_vector4<T>& v)
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
inline ky_vector4<T> ky_abs(const ky_vector4<T>& v)
{
    return ky_vector4<T>(ky_fabs(v.x), ky_fabs(v.y), ky_fabs(v.z), ky_fabs(v.z));
}

/*
        specialization for length(VectorX<int>)
    */

inline float ky_length(const ky_vector4<int>& v) {
    return ky_sqrt( float(ky_length_sq(v)) );
}

template<typename T>
inline ky_vector2<T> ky_center(const ky_vector4<T>& v)
{
    return v.pos () + (v.size ()*0.5);
}
template<typename T>
inline void ky_swap (ky_vector4<T> &v1, ky_vector4<T> &v2)
{
    ky_vector4<T> tmp = v1;
    v1 = v2;
    v2 = tmp;
}
template<typename T>
inline bool ky_contains(const ky_vector4<T>& v, const ky_vector4<T>& v1)
{
    T l1 = v.x;
    T r1 = v.x;
    if (v.width < 0)
        l1 += v.width;
    else
        r1 += v.width;
    if (l1 == r1) // null rect
        return false;

    T l2 = v1.x;
    T r2 = v1.x;
    if (v1.width < 0)
        l2 += v1.width;
    else
        r2 += v1.width;
    if (l2 == r2) // null rect
        return false;

    if (l2 < l1 || r2 > r1)
        return false;

    T t1 = v.y;
    T b1 = v.y;
    if (v.height < 0)
        t1 += v.height ;
    else
        b1 += v.height ;
    if (t1 == b1) // null rect
        return false;

    T t2 = v1.y;
    T b2 = v1.y;
    if (v1.height  < 0)
        t2 += v1.height ;
    else
        b2 += v1.height ;
    if (t2 == b2) // null rect
        return false;

    if (t2 < t1 || b2 > b1)
        return false;

    return true;
}
template<typename T>
inline bool ky_contains(const ky_vector4<T>& v, const ky_vector2<T>&p)
{
    T l = v.x;
    T r = v.x;
    if (v.width < 0)
        l += v.width ;
    else
        r += v.width ;
    if (l == r) // null rect
        return false;

    if (p.x < l || p.x > r)
        return false;

    T t = v.y;
    T b = v.y;
    if (v.height < 0)
        t += v.height ;
    else
        b += v.height ;
    if (t == b) // null rect
        return false;

    if (p.y < t || p.y > b)
        return false;

    return true;
}
template<typename T>
inline bool ky_intersects(const ky_vector4<T>& v, const ky_vector4<T> &rhs)
{
    T l1 = v.x;
    T r1 = v.x;
    if (v.width < 0)
        l1 += v.width;
    else
        r1 += v.width;
    if (l1 == r1) // null rect
        return false;

    T l2 = rhs.x;
    T r2 = rhs.x;
    if (rhs.width < 0)
        l2 += rhs.width;
    else
        r2 += rhs.width;
    if (l2 == r2) // null rect
        return false;

    if (l1 >= r2 || l2 >= r1)
        return false;

    T t1 = v.y;
    T b1 = v.y;
    if (v.height  < 0)
        t1 += v.height ;
    else
        b1 += v.height ;
    if (t1 == b1) // null rect
        return false;

    T t2 = rhs.y;
    T b2 = rhs.y;
    if (rhs.height < 0)
        t2 += rhs.height ;
    else
        b2 += rhs.height ;
    if (t2 == b2) // null rect
        return false;

    if (t1 >= b2 || t2 >= b1)
        return false;

    return true;
}
template<typename T>
ky_vector4<T> ky_intersects(const ky_vector4<T>& v, const ky_vector4<T> &rhs)
{
    T l1 = v.x;
    T r1 = v.x;
    if (v.width < 0)
        l1 += v.width ;
    else
        r1 += v.width ;
    if (l1 == r1) // null rect
        return ky_vector4<T>();

    T l2 = rhs.x;
    T r2 = rhs.x;
    if (rhs.width  < 0)
        l2 += rhs.width ;
    else
        r2 += rhs.width ;
    if (l2 == r2) // null rect
        return ky_vector4<T>();

    if (l1 >= r2 || l2 >= r1)
        return ky_vector4<T>();

    T t1 = v.y;
    T b1 = v.y;
    if (v.height ()< 0)
        t1 += v.height ;
    else
        b1 += v.height ;
    if (t1 == b1) // null rect
        return ky_vector4<T>();

    T t2 = rhs.y;
    T b2 = rhs.y;
    if (rhs.height < 0)
        t2 += rhs.height ;
    else
        b2 += rhs.height ;
    if (t2 == b2) // null rect
        return ky_vector4<T>();

    if (t1 >= b2 || t2 >= b1)
        return ky_vector4<T>();

    ky_vector4<T> tmp;
    tmp.x = ky_max(l1, l2);
    tmp.y = ky_max(t1, t2);
    tmp.width  = ky_min(r1, r2) - tmp.x;
    tmp.height = ky_min(b1, b2) - tmp.y;
    return tmp;
}

/*
         stream operators
    */

/// output ky_vector4d to stream
template<typename T>
std::ostream& operator << (std::ostream& s, const ky_vector4<T>& v) {
    return (s << "[" << v.elem[0] << " " << v.elem[1] << " " << v.elem[2] << " " << v.elem[3] << "]");
}


/// input ky_vector4d from stream
template<typename T>
std::istream& operator >> (std::istream& s, ky_vector4<T>& v) {
    char ch;

    s >> ch;
    if (ch =='[') {  // format "[ x y z w ]"
        s >> v[0] >> v[1] >> v[2] >> v[3] >> ch;
        if (ch != ']') {
            s.clear(std::ios::badbit);
        }
    } else { // format "x y z w"
        s.putback(ch);
        s >> v[0] >> v[1] >> v[2] >> v[3];
    }

    return s;
}

template<typename T>
ky_streamb &operator << (ky_streamb &in, const ky_vector4<T> &v)
{
    for (int i = 0;i < v.size; ++i)
        in << v.elem[i];
    return in;
}

template<typename T>
ky_streamb &operator >> (ky_streamb &out, ky_vector4<T> &v)
{
    for (int i = 0;i < v.size; ++i)
        out >> v.elem[i];
    return out;
}
template<typename T>
ky_streamt &operator << (ky_streamt &in, const ky_vector4<T> &v)
{
    for (int i = 0;i < v.size; ++i)
        in << v.elem[i];
    return in;
}

template<typename T>
ky_streamt &operator >> (ky_streamt &out, ky_vector4<T> &v)
{
    for (int i = 0;i < v.size; ++i)
        out >> v.elem[i];
    return out;
}
const fourwd_t kyVec4Code = kyFourWord(vec4);
template<typename T>
ky_variant &operator << (ky_variant &va, const ky_vector4<T> &col)
{
    va.set(kyVec4Code, 0, &col, sizeof(ky_vector4<T>));
    return va;
}
template<typename T>
ky_variant &operator >> (ky_variant &va, ky_vector4<T> &col)
{
    col = va.get<ky_vector3<T>>(kyVec4Code);
    return va;
}
#endif // VEC4_H
