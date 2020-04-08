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
 * @file     ky_linked.h
 * @brief    模板链式链表数据空间容器类(支持C++11)
 *       ky_linked 类主要完成一个链式链表型模板类，内部使用ky_alloc来完成内存分配，
 * 有ky_ref完成对类的引用管理，具备共享后修改会进行单独复制使用。
 *
 * @author   kunyang
 * @email    kunyang.yk@gmail.com
 * @version  1.0.0.1
 * @date     2018/03/09
 * @license  GNU General Public License (GPL)
 *
 * Change History :
 *    Date    |  Version  |  Author  |   Description
 * 2018/03/09 | 1.0.0.1   | kunyang  | 创建文件
 */

#ifndef KY_LINKED_H
#define KY_LINKED_H

#include "ky_define.h"
#include "ky_algorlthm.h"
#include "arch/ky_memory.h"

#ifdef kyHasSTL
#include <list>
#endif

struct _ky_linked_ : public ky_ref
{
    _ky_linked_  *prev;
    _ky_linked_  *next;
    int64 count;

    static const _ky_linked_ shared_nul;

};

template <typename T, typename Alloc = ky_alloc<T> >
class ky_linked : public Alloc
{
public:
    class iterator
    {
        friend class ky_linked;
    public:

        iterator():ope(0){}
        iterator(list_node *p): ope(p){}
        iterator(const iterator&ope_n): ope(ope_n.ope){}
        ~iterator(){}

        T* operator ->(){return &ope->data;}
        T& operator *() {return ope->data;}
        const T& operator *() const{return ope->data;}

        bool operator ==(const iterator &rhs) const { return ope == rhs.ope; }
        bool operator !=(const iterator &rhs) const { return ope != rhs.ope; }
        bool operator <(const iterator& rhs) const { return ope < rhs.ope; }
        bool operator <=(const iterator& rhs) const { return ope <= rhs.ope; }
        bool operator >(const iterator& rhs) const { return ope > rhs.ope; }
        bool operator >=(const iterator& rhs) const { return ope >= rhs.ope; }

        iterator operator +(int c);
        const iterator operator +(int c)const;
        inline iterator operator -(int c){return operator+(-c);}
        const iterator operator -(int c)const{return operator+(-c);}

        int operator -(const iterator &rhs)const;

        const iterator& operator++()const;
        const iterator operator++(int)const;

        const iterator& operator--()const;
        const iterator operator--(int)const;

        iterator& operator++() ;
        iterator operator++(int);

        iterator& operator--();
        iterator operator--(int);

        iterator &operator +=(int c){return *this = *this + c; }
        const iterator &operator +=(int c)const{return *this = *this + c; }
        iterator &operator -=(int c){return *this = *this - c; }
        const iterator &operator -=(int c)const{return *this = *this - c; }

    private:
        list_node *ope;
    };
    typedef const iterator const_iterator;

    // STL
public:
    ky_linked();
    ky_linked(const ky_linked<T, Alloc> &rhs);
    ~ky_linked();

    //!
    //! \brief operator = 链表赋值
    //! \param rhs
    //! \return
    //!
    ky_linked& operator = (const ky_linked<T, Alloc> &rhs);

    bool empty() const {return is_empty();}
    int64 size() const {return count();}
    //!
    //! \brief swap 交换链表
    //! \param rhs
    //!
    void swap(ky_linked<T, Alloc> &rhs);
    friend void ky_swap(ky_linked<T, Alloc>  &a, ky_linked<T, Alloc>  &b){a.swap(b);}
    //!
    //! \brief sort 排序链表-未实现
    //!
    void sort();
    //!
    //! \brief reverse 逆向链表-未实现
    //!
    void reverse();

    //!
    //! \brief clear 清空链表
    //!
    void clear();

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
    T& front();
    const T& front() const;

    //!
    //! \brief back 尾部元素
    //! \return
    //!
    T& back();
    const T& back() const;

    //!
    //! \brief push_front 在头和尾添加一个元素
    //! \param x
    //!
    void push_front( const T &x ){prepend(x);}
    void push_back( const T &x ){append(x);}

    //!
    //! \brief pop_front 在头和尾删掉一个元素
    //!
    void pop_front();
    void pop_back();

#if kyLanguage < kyLanguage11
    void insert(iterator itr, int64 n, const T &x );
    void splice(iterator pos, ky_linked& x);
    void splice(iterator pos, ky_linked& x, iterator i);
    void splice(iterator pos, ky_linked& x, iterator first, iterator last);
    void merge(ky_linked& x);
    //!
    //! \brief insert 插入操作
    //! \param itr
    //! \param x
    //! \return
    //!
    iterator insert(iterator itr, const ky_linked<T, Alloc> &x );
    iterator insert(iterator itr, const T &x );
    iterator insert(iterator itr, const iterator &x );

    //!
    //! \brief erase 擦除操作
    //! \param itr
    //! \return
    //!
    iterator erase(iterator itr );
    iterator erase(iterator start, iterator end );
#endif
    //!
    //! \brief assign 赋值操作
    //! \param itr
    //! \param x
    //!
    void assign(iterator itr, const T &x );
    void assign(iterator itr, const iterator &x );
    void assign(int64, const T &x );

