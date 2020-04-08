
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
 * @file     ky_vec3.h
 * @brief    3维向量
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
#ifndef ky_VEC3_H
#define ky_VEC3_H

#include "ky_vec2.h"

template<typename T>
struct ky_vector3_mem
{
    typedef T ElemType;

    enum {
        size = 3
    };

    union {
        struct {
            T x;
            T y;
            T z;
        };
        struct {
            T s;
            T t;
            T p;
        };
        struct {
            T r;
            T g;
            T b;
        };
        struct {
          T width;
          T height;
          T depth;
        };

        struct {
            T _xy[size-1];
            T _z;
        };
        T elem[size];
    };
};
/**
        This is a 3-dimensional Vector typename similar to vec3 of GLSL.
        All standard-operators are overloaded and go componentwisely
        similar to GLSL. For dot- and crossproduct use the functions dot and cross.
    */
template<typename T>
struct ky_vector3 :public ky_vector3_mem<T>
{
    typedef ky_vector3_mem<T> memory;
    /// default constructor
    ky_vector3() {}
    /// Init all elements with the same value
    explicit ky_vector3(T v) {
        for (int64 i = 0; i < memory::size; ++i)
            memory::elem[i] = v;
    }
    /// Init from array with equal size
    explicit ky_vector3(const T* v) {
        for (int64 i = 0; i < memory::size; ++i)
            memory::elem[i] = v[i];
    }
    /// Init componentwisely
    ky_vector3(T t1, T t2, T t3) {
        memory::elem[0] = t1;
        memory::elem[1] = t2;
        memory::elem[2] = t3;
    }
    /// Init x and y with v, z with _z
    ky_vector3(const ky_vector3<T>& vec, T z_) {
        memory::elem[0] = vec.elem[0];
        memory::elem[1] = vec.elem[1];
        memory::elem[2] = z_;
    }
    /// Init y and z with vec2, x with _x
    ky_vector3(T _x, const ky_vector3<T>& v) {
        memory::elem[0] = _x;
        memory::elem[1] = v.elem[1];
        memory::elem[2] = v.elem[2];
    }
    ky_vector3(const ky_vector2<T>& v) {
        memory::elem[0] = v.elem[0];
        memory::elem[1] = v.elem[1];
        memory::elem[2] = (T)0;
    }
    ky_vector3(const ky_vector2<T>& v, T z) {
        memory::elem[0] = v.elem[0];
        memory::elem[1] = v.elem[1];
        memory::elem[2] = z;
    }

    /// Init with another Vector of another type
    template<typename U>
    ky_vector3(const ky_vector3<U>& v) {
        for (int64 i = 0; i < v.size; ++i)
            memory::elem[i] = T(v.elem[i]);
    }

    ky_vector3(const ky_vector3<T> & v){
        for (int64 i = 0; i < v.size; ++i)
            memory::elem[i] = v.elem[i];
    }

    template<typename U>
    ky_vector3(const ky_vector3_mem<U>& v) {
        for (int64 i = 0; i < v.size; ++i)
            memory::elem[i] = T(v.elem[i]);
    }
    ky_vector3(const ky_vector3_mem<T> & v){
        for (int64 i = 0; i < v.size; ++i)
            memory::elem[i] = v.elem[i];
    }

    inline ky_vector3<T> &operator = (const ky_vector3<T> &v){
        for (int64 i = 0; i < v.size; ++i)
            memory::elem[i] = v.elem[i];
        return *this;
    }

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
        return *((ky_vector2<T>*) memory::_xy);
    }
    inline ky_vector2<T>& xy() {
        return *((ky_vector2<T>*) memory::_xy);
    }
    inline const ky_vector2<T>& yz() const {
        return *((ky_vector2<T>*) (memory::elem + 1));
    }
    inline ky_vector2<T>& yz() {
        return *((ky_vector2<T>*) (memory::elem + 1));
    }

    /*
        statics
    */
    static ky_vector3<T> zero;
};

/// init statics
template<typename T>
ky_vector3<T> ky_vector3<T>::zero = ky_vector3<T>(T(0), T(0), T(0));

typedef ky_vector3<float>   ky_vector3f;
typedef ky_vector3<real>  ky_vector3d;

typedef ky_vector3f        vector;

/*
        types like in GLSL
    */

