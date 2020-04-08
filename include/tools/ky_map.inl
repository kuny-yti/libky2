

template <typename K, typename V, typename Alloc>
ky_map<K, V, Alloc>::ky_map():
    impl(_map_header::shared_nul)
{
}

template <typename K, typename V, typename Alloc>
ky_map<K, V, Alloc>::ky_map(const ky_map<K, V, Alloc> &m):
    impl(m.impl)
{
    if (impl->has_shareable())
        impl->addref();
    else
        detach();
}
template <typename K, typename V, typename Alloc>
ky_map<K, V, Alloc>::~ky_map()
{
    if (!is_nul() && impl->lessref())
    {
        iterator i = begin();
        while (i != end())
        {
            node *x = i.x;
            ++i;
            destory_node(const_int<bool, kComplex | vComplex>::value, x);
        }

        destory_head(impl);
    }
}
template <typename K, typename V, typename Alloc>
void ky_map<K, V, Alloc>::detach()
{
    if (!is_nul() && impl->has_detach() && impl->is_shared())
        copy();
}
template <typename K, typename V, typename Alloc>
void ky_map<K, V, Alloc>::copy()
{
    ky_map<K, V, Alloc> tmp = *this;

    impl = create_head();
    for (iterator i = tmp.begin(); i != tmp.end(); ++i)
        this->append(i.key(), i.value());
}

template <typename K, typename V, typename Alloc>
typename ky_map<K, V, Alloc>::iterator ky_map<K, V, Alloc>::begin()
{
    detach();
    return iterator(impl->head.left());
}
template <typename K, typename V, typename Alloc>
typename ky_map<K, V, Alloc>::const_iterator ky_map<K, V, Alloc>::begin() const
{
    return const_iterator(impl->head.left());
}
template <typename K, typename V, typename Alloc>
typename ky_map<K, V, Alloc>::iterator ky_map<K, V, Alloc>::end()
{
    detach();
    return iterator(&impl->head);
}
template <typename K, typename V, typename Alloc>
typename ky_map<K, V, Alloc>::const_iterator ky_map<K, V, Alloc>::end() const
{
    return const_iterator(&impl->head);
}

template <typename K, typename V, typename Alloc>
ky_map<K, V, Alloc> &ky_map<K, V, Alloc>::operator = (const ky_map<K, V, Alloc> &rhs)
{
    if (!is_nul() && impl->lessref())
    {
        iterator i = begin();
        while (i != end())
        {
            node *x = i.x;
            ++i;
            destory_node(const_int<bool, kComplex | vComplex>::value, x);
        }

        destory_head(impl);
    }

    impl = rhs.impl;
    if (impl->has_shareable())
        impl->addref();
    else
        detach();
    return *this;
}

template <typename K, typename V, typename Alloc>
V &ky_map<K, V, Alloc>::operator [](const K &key)
{
    if (is_nul())
        impl = create_head();

    detach();
    iterator i = find(key);
    if (i == end())
        return *insert(key);
    return i.value();
}

template <typename K, typename V, typename Alloc>
bool ky_map<K, V, Alloc>::empty() const
{
    return is_empty();
}
template <typename K, typename V, typename Alloc>
int64 ky_map<K, V, Alloc>::size() const
{
    return count();
}

template <typename K, typename V, typename Alloc>
V &ky_map<K, V, Alloc>::at(const K &key)
{
    return operator [](key);
}
template <typename K, typename V, typename Alloc>
const V &ky_map<K, V, Alloc>::at(const K &key)const
{
    return operator [](key);
}

template <typename K, typename V, typename Alloc>
typename ky_map<K, V, Alloc>::iterator ky_map<K, V, Alloc>::insert(const K& key)
{
    return this->insert(key, V());
}
template <typename K, typename V, typename Alloc>
typename ky_map<K, V, Alloc>::iterator
ky_map<K, V, Alloc>::insert(const K& key, const V& val)
{
    if (is_nul())
        impl = create_head();

    detach();
    iterator i = find(key);
    if (i != end())
    {
        i.x->value = val;
        return i;
    }

    node *n = create_node(const_int<bool, kComplex | vComplex>::value, key, val);
    impl->insert(n);
    return iterator(n);
}

