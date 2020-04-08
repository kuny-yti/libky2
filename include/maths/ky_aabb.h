
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
 * @file     ky_aabb.h
 * @brief    AABB 轴对齐包围盒
 *
 * @author   kunyang
 * @email    kunyang.yk@gmail.com
 * @version  1.0.0.1
 * @date     2012/01/02
 * @license  GNU General Public License (GPL)
 *
 * Change History :
 *    Date    |  Version  |  Author  |   Description
 * 2014/04/09 | 1.0.0.1   | kunyang  | 创建文件
 */
#ifndef ky_aabb_H
#define ky_aabb_H
#include "ky_define.h"
#include "ky_mat4x4.h"
#include "ky_plane.h"
#include "tools/ky_stream.h"

// Axis-aligned bounding box
template<typename T>
struct ky_aabb
{
    typedef T ElemType;

    ky_aabb():
        _min(),
        _max()
    {

    }
    explicit ky_aabb(const ky_vector3<T>& v):
        _min(v),
        _max(v)
    {

    }

    ky_aabb(const ky_vector3<T>& v1, const ky_vector3<T>& v2):
        _min(v1),
        _max(v1)
    {
        extend(v2);
    }

    ky_aabb(const ky_aabb<T> & v)
    {
        *this = v;
    }

    inline void extend(const ky_vector3<T>& v)
    {
        _min = ky_min(min(), v);
        _max = ky_max(max(), v);
    }

    inline void extend(const ky_aabb& b)
    {
        extend(b.min());
        extend(b.max());
    }
    inline void expand(T amount)
    {
        _min -= ky_vector3<T>(amount);
        _max += ky_vector3<T>(amount);
    }

    inline ky_vector3<T> min() const { return _min; }
    inline ky_vector3<T> max() const { return _max; }

    inline T width() const{return diagonal().x;}
    inline T height() const{return diagonal().y;}
    inline T depth() const{return diagonal().z;}

    inline ky_aabb operator - () const
    {
        return ky_aabb(-this->max(), -this->min());
    }
    inline ky_aabb<T> &operator = (const ky_aabb<T> &v)
    {
        _min = v._min;
        _max = v._max;
        return *this;
    }

    //!
    //! \brief center 返回中心点
    //! \return
    //!
    inline ky_vector3<T> center() const {return (diagonal() * 0.5f + min());}
    //!
    //! \brief diagonal 返回对角线尺寸
    //! \return
    //!
    inline ky_vector3<T> diagonal() const {return (v._max - v._min);}
    //!
    //! \brief corner 返回角顶点
    //! \param index
    //! \return
    //!
    inline ky_vector3<T> corner(const ky_vector3<T> &index) const
    {
        ky_vector3<T> _corner;
        _corner.x = (index.x >0) ? v._max.x : v._min.x;
        _corner.y = (index.y >0) ? v._max.y : v._min.y;
        _corner.z = (index.z >0) ? v._max.z : v._min.z;
        return _corner;
    }
    //!
    //! \brief corners 返回8个角的顶点
    //! \param corners
    //! \return
    //!
    inline bool corners(ky_vector3<T> cs[8] ) const
    {
        int n = 0;
        if (is_valid())
        {
            ky_vector3<T> P;
            int i,j,k;
            for( i = 0; i < 2; i++ )
            {
                P.x = (i) ? max().x : min().x;
                for ( j = 0; j < 2; j++ )
                {
                    P.y = (j) ? max().y : min().y;
                    for ( k = 0; k < 2; k++ )
                    {
                        P.z = (k) ? max().z : min().z;
                        cs[n++] = P;
                    }
                }
            }
        }
        return (8 == n);
    }

    //!
    //! \brief volume 返回边界框的体积
    //! \return
    //!

    inline T volume() const
    {
        return ky_fabs((min().x - max().x) *
                       (min().y - max().y) *
                       (min().z - max().z));
    }

    //!
    //! \brief is_valid 返回边界框是否有效
    //! \return
    //!
    inline bool is_valid() const { return min() != max() && min() < max(); }

