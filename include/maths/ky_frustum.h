
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
 * @file     ky_frustum.h
 * @brief    三维视锥
 *
 * @author   kunyang
 * @email    kunyang.yk@gmail.com
 * @version  1.0.0.1
 * @date     2019/08/19
 * @license  GNU General Public License (GPL)
 *
 * Change History :
 *    Date    |  Version  |  Author  |   Description
 * 2019/08/19 | 1.0.0.1   | kunyang  | 创建文件
 */
#ifndef ky_FRUSTUM_H
#define ky_FRUSTUM_H

#include "ky_maths.h"
#include "tools/ky_stream.h"
#include "tools/ky_variant.h"
#include "maths/ky_aabb.h"
#include "maths/ky_plane.h"

template<typename T>
struct ky_frustum
{
    typedef enum
    {
        Left,
        Right,
        Top,
        Bottom,
        Near,
        Far,
        Count
    }PlaneType;
    T           _data[Count];
    ky_plane<T> _plane[Count];

    ky_frustum()
    {
        data[Left] = T(0);
        data[Right] = T(0);
        data[Top] =  T(0);
        data[Bottom] = T(0);
        data[Near] = T(0);
        data[Far] = T(0);
    }
    ky_frustum(T fovy, T ratio, T near, T far)
    {
        const T half_h = ky_tan(0.5f * ky_deg2rad(fovy)) * near;
        data[Near] = near;
        data[Far] = far;
        data[Top] =  half_h;
        data[Bottom] = -half_h;
        set_ratio(ratio);
    }
    ky_frustum(T left, T right, T bottom, T top, T near, T far)
    {
        data[Left] = left;
        data[Right] = right;
        data[Top] =  top;
        data[Bottom] = bottom;
        data[Near] = near;
        data[Far] = far;
    }

    T left() const {return data[Left];}
    T right() const {return data[Right];}
    T top() const {return data[Top];}
    T bottom() const {return data[Bottom];}
    T near() const {return data[Near];}
    T far() const {return data[Far];}
    T fovy() const
    {
        return ky_rad2deg(ky_atan(data[Top] / data[Near]) -
                          ky_atan(data[Bottom] / data[Near]));
    }
    T ratio() const
    {
        return (data[Right] - data[Left]) / (data[Top] - data[Bottom]);
    }

    void set_left(T v) {data[Left] = v;}
    void set_right(T v) {data[Right] = v;}
    void set_top(T v)  {data[Top] = v; }
    void set_bottom(T v)  {data[Bottom] = v;}
    void set_near(T v)  {data[Near] = v;}
    void set_far(T v)  {data[Far] = v;}
    void set_fovy(T fovy)
    {
        const T half_h = ky_tan(T(.5) * ky_deg2rad(fovy)) * data[Near];
        data[Top] = half_h;
        data[Bottom] = -half_h;
        data[Fovy] = fovy;
    }
    void set_ratio(T aspect_ratio)
    {
        const T half_w = T(.5) * (data[Top] - data[Bottom]) * aspect_ratio;
        data[Left] = -half_w;
        data[Right] =  half_w;
        data[Bottom] = -half_w / aspect_ratio;
        data[Top] =  half_w / aspect_ratio;
    }

    //!
    //! \brief generate_matrix 生成视锥矩阵
    //! \return
    //!
    ky_matrix4X4<T> generate_matrix()const
    {
        return ky_matrix4X4<T>::create_ortho(left(), right(),
                                             top(), bottom(),
                                             near(), far());
    }
};

//!
//! \brief ky_isCulled 在视锥中边界框是否被剔除
//! \param f
//! \param b
//! \return
//!
template<typename T>
inline bool ky_isCulled(const ky_frustum<T> &f, const ky_aabb<T>& b)
{
    ky_vector3<T> far_p;

    for (int i = 0; i < kyArraySizeOf(f._plane); ++i)
    {
        far_p.x = f._plane[i].norm.x > 0.0f ? b.max().x : b.min().x;
        far_p.y = f._plane[i].norm.y > 0.0f ? b.max().y : b.min().y;
        far_p.z = f._plane[i].norm.z > 0.0f ? b.max().z : b.min().z;

        if (f._plane[i].classify_point(far_p) == ky_plane::BackSide)
            return false;
    }

    return true;
}

//!
//! \brief ky_isCulled 在视锥中点是否被剔除
//! \param f
//! \param l
//! \return
//!
template<typename T>
inline bool ky_isCulled(const ky_frustum<T> &f, const ky_vector3<T>& v)
{
    for (int i = 0; i < kyArraySizeOf(f._plane); ++i)
    {
        if (f._plane[i].classify_point(v) == ky_plane::BackSide)
            return false;
    }
    return true;
}

//!
//! \brief ky_isCulled 在视锥中线段是否被剔除
//! \param f
//! \param l
//! \return
//!
template<typename T>
inline bool ky_isCulled(const ky_frustum<T> &f, const ky_line3<T>& l)
{
    if(ky_isCulled(f, l.start()) || ky_isCulled(f, l.end()))
        return true;

    ky_vector3<T> intersect;
    for (int i = 0; i < kyArraySizeOf(f._plane); ++i)
    {
        if (ky_intersects(f._plane[i], l, intersect))
        {
            if (ky_isCulled(f, intersect))
                return true;
        }
    }
    return false;
}
//!
//! \brief ky_isCulled 在视锥中球体是否被剔除
//! \param f
//! \param center
//! \param radius
//! \return
//!
template<typename T>
inline bool ky_isCulled(const ky_frustum<T> &f, const ky_vector3<T>& center, float radius)
{
    for (int i = 0; i < kyArraySizeOf(f._plane); ++i)
    {
        if (-ky_distance(f._plane[i], center) > radius)
            return false;
    }

    return true;
}
#endif
