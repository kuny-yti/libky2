
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
 * @file     ky_line2.h
 * @brief    2D线段类
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
#ifndef ky_line2_H
#define ky_line2_H
#include "ky_vec2.h"

///This implements a 2D line(segment).
template <typename T>
class ky_line2 {
    ky_vector2<T> p_; /// position
    ky_vector2<T> d_; /// direction

public:
    /// trivial standard constructor
    ky_line2() {}

    /// construct a line from starting and end point
    ky_line2(const ky_vector2<T>& start, const ky_vector2<T>& end)
        : p_(start)
        , d_(end - start)
    {}
    ky_line2(const ky_line2<T> &v)
    {
        p_ = v.p_;
        d_ = v.d_;
    }
    inline ky_line2<T> &operator = (const ky_line2<T> &c)
    {
        p_ = c.p_;
        d_ = c.d_;
        return *this;
    }
    /**
        * Interpolates between start and end
        * @param t Should be betweeen 0 and 1 for line segments
        * @return (start*(1-t) + end*t)
        */
    inline ky_vector2<T> interpolates(T t) {
        return p_ + t * d_;
    }

    inline ky_vector2<T> start() const { return p_; }
    inline ky_vector2<T> end() const { return p_ + d_; }
    ///Set startingpoint to start, leaves end unchanged.
    inline void start(const ky_vector2<T>& s) { d_ = (p_ + d_) - s; p_ = s; }
    ///Set endpoint to end, leaves start unchanged.
    inline void end(const ky_vector2<T>& e) { d_ = (e - p_); }

    ///@return Vector from start to end
    inline ky_vector2<T> direction() const { return d_; }
    ///Set direction, changing endpoint, leaves start unchanged.
    inline void direction(const ky_vector2<T>& dir) { d_ = dir; }

    /**
        * intersect a linesegment with another linesegment
        * See RTR p617.
        *
        * @param l the line to intersect this object with
        * @param v holds the result if an intersection took place,
        * otherwise it remains unchanged
        * @return true if these lines intersect, false otherwise
        */
    inline bool intersectSegments(const ky_line2<T>& l, ky_vector2<T>& v) const;

    /**
        * intersect a line with another line
        *
        * @param l the line to intersect this object with
        * @param v holds the result if an intersection took place,
        * otherwise it remains unchanged
        * @return true if these lines intersect, false otherwise
        */
    inline bool intersect(const ky_line2<T>& l, ky_vector2<T>& v) const;
};

// non inline implementation

template <typename T>
bool ky_line2<T>::intersectSegments(const ky_line2<T>& l, ky_vector2<T>& v) const {
    T f = dot(l.d_, ky_vector2<T>(-d_.y, d_.x));

    T e = dot((p_-l.p_), ky_vector2<T>(-d_.y, d_.x));
    T d = dot((p_-l.p_), ky_vector2<T>(-l.d_.y, l.d_.x));

    //     std::cout << "d: " << d << " e: "<< e << " f: " << f << "\n";

    //test if s and t are in range [0, 1]:
    if(f > 0.0) {
        if((d < 0.0) || (d > f))
            return false;
    }
    else {
        if((d > 0.0) || (d < f))
            return false;
    }

    if(f > 0.0) {
        if((e < 0.0) || (e > f))
            return false;
    }
    else {
        if((e > 0.0) || (e < f))
            return false;
    }

    //     T t = e/f;
    T s = d/f;
    //     std::cout << "s: " << s << " t: "<< t << "\n";

    //     v = l.p_ + t * l.d_;
    //     std::cout << "v: " << v << "\n";
    v = p_ + (s * d_);
    //     std::cout << "v: " << v << "\n";
    return true;
}


template <typename T>
bool ky_line2<T>::intersect(const ky_line2<T>& l, ky_vector2<T>& v) const {
    // check whether these lines interesect
    T q = d_.y * l.d_.x - d_.x * l.d_.y;

    if (q == 0.f)
        return false; // these lines a collinear or stack

    // calculate paramter for this line where the lines intersect
    T t = -( l.d_.x * (p_.y - l.p_.y) + l.d_.y * l.p_.x - l.d_.y * p_.x ) / q;
    v = p_ + t * d_;

    return true;
}

template <typename T>
std::ostream& operator << (std::ostream& s, const ky_line2<T>& l) {
    return (s << "x = " << l.start() << " + t * " << l.direction());
}

typedef ky_line2<int>     iline2;
typedef ky_line2<float>   fline2;
typedef ky_line2<real>    dline2;


template<typename T>
ky_streamb &operator << (ky_streamb &in, const ky_line2<T> &v)
{
    in << v.start();
    in << v.direction();
    return in;
}

template<typename T>
ky_streamb &operator >> (ky_streamb &out, ky_line2<T> &v)
{
    ky_vector2<T> tmp;
    out >> tmp; v.start(tmp);
    out >> tmp; v.direction(tmp);
    return out;
}

template<typename T>
ky_streamt &operator << (ky_streamt &in, const ky_line2<T> &v)
{
    in << v.start();
    in << v.direction();
    return in;
}
template<typename T>
ky_streamt &operator >> (ky_streamt &out, ky_line2<T> &v)
{
    ky_vector2<T> tmp;
    out >> tmp; v.start(tmp);
    out >> tmp; v.direction(tmp);
    return out;
}
#endif // ky_line2_H
