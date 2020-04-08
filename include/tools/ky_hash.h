
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
 * @file     ky_hash.h
 * @brief    哈希桶模式实现的关联容器
 *
 * @author   kunyang
 * @email    kunyang.yk@gmail.com
 * @version  1.0.0.1
 * @date     2016/11/10
 * @license  GNU General Public License (GPL)
 *
 * Change History :
 *    Date    |  Version  |  Author  |   Description
 * 2016/11/10 | 1.0.0.1   | kunyang  | 创建文件
 *
 */
#ifndef ky_hash_H
#define ky_hash_H

#include "ky_define.h"
#include "tools/ky_typeinfo.h"
#include "tools/ky_algorlthm.h"
#include "tools/ky_list.h"
#include "arch/ky_memory.h"

struct _hash_header : public ky_ref
{
    i64 bcapacity;           ///< 桶储备空间
    i64 ecount;              ///< 桶内元素数
    i64 ecountmax;           ///< 桶能承受的最大元素数

    static f32 sMaxUsageRate;  ///< 桶的最大使用率
    static f32 sGrowRate;      ///< 桶的增长率

    static const _hash_header shared_nul;
};

template <typename K, typename V, typename Alloc = ky_alloc<void> >
class ky_hash : public Alloc
{
public:
    enum
    {
        kComplex = is_complex<K>::value,
        vComplex = is_complex<V>::value,
    };
    //!
    //! \brief The entry struct
    //!
    struct entry
    {
        //!
        //! \brief entry
        //! \param k
        //!
        explicit entry(const K& k):_key(k){}
        //!
        //! \brief entry
        //! \param k
        //! \param v
        //!
        entry(const K& k,const V& v):_key(k), _value(v){}

        //!
        //! \brief key
        //! \return
        //!
        const K& key() const {return _key;}
        //!
        //! \brief value
        //! \return
        //!
        const V& value() const {return _value;}
        //!
        //! \brief value
        //! \return
        //!
        V& value()  {return _value;}
        //!
        //! \brief operator =
        //! \param nv
        //! \return
        //!
        entry& operator =(const V& nv){_value = nv; return *this;}

        K _key;
        V _value;
    };

    //!
    //! \brief The iterator class
    //!
    class iterator
    {
        friend class ky_hash;
    private:
        ky_hash*      table;
        i64           index;
        bucket::item* item;

    public:
        iterator():table(0), bindex(0), bitem(0){}

    private:
        iterator(ky_hash* tab):
            table(tab),
            index(0),
            item(table->b[index].first)
        {
            while(!item)
            {
                ++index;
                if(index == table->capacity ())
                    break;
                item = table->b[index].first;
            }
        }
        iterator(ky_hash* tab, i64 index, bucket::item* item):
            table(tab),
            index(index),
            item(item)
        {

        }

    public:
        bool is_finished(void) const
        {
            return index >= table->capacity ();
        }
        friend bool operator == (const iterator& it1,const iterator& it2)
        {
            return it1.item == it2.item;
        }
        friend bool operator!=(const iterator& it1,const iterator& it2)
        {
            return it1.item != it2.item;
        }
        entry& operator*(void) const
        {
            return *item;
        }
        entry* operator->(void) const
        {
            return item;
        }
        iterator& operator++(void)
        {
            item = item->next;
            while (!item )
            {
                ++index;
                if(index == table->capacity ())
                    break;
                item = table->b[index].first;
            }
            return *this;
        }
    };
    typedef const iterator const_iterator;

private:
    //!
    //! \brief The bucket struct 桶类
    //!
    struct bucket
    {
        //!
        //! \brief The item struct 桶的节点
        //!
        struct item : public entry
        {
            item* next;

            //!
            //! \brief item
            //! \param x
            //!
            item (const entry& x):entry(x), next(0){}
            //!
            //! \brief operator =
            //! \param x
            //! \return
            //!
            item& operator =(const entry& x){entry::operator = (x); return *this;}
        };

        item* first;
    };

    union {
        _hash_header *h;
        bucket       *b;
    };

protected:
    void grow(i64 newsize);
    void detach();

    /// STL style
public:
    ky_hash();
    ky_hash(const ky_hash & rhs);
    ~ky_hash();

    iterator begin();
    const_iterator begin() const;
    iterator end(void);
    const_iterator end(void) const;

#if kyLanguage < kyLanguage11
    //!
    //! \brief erase 擦除指定pos的元素
    //! \param pos
    //!
    void erase(iterator pos)

