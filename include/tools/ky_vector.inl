#ifndef ky_VECTOR_INL
#define ky_VECTOR_INL

template <typename T >
ky_vector<T>::ky_vector():
    VecBase()
{

}

template <typename T >
ky_vector<T>::ky_vector(int64 count):
    VecBase()
{
    VecBase::resize(count);
}

template <typename T >
ky_vector<T>::ky_vector(const ky_vector& rhs):
    VecBase(rhs)
{

}


template <typename T >
ky_vector<T>& ky_vector<T>::operator =(const ky_vector& rhs)
{
    VecBase::operator =(rhs);
    return *this;
}

template <typename T >
typename ky_vector<T>::iterator ky_vector<T>::begin()
{
    return iterator(VecBase::data());
}
template <typename T >
typename ky_vector<T>::iterator ky_vector<T>::end()
{
    return iterator(VecBase::data()+VecBase::size());
}
template <typename T >
typename ky_vector<T>::const_iterator ky_vector<T>::begin()const
{
    return const_iterator(VecBase::data());
}
template <typename T >
typename ky_vector<T>::const_iterator ky_vector<T>::end()const
{
    return const_iterator(VecBase::data()+VecBase::size());
}

template <typename T >
bool ky_vector<T>::empty() const
{
    return VecBase::is_empty();
}

template <typename T >
T &ky_vector<T>::front()
{
    return *begin();
}
template <typename T >
const T &ky_vector<T>::front()const
{
    return *begin();
}

template <typename T >
T &ky_vector<T>::back()
{
    return *(end()-1);
}
template <typename T >
const T &ky_vector<T>::back()const
{
    return *(end()-1);
}

template <typename T >
void ky_vector<T>::push_back(const T& v)
{
    VecBase::append(v);
}
template <typename T >
void ky_vector<T>::pop_back()
{
    VecBase::remove(VecBase::size()-1);
}

#if kyLanguage < kyLanguage11
template <typename T >
typename ky_vector<T>::iterator ky_vector<T>::insert(iterator pos, const T& v)
{
    VecBase::insert(pos.ope - begin().ope, v);
    return pos+1;
}
template <typename T >
void ky_vector<T>::insert(iterator pos, int64 count, const T& v)
{
    VecBase::fill(v, count, pos.ope - begin().ope);
}
template <typename T >
typename ky_vector<T>::iterator ky_vector<T>::erase(iterator pos)
{
    if (pos == end())
        VecBase::remove(count() -1);
    else
        VecBase::remove(pos.ope - begin().ope);
    return begin()+(pos.ope - begin().ope);
}
template <typename T >
typename ky_vector<T>::iterator ky_vector<T>::erase(iterator first, iterator last)
{
    if (first == end())
        return end();
    int64 offset = (first.ope - begin().ope);
    int64 len = (last.ope - first.ope);
    VecBase::remove(offset, len);
    return begin() + offset;
}
#endif

// C++11
#if kyLanguage >= kyLanguage11

template <typename T >
ky_vector<T>::ky_vector(ky_vector&& rhs):
    VecBase(rhs)
{

}
template <typename T >
ky_vector<T>::ky_vector(const std::initializer_list<T> & il):
    VecBase(il)
{

}
template <typename T >
void ky_vector<T>::push_back(T&& v)
{
    VecBase::append(v);
}
template <typename T >
typename ky_vector<T>::iterator ky_vector<T>::insert(const_iterator pos, const T& v)
{
    VecBase::insert(pos.ope - begin().ope, v);
    return iterator(pos+1);
}
template <typename T >
typename ky_vector<T>::iterator ky_vector<T>::insert(const_iterator pos, T&& v)
{
    VecBase::insert(pos.ope - begin().ope, v);
    return iterator(pos.ope+1);
}
template <typename T >
typename ky_vector<T>::iterator ky_vector<T>::insert(const_iterator pos, int64 count, const T& v)
{
    VecBase::fill(v, count, pos.ope - begin().ope);
    return iterator(pos+count);
}
template <typename T >
typename ky_vector<T>::iterator ky_vector<T>::erase(const_iterator pos)
{
    if (pos == end())
        VecBase::remove(count() -1);
    else
        VecBase::remove(pos.ope - begin().ope);
    return begin()+(pos.ope - begin().ope);
}
template <typename T >
typename ky_vector<T>::iterator ky_vector<T>::erase(const_iterator first, const_iterator last)
{
    if (first == end())
        return end();
    int64 offset = (first.ope - begin().ope);
    int64 len = (last.ope - first.ope);
    VecBase::remove(offset, len);
    return begin() + offset;
}
#endif

// base
template <typename T >
ky_vector<T>::ky_vector(const std::vector<T> &rhs):VecBase()
{
    form(rhs);
}

template <typename T >
std::vector<T> ky_vector<T>::to_std()
{
    std::vector<T> dat;
    dat.resize(VecBase::size());
    ky_memory::copy(dat.data(), VecBase::data(), VecBase::bytecount());
    return dat;
}
template <typename T >
void ky_vector<T>::form(const std::vector<T> &rhs)
{
    VecBase::resize(rhs.size());
    ky_memory::copy(VecBase::data(), rhs.data(), VecBase::bytecount());
}
template <typename T >
ky_vector<T>& ky_vector<T>::operator =(const std::vector<T>& rhs)
{
    form(rhs);
    return *this;
}
template <typename T >
bool ky_vector<T>::operator ==(const ky_vector<T> &v) const
{
    return VecBase::data() == v.data() && count() == v.count();
}

