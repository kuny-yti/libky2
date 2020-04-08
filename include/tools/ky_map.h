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
 * @file     ky_map.h
 * @brief    模板数组类(支持C++11)
 *       ky_array 类主要完成一个连续性地址的模板数组管理，内部使用ky_alloc来完成内存分配，
 * 有ky_ref完成对类的引用管理，具备共享后修改时会进行单独复制使用。
 *
 * @author   kunyang
 * @email    kunyang.yk@gmail.com
 * @version  1.0.2.3
 * @date     2010/05/02
 * @license  GNU General Public License (GPL)
 *
 * Change History :
 *    Date    |  Version  |  Author  |   Description
 * 2010/05/02 | 1.0.0.1   | kunyang  | 创建文件
 * 2011/02/12 | 1.0.1.0   | kunyang  | 加入ky_alloc模板分配类
 * 2014/08/10 | 1.0.2.0   | kunyang  | 加入对象引用计数，同时加入C++11支持
 * 2015/03/06 | 1.0.2.2   | kunyang  | 修改将头信息和实际类数据分开
 * 2016/06/29 | 1.0.2.3   | kunyang  | 修改引用计数的可复制对象
 */
#ifndef ky_MAP
#define ky_MAP

#include "ky_define.h"
#include "ky_list.h"
#include "ky_rbtree.h"

#ifdef kyHasSTL
#include <map>
#endif

struct _map_header : protected rb_tree, public ky_ref
{
    i64   count;

    static const _map_header *shared_nul;
};

template <typename K, typename V, typename Alloc = ky_alloc<void>>
class ky_map : Alloc
{
public:
    enum
    {
        kComplex = is_complex<K>::value,
        vComplex = is_complex<V>::value,
    };
    typedef K key_t;
    typedef V value_t;

    class node : protected rb_node
    {
    public:
        key_t   key;
        value_t value;

    public:
        inline node *left() const;
        inline node *right() const;
        inline node *next();
        inline node *prev();

        inline void destroy();
        virtual char compare(rb_node *rhs) const;

    protected:
        inline void destroy_child(false_type_t);
        inline void destroy_child(true_type_t);
    };

    class iterator
    {
        friend class ky_map;
    private:
        node *x;

    public:
        iterator();
        iterator(const node *p);
        iterator(const iterator &n);
        ~iterator();
        iterator &operator=(const iterator &n) const ;

        const K &key() const;
        V &value() const;
        V &operator*() const ;
        V *operator->() const;
        bool operator==(const iterator &o) const;
        bool operator!=(const iterator &o) const ;
        bool operator<(const iterator& rhs) const;
        bool operator<=(const iterator& rhs) const;
        bool operator>(const iterator& rhs) const;
        bool operator>=(const iterator& rhs) const;

        int operator -(const iterator &rhs)const;

        iterator operator +(int c);
        const iterator operator +(int c)const;
        iterator operator -(int c) ;
        const iterator operator -(int c)const;

        const iterator& operator++()const;
        const iterator operator++(int)const;

        const iterator& operator--()const;
        const iterator operator--(int)const;

        iterator& operator++();
        iterator operator++(int);

        iterator& operator--();
        iterator operator--(int);

        iterator &operator +=(int c);
        const iterator &operator +=(int c)const;
        iterator &operator -=(int c);
        const iterator &operator -=(int c)const;
    };
    typedef const iterator const_iterator;

    /// STL style
public:
    ky_map();
    ky_map(const ky_map<K, V, Alloc> &m);
    ~ky_map();

    //!
    //! \brief begin 返回开始迭代和结束迭代
    //! \return
    //!
    iterator begin();
    const_iterator begin() const;
    iterator end();
    const_iterator end() const ;

    //!
    //! \brief operator = 默认赋值
    //! \param rhs
    //! \return
    //!
    ky_map<K, V, Alloc> &operator = (const ky_map<K, V, Alloc> &rhs);

    //!
    //! \brief at 返回key的值
    //! \param key
    //! \return
    //!
    V &at(const K &key);
    const V &at(const K &key)const;
    V &operator [](const K &key);

