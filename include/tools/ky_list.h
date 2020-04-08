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
 * @file     ky_list.h
 * @brief    模板链表数据空间容器类(支持C++11)
 *       1.支持初始化列表初始化
 *       2.内部算法使用ky_memory::array_addr
 *       3.支持引用计数和写时拷贝
 *       4.采用连续地址指针模式实现快速链表遍历
 *
 * @author   kunyang
 * @email    kunyang.yk@gmail.com
 * @version  1.2.1.1
 * @date     2010/05/08
 * @license  GNU General Public License (GPL)
 *
 * Change History :
 *    Date    |  Version  |  Author  |   Description
 * 2010/06/01 | 1.0.0.1   | kunyang  | 创建文件
 * 2011/02/16 | 1.0.1.0   | kunyang  | 加入ky_alloc模板分配类
 * 2014/08/10 | 1.0.2.0   | kunyang  | 加入对象引用计数，同时加入C++11支持
 * 2015/03/06 | 1.0.2.2   | kunyang  | 修改将头信息和链表地址共用
 * 2017/06/12 | 1.1.0.1   | kunyang  | 将原有指针型链表修改为ky_linked类，采样连续地址指针模式
 * 2018/02/27 | 1.2.0.1   | kunyang  | 将连续地址指针思想重构并移入ky_memory::array_addr
 * 2018/03/18 | 1.2.1.1   | kunyang  | 将模板对象分为可构造来优化速度
 */

#ifndef KY_LIST
#define KY_LIST

#include "ky_define.h"
#include "ky_algorlthm.h"
#include "arch/ky_memory.h"
#include "ky_typeinfo.h"

#ifdef kyHasSTL
#include <list>
#endif

template <typename T>
class ky_list : protected ky_memory::dynaddr
{
    typedef T                  Type ;
    typedef ky_memory::dynaddr Layout;
    typedef Layout::header_t   LayoutHeader;
    enum
    {
        sizeOf = sizeof(Type),
        tComplex = is_complex<Type>::value || (sizeOf > Layout::NodeSize)
    };

    struct node_t : Layout::node_t
    {
        T &v()
        {
            if (kyUnLikely(tComplex))
                return *(Type*)(this);
            return *(Type*)(value);
        }
    };

    // base
public:
    ky_list();
    ky_list(const ky_list &rhs);
    virtual ~ky_list();

    //!
    //! \brief operator =
    //! \param rhs
    //! \return
    //!
    ky_list& operator = (const ky_list &rhs);

    //!
    //! \brief operator == 比较
    //! \param rhs
    //! \return
    //!
    bool operator ==(const ky_list &rhs) const;
    bool operator !=(const ky_list &rhs) const;

    //!
    //! \brief is_null 返回true则未储备空间
    //! \return
    //!
    inline bool is_nul()const {return Layout::is_nul();}

    //!
    //! \brief is_empty 返回true则为空
    //! \return
    //!
    inline bool is_empty()const{return Layout::is_empty ();}
    //!
    //! \brief count 返回元素数量
    //! \return
    //!
    inline int64 count()const{return Layout::count();}

    //!
    //! \brief prepend 前置添加元素
    //! \param val
    //!
    void prepend(const Type& val);
    //!
    //! \brief append 后置添加元素
    //! \param val
    //!
    void append(const Type& val) ;
    void append(const ky_list& val);

    //!
    //! \brief insert 根据位置插入元素
    //! \param pos
    //! \param val
    //!
    void insert(int pos, const Type& val) ;
    void insert(int pos, const ky_list& val);

    //!
    //! \brief replace 根据位置替换元素
    //! \param i
    //! \param val
    //!
    void replace(int i, const Type &val);
    //!
    //! \brief move 元素位置移动
    //! \param inx
    //! \param to
    //!
    void move(int inx, int to);

    //!
    //! \brief remove 根据位置删除元素
    //! \param pos
    //!
    void remove(int pos) ;

    //!
    //! \brief swap 交换元素
    //! \param i
    //! \param j
    //!
    void swap(int i, int j);