typedef ky_vector3<float>   vec3;
typedef ky_vector3<real>    dvec3; // currently not supported in GLSL but a reserved keyword
typedef ky_vector3<int>     ivec3;
typedef ky_vector3<int64>   svec3;
typedef ky_vector3<bool>    bvec3;

typedef ky_vector3<int>   isize3;
typedef ky_vector3<float> fsize3;
typedef ky_vector3<real>  dsize3;

typedef ky_vector3<int>   ipoint3;
typedef ky_vector3<float> fpoint3;
typedef ky_vector3<real>  dpoint3;

template<typename T>
inline ky_vector3<T> ky_perpendicular( const ky_vector3<T>& src, const ky_vector3<T>& vec )  {
    T crossX = src.y*vec.z - src.z*vec.y;
    T crossY = src.z*vec.x - src.x*vec.z;
    T crossZ = src.x*vec.y - src.y*vec.x;

    T length = (float)ky_sqrt(crossX*crossX +
                              crossY*crossY +
                              crossZ*crossZ);

    if( length > 0 )
        return ky_vector3<T>( crossX/length, crossY/length, crossZ/length );
    else
        return ky_vector3<T>();
}

template<typename T>
inline ky_vector3<T> ky_crossed( const ky_vector3<T>& src ,const ky_vector3<T>& vec )  {
    return ky_vector3<T>( src.y*vec.z - src.z*vec.y,
                          src.z*vec.x - src.x*vec.z,
                          src.x*vec.y - src.y*vec.x );
}
template<typename T>
inline ky_vector3<T> ky_rotated_rad(const ky_vector3<T> &src, T angle, const ky_vector3<T>& axis )
{
    ky_vector3<T> ax = ky_normalize(axis);
    T a = angle;
    T sina = ky_sin( a );
    T cosa = ky_cos( a );
    T cosb = (T)1 - cosa;

    return ky_vector3<T>( src.x*(ax.x*ax.x*cosb + cosa)
                          + src.y*(ax.x*ax.y*cosb - ax.z*sina)
                          + src.z*(ax.x*ax.z*cosb + ax.y*sina),
                          src.x*(ax.y*ax.x*cosb + ax.z*sina)
                          + src.y*(ax.y*ax.y*cosb + cosa)
                          + src.z*(ax.y*ax.z*cosb - ax.x*sina),
                          src.x*(ax.z*ax.x*cosb - ax.y*sina)
                          + src.y*(ax.z*ax.y*cosb + ax.x*sina)
                          + src.z*(ax.z*ax.z*cosb + cosa) );
}

/*
        Prepare to implement 3 * 5 * 8 = 120 operators and dozens of functions
        the lazy way with evil voodoo macro magic
    */
template<typename T> inline ky_vector3<T> operator + (const ky_vector3<T>& v1, const ky_vector3<T>& v2)
{
    ky_vector3<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = v1.elem[i] + v2.elem[i];
    return vRes;
}


template<typename T> inline ky_vector3<T> operator + (const ky_vector3<T>& v, T t)
{
    ky_vector3<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = v.elem[i] + t;
    return vRes;
}
template<typename T> inline ky_vector3<T> operator + (T t, const ky_vector3<T>& v)
{
    ky_vector3<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = t + v.elem[i];
    return vRes;
}
template<typename T> inline ky_vector3<T>& operator += (ky_vector3<T>& v1, const ky_vector3<T>& v2)
{
    for (int64 i = 0; i < v1.size; ++i)
        v1.elem[i] += v2.elem[i];
    return v1;
}
template<typename T> inline ky_vector3<T>& operator += (ky_vector3<T>& v, T t)
{
    for (int64 i = 0; i < v.size; ++i)
        v.elem[i] += t;
    return v;
}

template<typename T> inline ky_vector3<T> operator - (const ky_vector3<T>& v1, const ky_vector3<T>& v2)
{
    ky_vector3<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = v1.elem[i] - v2.elem[i];
    return vRes;
}


template<typename T> inline ky_vector3<T> operator - (const ky_vector3<T>& v, T t)
{
    ky_vector3<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = v.elem[i] - t;
    return vRes;
}
template<typename T> inline ky_vector3<T> operator - (T t, const ky_vector3<T>& v)
{
    ky_vector3<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = t - v.elem[i];
    return vRes;
}
template<typename T> inline ky_vector3<T>& operator -= (ky_vector3<T>& v1, const ky_vector3<T>& v2)
{
    for (int64 i = 0; i < v1.size; ++i)
        v1.elem[i] -= v2.elem[i];
    return v1;
}
template<typename T> inline ky_vector3<T>& operator -= (ky_vector3<T>& v, T t)
{
    for (int64 i = 0; i < v.size; ++i)
        v.elem[i] -= t;
    return v;
}

