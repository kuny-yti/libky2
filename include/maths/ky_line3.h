
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
 * @file     ky_line3.h
 * @brief    3D线段类
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
#ifndef ky_line3_H
#define ky_line3_H

#include "ky_vec3.h"

/// 3D 线段
template <typename T>
struct ky_line3
{
    ky_vector3<T> _pos; /// position
    ky_vector3<T> _dir; /// direction

    ky_line3() :_pos(),_dir()
    {

    }

    ky_line3(const ky_vector3<T>& start, const ky_vector3<T>& end):
        _pos(start),
        _dir(end - start)
    {

    }

    ky_line3(const ky_line3<T> &v):
        _pos(v._pos),
        _dir(v._dir)
    {
    }
    inline ky_line3<T> &operator = (const ky_line3<T> &c)
    {
        _pos = c._pos;
        _dir = c._dir;
        return *this;
    }
    /**
        * 线段中进行线性插值
        * @param t Should be betweeen 0 and 1.
        * @return (start*(1-t) + end*t)
        */
    inline ky_vector3<T> interpolates(T t) const
    {
        return _pos + t * _dir;
    }

    inline ky_vector3<T> start() const { return _pos; }
    inline ky_vector3<T> end() const { return _pos + _dir; }
    inline ky_vector3<T> position() const {return _pos;}

    inline void set_start(const ky_vector3<T>& s)
    {
        _dir = (_pos + _dir) - s;
        _pos = s;
    }

    inline void set_end(const ky_vector3<T>& e)
    {
        _dir = (e - _pos);
    }

    inline ky_vector3<T> direction() const { return _dir; }
    inline void set_direction(const ky_vector3<T>& dir) { _dir = dir; }
};


typedef ky_line3<int>     iline3;
typedef ky_line3<float>   fline3;
typedef ky_line3<real>    dline3;

//!
//! ky_intersects_segment 线段与线段相交(线段是有长度的)
//! \param src
//! \param l 要检测的相交线
//! \param v 如果发生相交，则保存结果，否则保持不变
//! \return
//!
template <typename T>
inline bool ky_intersects_segment(const ky_line3<T>& src, const ky_line3<T>& l, ky_vector3<T>& v)
{
    ky_vector3<T> c = ky_cross(src.direction(), l.direction());
    T q = ky_length_sq(c);

    if (ky_fabs(q) > 0.00001f)
    {
        ky_vector3<T> o = l.position() - src.position();
        ky_vector3<T> d = l.direction();

        T t = o.x * src.direction().y * c.z +
                src.direction().x * c.y * o.z +
                c.x * o.y * src.direction().z -
                c.x * src.direction().y * o.z -
                src.direction().x * o.y * c.z -
                o.x * c.y * src.direction().z;

        T s = o.x * l.direction().y * c.z +
                l.direction().x * c.y * o.z +
                c.x * o.y * l.direction().z -
                c.x * l.direction().y * o.z -
                l.direction().x * o.y * c.z -
                o.x * c.y * l.direction().z;

        t /= q;
        s /= q;

        v = src.position() + t * src.direction();
        if( (s >= 0.0) && (s <= 1.0) &&(t >= 0.0) && (t <= 1.0) )
            return true;
    }
    return false;
}

//!
//! ky_intersects 线与线相交
//! \param src
//! \param l 要检测的相交线
//! \param v 如果发生相交，则保存结果，否则保持不变
//! \return
//!
template <typename T>
inline bool ky_intersects(const ky_line3<T>& src, const ky_line3<T>& l, ky_vector3<T>& v)
{
    ky_vector3<T> c = ky_cross(src.direction(), l.direction());
    T q = ky_length_sq(c);

    if (ky_fabs(q) > 0.00001f)
    {
        ky_vector3<T> o = l.position() - src.position();
        ky_vector3<T> d = l.direction();

        T t = o.x * src.direction().y * c.z +
                src.direction().x * c.y * o.z +
                c.x * o.y * src.direction().z -
                c.x * src.direction().y * o.z -
                src.direction().x * o.y * c.z -
                o.x * c.y * src.direction().z;

        t /= q;
        v = src.position() + t * src.direction();

        return true;
    }
    return false;
}

template <typename T>
std::ostream& operator << (std::ostream& s, const ky_line3<T>& l) {
    return (s << "x = " << l.start() << " + t * " << l.direction());
}


template<typename T>
ky_streamb &operator << (ky_streamb &in, const ky_line3<T> &v)
{
    in << v.start();
    in << v.direction();
    return in;
}

template<typename T>
ky_streamb &operator >> (ky_streamb &out, ky_line3<T> &v)
{
    ky_vector3<T> tmp;
    out >> tmp; v.start(tmp);
    out >> tmp; v.direction(tmp);
    return out;
}
template<typename T>
ky_streamt &operator << (ky_streamt &in, const ky_line3<T> &v)
{
    in << v.start();
    in << v.direction();
    return in;
}
template<typename T>
ky_streamt &operator >> (ky_streamt &out, ky_line3<T> &v)
{
    ky_vector3<T> tmp;
    out >> tmp; v.start(tmp);
    out >> tmp; v.direction(tmp);
    return out;
}
#endif // ky_line3_H