    //!
    //! \brief erase 擦除从first开始到last之间的元素
    //! \param first
    //! \param last
    //!
    void erase(iterator first, iterator last);

    //!
    //! \brief insert 插入key，val
    //! \param key
    //! \param val
    //!
    void insert(const K& key, const V& val);
#endif

    //!
    //! \brief find 查找key并返回一个迭代器
    //! \param key
    //! \return
    //!
    const_iterator find(const K &key) const;
    //!
    //! \brief operator = 默认赋值
    //! \param rhs
    //! \return
    //!
    ky_hash<K, V, Alloc> &operator = (const ky_hash<K, V, Alloc> &rhs);
    //!
    //! \brief at 返回key的值
    //! \param key
    //! \return
    //!
    V &at(const K &key);
    const V &at(const K &key)const;
    //!
    //! \brief operator []
    //! \param key
    //! \return 返回指定key的值
    //!
    const V &operator [](const K &key)const;
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
    void swap(ky_hash<K, V, Alloc> &rhs);
    friend void ky_swap (ky_hash<K, V, Alloc> &a, ky_hash<K, V, Alloc> &b){a.swap (b);}
    //!
    //! \brief clear 清空map
    //!
    void clear();
    //!
    //! \brief empty 是否为空map
    //! \return
    //!
    bool empty() const{return is_empty ();}
    //!
    //! \brief size map的元素数
    //! \return
    //!
    i64 size() const{return count();}
    //!
    //! \brief find 查找第一个key出现的迭代器
    //! \param key
    //! \return
    //!
    iterator find(const K &key);

    void remove(const iterator& it);

    /// C++ 11
    #if kyLanguage >= kyLanguage11
public:
    ky_hash(ky_hash<K, V, Alloc> &&m);
    //ky_hash<K, V, Alloc>& operator = (ky_hash<K, V, Alloc> &&m) ;
    const_iterator cbegin() const;
    const_iterator cend() const;

    V &operator []( K &&key);
    iterator erase(const_iterator pos);
    iterator erase(iterator pos);
    i64 erase(const K& key);
    iterator erase(const_iterator first, const_iterator last);

    #endif

    // base
public:
    explicit ky_hash(i64 capacity);

    void append(const K& k, const V& v);
    void append(const entry& x);
    bool contains(const K& k) const;
    bool contains(const entry& x) const;
    const entry& search(const K& k) const;
    entry& search(const K& k);

    //!
    //! \brief value
    //! \param key
    //! \return 返回指定key的值
    //!
    V value(const K& key);
    //!
    //! \brief remove 删除指定key ok
    //! \param key
    //!
    void remove(const K& key);
    //!
    //! \brief value
    //! \param key
    //! \return 返回指定key的值
    //!
    const V value(const K &key) const;

    //!
    //! \brief reserve 分配储备空间 ok
    //! \param size
    //!
    void reserve(i64 size);
    //!
    //! \brief values
    //! \return 返回map的全部或指定key的值
    //!
    ky_list<V> values(const K &key) const;

    bool is_empty() const;
    bool is_nul()const;

    i64 capacity()const;
    i64 count()const;

private:
    bucket::item *create_item(false_type_t, const K &k, const V &v)
    {
        bucket::item *n = Alloc::alloc (sizeof(bucket::item));
        n->next = 0;
        n->_key = k;
        n->_value = v;
        return n;
    }
    bucket::item *create_item(true_type_t, const K &k, const V &v)
    {
        bucket::item *n = Alloc::alloc (sizeof(bucket::item));
        n->next = 0;
        new (n->_key) K(k);
        new (n->_value) V(v);
        return n;
    }
    void destory_item(false_type_t, bucket::item *n)
    {
        if (n->next)
            destory_item(const_int<bool, kComplex | vComplex>::value, n->next);

        Alloc::destroy(n);
    }
    void destory_item(true_type_t, bucket::item *n)
    {
        if (n->next)
            destory_item(const_int<bool, kComplex | vComplex>::value, n->next);

        n->_key.~K();
        n->_value.~V();
        Alloc::destroy(n);
    }

    bucket *create(i64 mins = 4);
    void destroy(bucket *b);

};

template <typename K, typename V, typename Alloc>
ky_hash<K, V, Alloc>::ky_hash():
    b(_hash_header::shared_nul)
{

}
template <typename K, typename V, typename Alloc>
ky_hash<K, V, Alloc>::ky_hash(const ky_hash & rhs):
    b(rhs.b)
{
    if (!is_nul ())
       (h-1)->addref ();
}
template <typename K, typename V, typename Alloc>
ky_hash<K, V, Alloc>::~ky_hash()
{
    ky_hash::destroy(b);
}