template<typename T> inline ky_vector3<T> operator * (const ky_vector3<T>& v1, const ky_vector3<T>& v2)
{
    ky_vector3<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = v1.elem[i] * v2.elem[i];
    return vRes;
}


template<typename T> inline ky_vector3<T> operator * (const ky_vector3<T>& v, T t)
{
    ky_vector3<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = v.elem[i] * t;
    return vRes;
}
template<typename T> inline ky_vector3<T> operator * (T t, const ky_vector3<T>& v)
{
    ky_vector3<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = t * v.elem[i];
    return vRes;
}
template<typename T> inline ky_vector3<T>& operator *= (ky_vector3<T>& v1, const ky_vector3<T>& v2)
{
    for (int64 i = 0; i < v1.size; ++i)
        v1.elem[i] *= v2.elem[i];
    return v1;
}
template<typename T> inline ky_vector3<T>& operator *= (ky_vector3<T>& v, T t)
{
    for (int64 i = 0; i < v.size; ++i)
        v.elem[i] *= t;
    return v;
}

template<typename T> inline ky_vector3<T> operator / (const ky_vector3<T>& v1, const ky_vector3<T>& v2)
{
    ky_vector3<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = v1.elem[i] / v2.elem[i];
    return vRes;
}


template<typename T> inline ky_vector3<T> operator / (const ky_vector3<T>& v, T t)
{
    ky_vector3<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = v.elem[i] / t;
    return vRes;
}
template<typename T> inline ky_vector3<T> operator / (T t, const ky_vector3<T>& v)
{
    ky_vector3<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = t / v.elem[i];
    return vRes;
}
template<typename T> inline ky_vector3<T>& operator /= (ky_vector3<T>& v1, const ky_vector3<T>& v2)
{
    for (int64 i = 0; i < v1.size; ++i)
        v1.elem[i] /= v2.elem[i];
    return v1;
}
template<typename T> inline ky_vector3<T>& operator /= (ky_vector3<T>& v, T t)
{
    for (int64 i = 0; i < v.size; ++i)
        v.elem[i] /= t;
    return v;
}

template<typename T> inline ky_vector3<T> operator % (const ky_vector3<T>& v1, const ky_vector3<T>& v2)
{
    ky_vector3<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = v1.elem[i] % v2.elem[i];
    return vRes;
}


template<typename T> inline ky_vector3<T> operator % (const ky_vector3<T>& v, T t)
{
    ky_vector3<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = v.elem[i] % t;
    return vRes;
}
template<typename T> inline ky_vector3<T> operator % (T t, const ky_vector3<T>& v)
{
    ky_vector3<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = t % v.elem[i];
    return vRes;
}
template<typename T> inline ky_vector3<T>& operator %= (ky_vector3<T>& v1, const ky_vector3<T>& v2)
{
    for (int64 i = 0; i < v1.size; ++i)
        v1.elem[i] %= v2.elem[i];
    return v1;
}
template<typename T> inline ky_vector3<T>& operator %= (ky_vector3<T>& v, T t)
{
    for (int64 i = 0; i < v.size; ++i)
        v.elem[i] %= t;
    return v;
}

template<typename T> inline ky_vector3<T> operator & (const ky_vector3<T>& v1, const ky_vector3<T>& v2)
{
    ky_vector3<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = v1.elem[i] & v2.elem[i];
    return vRes;
}


template<typename T> inline ky_vector3<T> operator & (const ky_vector3<T>& v, T t)
{
    ky_vector3<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = v.elem[i] & t;
    return vRes;
}
template<typename T> inline ky_vector3<T> operator & (T t, const ky_vector3<T>& v)
{
    ky_vector3<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = t & v.elem[i];
    return vRes;
}
template<typename T> inline ky_vector3<T>& operator &= (ky_vector3<T>& v1, const ky_vector3<T>& v2)
{
    for (int64 i = 0; i < v1.size; ++i)
        v1.elem[i] &= v2.elem[i];
    return v1;
}
template<typename T> inline ky_vector3<T>& operator &= (ky_vector3<T>& v, T t)
{
    for (int64 i = 0; i < v.size; ++i)
        v.elem[i] &= t;
    return v;
}