    //!
    //! \brief contains 查找是否包含指定元素
    //! \param t
    //! \return
    //!
    bool contains(const Type &t) const;

    //!
    //! \brief find 查找指定元素位置
    //! \param t
    //! \return
    //!
    int find(const Type &t) const;

    //!
    //! \brief first 第一个元素
    //! \return
    //!
    Type& first();
    const Type& first() const ;
    //!
    //! \brief last 最后一个元素
    //! \return
    //!
    Type& last();
    const Type& last() const;

    //!
    //! \brief value 取出指定位置值
    //! \param i
    //! \return
    //!
    T value(int i) const;
    //!
    //! \brief at 取出指定位置元素
    //! \param i
    //! \return
    //!
    const T &at(int i) const;
    //!
    //! \brief operator [] 重载下标访问
    //! \param i
    //! \return
    //!
    const Type &operator[](int i) const;
    T &operator[](int i);

    //!
    //! \brief clear 清空
    //!
    void clear();

    //!
    //! \brief swap 交换
    //! \param rhs
    //!
    void swap(ky_list &rhs);

    //!
    //! \brief reserve 储备空间
    //! \param s
    //!
    void reserve(int64 s);

    //!
    //! \brief operator += 重载追加
    //! \param rhs
    //! \return
    //!
    ky_list &operator +=(const ky_list &rhs);
    ky_list &operator +=(const Type &val);

    //!
    //! \brief operator + 重载相加
    //! \param rhs
    //! \return
    //!
    ky_list operator +(const ky_list &rhs) const;

    //!
    //! \brief operator << 重载添加
    //! \param val
    //! \return
    //!
    ky_list &operator << (const Type &val);
    ky_list &operator << (const ky_list<T> &rhs);

    //!
    //! \brief ky_swap 友元交换
    //! \param a
    //! \param b
    //!
    friend void ky_swap(ky_list &a, ky_list &b){a.swap(b);}

// STL
#ifdef kyHasSTL
    class const_iterator;
    class iterator
    {
        friend class ky_list;
    private:
        node_t *ope;

    public:
        inline iterator():ope(0){}
        inline iterator(node_t *n):ope(n){}

        inline T &operator *() const { return ope->v();}
        inline T *operator ->() const { return &ope->v();}
        inline T &operator [](int i) const { return ope[i].v(); }
        inline bool operator ==(const iterator &rhs)const{return ope == rhs.ope;}
        inline bool operator !=(const iterator &rhs)const{return ope != rhs.ope;}
        inline bool operator <(const iterator& rhs)const{return ope < rhs.ope;}
        inline bool operator <=(const iterator& rhs)const{return ope <= rhs.ope;}
        inline bool operator >(const iterator& rhs)const{return ope > rhs.ope;}
        inline bool operator>=(const iterator& rhs)const{return ope >= rhs.ope;}

        inline bool operator ==(const const_iterator &rhs)const
        {return ope == rhs.ope;}
        inline bool operator !=(const const_iterator &rhs)const
        {return ope != rhs.ope;}
        inline bool operator <(const const_iterator& rhs)const
        {return ope < rhs.ope;}
        inline bool operator <=(const const_iterator& rhs)const
        {return ope <= rhs.ope;}
        inline bool operator >(const const_iterator& rhs)const
        {return ope > rhs.ope;}
        inline bool operator>=(const const_iterator& rhs)const
        {return ope >= rhs.ope;}

        inline iterator &operator ++() {++ope; return *this;}
        inline iterator operator ++(int) {node_t *n = ope; ++ope; return n;}
        inline iterator &operator --() {ope--; return *this;}
        inline iterator operator --(int) {node_t *n = ope; ope--; return n;}
        inline iterator &operator +=(int i) { ope += i; return *this;}
        inline iterator &operator -=(int i) { ope -= i; return *this;}
        inline iterator operator +(int i) const { return iterator(ope +i);}
        inline iterator operator -(int i) const { return iterator(ope -i);}
        inline int operator -(iterator i) const { return int(ope - i.ope);}
    };
    friend class iterator;
    class const_iterator
    {
        friend class ky_list;
    private:
        node_t *ope;