#if kyLanguage >= kyLanguage11
template <typename K, typename V, typename Alloc>
ky_hash<K, V, Alloc>::ky_hash(ky_hash<K, V, Alloc> &&m)
{
    if (b == m.b)
        return;

    if (!is_nul() && (h-1)->lessref())
        destroy(b);

    b = m.b;
    m.b = _hash_header::shared_nul;
}
//ky_hash<K, V, Alloc>& operator = (ky_hash<K, V, Alloc> &&m) ;
template <typename K, typename V, typename Alloc>
ky_hash<K, V, Alloc>::const_iterator ky_hash<K, V, Alloc>::cbegin() const
{
    return begin();
}
ky_hash<K, V, Alloc>::const_iterator ky_hash<K, V, Alloc>::cend() const
{
    return end();
}
#endif
template <typename K, typename V, typename Alloc>
ky_hash<K, V, Alloc>::ky_hash(i64 capacity):
    b(create(capacity))
{
}
template <typename K, typename V, typename Alloc>
bool ky_hash<K, V, Alloc>::is_empty() const
{
    return (h-1)->ecount;
}
template <typename K, typename V, typename Alloc>
bool ky_hash<K, V, Alloc>::is_nul()const
{
    return _hash_header::shared_nul == b;
}

template <typename K, typename V, typename Alloc>
i64 ky_hash<K, V, Alloc>::capacity()const
{
    return (h-1)->bcapacity;
}
template <typename K, typename V, typename Alloc>
i64 ky_hash<K, V, Alloc>::count()const
{
    return (h-1)->ecount;
}

template <typename K, typename V, typename Alloc>
void ky_hash<K, V, Alloc>::swap(ky_hash<K, V, Alloc> &rhs)
{
    if (rhs.b != b)
        ky_swap (rhs.b, b);
}

template <typename K, typename V, typename Alloc>
typename ky_hash<K, V, Alloc>::iterator ky_hash<K, V, Alloc>::insert(const K& key)
{
    return insert(key, V());
}

template <typename K, typename V, typename Alloc>
#if kyLanguage >= kyLanguage11
typename ky_hash<K, V, Alloc>::iterator ky_hash<K, V, Alloc>::insert(const K& k, const V& val)
#else
void ky_hash<K, V, Alloc>::insert(const K& k, const V& val);
#endif
{
    detach ();
    const i64 index = ky_hash_f(k) % capacity ();

    bucket::item* pred = 0;
    bucket::item* item = b[index].first;
    while (item && (item->key() != k))
    {
        pred = item;
        item = item->next;
    }

    if (!item)
    {
        item = create_item(const_int<bool, kComplex | vComplex>::value, k, val);

        if (pred)
            pred->next = item;
        else
            b[index].first = item;
        ++((h-1)->ecount);

        if((h-1)->ecount > (h-1)->ecountmax)
            grow((i64)(capacity () * _hash_header::sGrowRate)+1);
    }

#if kyLanguage >= kyLanguage11
    return iterator(this, index, item);
#endif
}

template <typename K, typename V, typename Alloc>
V &ky_hash<K, V, Alloc>::at(const K &key)
{
    return this->operator [] (key);
}
template <typename K, typename V, typename Alloc>
const V &ky_hash<K, V, Alloc>::at(const K &key)const
{
    return this->operator [] (key);
}

template <typename K, typename V, typename Alloc>
V& ky_hash<K, V, Alloc>::operator[](const K& k)
{
    detach ();
    const i64 index = ky_hash_f(k) % capacity ();

    bucket::item* pred = 0;
    bucket::item* item = b[index].first;
    while (item && (item->key() != k))
    {
        pred = item;
        item = item->next;
    }

    if (!item)
    {
        item = create_item(const_int<bool, kComplex | vComplex>::value, k, V());

        if (pred)
            pred->next = item;
        else
            b[index].first = item;
        ++((h-1)->ecount);

        if((h-1)->ecount > (h-1)->ecountmax)
            grow((i64)(capacity () * _hash_header::sGrowRate)+1);
    }

    return item->value ();
}

template <typename K, typename V, typename Alloc>
const V& ky_hash<K, V, Alloc>::operator[](const K& k) const
{
    return search(k).value ();
}