    //!
    //! \brief insert 插入key并返回迭代器
    //! \param key
    //! \return
    //!
    iterator insert(const K& key);
    //!
    //! \brief insert 插入key，val并返回迭代器
    //! \param key
    //! \param val
    //! \return
    //!
    iterator insert(const K& key, const V& val);

    //!
    //! \brief swap 交换两个map
    //! \param rhs
    //!
    void swap(ky_map<K, V, Alloc> &rhs);
    friend void ky_swap(ky_map<K, V, Alloc> &a, ky_map<K, V, Alloc> &b) {a.swap(b);}
    //!
    //! \brief clear 清空map
    //!
    void clear();
    //!
    //! \brief empty 是否为空map
    //! \return
    //!
    bool empty() const;
    //!
    //! \brief size map的元素数
    //! \return
    //!
    int64 size() const;
    //!
    //! \brief find 查找第一个key出现的迭代器
    //! \param key
    //! \return
    //!
    iterator find(const K &key);

    /// C++ 11
#if kyLanguage >= kyLanguage11
public:
    ky_map(ky_map<K, V, Alloc> &&m);
    const_iterator cbegin() const;
    const_iterator cend() const;

    V &operator []( K &&key);
    iterator erase(const_iterator pos);
    int64 erase(const K& key);
    iterator erase(const_iterator first, const_iterator last);

    const_iterator find(const K &key) const;

#  ifdef kyHasSTL
    //!
    //! \brief 和stl模板进行转换
    //!
    explicit ky_map(const std::map<K, V> &m);
    ky_map(std::initializer_list<std::pair<K,V> > list);
    std::map<K, V> &to_std();
    void form(std::map<K, V> &m);
#  endif
#endif

public :
    //!
    //! \brief operator []
    //! \param key
    //! \return 返回指定key的值
    //!
    const V operator [](const K &key)const;
    //!
    //! \brief operator ==
    //! \param rhs
    //! \return 返回两个map是否相等
    //!
    bool operator==(const ky_map<K, V, Alloc> &rhs) const;
    //!
    //! \brief operator !=
    //! \param rhs
    //! \return 返回两个map是否不相等
    //!
    bool operator!=(const ky_map<K, V, Alloc> &rhs) const { return !(*this == rhs); }

    //!
    //! \brief value
    //! \param key
    //! \return 返回指定key的值
    //!
    V value(const K& key);

    //!
    //! \brief append 添加一个关联
    //! \param key
    //! \param val
    //!
    void append(const K& key, const V& val);
    //!
    //! \brief remove 删除指定key
    //! \param key
    //!
    void remove(const K& key);
    //!
    //! \brief remove 删除全部关联
    //!
    void remove();

    //!
    //! \brief contains
    //! \param key
    //! \return 是否包括key
    //!
    bool contains(const K &key) const;
    //!
    //! \brief key
    //! \param value
    //! \return 返回指定值的第一个key
    //!
    const K key(const V &value) const;
    //!
    //! \brief value
    //! \param key
    //! \return 返回指定key的值
    //!
    const V value(const K &key) const;
    //!
    //! \brief keys
    //! \return 返回map的全部或指定值的key
    //!
    ky_list<K> keys() const;
    ky_list<K> keys(const V &value) const;
    //!
    //! \brief values
    //! \return 返回map的全部或指定key的值
    //!
    ky_list<V> values() const;
    ky_list<V> values(const K &key) const;

    //!
    //! \brief first
    //! \return 返回第一个的值
    //!
    V &first() ;
    const V &first()const;
    //!
    //! \brief last
    //! \return 返回最后一个的值
    //!
    V &last();
    const V &last()const;

    //!
    //! \brief count
    //! \return 返回关联数
    //!
    int64 count()const;
    //!
    //! \brief is_empty
    //! \return 返回是否为空
    //!
    bool is_empty()const ;
    bool is_nul()const ;

protected:
    void detach();

    node *create_node(false_type_t, const K &k, const V &v);
    node *create_node(true_type_t, const K &k, const V &v);
    void destory_node(false_type_t, node *n);
    void destory_node(true_type_t, node *n);

    _map_header *create_head();
    void destory_head(_map_header* x);

private:
    void copy();

private:
    _map_header *impl;
};

#include "ky_map.inl"

#endif // ky_MAP