    //void remove(const T& val)
    //void unique()

    // C++11
#if kyLanguage >= kyLanguage11
public:
    explicit ky_linked(const ky_linked<T, Alloc> &&rhs);
    ky_linked(const std::initializer_list<T> & il);
    ky_linked<T, Alloc>& operator = (ky_linked<T, Alloc> &&rhs);

    void push_front( T &&x ){prepend(x);}
    void push_back( T &&x ){append(x);}

    void splice(const_iterator pos, ky_linked<T, Alloc>&& x);
    void splice(const_iterator pos, ky_linked<T, Alloc>& x);
    void splice(const_iterator pos, ky_linked<T, Alloc>&& x, const_iterator i);
    void splice(const_iterator pos, ky_linked<T, Alloc>& x, const_iterator i);
    void splice(const_iterator pos, ky_linked<T, Alloc>&& x, const_iterator first, const_iterator last);
    void splice(const_iterator pos, ky_linked<T, Alloc>& x, const_iterator first, const_iterator last);
    void merge(ky_linked<T, Alloc>&& x);
    //!
    //! \brief insert 插入操作
    //! \param itr
    //! \param x
    //! \return
    //!
    iterator insert(const_iterator itr, const ky_linked<T, Alloc> &x );
    iterator insert(const_iterator itr, const T &x );
    iterator insert(const_iterator itr, const iterator &x );
    iterator insert(const_iterator itr, T &&x );
    iterator insert(const_iterator itr, int64 n, const T &x );

    //!
    //! \brief erase 擦除操作
    //! \param itr
    //! \return
    //!
    iterator erase(const_iterator itr );
    iterator erase(const_iterator start, const_iterator end );
#endif

    // base
public:
    explicit ky_linked(const std::list<T> &rhs);
    //!
    //! \brief to_std
    //! \return 返回stl型链表
    //!
    std::list<T> to_std();
    //!
    //! \brief form stl型链表输入
    //! \param rhs
    //!
    void form(const std::list<T> &rhs);
    ky_linked<T, Alloc>& operator = (const std::list<T> &rhs);

    //!
    //! \brief operator += 链表追加
    //! \param rhs
    //! \return
    //!
    ky_linked<T, Alloc> &operator +=(const ky_linked<T, Alloc> &rhs);
    ky_linked<T, Alloc> &operator +=(const T &val);

    //!
    //! \brief operator + 链表相加
    //! \param rhs
    //! \return
    //!
    ky_linked<T, Alloc> operator +(const ky_linked<T, Alloc> &rhs) const;

    //!
    //! \brief operator << 链表添加
    //! \param val
    //! \return
    //!
    ky_linked<T, Alloc> &operator << (const T &val);
    ky_linked<T, Alloc> &operator << (const ky_linked<T, Alloc> &rhs);

    //!
    //! \brief operator 链表比对
    //! \param rhs
    //! \return
    //!
    bool operator ==(const ky_linked<T, Alloc> &rhs) const;
    bool operator !=(const ky_linked<T, Alloc> &rhs) const { return !(*this == rhs); }

    //!
    //! \brief is_empty 是否空链表
    //! \return
    //!
    bool is_empty() const;
    bool is_null() const;
    //!
    //! \brief count 链表长度
    //! \return
    //!
    int64 count() const;
    //!
    //! \brief prepend 前置添加元素
    //! \param val
    //!
    void prepend(const T& val);
    void prepend(const ky_linked<T, Alloc>& val) ;
    //!
    //! \brief append 后置添加元素
    //! \param val
    //!
    void append(const T& val) ;
    void append(const ky_linked& val);
    //!
    //! \brief insert 根据位置插入元素
    //! \param pos
    //! \param val
    //!
    iterator insert(int64 pos, const T& val) ;
    void insert(int64 pos, const ky_linked<T, Alloc>& val);

    //!
    //! \brief replace 替换操作
    //! \param i
    //! \param t
    //!
    void replace(int64 i, const T &t);
    //!
    //! \brief move 移动操作
    //! \param inx
    //! \param to
    //!
    void move(int64 inx, int64 to);

    //!
    //! \brief remove 根据位置删除元素
    //! \param pos
    //!
    void remove(int64 pos) ;
    void remove() {pop_back();}
    //!
    //! \brief swap 交换操作
    //! \param i
    //! \param j
    //!
    void swap(int64 i, int64 j);
    void swap(iterator x, iterator y);
    //!
    //! \brief contains 查找是否包含指定元素
    //! \param t
    //! \return
    //!
    bool contains(const T &t) const;
    int find(const T &t) const;

protected:
    struct list_node
    {
        list_node  *prev;
        list_node  *next;

        T        data;
    };

private:
    union {_ky_linked_ *impl; list_node *list;};

private:
    list_node *newnode(const T &);
    void freenode(list_node *n);
    void free(struct _ky_linked_ *n);
    void copy();
    void detach();

    _ky_linked_ *header() {return impl;}
    const _ky_linked_ *header()const {return impl;}
};


#include "ky_linked.inl"
#endif // KY_LINKED_H