template <typename K, typename V, typename Alloc>
void ky_map<K, V, Alloc>::swap(ky_map<K, V, Alloc> &rhs)
{
    _map_header *tmp = impl;
    impl = rhs.impl;
    rhs.impl = tmp;
}
template <typename K, typename V, typename Alloc>
void ky_map<K, V, Alloc>::clear()
{
    *this = ky_map<K, V, Alloc>();
}
template <typename K, typename V, typename Alloc>
typename ky_map<K, V, Alloc>::iterator ky_map<K, V, Alloc>::find(const K &key)
{
    iterator i = begin();
    while ((i.key() != key) && (i != end()))
        ++i;
    return i;
}

/// C++ 11
#if kyLanguage >= kyLanguage11
template <typename K, typename V, typename Alloc>
ky_map<K, V, Alloc>::ky_map(ky_map<K, V, Alloc> &&m):
    impl(m.impl)
{
    m.impl = _map_header::shared_nul;
}

template <typename K, typename V, typename Alloc>
typename ky_map<K, V, Alloc>::const_iterator ky_map<K, V, Alloc>::cbegin() const
{
    return const_iterator(impl->head.left());
}
template <typename K, typename V, typename Alloc>
typename ky_map<K, V, Alloc>::const_iterator ky_map<K, V, Alloc>::cend() const
{
    return const_iterator(&impl->head);
}

template <typename K, typename V, typename Alloc>
V &ky_map<K, V, Alloc>::operator []( K &&key)
{
    if (is_nul())
        impl = create_head();

    detach();
    iterator i = find(key);
    if (i == cend())
        return *insert(key);
    return i.value();
}
template <typename K, typename V, typename Alloc>
typename ky_map<K, V, Alloc>::iterator ky_map<K, V, Alloc>::erase(const_iterator pos)
{
    if (is_nul() || (pos == cend()))
        return pos;

    detach ();
    const_iterator next_pos = pos;
    impl->remove(pos.x);
    destory_node(const_int<bool, kComplex | vComplex>::value, pos.x);
    return ++next_pos;
}


#  ifdef kyHasSTL
template <typename K, typename V, typename Alloc>
ky_map<K, V, Alloc>::ky_map(std::initializer_list<std::pair<K, V> > list):
    impl(_map_header::shared_nul)
{
    for (typename std::initializer_list<std::pair<K, V> >::const_iterator it = list.begin();
         it != list.end(); ++it)
        append(it->first, it->second);
}
template <typename K, typename V, typename Alloc>
ky_map<K, V, Alloc>::ky_map(const std::map<K, V> &m):
    impl(_map_header::shared_nul)
{
    form(m);
}
template <typename K, typename V, typename Alloc>
std::map<K, V> &ky_map<K, V, Alloc>::to_std()
{
    std::map<K, V> map;
    for (const_iterator it = cbegin(); it != cend(); ++it)
        map.insert(map.end(), std::pair<K, V>(it.key(), it.value()));
    return map;
}
template <typename K, typename V, typename Alloc>
void ky_map<K, V, Alloc>::form(std::map<K, V> &m)
{
    detach ();
    clear();
    for (typename std::map<K, V>::const_iterator it = m.begin(); it != m.end(); ++it)
        this->insert((*it).first, (*it).second);
}
#  endif
#endif

template <typename K, typename V, typename Alloc>
const V ky_map<K, V, Alloc>::operator [](const K &key)const
{
    return value(key);
}

template <typename K, typename V, typename Alloc>
bool ky_map<K, V, Alloc>::operator== (const ky_map<K, V, Alloc> &rhs) const
{
    if (size() != rhs.size())
        return false;
    if (impl == rhs.impl)
        return true;

    const_iterator it1 = cbegin();
    const_iterator it2 = rhs.cbegin();

    while (it1 != end())
    {
        if ((it1.value() != it2.value()) || (it1.key() != it2.key()))
            return false;
        ++it2;
        ++it1;
    }
    return true;
}

template <typename K, typename V, typename Alloc>
V ky_map<K, V, Alloc>::value(const K& key)
{
    const_iterator i = find(key);
    return i != cend() ? i.value() : V();
}

template <typename K, typename V, typename Alloc>
void ky_map<K, V, Alloc>::append(const K& key, const V& val)
{
    this->insert(key, val);
}