    public:
        inline const_iterator(): ope(0){}
        inline const_iterator(node_t *n): ope(n){}

        inline const_iterator(const iterator &rhs): ope(rhs.ope){}
        inline const T &operator *()const{return ope->v();}
        inline const T *operator ->()const{return &ope->v();}
        inline const T &operator [](int i)const{return ope[i].v();}
        inline bool operator ==(const const_iterator &rhs)const
        {return ope == rhs.ope;}
        inline bool operator !=(const const_iterator &rhs)const
        {return ope != rhs.ope;}
        inline bool operator <(const const_iterator& rhs)const
        {return ope < rhs.ope;}
        inline bool operator <=(const const_iterator& rhs)const
        {return ope <= rhs.ope;}
        inline bool operator >(const const_iterator& rhs)const
        {return ope > rhs.ope;}
        inline bool operator >=(const const_iterator& rhs)const
        {return ope >= rhs.ope;}
        inline const_iterator &operator ++() {++ope; return *this;}
        inline const_iterator operator ++(int)
        {node_t *n = ope; ++ope; return n; }
        inline const_iterator &operator --() {--ope; return *this;}
        inline const_iterator operator --(int)
        {node_t *n = ope; --ope; return n;}
        inline const_iterator &operator +=(int i) {ope += i; return *this;}
        inline const_iterator &operator -=(int i) {ope -= i; return *this;}
        inline const_iterator operator +(int i) const
        {return const_iterator(ope + i);}
        inline const_iterator operator -(int i) const
        {return const_iterator(ope - i);}
        inline int operator -(const_iterator i) const
        {return int(ope - i.ope);}
    };
    friend class const_iterator;

public:
    //!
    //! \brief ky_list
    //! \param il
    //!
    ky_list(const std::initializer_list<Type> & il);

    //!
    //! \brief ky_list
    //! \param rhs
    //!
    explicit ky_list(const std::list<Type> &rhs);

    //!
    //! \brief form stl型链表输入
    //! \param rhs
    //!
    void form(const std::list<Type> &rhs);

    //!
    //! \brief to_std
    //! \return 返回stl型链表
    //!
    std::list<Type> to_std();

    //!
    //! \brief operator =
    //! \param rhs
    //! \return
    //!
    ky_list<Type>& operator = (const std::list<Type> &rhs);

    inline bool empty() const {return is_empty();}
    inline int64 size() const {return count();}

    //!
    //! \brief sort 排序链表-未实现
    //!
    void sort();
    //!
    //! \brief reverse 逆向链表-未实现
    //!
    void reverse();

    //!
    //! \brief begin 开始节点
    //! \return
    //!
    iterator begin();
    const_iterator begin() const;
    inline const_iterator cbegin() const{return begin();}

    //!
    //! \brief end 结束节点
    //! \return
    //!
    iterator end();
    const_iterator end() const;
    inline const_iterator cend() const{return end();}

    //!
    //! \brief front 头部元素
    //! \return
    //!
    inline Type& front(){return first();}
    inline const Type& front() const{return first();}

    //!
    //! \brief back 尾部元素
    //! \return
    //!
    inline Type& back(){return last();}
    inline const Type& back() const{return last();}

    //!
    //! \brief push_front 在头和尾添加元素
    //! \param x
    //!
    inline void push_front(const Type &x){prepend(x);}
    inline void push_back(const Type &x){append(x);}

    //!
    //! \brief pop_front 在头和尾删掉一个元素
    //!
    inline void pop_front(){remove(0);}
    inline void pop_back(){remove(size()-1);}
    //!
    //! \brief assign 赋值
    //! \param itr
    //! \param x
    //!
    void assign(int inx, const Type &x);

    //! 擦除
    iterator erase(const_iterator itr);
    iterator erase(const_iterator start, const_iterator end);
#endif

protected:

    void    __construct(node_t *n, const Type &p);
    void    __destruct(node_t *n);
    void    __destruct(node_t *from, node_t *to);
    void    __copy(node_t *from, node_t *to, node_t *src);
    void    __detach_helper();
    void    __destroy_helper();
    node_t* __detach_insert(int i, int c);
};


template <typename T>
void ky_list<T>::__construct(node_t *n, const Type &p)
{
    if (kyUnLikely (tComplex))
        new (n) Type(p);
    else
    {
        Type *vd = (Type*)kyMalloc(sizeOf);
        complex_construct(*vd, p);
        n->value = vd;
    }
}
template <typename T>
void ky_list<T>::__destruct(node_t *n)
{
    if (kyLikely(tComplex))
    {
        ((Type*)n->value)->~Type();
        kyFree(n->value);
    }
}
template <typename T>
void ky_list<T>::__copy(node_t *from, node_t *to, node_t *src)
{
    node_t *cur = from;
    while(cur != to)
    {
        __construct(cur, src->v ());
        ++cur;
        ++src;
    }
}
template <typename T>
void ky_list<T>::__destruct(node_t *from, node_t *to)
{
    while (from != to)
    {
        --to;
        __destruct (to);
    }
}
template <typename T>
void ky_list<T>::__destroy_helper()
{
    if (!is_nul () && lessref ())
    {
        __destruct ((node_t*)Layout::begin (), (node_t*)Layout::end ());
        Layout::destroy ();
    }
    header = nul();
}
template <typename T>
ky_list<T>::ky_list():
    Layout()
{
}

template <typename T>
ky_list<T>::ky_list(const ky_list &rhs):
    Layout()
{
    *this = rhs;
}
template <typename T>
ky_list<T>::~ky_list()
{
    __destroy_helper();
}

template <typename T>
ky_list<T>& ky_list<T>::operator = (const ky_list<T> &rhs)
{
    __destroy_helper();

    if (!rhs.is_null ())
    {
        Layout *x = (Layout*)&rhs;
        if (x->refer ().has_shareable ())
        {
            x->addref ();
            header = x->header;
        }
        else
        {
            node_t *n = __detach_insert (0, rhs.count ());
            __copy(n, (node_t *)Layout::end(),
                   (node_t *)rhs.Layout::begin());
        }
    }
    return *this;
}

template <typename T>
bool ky_list<T>::operator ==(const ky_list<T> &rhs) const
{
    if (header == rhs.header)
        return true;
    if (count() != rhs.count())
        return false;

    node_t *i = (node_t *)Layout::begin();
    node_t *e = (node_t *)Layout::end();
    node_t *ri = (node_t *)rhs.Layout::begin();
    for (; i != e; ++i, ++ri)
    {
        if (i->v() != ri->v())
            return false;
    }
    return true;
}
template <typename T>
bool ky_list<T>::operator !=(const ky_list<T> &rhs) const
{
    return !operator ==(rhs);
}

template <typename T>
void ky_list<T>::prepend(const T& val)
{
    if (Layout::refer ().is_shared ())
    {
        node_t *n = __detach_insert (0, 1);
        __construct(n, val);
    }
    else
    {
        node_t *n = (node_t *)Layout::prepend (1);
        __construct(n, val);
    }
}

template <typename T>
void ky_list<T>::append(const T& val)
{
    if (Layout::refer ().is_shared ())
    {
        node_t *n = __detach_insert(INT_MAX, 1);
        __construct(n, val);
    }
    else
    {
        node_t *n = (node_t*)Layout::append(1);
        __construct(n, val);
    }

}
template <typename T>
void ky_list<T>::append(const ky_list<T>& val)
{
    *this += val;
}

template <typename T>
void ky_list<T>::insert(int pos, const T& val)
{
    if (Layout::refer ().is_shared ())
    {
        node_t *n = __detach_insert(pos, 1);
        __construct(n, val);
    }
    else
    {
        node_t *n = (node_t*)Layout::insert(pos, 1);
        __construct(n, val);
    }
}

template <typename T>
void ky_list<T>::replace(int i, const T &val)
{
    if (i >=0 && i < count())
    {
        __detach_helper ();
        ((node_t*)Layout::at(i))->v() = val;
    }
}

