
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
 * @file     ky_matMxN.h
 * @brief    MxN矩阵类
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
#ifndef MATMXN_H
#define MATMXN_H
#include "ky_vec4.h"
#include "tools/ky_memory.h"
#include "ky_debug.h"

template<typename T>
struct ky_matrixMxN
{
    T *elem;
    int64 cols;
    int64 rows;

    ky_matrixMxN()
    {
        elem == NULL;
        cols = 0;
        rows = 0;
    }
    ky_matrixMxN(int64 m, int64 n)
    {
        elem == NULL;
        cols = 0;
        rows = 0;
        create(m,n);
    }
    ky_matrixMxN(const ky_matrixMxN<T> &mn)
    {
        elem == NULL;
        cols = 0;
        rows = 0;
        *this = mn;
    }
    template<typename U>
    ky_matrixMxN(const ky_matrixMxN<U> &mn) {
        elem == NULL;
        cols = 0;
        rows = 0;
        *this = mn;
    }
    ~ky_matrixMxN(){
        destroy();
    }

    inline int64 size(){return cols * rows;}
    inline int64 bytecount(){return cols * rows * sizeof(T);}
    inline void create(int64 m, int64 n)
    {
        cols = m;
        rows = n;
        elem = (T*)kyMalloc(sizeof(T) *m*n);
        ky_memory::zero(elem, sizeof(T) *m*n);
    }
    inline void destroy()
    {
        if (elem && cols)
            kyFree(elem);
        cols = 0;
        rows = 0;
        elem = NULL;
    }

    inline void resize(int64 m, int64 n)
    {
        destroy();
        create(m, n);
    }
    inline void clean()
    {
        ky_memory::zero (elem, sizeof(T) *cols*rows);
    }

    inline ky_matrixMxN<T> &operator =(const T** mn)
    {
        for (int64 i = 0; i < cols; ++i)
            for (int64 j = 0; j < rows; ++j)
                elem[i*rows+j] = mn[i][j];
        return *this;
    }
    inline ky_matrixMxN<T> &operator =(const ky_matrixMxN<T> &mn)
    {
        destroy();
        create(mn.cols, mn.rows);
        ky_memory::copy (elem, mn.elem, sizeof(T) *mn.cols*mn.rows);

        return *this;
    }
    template<typename U>
    inline ky_matrixMxN<T> &operator =(const ky_matrixMxN<U> &mn)
    {
        destroy();
        create(mn.cols, mn.rows);
        for (int64 i = 0; i < cols *rows; ++i)
                elem[i] = mn.elem[i];

        return *this;
    }

    inline T* operator [](int64 mi)
    {
        kyASSERT((mi >= cols), "i must be less than size");
        return &elem[mi*rows];
    }
    inline const T* operator [](int64 mi)const
    {
        kyASSERT((mi >= cols), "i must be less than size");
        return &elem[mi*rows];
    }
    inline T& pos(int64 mi, int64 ni)
    {
        kyASSERT((mi >= cols) && (ni >= rows), "i must be less than size");
        return elem[mi*rows+ni];
    }
    inline const T& pos(int64 mi, int64 ni)const
    {
        kyASSERT((mi >= cols) && (ni >= rows), "i must be less than size");
        return elem[mi*rows+ni];
    }
    inline T* pos(int64 mi)
    {
        return *this[mi];
    }
    inline const T* pos(int64 mi)const
    {
        return *this[mi];
    }

};

typedef ky_matrixMxN<float> ky_matMxNi;
typedef ky_matrixMxN<float> ky_matMxNf;
typedef ky_matrixMxN<real> ky_matMxNd;

typedef ky_matrixMxN<int>  imatMxN;
typedef ky_matrixMxN<float> fmatMxN;
typedef ky_matrixMxN<real> dmatMxN;

template<typename T>
ky_streamb &operator << (ky_streamb &in, const ky_matrixMxN<T> &v)
{
    in << v.cols;
    in << v.rows;
    in << ky_byte((uint8*)v.elem, sizeof(T)*v.cols*v.rows);
    return in;
}

template<typename T>
ky_streamb &operator >> (ky_streamb &out, ky_matrixMxN<T> &v)
{
    ky_byte tmp;
    out >> v.cols;
    out >> v.rows;
    out >> tmp;
    v.resize(v.cols, v.rows);
    ky_memory::copy(v.elem, tmp.data(), tmp.size());

    return out;
}
template<typename T>
ky_streamt &operator << (ky_streamt &in, const ky_matrixMxN<T> &v)
{
    in << v.cols;
    in << v.rows;
    in << '\n';
    for (int64 m = 0; m < v.cols; ++m)
    {
        for (int64 n = 0; n < v.rows; ++n)
            in <<  v[m][n];
        in << '\n';
    }
    return in;
}
template<typename T>
ky_streamt &operator >> (ky_streamt &out, ky_matrixMxN<T> &v)
{
    int64 col = 0, row = 0;
    char tm;
    out >> col >> row >> tm;
    v.resize(col, row);
    for (int64 m = 0; m < col; ++m)
    {
        for (int64 n = 0; n < row; ++n)
            out >> v[m][n];
        out >> tm;
    }
    return out;
}
#endif // MAT4X4_H