template <typename T >
bool ky_vector<T>::contains(const T &t) const
{
    for (iterator it = begin(); it != end(); ++it)
        if (*it == t)
            return true;
    return false;
}
template <typename T >
int64 ky_vector<T>::count()const
{
    return VecBase::size();
}

template <typename T >
ky_vector<T> &ky_vector<T>::prepend(T c)
{
    return *(ky_vector<T>*)&VecBase::prepend(c);
}
template <typename T >
ky_vector<T> &ky_vector<T>::prepend(const T *s, int len)
{
    return *(ky_vector<T>*)&VecBase::prepend(s, len);
}
template <typename T >
ky_vector<T> &ky_vector<T>::prepend(const ky_vector<T> &a)
{
    return *(ky_vector<T>*)&VecBase::prepend(a);
}

template <typename T >
ky_vector<T> &ky_vector<T>::append(T c)
{
    return *(ky_vector<T>*)&VecBase::append(c);
}
template <typename T >
ky_vector<T> &ky_vector<T>::append(const T *s, int64 len)
{
    return *(ky_vector<T>*)&VecBase::append(s, len);
}
template <typename T >
ky_vector<T> &ky_vector<T>::append(const ky_vector<T> &a)
{
    return *(ky_vector<T>*)&VecBase::append(a);
}

template <typename T >
ky_vector<T> &ky_vector<T>::insert(int64 i, T c)
{
    return *(ky_vector<T>*)&VecBase::insert(i, c);
}
template <typename T >
ky_vector<T> &ky_vector<T>::insert(int64 i, const T *s, int64 len)
{
    return *(ky_vector<T>*)&VecBase::insert(i, s, len);
}
template <typename T >
ky_vector<T> &ky_vector<T>::insert(int64 i, const ky_vector<T> &a)
{
    return *(ky_vector<T>*)&VecBase::insert(i, a);
}

template <typename T >
void ky_vector<T>::remove(int64 index, int64 len)
{
    remove(index, len);
}

template <typename T >
ky_vector<T> &ky_vector<T>::replace(int64 index, int64 len, const T *s, int64 alen)
{
    return *(ky_vector<T>*)&VecBase::replace(index, len, s, alen);
}
template <typename T >
ky_vector<T> &ky_vector<T>::replace(int64 index, int64 len, const ky_vector<T> &s)
{
    return *(ky_vector<T>*)&VecBase::replace(index, len, s);
}

template <typename T >
ky_vector<T> ky_vector<T>::extract( int pos, int count)
{
    return *(ky_vector<T>*)&VecBase::extract(pos, count);
}
template <typename T >
ky_vector<T> ky_vector<T>::extract( int pos )
{
    return *(ky_vector<T>*)&VecBase::extract(pos);
}
template <typename T >
ky_vector<T> ky_vector<T>::start( int count)
{
    return *(ky_vector<T>*)&VecBase::start(count);
}
template <typename T >
ky_vector<T> ky_vector<T>::end( int count)
{
    return *(ky_vector<T>*)&VecBase::end(count);
}
template <typename T >
void ky_vector<T>::swap(ky_vector<T> &rhs)
{
    VecBase::swap(rhs);
}

template <typename T >
typename ky_vector<T>::opevec_node ky_vector<T>::opevec_node::operator +(int c)
{
    opevec_node node = *this;
    if (c > 0)
        while (c--) ++node;
    else
        while (c++) --node;
    return (node);
}
template <typename T >
const typename ky_vector<T>::opevec_node ky_vector<T>::opevec_node::operator +(int c)const
{
    opevec_node node = *this;
    if (c > 0)
        while (c--) ++node;
    else
        while (c++) --node;
    return (node);
}

template <typename T >
const typename ky_vector<T>::opevec_node & ky_vector<T>::opevec_node::operator++()const
{
    opevec_node *take = (opevec_node *)this;
    ++take->ope;
    return *this;
}
template <typename T >
const typename ky_vector<T>::opevec_node ky_vector<T>::opevec_node::operator++(int)const
{
    opevec_node old = *this;
    ++ope;
    return old;
}
template <typename T >
const typename ky_vector<T>::opevec_node& ky_vector<T>::opevec_node::operator--()const
{
    opevec_node *take = (opevec_node *)this;
    --take->ope;
    return *this;
}
template <typename T >
const typename ky_vector<T>::opevec_node ky_vector<T>::opevec_node::operator--(int)const
{
    opevec_node old = *this;
    --ope;
    return old;
}
template <typename T >
typename ky_vector<T>::opevec_node& ky_vector<T>::opevec_node::operator++()
{
    ++ope;
    return *this;
}
template <typename T >
typename ky_vector<T>::opevec_node ky_vector<T>::opevec_node::operator++(int)
{
    opevec_node old = *this;
    ++ope;
    return old;
}
template <typename T >
typename ky_vector<T>::opevec_node& ky_vector<T>::opevec_node::operator--()
{
    --ope;
    return *this;
}
template <typename T >
typename ky_vector<T>::opevec_node ky_vector<T>::opevec_node::operator--(int)
{
    opevec_node old = *this;
    --ope;
    return old;
}
template <typename T >
int ky_vector<T>::opevec_node::operator - (const opevec_node &rhs)
{
    return int(ope - rhs.ope);
}
#endif // ky_VECTOR_INL