template <typename T>
void ky_list<T>:: move(int inx, int to)
{
    if (inx >= 0 && inx < count() && to >= 0 && to < count())
    {
        __detach_helper ();
        Layout::move(inx, 1, to);
    }
}

template <typename T>
void ky_list<T>::remove(int pos)
{
    if (pos >= 0 && pos < (int)count ())
    {
        __detach_helper ();
        node_t *n = (node_t*)Layout::at(pos);
        __destruct(n);
        Layout::remove(pos, 1);
    }
}

template <typename T>
void ky_list<T>::swap(int i, int j)
{
    if (i >=0 && i < (int)count() && j >= 0 && j < (int)count())
    {
        __detach_helper ();
        node_t *ni = (node_t *)Layout::begin() + i;
        node_t *nj = (node_t *)Layout::begin() + j;
        node_t *tmp = ni;
        *ni = *nj;
        *nj = *tmp;
    }
}

template <typename T>
bool ky_list<T>::contains(const T &t) const
{
    return find(t) >= 0;
}

template <typename T>
int ky_list<T>::find(const T &t) const
{
    if (is_empty() || is_nul())
        return -1;

    node_t *b = (node_t *)Layout::begin();
    for (int i = 0; i < (int)count(); ++i)
    {
        if (b[i].v() == t)
            return i;
    }
    return -1;
}

template <typename T>
T& ky_list<T>::first()
{
    kyASSERT(!is_empty ());
    __detach_helper ();
    return ((node_t *)Layout::begin())->v();
}

template <typename T>
const T& ky_list<T>::first() const
{
    kyASSERT(!is_empty ());
    return ((node_t *)Layout::begin())->v();
}

template <typename T>
T& ky_list<T>::last()
{
    kyASSERT(!is_empty ());
    __detach_helper ();
    return ((node_t *)(Layout::end()-1))->v();
}

template <typename T>
const T& ky_list<T>::last() const
{
    kyASSERT(!is_empty ());
    return ((node_t *)(Layout::end()-1))->v();
}

template <typename T>
T ky_list<T>::value(int i) const
{
    if (i >= 0 && i < count ())
        return ((node_t *)Layout::at(i))->v();
    return T();
}

template <typename T>
const T &ky_list<T>::at(int i) const
{
    kyASSERT(!is_empty ());
    return ((node_t *)Layout::at(i))->v();
}

template <typename T>
const T &ky_list<T>::operator[](int i) const
{
    kyASSERT(!is_empty ());
    return ((node_t *)Layout::at(i))->v();
}

template <typename T>
T &ky_list<T>::operator[](int i)
{
    kyASSERT(!is_empty ());
    __detach_helper ();
    return ((node_t *)Layout::at(i))->v();
}

template <typename T>
void ky_list<T>::clear()
{
    if (!count ())
        return ;
    __detach_helper ();
    __destruct((node_t*)Layout::begin (), (node_t*)Layout::end ());
}

template <typename T>
void ky_list<T>::swap(ky_list<T> &rhs)
{
    ky_list<T> tmp(rhs);
    rhs = *this;
    *this = tmp;
}

template <typename T>
void ky_list<T>::reserve(int64 s)
{
    if (Layout::capacity () < s)
    {
        if (Layout::refer ().is_shared ())
            __detach_helper ();

        Layout::reserve(s);
    }
}

template <typename T>
void ky_list<T>::__detach_helper ()
{
    if (refer ().is_shared ())
    {
        if (!is_nul () && refer ().has_detach ())
            __detach_insert (0,0);
    }
}

template <typename T>
typename ky_list<T>::node_t *ky_list<T>::__detach_insert(int i, int c)
{
    const bool the_copy = !is_empty ();
    node_t *n = (node_t *)Layout::begin();
    ky_memory::dynaddr::header_t *oldh = Layout::detach(&i, c);

    if (the_copy)
    {
        __copy((node_t *)(Layout::begin()), (node_t *)(Layout::begin() + i), n);
        __copy((node_t *)(Layout::begin() + i + c), (node_t *)(Layout::end()),
                  n + i);
    }

    Layout old(oldh);
    if (!old.is_nul () && !old.refer ().lessref ())
    {
        __destruct((node_t*)old.begin(), (node_t*)old.end ());
        old.destroy ();
    }

    return (node_t *)(Layout::begin() + i);
}