template<typename T> inline ky_vector3<T> operator | (const ky_vector3<T>& v1, const ky_vector3<T>& v2)
{
    ky_vector3<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = v1.elem[i] | v2.elem[i];
    return vRes;
}

template<typename T> inline ky_vector3<T> operator | (const ky_vector3<T>& v, T t)
{
    ky_vector3<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = v.elem[i] | t;
    return vRes;
}
template<typename T> inline ky_vector3<T> operator | (T t, const ky_vector3<T>& v)
{
    ky_vector3<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = t | v.elem[i];
    return vRes;
}
template<typename T> inline ky_vector3<T>& operator |= (ky_vector3<T>& v1, const ky_vector3<T>& v2)
{
    for (int64 i = 0; i < v1.size; ++i)
        v1.elem[i] |= v2.elem[i];
    return v1;
}
template<typename T> inline ky_vector3<T>& operator |= (ky_vector3<T>& v, T t)
{
    for (int64 i = 0; i < v.size; ++i)
        v.elem[i] |= t;
    return v;
}

template<typename T> inline ky_vector3<T> operator ^ (const ky_vector3<T>& v1, const ky_vector3<T>& v2)
{
    ky_vector3<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = v1.elem[i] ^ v2.elem[i];
    return vRes;
}


template<typename T> inline ky_vector3<T> operator ^ (const ky_vector3<T>& v, T t)
{
    ky_vector3<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = v.elem[i] ^ t;
    return vRes;
}
template<typename T> inline ky_vector3<T> operator ^ (T t, const ky_vector3<T>& v)
{
    ky_vector3<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = t ^ v.elem[i];
    return vRes;
}
template<typename T> inline ky_vector3<T>& operator ^= (ky_vector3<T>& v1, const ky_vector3<T>& v2)
{
    for (int64 i = 0; i < v1.size; ++i)
        v1.elem[i] ^= v2.elem[i];
    return v1;
}
template<typename T> inline ky_vector3<T>& operator ^= (ky_vector3<T>& v, T t)
{
    for (int64 i = 0; i < v.size; ++i)
        v.elem[i] ^= t;
    return v;
}

/*
        relational operators
    */
template<typename T>
inline ky_vector3<bool> ky_less_than (const ky_vector3<T>& v1, const ky_vector3<T>& v2)
{
    ky_vector3<bool> vRes;
    for (int64 i = 0; i < vRes.size; ++i)
        vRes.elem[i] = (v1.elem[i] < v2.elem[i]);
    return vRes;
}

template<typename T>
inline ky_vector3<bool> ky_less_than_equal (const ky_vector3<T>& v1, const ky_vector3<T>& v2)
{
    ky_vector3<bool> vRes;
    for (int64 i = 0; i < vRes.size; ++i)
        vRes.elem[i] = (v1.elem[i] <= v2.elem[i]);
    return vRes;
}

template<typename T>
inline ky_vector3<bool> ky_greater_than (const ky_vector3<T>& v1, const ky_vector3<T>& v2)
{
    ky_vector3<bool> vRes;
    for (int64 i = 0; i < vRes.size; ++i)
        vRes.elem[i] = (v1.elem[i] > v2.elem[i]);
    return vRes;
}

template<typename T>
inline ky_vector3<bool> ky_greater_than_equal (const ky_vector3<T>& v1, const ky_vector3<T>& v2)
{
    ky_vector3<bool> vRes;
    for (int64 i = 0; i < vRes.size; ++i)
        vRes.elem[i] = (v1.elem[i] >= v2.elem[i]);
    return vRes;
}
template<typename T>
inline ky_vector3<bool> ky_equal (const ky_vector3<T>& v1, const ky_vector3<T>& v2)
{
    ky_vector3<bool> vRes;
    for (int64 i = 0; i < vRes.size; ++i)
        vRes.elem[i] = (v1.elem[i] == v2.elem[i]);
    return vRes;
}