template <typename K, typename V, typename Alloc>
void ky_map<K, V, Alloc>::remove(const K& key)
{
    detach();
    const_iterator i = find(key);
    while (i != cend())
    {
        impl->remove(i.x);
        destory_node(const_int<bool, kComplex | vComplex>::value, i.x);
        i = find(key);
    }
}

template <typename K, typename V, typename Alloc>
void ky_map<K, V, Alloc>::remove()
{
    detach ();
    clear();
}

template <typename K, typename V, typename Alloc>
bool ky_map<K, V, Alloc>::contains(const K &key) const
{
    return find(key) != cend();
}

template <typename K, typename V, typename Alloc>
const K ky_map<K, V, Alloc>::key(const V &value) const
{
    const_iterator i = cbegin();
    while (i != cend())
    {
        if (i.value() == value)
            return i.key();
        ++i;
    }

    return K();
}

template <typename K, typename V, typename Alloc>
const V ky_map<K, V, Alloc>::value(const K &key) const
{
    const_iterator i = find(key);
    return i != cend() ? i.value() : V();
}

template <typename K, typename V, typename Alloc>
ky_list<K> ky_map<K, V, Alloc>::keys() const
{
    ky_list<K> res;
    const_iterator i = cbegin();
    while (i != cend())
    {
        res.append(i.key());
        ++i;
    }
    return res;
}
template <typename K, typename V, typename Alloc>
ky_list<K> ky_map<K, V, Alloc>::keys(const V &value) const
{
    ky_list<K> res;
    const_iterator i = cbegin();
    while (i != cend())
    {
        if (i.value() == value)
            res.append(i.key());
        ++i;
    }
    return res;
}

template <typename K, typename V, typename Alloc>
ky_list<V> ky_map<K, V, Alloc>::values() const
{
    ky_list<V> res;
    const_iterator i = cbegin();
    while (i != cend())
    {
        res.append(i.value());
        ++i;
    }
    return res;
}
template <typename K, typename V, typename Alloc>
ky_list<V> ky_map<K, V, Alloc>::values(const K &key) const
{
    ky_list<V> res;
    const_iterator i = cbegin();
    while (i != cend())
    {
        if (key == i.key())
            res.append(i.value());
        ++i;
    }
    return res;
}

template <typename K, typename V, typename Alloc>
V &ky_map<K, V, Alloc>::first()
{
    return *begin();
}
template <typename K, typename V, typename Alloc>
const V &ky_map<K, V, Alloc>::first()const
{
    return *cbegin();
}

template <typename K, typename V, typename Alloc>
V &ky_map<K, V, Alloc>::last()
{
    return *(--end());
}
template <typename K, typename V, typename Alloc>
const V &ky_map<K, V, Alloc>::last()const
{
    return *(--cend());
}

template <typename K, typename V, typename Alloc>
int64 ky_map<K, V, Alloc>::count()const
{
    return impl->count;
}
template <typename K, typename V, typename Alloc>
bool ky_map<K, V, Alloc>::is_empty()const
{
    return (count() == 0);
}
template <typename K, typename V, typename Alloc>
bool ky_map<K, V, Alloc>::is_nul()const
{
    return impl == _map_header::shared_nul;
}


template <typename K, typename V, typename Alloc>
typename ky_map<K, V, Alloc>::node *
ky_map<K, V, Alloc>::create_node(false_type_t, const K &k, const V &v)
{
    node *n = Alloc::alloc(sizeof(node));
    new (n) node();
    n->key = k;
    n->value = v;
    return n;
}
template <typename K, typename V, typename Alloc>
typename ky_map<K, V, Alloc>::node *
ky_map<K, V, Alloc>::create_node(true_type_t, const K &k, const V &v)
{
    node *n = Alloc::alloc(sizeof(node));
    new (n) node();
    new (&n->key) K(k);
    new (&n->value) V(v);
    return n;
}
template <typename K, typename V, typename Alloc>
void ky_map<K, V, Alloc>::destory_node(false_type_t, node *n)
{
    Alloc::destroy(n);
}
template <typename K, typename V, typename Alloc>
void ky_map<K, V, Alloc>::destory_node(true_type_t, node *n)
{
    n->key.~K();
    n->value.~V();
    Alloc::destroy(n);
}