template <typename T>
ky_list<T> &ky_list<T>::operator +=(const ky_list<T> &rhs)
{
    append(rhs);
    return *this;
}

template <typename T>
ky_list<T> &ky_list<T>::operator +=(const T &val)
{
    append(val);
    return *this;
}

template <typename T>
ky_list<T> ky_list<T>::operator +(const ky_list<T> &rhs) const
{
    ky_list<T> tmp(*this);
    tmp += rhs;
    return tmp;
}

template <typename T>
ky_list<T> &ky_list<T>::operator << (const T &val)
{
    return *this += val;
}

template <typename T>
ky_list<T> &ky_list<T>::operator << (const ky_list<T> &rhs)
{
    return *this += rhs;
}

#ifdef kyHasSTL
template <typename T>
ky_list<T>::ky_list(const std::initializer_list<T> & il):
    Layout()
{
    foreach (const T & a, il)
        append (a);
}

template <typename T>
ky_list<T>::ky_list(const std::list<T> &rhs):
    Layout()
{
    foreach (const T & a, rhs)
        append (a);
}

template <typename T>
void ky_list<T>::form(const std::list<T> &rhs)
{
    clear();
    foreach (const T & a, rhs)
        append (a);
}

template <typename T>
std::list<T> ky_list<T>::to_std()
{
    std::list<T> retlist;

    foreach (const iterator &itr , *this)
        retlist.push_back(*itr);
    return retlist;
}

template <typename T>
ky_list<T>& ky_list<T>::operator = (const std::list<T> &rhs)
{
    form (rhs);
}

template <typename T>
void ky_list<T>::sort()
{
    ky_qsort<iterator>(begin(), end());
}
template <typename T>
void ky_list<T>::reverse()
{

}

template <typename T>
typename ky_list<T>::iterator ky_list<T>::begin()
{
    return iterator((node_t*)Layout::begin());
}

template <typename T>
typename ky_list<T>::const_iterator ky_list<T>::begin() const
{
    return const_iterator((node_t*)Layout::begin());
}

template <typename T>
typename ky_list<T>::iterator ky_list<T>::end()
{
    return iterator((node_t*)Layout::end());
}

template <typename T>
typename ky_list<T>::const_iterator ky_list<T>::end() const
{
    return const_iterator((node_t*)Layout::end());
}

template <typename T>
void ky_list<T>::assign(int inx, const T &x)
{
    replace(inx, x);
}

template <typename T>
typename ky_list<T>::iterator ky_list<T>::erase(const_iterator itr)
{
    if (itr.ope >= Layout::begin() && itr.ope < Layout::end())
    {
        if (Layout::refer ().is_shared ())
        {
            int pos = itr - begin();
            __detach_helper ();
            itr = begin();
            itr += pos;
        }
        __destruct(itr.ope);
        return iterator((node_t*)Layout::erase (itr.ope));
    }
    return iterator();
}

template <typename T>
typename ky_list<T>::iterator ky_list<T>::erase(const_iterator start, const_iterator end)
{
    if ((start.ope >= Layout::begin() && start.ope < Layout::end()) &&
        (end.ope >= Layout::begin() && end.ope < Layout::end()))
    {
        if (Layout::refer().is_shared())
        {
            int offsetb = int(start.ope - (node_t*)(Layout::begin()));
            int offsete = int(end.ope - (node_t*)(Layout::begin()));
            __detach_helper ();
            start = begin() + offsetb;
            end = begin() + offsete;
        }

        __destruct(start.ope, end.ope);
        int pos = start - begin();
        Layout::remove(pos, end - start);
        return begin() + pos;
    }
    return iterator();
}

#endif


#include "ky_list.inl"

#endif // ky_LIST