template<typename T>
inline ky_vector3<bool> ky_not_equal (const ky_vector3<T>& v1, const ky_vector3<T>& v2)
{
    ky_vector3<bool> vRes;
    for (int64 i = 0; i < vRes.size; ++i)
        vRes.elem[i] = (v1.elem[i] != v2.elem[i]);
    return vRes;
}

template<typename T>
inline bool operator == (const ky_vector3<T>& v1, const ky_vector3<T>& v2)
{
    bool result = true;
    for (int64 i = 0; i < v1.size; ++i)
        result &= (v1.elem[i] == v2.elem[i]);
    return result;
}

template<typename T>
inline bool operator != (const ky_vector3<T>& v1, const ky_vector3<T>& v2)
{
    return !(v1 == v2);
}
template<typename T> inline bool operator > (const ky_vector3<T>& v1, const ky_vector3<T>& v2)
{
    bool result = true;
    for (int64 i = 0; i < v1.size; ++i)
        result &= (v1.elem[i] > v2.elem[i]);
    return result;
}
template<typename T> inline bool operator < (const ky_vector3<T>& v1, const ky_vector3<T>& v2)
{
    bool result = true;
    for (int64 i = 0; i < v1.size; ++i)
        result &= (v1.elem[i] < v2.elem[i]);
    return result;
}


/*
        further vector functions
    */
template<typename T> inline T ky_dot(const ky_vector3<T>& v1, const ky_vector3<T>& v2)
{
    T result = 0;
    for (int64 i = 0; i < v1.size; ++i)
        result += v1.elem[i] * v2.elem[i];
    return result;
}

template<typename T> inline T ky_length_sq(const ky_vector3<T>& v)
{
    T result = 0;
    for (int64 i = 0; i < v.size; ++i)
        result += (v.elem[i] * v.elem[i]);
    return result;
}

template<typename T> inline T ky_length(const ky_vector3<T>& v)
{
    return ky_sqrt(ky_length_sq(v));
}

template<typename T> inline ky_vector3<T> ky_normalize(const ky_vector3<T>& v)
{
    ky_vector3<T> vRes(v);
    T recipLength = 1 / ky_length(v);
    vRes *= recipLength;
    return vRes;
}

template<typename T> inline T ky_distance(const ky_vector3<T>& v1, const ky_vector3<T>& v2)
{
    return ky_length(v2 - v1);
}

template<typename T> inline T ky_distance_sq(const ky_vector3<T>& v1, const ky_vector3<T>& v2)
{
    return ky_length_sq(v2 - v1);
}

template<typename T> inline ky_vector3<T> operator - (const ky_vector3<T>& v)
{
    ky_vector3<T> vRes(v);
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = -v.elem[i];
    return vRes;
}

template<typename T> inline ky_vector3<T> ky_min(const ky_vector3<T>& v1, const ky_vector3<T>& v2)
{
    ky_vector3<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = ky_min(v1.elem[i], v2.elem[i]);
    return vRes;
}

template<typename T> inline ky_vector3<T> ky_max(const ky_vector3<T>& v1, const ky_vector3<T>& v2)
{
    ky_vector3<T> vRes;
    for (int64 i = 0; i < v1.size; ++i)
        vRes.elem[i] = ky_max(v1.elem[i], v2.elem[i]);
    return vRes;
}

template<typename T> inline ky_vector3<T> ky_min(const ky_vector3<T>& v, T t)
{
    ky_vector3<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = ky_min(v.elem[i], t);
    return vRes;
}

template<typename T> inline ky_vector3<T> ky_max(const ky_vector3<T>& v, T t)
{
    ky_vector3<T> vRes;
    for (int64 i = 0; i < v.size; ++i)
        vRes.elem[i] = ky_max(v.elem[i], t);
    return vRes;
}

template<typename T> inline T ky_min(const ky_vector3<T>& v)
{
    T res = v.elem[0];
    for (int64 i = 1; i < v.size; ++i)
        res = ky_min(v.elem[i], res);
    return res;
}

template<typename T> inline T ky_max(const ky_vector3<T>& v)
{
    T res = v.elem[0];
    for (int64 i = 1; i < v.size; ++i)
        res = ky_max(v.elem[i], res);
    return res;
}

template<typename T>
inline ky_vector3<T> ky_clamp(const ky_vector3<T>& v, const ky_vector3<T>& minVal,
                              const ky_vector3<T>& maxVal)
{
    return ky_min( ky_max(v, minVal), maxVal );
}