template <typename K, typename V, typename Alloc>
_map_header* ky_map<K, V, Alloc>::create_head()
{
    _map_header *x = Alloc::alloc(sizeof(_map_header));
    x->count = 0;
    x->set(ky_ref::ShareableDetach);
    ky_memory::zero(&x->head, sizeof(x->head));
    x->most_left = &(x->head);
    return x;
}
template <typename K, typename V, typename Alloc>
void ky_map<K, V, Alloc>::destory_head(_map_header* x)
{
    Alloc::destroy(x);
}

template<typename T, typename V, typename Alloc>
typename ky_map<T, V, Alloc>::node *ky_map<T, V, Alloc>::node::left() const
{
    return (node *)rb_node::left();
}
template<typename T, typename V, typename Alloc>
typename ky_map<T, V, Alloc>::node *ky_map<T, V, Alloc>::node::right() const
{
    return (node *)rb_node::right();
}
template<typename T, typename V, typename Alloc>
typename ky_map<T, V, Alloc>::node *ky_map<T, V, Alloc>::node::next()
{
    return (node*)rb_node::next();
}
template<typename T, typename V, typename Alloc>
typename ky_map<T, V, Alloc>::node *ky_map<T, V, Alloc>::node::prev()
{
    return (node*)rb_node::prev();
}
template<typename T, typename V, typename Alloc>
char ky_map<T, V, Alloc>::node::compare (rb_node *rhs) const
{
    node *z = (node*)rhs;
    if (key < z->key)
        return -1;
    else if (key > z->key)
        return 1;
    return 0;
}
template<typename T, typename V, typename Alloc>
void ky_map<T, V, Alloc>::node::destroy()
{
    complex_destruct<T>(key);
    complex_destruct<V>(value);
    destroy_child(const_int<bool, kComplex | vComplex>::value);
}
template<typename T, typename V, typename Alloc>
void ky_map<T, V, Alloc>::node::destroy_child(false_type_t)
{

}

template<typename T, typename V, typename Alloc>
void ky_map<T, V, Alloc>::node::destroy_child(true_type_t)
{
    if (left()) left()->destroy();
    if (right()) right()->destroy();
}

template <typename K, typename V, typename Alloc>
ky_map<K, V, Alloc>::iterator::iterator():
    x(0)
{

}
template <typename K, typename V, typename Alloc>
ky_map<K, V, Alloc>::iterator::iterator(const node *p):
    x(p)
{

}
template <typename K, typename V, typename Alloc>
ky_map<K, V, Alloc>::iterator::iterator(const iterator &n):
    x(n.x)
{

}

template <typename K, typename V, typename Alloc>
ky_map<K, V, Alloc>::iterator::~iterator()
{

}

template <typename K, typename V, typename Alloc>
typename ky_map<K, V, Alloc>::iterator &
ky_map<K, V, Alloc>::iterator::operator=(const iterator &n) const
{
    x = n.x;
    return *this;
}


template <typename K, typename V, typename Alloc>
const K &ky_map<K, V, Alloc>::iterator::key() const
{
    return x->key;
}
template <typename K, typename V, typename Alloc>
V &ky_map<K, V, Alloc>::iterator::value() const
{
    return x->value;
}
template <typename K, typename V, typename Alloc>
V &ky_map<K, V, Alloc>::iterator::operator*() const
{
    return x->value;
}
template <typename K, typename V, typename Alloc>
V *ky_map<K, V, Alloc>::iterator::operator->() const
{
    return &x->value;
}
template <typename K, typename V, typename Alloc>
bool ky_map<K, V, Alloc>::iterator::operator==(const iterator &o) const
{
    return x == o.x;
}
template <typename K, typename V, typename Alloc>
bool ky_map<K, V, Alloc>::iterator::operator!=(const iterator &o) const
{
    return x != o.x;
}
template <typename K, typename V, typename Alloc>
bool ky_map<K, V, Alloc>::iterator::operator<(const iterator& rhs) const
{
    return x < rhs.x;
}
template <typename K, typename V, typename Alloc>
bool ky_map<K, V, Alloc>::iterator::operator<=(const iterator& rhs) const
{
    return x <= rhs.x;
}
template <typename K, typename V, typename Alloc>
bool ky_map<K, V, Alloc>::iterator::operator>(const iterator& rhs) const
{
    return x > rhs.x;
}
template <typename K, typename V, typename Alloc>
bool ky_map<K, V, Alloc>::iterator::operator>=(const iterator& rhs) const
{
    return x >= rhs.x;
}

