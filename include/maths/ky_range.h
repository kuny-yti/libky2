
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
 * @file     ky_range.h
 * @brief    范围值类型
 *
 * @author   kunyang
 * @email    kunyang.yk@gmail.com
 * @version  1.1.0.1
 * @date     2019/04/16
 * @license  GNU General Public License (GPL)
 *
 * Change History :
 *    Date    |  Version  |  Author  |   Description
 * 2019/04/16 | 1.0.0.1   | kunyang  | 创建文件
 * 2019/05/03 | 1.0.0.1   | kunyang  | 加入双向运算
 */
#ifndef KY_RANGE_H
#define KY_RANGE_H

#include "ky_maths.h"
#include "tools/ky_stream.h"
#include "tools/ky_variant.h"

template<typename T>
struct ky_range_mem
{
    typedef T ElemType;

    enum
    {
        size = 3
    };

    union
    {
        struct {
            T r_b;  // begin value
            T r_s;  // step value
            T r_e;  // end value
        };
        T elem[size];
    };
};

template<typename T>
struct ky_range : ky_range_mem<T>
{
    typedef ky_range_mem<T> memory;

    //!
    //! \brief set 设置范围值
    //! \param b 起始值
    //! \param s 步进值
    //! \param e 结束值
    //! \return
    //!
    inline bool set (T b, T s, T e)
    {
        if (s > 0 && b >= e)
            return false;
            //throw std::logic_error("end must greater than begin.");
        else if (s < 0 && b <= e)
            return false;
            //throw std::logic_error("end must less than begin.");

        memory::elem[0] = b;
        memory::elem[1] = s;
        memory::elem[2] = e;
        return true;
    }

    //!
    //! \brief ky_range 构造范围值
    //! \param b 起始值
    //! \param s 步进值
    //! \param e 结束值
    //!
    ky_range(T b, T s, T e) {set (b, s, e);}

    template<typename U>
    ky_range(const ky_range<U>& v)
    {
        for (int64 i = 0; i < v.size; ++i)
            memory::elem[i] = T(v.elem[i]);
    }
    ky_range(const ky_range<T> & v)
    {
        for (int64 i = 0; i < v.size; ++i)
            memory::elem[i] = v.elem[i];
    }

    template<typename U>
    ky_range(const ky_range_mem<U>& v) {
        for (int64 i = 0; i < v.size; ++i)
            memory::elem[i] = T(v.elem[i]);
    }
    ky_range(const ky_range_mem<T> & v){
        for (int64 i = 0; i < v.size; ++i)
            memory::elem[i] = v.elem[i];
    }

    inline ky_range<T> &operator = (const ky_range<T> &v)
    {
        for (int64 i = 0; i < v.size; ++i)
            memory::elem[i] = v.elem[i];
        return *this;
    }

    //!
    //! \brief operator [] 返回s位置的值
    //! \param s
    //! \return
    //!
    inline T operator [] (int64 s)
    {
        return memory::r_b + s * memory::r_s;
    }

    //!
    //! \brief count 返回范围值可执行的步数
    //! \return
    //!
    inline uint32 count() const
    {
        T num = (memory::r_e - memory::r_b) / memory::r_s;
        if ((memory::r_b + num * memory::r_s) != memory::r_e)
            ++num;
        return num;
    }
    //!
    //! \brief is_valid 返回范围值是否有效
    //! \return
    //!
    inline bool is_valid() const
    {
        if (memory::r_s > 0 && memory::r_b >= memory::r_e)
            return false;
        else if (memory::r_s < 0 && memory::r_b <= memory::r_e)
            return false;
        else if (memory::r_s == 0)
            return false;

        return true;
    }

    struct iterator
    {
    public:
        iterator(ky_range<T>& r, int start) : step(start), range(r)
        {
            value = range.r_b + step * range.r_s;
        }

        inline T operator*() { return value; }

        inline iterator operator ++()
        {
            value += range.r_s;
            ++step;
            return *this;
        }
        inline iterator operator ++(int)
        {
            iterator tmp(*this);
            operator++();
            return tmp;
        }

        inline bool operator==(const iterator& other)
        {
            return step == other.step;
        }
        inline bool operator!=(const iterator& other)
        {
            return step != other.step;
        }

        inline iterator operator--()
        {
            value -= range.r_s;
            --step;
            return *this;
        }
        inline iterator operator --(int)
        {
            iterator tmp(*this);
            operator--();
            return tmp;
        }

        T            value;
        int          step;
        ky_range<T>& range;
    };

    //!
    //! \brief begin 迭代开始
    //! \return
    //!
    inline iterator begin(){return iterator(*this, 0);}
    //!
    //! \brief begin 迭代结束
    //! \return
    //!
    inline iterator end(){return iterator(*this, count());}

    static ky_range<T> zero;
};

template<typename T>
ky_range<T> ky_range<T>::zero = ky_range<T>(T(0), T(0), T(0));

typedef ky_range<int> ky_rangei;
typedef ky_range<float> ky_rangef;
typedef ky_range<real> ky_ranged;

const fourwd_t kyRangeCode = kyFourWord(rang);
template<typename T>
ky_variant &operator << (ky_variant &va, const ky_range<T> &col)
{
    va.set(kyRangeCode, 0, &col, sizeof(ky_range<T>));
    return va;
}
template<typename T>
ky_variant &operator >> (ky_variant &va, ky_range<T> &col)
{
    col = va.get<ky_range<T>>(kyRangeCode);
    return va;
}

#endif // KY_RANGE_H