template<typename T> inline ky_vector3<T> ky_clamp(const ky_vector3<T>& v, T minVal, T maxVal)
{
    return ky_min( ky_max(v, minVal), maxVal );
}

template<typename T> inline ky_vector3<T> ky_ceil(const ky_vector3<T>& v)
{
    ky_vector3<T> result;
    for (int64 i = 0; i < v.size; ++i)
        result[i] = ky_ceil(v.elem[i]);
    return result;
}

template<typename T> inline ky_vector3<T> ky_floor(const ky_vector3<T>& v)
{
    ky_vector3<T> result;
    for (int64 i = 0; i < v.size; ++i)
        result[i] = ky_floor(v.elem[i]);
    return result;
}

template<typename T> inline ky_vector3<T> ky_fix(const ky_vector3<T>& v)
{
    ky_vector3<T> result;
    for (int64 i = 0; i < v.size; ++i)
        result[i] = ky_fix(v.elem[i]);
    return result;
}

template<typename T> inline ky_vector3<T> ky_round(const ky_vector3<T>& v)
{
    ky_vector3<T> result;
    for (int64 i = 0; i < v.size; ++i)
        result[i] = ky_round(v.elem[i]);
    return result;
}

template<typename T> inline ky_vector3<T> ky_sign(const ky_vector3<T>& v) {
    ky_vector3<T> result;
    for (int64 i = 0; i < v.size; ++i)
        result[i] = ky_sign(v.elem[i]);
    return result;
}

template<typename T> inline ky_vector3<int> ky_iceil(const ky_vector3<T>& v)
{
    ky_vector3<int> result;
    for (int64 i = 0; i < v.size; ++i)
        result[i] = ky_iceil(v.elem[i]);
    return result;
}

template<typename T> inline ky_vector3<int> ky_ifloor(const ky_vector3<T>& v)
{
    ky_vector3<int> result;
    for (int64 i = 0; i < v.size; ++i)
        result[i] = ky_ifloor(v.elem[i]);
    return result;
}

template<typename T> inline ky_vector3<int> ky_iround(const ky_vector3<T>& v)
{
    ky_vector3<int> result;
    for (int64 i = 0; i < v.size; ++i)
        result[i] = ky_iround(v.elem[i]);
    return result;
}

template<typename T> inline ky_vector3<int> ky_ifix(const ky_vector3<T>& v)
{
    ky_vector3<int> result;
    for (int64 i = 0; i < v.size; ++i)
        result[i] = ky_ifix(v.elem[i]);
    return result;
}

template<typename T> inline ky_vector3<int> ky_isign(const ky_vector3<T>& v)
{
    ky_vector3<int> result;
    for (int64 i = 0; i < v.size; ++i)
        result[i] = ky_isign(v.elem[i]);
    return result;
}

template<typename T> inline ky_vector3<T> ky_mix(const ky_vector3<T>& v, const ky_vector3<T>& v2, T t)
{
    ky_vector3<T> result;
    for (int64 i = 0; i < v.size; ++i)
        result.elem[i] = (v.elem[i]*(1-t)) + (v2.elem[i]*t);
    return result;
}

/*
        horizontally working functions
    */
template<typename T> inline T ky_hadd(const ky_vector3<T>& v)
{
    T result = v.elem[0];
    for (int64 i = 1; i < v.size; ++i)
        result += v.elem[i];
    return result;
}

template<typename T> inline T ky_hsub(const ky_vector3<T>& v)
{
    T result = v.elem[0];
    for (int64 i = 1; i < v.size; ++i)
        result -= v.elem[i];
    return result;
}

template<typename T> inline T ky_hmul(const ky_vector3<T>& v)
{
    T result = v.elem[0];
    for (int64 i = 1; i < v.size; ++i)
        result *= v.elem[i];
    return result;
}

template<typename T> inline T ky_hdiv(const ky_vector3<T>& v)
{
    T result = v.elem[0];
    for (int64 i = 1; i < v.size; ++i)
        result /= v.elem[i];
    return result;
}

template<typename T> inline T ky_hmod(const ky_vector3<T>& v)
{
    T result = v.elem[0];
    for (int64 i = 1; i < v.size; ++i)
        result %= v.elem[i];
    return result;
}