    inline ky_aabb transform(const ky_matrix4X4<T>& m) const
    {
        ky_aabb b;
        b.extend(m * ky_vector3<T>(min().x, min().y, min().z));
        b.extend(m * ky_vector3<T>(max().x, min().y, min().z));
        b.extend(m * ky_vector3<T>(min().x, max().y, min().z));
        b.extend(m * ky_vector3<T>(min().x, min().y, max().z));
        b.extend(m * ky_vector3<T>(max().x, max().y, min().z));
        b.extend(m * ky_vector3<T>(min().x, max().y, max().z));
        b.extend(m * ky_vector3<T>(max().x, min().y, max().z));
        b.extend(m * ky_vector3<T>(max().x, max().y, max().z));
        return b;
    }
    inline ky_aabb translate(const ky_vector3<T>& v) const
    {
        return ky_aabb(min() + v, max() + v);
    }

    ky_vector3<T> _min; //lower left front
    ky_vector3<T> _max; //upper right back
};

typedef ky_aabb<int>   iaabb;
typedef ky_aabb<float> faabb;
typedef ky_aabb<real>  daabb;


template<typename T>
inline ky_aabb<T>& operator |= (ky_aabb<T> &v, const ky_aabb<T>  &rhs)
{
    v._min = ky_min(v.min(), rhs.min());
    v._max = ky_max(v.max(), rhs.max());
    return v;
}
template<typename T>
inline ky_aabb<T>& operator &= (ky_aabb<T> &v, const ky_aabb<T>  &rhs)
{
    v._min = ky_max(v.min(), rhs.min());
    v._max = ky_min(v.max(), rhs.max());
    return v;
}
template<typename T>
inline ky_aabb<T>& operator /= (ky_aabb<T> &v, T rhs)
{
    v *= ((T)1.0 / rhs);
    return v;
}
template<typename T>
inline ky_aabb<T>& operator *= (ky_aabb<T> &v, T rhs)
{
    v._min *= rhs;
    v._max *= rhs;
    return v;
}
template<typename T>
inline ky_aabb<T>& operator -= (ky_aabb<T> &v, const ky_vector3<T>& rhs)
{
    v._min -= rhs;
    v._max -= rhs;
    return v;
}
template<typename T>
inline ky_aabb<T>& operator += (ky_aabb<T> &v, const ky_vector3<T>& rhs)
{
    v._min += rhs;
    v._max += rhs;
    return *this;
}

//!
//! \brief ky_distance 返回点v到边界框的最短距离
//! \param v
//! \return
//!
template<typename T>
inline T ky_distance (const ky_aabb<T> &t, const ky_vector3<T>& v) const
{
    return ky_sqrt(ky_distance_sq(t, v));
}
template<typename T>
inline T ky_distance_sq (const ky_aabb<T> &t, const ky_vector3<T>& v)const
{
    if (ky_contains(t, v))
        return T(0);
    else
    {
        ky_vector3<T> maxDist(0,0,0);

        if (v.x < t.min().x)
            maxDist.x = t.min().x - v.x;
        else if (v.x > t.max().x)
            maxDist.x = v.x - t.max().x;

        if (v.y < t.min().y)
            maxDist.y = t.min().y - v.y;
        else if (v.y > t.max().y)
            maxDist.y = v.y - t.max().y;

        if (v.z < t.min().z)
            maxDist.z = t.min().z - v.z;
        else if (v.z > t.max().z)
            maxDist.z = v.z - t.max().z;

        return ky_length_sq(maxDist);
    }
}

//!
//! \brief contains 是否包含p点
//! \param p
//! \return
//!
template<typename T>
inline bool ky_contains(const ky_aabb<T> &t,const ky_vector3<T>& p)
{
    return ( (p.x >= t.min().x) && (p.y >= t.min().y) && (p.z >= t.min().z) &&
             (p.x <= t.max().x) && (p.y <= t.max().y) && (p.z <= t.max().z) );
}

//!
//! \brief contains 是否包含b边界框
//! \param b
//! \return
//!
template<typename T>
inline bool ky_contains(const ky_aabb<T> &t, const ky_aabb<T>& b)
{
    return ( ky_contains(t, b.min()) && ky_contains(t, b.max()) );
}

