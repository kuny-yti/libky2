
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
 * @file     ky_obb.h
 * @brief    OBB 定向包围盒
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

// Oriented Bounding Box
template<typename T>
struct ky_obb
{
    typedef T ElemType;

    ky_obb() { empty(); }

    ky_obb (const ky_vector3<T>& center,
            const ky_vector3<T>& u, const ky_vector3<T>& v, const ky_vector3<T>& w,
            const ky_vector3<T>& half_size):
        _center(center), _half_size(half_size)
    {
        _basis[0] = u;
        _basis[1] = v;
        _basis[2] = w;
    }

    //!
    //! \brief corner 返回指定角
    //! \param u
    //! \param v
    //! \param w
    //! \return
    //!
    ky_vector3<T> corner(int u, int v, int w) const
    {
        return _center +
                _basis[0] * (u * _half_size[0]) +
                _basis[1] * (v * _half_size[1]) +
                _basis[2] * (w * _half_size[2]);
    }

    explicit ky_obb(const ky_aabb<T>& bound)
    {
        if (bound.is_empty())
            empty();
        else
        {
            _center = bound.centre();

            // the axes of an AABB are the world-space axes
            _basis[0].X = 1.f; _basis[0].Y = 0.f; _basis[0].Z = 0.f;
            _basis[1].X = 0.f; _basis[1].Y = 1.f; _basis[1].Z = 0.f;
            _basis[2].X = 0.f; _basis[2].Y = 0.f; _basis[2].Z = 1.f;

            _half_size = (bound[1] - bound[0]) * 0.5f;
        }
    }

    void empty()
    {
        _center = ky_vector3<T>();
        _basis[0] = ky_vector3<T>();
        _basis[1] = ky_vector3<T>();
        _basis[2] = ky_vector3<T>();
        _half_size = ky_vector3<T>();
    }

    bool is_empty() const
    {
        ky_vector3<T> empty;
        return (_center == empty && _half_size == empty &&
                _basis[0] == empty && _basis[1] == empty &&
                _basis[2] == empty);
    }

    ky_vector3<T> _center;    ///< 中心位置
    ky_vector3<T> _half_size; ///< 包围盒尺寸
    ky_vector3<T> _basis[3];

    static const ky_obb EMPTY;
};

template<typename T>
bool ky_intersects(const ky_obb<T> &o,
                   const ky_vector3<T>& origin, const ky_vector3<T>& dir,
                   float& tMin_out, float& tMax_out)
{
    T tMin = -FLT_MAX;
    T tMax = FLT_MAX;

    ky_vector3<T> p = o._center - origin;

    for (int i = 0; i < 3; ++i)
    {
        // 测试射线与法线向量面的交点
        T e = ky_dot(o._basis[i], p);   // 射线起点与投影到平面法线上的框中心之间的距离
        T f = ky_dot(o._basis[i], dir); // 平面法线和射线方向之间的角度的余弦值

        if(ky_fabs(f) > 1e-10f)
        {
            // 确定从射线的原点到与平板相交的点的距离t1和t2
            T invF = T(1) /f;
            T t1 = (e + o._half_size[i]) * invF;
            T t2 = (e - o._half_size[i]) * invF;

            if (t1 > t2)
                ky_swap(t1, t2);

            if (t1 > tMin) tMin = t1;
            if (t2 < tMax) tMax = t2;

            if (tMin > tMax) return false;
            if (tMax < 0) return false;
        }
        else
        {
            if (e > m_HalfSizes[i] || -e > m_HalfSizes[i])
                return false;
        }
    }

    tMin_out = tMin;
    tMax_out = tMax;
    return true;
}

#endif