template<typename T> inline bool ky_hand(const ky_vector3<T>& v)
{
    bool result = v.elem[0] && v.elem[1];
    for (int64 i = 2; i < v.size; ++i)
        result &= v.elem[i];
    return result;
}

template<typename T> inline bool ky_hor(const ky_vector3<T>& v)
{
    T result = v.elem[0] || v.elem[1];
    for (int64 i = 2; i < v.size; ++i)
        result |= v.elem[i];
    return result;
}

template<typename T> inline bool ky_hxor(const ky_vector3<T>& v)
{
    T result = v.elem[0] ^ v.elem[1];
    for (int64 i = 2; i < v.size; ++i)
        result ^= v.elem[i];
    return result;
}


/**
        cross product
    */
template<typename T>
inline ky_vector3<T> ky_cross(const ky_vector3<T>& v1, const ky_vector3<T>& v2)
{
    ky_vector3<T> vResult;
    vResult.elem[0] = v1.elem[1] * v2.elem[2] - v1.elem[2] * v2.elem[1];
    vResult.elem[1] = v1.elem[2] * v2.elem[0] - v1.elem[0] * v2.elem[2];
    vResult.elem[2] = v1.elem[0] * v2.elem[1] - v1.elem[1] * v2.elem[0];
    return vResult;
}

/*
        Component-wise absolute value
    */

template<typename T>
inline ky_vector3<T> ky_abs(const ky_vector3<T>& v)
{
    return ky_vector3<T>(ky_fabs(v.x), ky_fabs(v.y), ky_fabs(v.z));
}


/*
        specialization for length(VectorX<int>)
    */

inline float ky_length(const ky_vector3<int>& v) {
    return ky_sqrt( float(ky_length_sq(v)) );
}

template<typename T>
inline void ky_swap (ky_vector3<T> &v1, ky_vector3<T> &v2)
{
    ky_vector3<T> tmp = v1;
    v1 = v2;
    v2 = tmp;
}

/*
         stream operators
    */

/// output ky_vector3d to stream
template<typename T>
std::ostream& operator << (std::ostream& s, const ky_vector3<T>& v) {
    return (s << "[" << v.elem[0] << " " << v.elem[1] << " " << v.elem[2] << "]");
}

// specialication for (unsigned) int, to make it output numbers and not characters

template<>
inline std::ostream& operator << <unsigned char>(std::ostream& s, const ky_vector3<uchar>& v) {
    return (s << ky_vector3<int>(v));
}

template<>
inline std::ostream& operator << <char>(std::ostream& s, const ky_vector3<char>& v) {
    return (s << ky_vector3<int>(v));
}

/// input ky_vector3d from stream
template<typename T>
std::istream& operator >> (std::istream& s, ky_vector3<T>& v) {
    char ch;

    s >> ch;
    if (ch == '[') {  // format "[ x y z ]"
        s >> v[0] >> v[1] >> v[2] >> ch;
        if (ch != ']') {
            s.clear(std::ios::badbit);
        }
    } else { // format "x y z"
        s.putback(ch);
        s >> v[0] >> v[1] >> v[2];
    }

    return s;
}



template<typename T>
ky_streamb &operator << (ky_streamb &in, const ky_vector3<T> &v)
{
    for (int i = 0;i < v.size; ++i)
        in << v.elem[i];
    return in;
}

template<typename T>
ky_streamb &operator >> (ky_streamb &out, ky_vector3<T> &v)
{
    for (int i = 0;i < v.size; ++i)
        out >> v.elem[i];
    return out;
}
template<typename T>
ky_streamt &operator << (ky_streamt &in, const ky_vector3<T> &v)
{
    for (int i = 0;i < v.size; ++i)
        in << v.elem[i];
    return in;
}

template<typename T>
ky_streamt &operator >> (ky_streamt &out, ky_vector3<T> &v)
{
    for (int i = 0;i < v.size; ++i)
        out >> v.elem[i];
    return out;
}
const fourwd_t kyVec3Code = kyFourWord(vec3);
template<typename T>
ky_variant &operator << (ky_variant &va, const ky_vector3<T> &col)
{
    va.set(kyVec3Code, 0, &col, sizeof(ky_vector3<T>));
    return va;
}
template<typename T>
ky_variant &operator >> (ky_variant &va, ky_vector3<T> &col)
{
    col = va.get<ky_vector3<T>>(kyVec3Code);
    return va;
}
#endif // VEC3_H