//!
//! \brief ky_intersects 返回边界框是否相交
//! \param b
//! \return
//!
template<typename T>
inline bool ky_intersects(const ky_aabb<T> &t,const ky_aabb<T>& b)
{
    if ((t.min().x > b.max().x) || (b.min().x > t.max().x))
        return false;
    if ((t.min().y > b.max().y) || (b.min().y > t.max().y))
        return false;
    if ((t.min().z > b.max().z) || (b.min().z > t.max().z))
        return false;

    return true;
}
//!
//! \brief ky_intersects 是否和平面p相交
//! \param p
//! \return
//!
template<typename T>
inline bool ky_intersects(const ky_aabb<T> &t, const ky_plane<T>& p) const
{
    bool pointsNeg = false;
    bool pointsPos = false;

    T d = ky_distance(p, vec3(t.min().x, t.min().y, t.min().z));
    if(d < 0.0) pointsNeg = true; else if(d > 0.0) pointsPos = true;
    d = ky_distance(p, ky_vector3<T>(t.max().x, t.min().y, t.min().z));
    if(d < 0.0) pointsNeg = true; else if(d > 0.0) pointsPos = true;
    d = ky_distance(p, ky_vector3<T>(t.min().x, t.max().y, t.min().z));
    if(d < 0.0) pointsNeg = true; else if(d > 0.0) pointsPos = true;
    d = ky_distance(p, ky_vector3<T>(t.min().x, t.min().y, t.max().z));
    if(d < 0.0) pointsNeg = true; else if(d > 0.0) pointsPos = true;
    d = ky_distance(p, ky_vector3<T>(t.max().x, t.max().y, t.min().z));
    if(d < 0.0) pointsNeg = true; else if(d > 0.0) pointsPos = true;
    d = ky_distance(p, ky_vector3<T>(t.min().x, t.max().y, t.max().z));
    if(d < 0.0) pointsNeg = true; else if(d > 0.0) pointsPos = true;
    d = ky_distance(p, ky_vector3<T>(t.max().x, t.min().y, t.max().z));
    if(d < 0.0) pointsNeg = true; else if(d > 0.0f) pointsPos = true;
    d = ky_distance(p, ky_vector3<T>(t.max().x, t.max().y, t.max().z));
    if(d < 0.0) pointsNeg = true; else if(d > 0.0) pointsPos = true;

    return (pointsNeg && pointsPos);
}

//!
//! \brief ky_intersects 是否和射线ray相交
//! \param ray 射线的位置,射线的方向
//! \param tmin, tmax 从射线的位置到边界框中的入口和出口点在方向上的距离.
//! \return
//! \note 如果位置在AABB内，则将其计为交点，tmin和tmax之一可能为负
//!
template<typename T>
inline bool ky_intersects(const ky_aabb<T> &t, const ky_line3<T> &ray, T* tmin = 0, T* tmax = 0)
{
    T t1,t2;
    T tnear,tfar;

    if (ray.direction().x == 0)
    {
        if (ray.position().x < t.min().x || ray.position().x > t.max().x)
            return false;
        else
        {
            tnear = (T)-FLT_MAX;
            tfar = (T)FLT_MAX;
        }
    }
    else
    {
        t1 = (t.min().x - ray.position().x) / ray.direction().x;
        t2 = (t.max().x - ray.position().x) / ray.direction().x;

        if (ray.direction().x < 0)
        {
            tnear = t2;
            tfar = t1;
        }
        else
        {
            tnear = t1;
            tfar = t2;
        }

        if (tfar < 0)
            return false;
    }

    if (ray.direction().y == 0 && (ray.position().y < t.min().y || ray.position().y > t.max().y))
        return false;
    else
    {
        t1 = (t.min().y - ray.position().y) / ray.direction().y;
        t2 = (t.max().y - ray.position().y) / ray.direction().y;

        if (ray.direction().y < 0)
        {
            if (t2 > tnear)
                tnear = t2;
            if (t1 < tfar)
                tfar = t1;
        }
        else
        {
            if (t1 > tnear)
                tnear = t1;
            if (t2 < tfar)
                tfar = t2;
        }

        if (tnear > tfar || tfar < 0)
            return false;
    }

    if (ray.direction().z == 0 && (ray.position().z < t.min().z || ray.position().z > t.max().z))
        return false;
    else
    {
        t1 = (t.min().z - ray.position().z) / ray.direction().z;
        t2 = (t.max().z - ray.position().z) / ray.direction().z;

        if (ray.direction().z < 0)
        {
            if (t2 > tnear)
                tnear = t2;
            if (t1 < tfar)
                tfar = t1;
        }
        else
        {
            if (t1 > tnear)
                tnear = t1;
            if (t2 < tfar)
                tfar = t2;
        }

        if (tnear > tfar || tfar < 0)
        return false;
    }

    if (tmin)
        *tmin = tnear;
    if (tmax)
        *tmax = tfar;

    return true;
}