template <typename K, typename V, typename Alloc>
ky_hash<K, V, Alloc> &ky_hash<K, V, Alloc>::operator = (const ky_hash<K, V, Alloc> &rhs)
{
    if (!is_nul() && (h-1)->lessref ())
        destroy (b);

    b = rhs.b;
    if (!is_nul ())
        (h-1)->addref ();
}

template <typename K, typename V, typename Alloc>
void ky_hash<K, V, Alloc>::reserve(i64 size)
{
    if (!is_nul() && (h-1)->lessref())
        destroy(b);
    detach();
    grow(size);
}
template <typename K, typename V, typename Alloc >
ky_list<V> ky_hash<K, V, Alloc>::values(const K &k) const
{
    ky_list<V> ov;
    if (is_nul ())
        return ov;
    const i64 index = ky_hash_f(k) % capacity ();

    const bucket::item* item = b[index].first;
    while(item)
    {
        if (item->key () == k)
            ov.append (item->value ());
        item = item->next;
    }

    return ov;
}
template <typename K, typename V, typename Alloc >
void ky_hash<K, V, Alloc>::append(const K& k, const V& v){append(entry(k, v));}

template <typename K, typename V, typename Alloc>
void ky_hash<K, V, Alloc>::append(const entry& x)
{
    detach ();
    const i64 index = ky_hash_f(x.key ()) % capacity ();

    bucket::item* pred=0;
    bucket::item* item = b[index].first;
    while(item && (item->key () != x.key ()))
    {
        pred = item;
        item = item->next;
    }

    if(item)
        *item == (x);
    else
    {
        bucket::item* nitem = create_item(const_int<bool, kComplex | vComplex>::value,
                                          x.key (), x.value ());

        if(pred)
            pred->next = nitem;
        else
            b[index].first = nitem;
        ++(h-1)->ecount;

        if((h-1)->ecount > (h-1)->ecountmax)
            grow((i64)(capacity () * _hash_header::sGrowRate) + 1);
    }
}

template <typename K, typename V, typename Alloc>
void ky_hash<K, V, Alloc>::remove(const K& k)
{
    detach ();
    const i64 index = ky_hash_f(k) % capacity ();

    bucket::item* pred=0;
    bucket::item* item = b[index].first;
    while (item && (item->key ()!= k))
    {
        pred = item;
        item = item->next;
    }

    if(item)
    {
        if(pred)
            pred->next = item->next;
        else
            b[index].first = item->next;

        destory_item(const_int<bool, kComplex | vComplex>::value, item);
        --((h -1)->ecount);
    }
}

template <typename K, typename V, typename Alloc>
bool ky_hash<K, V, Alloc>::contains(const K& k) const
{
    if (is_nul())
        return false;
    const i64 index = ky_hash_f(k) % capacity ();

    const bucket::item* item = b[index].first;
    while (item && (item->key() != k))
        item = item->next;

    return item != 0;
}

template <typename K, typename V, typename Alloc>
bool ky_hash<K, V, Alloc>::contains(const entry& x) const
{
    return contains(x.key ());
}

template <typename K, typename V, typename Alloc>
const typename ky_hash<K, V, Alloc>::entry& ky_hash<K, V, Alloc>::search(const K& k) const
{
    detach ();
    const i64 index = ky_hash_f(k) % capacity ();

    const bucket::item* item = b[index].first;
    while (item && (item->key () != k))
        item = item->next;

    kyASSERT(item == 0);

    return *item;
}

template <typename K, typename V, typename Alloc>
typename ky_hash<K, V, Alloc>::entry&  ky_hash<K, V, Alloc>::search(const K& k)
{
    detach ();
    const i64 index = ky_hash_f(k) % capacity ();

    bucket::item* item = b[index].first;
    while (item && (item->key () != k))
        item = item->next;

    kyASSERT(item == 0);

    return *item;
}

template <typename K, typename V, typename Alloc>
typename ky_hash<K, V, Alloc>::iterator ky_hash<K, V, Alloc>::begin()
{
    return iterator(this);
}

template <typename K, typename V, typename Alloc>
typename ky_hash<K, V, Alloc>::const_iterator ky_hash<K, V, Alloc>::begin() const
{
    return const_iterator(this);
}

template <typename K, typename V, typename Alloc>
typename ky_hash<K, V, Alloc>::iterator ky_hash<K, V, Alloc>::end(void)
{
    return iterator(this, capacity (), 0);
}