template <typename K, typename V, typename Alloc>
int ky_map<K, V, Alloc>::iterator::operator -(const iterator &rhs)const
{
    iterator *take = (iterator*)this;
    int retval = 0;
    node *nt = x;
    forever (x && (x != rhs.x))
    {
        this->operator --();
        ++retval;
    }
    if (!x)
    {
        take->x = nt;
        retval = 0;
        forever (x && (x != rhs.x))
        {
            this->operator ++();
            --retval;
        }
    }
    take->x = nt;
    return retval;
}

template <typename K, typename V, typename Alloc>
typename ky_map<K, V, Alloc>::iterator
ky_map<K, V, Alloc>::iterator::operator +(int c)
{
    iterator node = *this;
    if (c > 0)
        while (c--) ++node;
    else
        while (c++) --node;
    return (node);
}
template <typename K, typename V, typename Alloc>
const typename ky_map<K, V, Alloc>::iterator
ky_map<K, V, Alloc>::iterator::operator +(int c)const
{
    iterator node = *this;
    if (c > 0)
        while (c--) ++node;
    else
        while (c++) --node;
    return node;
}
template <typename K, typename V, typename Alloc>
typename ky_map<K, V, Alloc>::iterator
ky_map<K, V, Alloc>::iterator::operator -(int c)
{
    return operator+(-c);
}
template <typename K, typename V, typename Alloc>
const typename ky_map<K, V, Alloc>::iterator
ky_map<K, V, Alloc>::iterator::operator -(int c)const
{
    return operator+(-c);
}
template <typename K, typename V, typename Alloc>
const typename ky_map<K, V, Alloc>::iterator&
ky_map<K, V, Alloc>::iterator::operator++()const
{
    x = x->next();
    return *this;
}
template <typename K, typename V, typename Alloc>
const typename ky_map<K, V, Alloc>::iterator
ky_map<K, V, Alloc>::iterator::operator++(int)const
{
    iterator tn = *this;
    x = x->next();
    return tn;
}
template <typename K, typename V, typename Alloc>
const typename ky_map<K, V, Alloc>::iterator&
ky_map<K, V, Alloc>::iterator::operator--()const
{
    x = x->prev();
    return *this;
}
template <typename K, typename V, typename Alloc>
const typename ky_map<K, V, Alloc>::iterator
ky_map<K, V, Alloc>::iterator::operator--(int)const
{
    iterator tn = *this;
    x = x->prev();
    return tn;
}

template <typename K, typename V, typename Alloc>
typename ky_map<K, V, Alloc>::iterator&
ky_map<K, V, Alloc>::iterator::operator++()
{
    x = x->next();
    return *this;
}

template <typename K, typename V, typename Alloc>
typename ky_map<K, V, Alloc>::iterator
ky_map<K, V, Alloc>::iterator::operator++(int)
{
    iterator tn = *this;
    x = x->next();
    return tn;
}

template <typename K, typename V, typename Alloc>
typename ky_map<K, V, Alloc>::iterator&
ky_map<K, V, Alloc>::iterator::operator--()
{
    x = x->prev();
    return *this;
}

template <typename K, typename V, typename Alloc>
typename ky_map<K, V, Alloc>::iterator
ky_map<K, V, Alloc>::iterator::operator--(int)
{
    iterator tn = *this;
    x = x->prev();
    return tn;
}

template <typename K, typename V, typename Alloc>
typename ky_map<K, V, Alloc>::iterator &
ky_map<K, V, Alloc>::iterator::operator +=(int c)
{
    *this = (*this + c);
    return *this;
}

template <typename K, typename V, typename Alloc>
const typename ky_map<K, V, Alloc>::iterator &
ky_map<K, V, Alloc>::iterator::operator +=(int c)const
{
    *this = (*this + c);
    return *this;
}

template <typename K, typename V, typename Alloc>
typename ky_map<K, V, Alloc>::iterator &
ky_map<K, V, Alloc>::iterator::operator -=(int c)
{
    *this = (*this - c);
    return *this;
}

template <typename K, typename V, typename Alloc>
const typename ky_map<K, V, Alloc>::iterator &
ky_map<K, V, Alloc>::iterator::operator -=(int c)const
{
    *this = (*this - c);
    return *this;
}