template<typename T>
inline bool ky_inside_xz(const ky_aabb<T> &t, const ky_aabb<T>& bound) const
{
    kyASSERT(t.is_valid ());
    kyASSERT(bound.is_valid());

    ky_vector3<T> llfb = bound.min();
    ky_vector3<T> urbb = bound.max();

    T r0x = ky_min(t.min()[0], t.max()[0]);
    T r1x = ky_max(t.min()[0], t.max()[0]);
    T r0y = ky_min(t.min()[2], t.max()[2]);
    T r1y = ky_max(t.min()[2], t.max()[2]);
    T r2x = ky_min(llfb[0], urbb[0]);
    T r3x = ky_max(llfb[0], urbb[0]);
    T r2y = ky_min(llfb[2], urbb[2]);
    T r3y = ky_max(llfb[2], urbb[2]);

    return (r0x >= r2x) && (r0y >= r2y) && (r1x <= r3x) && (r1y <= r3y);
}
template<typename T>
inline bool ky_inside_xz(const ky_aabb<T> &t, const ky_vector3<T>& v) const
{
    kyASSERT(t.is_valid());

    return (t.min()[0] <= v[0]) && (v[0] <= t.max()[0]) &&
            (t.min()[2] <= v[2]) && (v[2] <= t.max()[2]);
}

//!
//! \brief inside 返回bound边界框是否在内部
//! \param bound
//! \return
//!
template<typename T>
inline bool ky_inside(const ky_aabb<T> &t, const ky_aabb<T>& bound) const
{
    kyASSERT(t.is_valid());
    kyASSERT(bound.is_valid());

    vec3 llfb = bound.min();
    vec3 urbb = bound.max();

    T r0x = ky_min(t.min()[0], t.max()[0]);
    T r1x = ky_max(t.min()[0], t.max()[0]);
    T r0y = ky_min(t.min()[1], t.max()[1]);
    T r1y = ky_max(t.min()[1], t.max()[1]);
    T r0z = ky_min(t.min()[2], t.max()[2]);
    T r1z = ky_max(t.min()[2], t.max()[2]);

    T r2x = ky_min(llfb[0], urbb[0]);
    T r3x = ky_max(llfb[0], urbb[0]);
    T r2y = ky_min(llfb[1], urbb[1]);
    T r3y = ky_max(llfb[1], urbb[1]);
    T r2z = ky_min(llfb[2], urbb[2]);
    T r3z = ky_max(llfb[2], urbb[2]);

    return (r0x >= r2x) && (r1x <= r3x) &&
            (r0y >= r2y) && (r1y <= r3y) &&
            (r0z >= r2z) && (r1z <= r3z);
}

//!
//! \brief inside 返回v点是否在内部
//! \param v
//! \return
//!
template<typename T>
inline bool ky_inside(const ky_aabb<T> &t, const ky_vector3<T>& v) const
{
    kyASSERT(t.is_valid());

    return (t.min()[0] <= v[0]) && (v[0] <= t.max()[0]) &&
            (t.min()[1] <= v[1]) && (v[1] <= t.max()[1]) &&
            (t.min()[2] <= v[2]) && (v[2] <= t.max()[2]);
}


ky_streamb &operator << (ky_streamb &in, const ky_aabb &v)
{
    in << v._min;
    in << v._max;
    return in;
}
ky_streamb &operator >> (ky_streamb &out, ky_aabb &v)
{
    out >> v._min;
    out >> v._max;
    return out;
}
ky_streamt &operator << (ky_streamt &in, const ky_aabb &v)
{
    in << v._min;
    in << v._max;
    return in;
}
ky_streamt &operator >> (ky_streamt &out, ky_aabb &v)
{
    out >> v._min;
    out >> v._max;
    return out;
}

const fourwd_t kyAabbCode = kyFourWord(aabb);
template<typename T>
ky_variant &operator << (ky_variant &va, const ky_aabb<T> &col)
{
    va.set(kyAabbCode, 0, &col, sizeof(ky_aabb<T>));
    return va;
}
template<typename T>
ky_variant &operator >> (ky_variant &va, ky_aabb<T> &col)
{
    col = va.get<ky_aabb<T>>(kyAabbCode);
    return va;
}
#endif // ky_aabb_H