template <typename K, typename V, typename Alloc>
typename ky_hash<K, V, Alloc>::const_iterator ky_hash<K, V, Alloc>::end(void) const
{
    return const_iterator(this, capacity (), 0);
}

template <typename K, typename V, typename Alloc>
typename ky_hash<K, V, Alloc>::iterator ky_hash<K, V, Alloc>::find(const K& k)
{
    detach ();
    const i64 index = ky_hash_f(k) % capacity ();

    bucket::item* item = b[index].first;
    while(item && (item->key () != k))
        item = item->next;

    if(item)
        return iterator(this, index, item);

    return end();
}

#if kyLanguage >= kyLanguage11
template <typename K, typename V, typename Alloc>
V &ky_hash<K, V, Alloc>::operator []( K &&k)
{
    detach ();
    const i64 index = ky_hash_f(k) % capacity ();

    bucket::item* pred = 0;
    bucket::item* item = b[index].first;
    while (item && (item->key() != k))
    {
        pred = item;
        item = item->next;
    }

    if (!item)
    {
        item = create_item(const_int<bool, kComplex | vComplex>::value, k, V());

        if (pred)
            pred->next = item;
        else
            b[index].first = item;
        ++((h -1)->ecount);

        if((h -1)->ecount > (h -1)->ecountmax)
            grow((i64)(capacity () * _hash_header::sGrowRate)+1);
    }

    return item->value ();
}
template <typename K, typename V, typename Alloc>
typename ky_hash<K, V, Alloc>::const_iterator ky_hash<K, V, Alloc>::find(const K& k) const
{
    detach ();
    const i64 index = ky_hash(k) % capacity ();

    const bucket::item* item = b[index].first;
    while(item && (item->key () != k))
        item = item->next;

    if(item)
        return const_iterator(this, index, item);

    return end();
}
#endif


#if kyLanguage >= kyLanguage11
template <typename K, typename V, typename Alloc>
typename ky_hash<K, V, Alloc>::iterator ky_hash<K, V, Alloc>::erase(const_iterator it)
{
    detach ();
    iterator nit = it;
    if((it.table == this) && (it.index < capacity ()) && it.item)
    {
        bucket_item* pred = 0;
        bucket_item* item = b[it.index].first;
        while (item && (item != it.bitem))
        {
            pred = item;
            item = item->next;
        }

        if(pred)
            pred->next = item->next;
        else
            b[it.index].first = item->next;

        destory_item(const_int<bool, kComplex | vComplex>::value, item);
        --((h -1)->ecount);
    }
    return ++nit;
}
#endif

template <typename K, typename V, typename Alloc>
#if kyLanguage >= kyLanguage11
typename ky_hash<K, V, Alloc>::iterator ky_hash<K, V, Alloc>::erase(iterator it)
#else
void ky_hash<K, V, Alloc>::erase(iterator pos)
#endif
{
    detach ();
#if kyLanguage >= kyLanguage11
    iterator nit = it;
    ++nit;
#endif
    if((it.table == this) && (it.index < capacity ()) && it.item)
    {
        bucket::item* pred = 0;
        bucket::item* item = b[it.index].first;
        while (item && (item != it.item))
        {
            pred = item;
            item = item->next;
        }

        if(pred)
            pred->next = item->next;
        else
            b[it.index].first = item->next;

        destory_item(const_int<bool, kComplex | vComplex>::value, item);
        --((h -1)->ecount);
    }
    #if kyLanguage >= kyLanguage11
    return nit;
#endif
}
template <typename K, typename V, typename Alloc>
i64 ky_hash<K, V, Alloc>::erase(const K& key)
{
    const i64 idx = ky_hash_f (key) % capacity ();
    remove(key);
    return idx;
}
template <typename K, typename V, typename Alloc>
#if kyLanguage >= kyLanguage11
typename ky_hash<K, V, Alloc>::iterator ky_hash<K, V, Alloc>::erase(const_iterator first, const_iterator last)
#else
void ky_hash<K, V, Alloc>::erase(iterator first, iterator last);
#endif
{
    detach ();
#if kyLanguage >= kyLanguage11
    iterator nit = last;
    ++nit;
#endif
    for (const_iterator bit = first; bit != last;)
        bit = erase(bit);
#if kyLanguage >= kyLanguage11
    return nit;
#endif
}

template <typename K, typename V, typename Alloc>
void ky_hash<K, V, Alloc>::remove(const iterator& it)
{
    detach ();
    if((it.table == this) && (it.index < capacity ()) && it.item)
    {
        bucket::item* pred = 0;
        bucket::item* item = b[it.index].first;
        while (item && (item != it.item))
        {
            pred = item;
            item = item->next;
        }

        if(pred)
            pred->next = item->next;
        else
            b[it.index].first = item->next;

        destory_item(const_int<bool, kComplex | vComplex>::value, item);
        --((h -1)->ecount);
    }
}

template <typename K, typename V, typename Alloc>
void ky_hash<K, V, Alloc>::clear()
{
    detach ();
    for(i64 i = 0; i < count ();++i)
    {
        if (b[i].first)
            destory_item(const_int<bool, kComplex | vComplex>::value, b[i].first);
    }
    (h -1)->ecount = 0;
}

template <typename K, typename V, typename Alloc>
typename ky_hash<K, V, Alloc>::bucket *ky_hash<K, V, Alloc>::create(i64 mins)
{
    struct {_hash_header *d; bucket* e;} _h;
    _h.d = (_hash_header*)Alloc::alloc (sizeof(_hash_header) + sizeof(bucket) * mins);
    _h.d->bcapacity = mins;
    _h.d->ecount = 0;
    _h.d->ecountmax = mins * _hash_header::sMaxUsageRate;
    _h.d->set (ky_ref::ShareableDetach);

    _h.e = (bucket*)(_h.d +1);
    ky_memory::zero (_h.e, sizeof(bucket) * mins);
    return _h.e;
}

template <typename K, typename V, typename Alloc>
void ky_hash<K, V, Alloc>::destroy(bucket *b)
{
    union{_hash_header *h; bucket* b;} _h = {.b = b};
    for(i64 i = 0; i < _h.h->bcapacity; ++i)
    {
        if (_h.b[i].first)
            destory_item(const_int<bool, kComplex | vComplex>::value, _h.b[i].first);
    }

    Alloc::destroy ((_h.h -1));
}

template <typename K, typename V, typename Alloc>
void ky_hash<K, V, Alloc>::grow(i64 newsize)
{
    struct {_hash_header *d; bucket *e;} hah;
    struct {_hash_header *d; bucket *e;} old = {h -1, b};
    hah.d = Alloc::alloc (sizeof(_hash_header) + sizeof(bucket) * newsize);
    hah.e = (bucket*)(hah.d +1);
    ky_memory::zero (hah.e, sizeof(bucket) * newsize);

    for(i64 i = 0; i < capacity (); ++i)
    {
        while(olds.e[i].first)
        {
            bucket::item* item = old.e[i].first;
            old.e[i].first = item->next;

            i64 nbi = ky_hash_f(item->key ()) % newsize;

            item->next = hah.e[nbi].first;
            hah.e[nbi].first = item;
        }
    }

    hah.d->set (ky_ref::ShareableDetach);
    hah.d->bcapacity = newsize;
    hah.d->ecount = old.d->ecount;
    hah.d->ecountmax = (i64)(newsize* _hash_header::sMaxUsageRate);
    b = hah.e;
    Alloc::destroy (old.d);
}

template <typename K, typename V, typename Alloc>
void ky_hash<K, V, Alloc>::detach()
{
    if (!is_nul() && (h -1)->is_shared () && (h -1)->has_detach ())
    {
        struct{_hash_header *d; bucket* e;} old = {h-1, b};
        struct{_hash_header *d; bucket* e;} hah;

        hah.d = Alloc::alloc(sizeof(_hash_header)+sizeof(bucket)*capacity ());
        hah.d->bcapacity = old.d->bcapacity;
        hah.d->ecount = old.d->ecount;
        hah.d->ecountmax = old.d->ecountmax;
        hah.d->set (ky_ref::ShareableDetach);

        hah.e = (bucket*)(hah.d+1);
        ky_memory::zero (hah.e, sizeof(bucket) *capacity ());
        for(i64 i = 0; i < capacity (); ++i)
        {
            while (old.e[i].first)
            {
                bucket::item* item = old.e[i].first;
                old.e[i].first = item->next;

                bucket::item *nitem = create_item(const_int<bool, kComplex | vComplex>::value,
                                                  item->key (), item->value ());
                const i64 nbi = ky_hash_f(item->key ()) % capacity ();
                nitem->next = hah.e[nbi].first;
                hah.e[nbi].first = nitem;
            }
        }
        if (old.d->lessref ())
            destroy (old.b);
        b = hah.e;
    }
    else if (is_nul())
        b = create();
}

#include "ky_hash.inl"
#endif // ky_hash_H
