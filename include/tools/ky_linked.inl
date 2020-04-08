#ifndef KY_LINKED_INL
#define KY_LINKED_INL


#include "ky_typeinfo.h"

template< typename T , typename Alloc>
bool ky_linked<T, Alloc>::is_null() const
{
    return _ky_linked_::is_null((intptr)impl);
}

template< typename T , typename Alloc>
void ky_linked<T, Alloc>::free(_ky_linked_ *x)
{
    list_node *end = (list_node*)x;
    list_node *current = end->next;
    while (current != end)
    {
        list_node *node = current;
        current= current->next;
        freenode(node);
    }
    if (end != current)
        this->destroy((T*)x);
}
template< typename T , typename Alloc>
typename ky_linked<T, Alloc>::list_node *ky_linked<T, Alloc>::newnode(const T &d)
{
    list_node *n = (list_node*)this->alloc(sizeof(list_node));
    n->next = NULL;
    n->prev = NULL;
    if (!ky_is_type(T))
        new (&n->data) T(d);
    else
        n->data = d;
    return n;
}
template< typename T , typename Alloc>
void ky_linked<T, Alloc>::freenode(list_node *n)
{
    if (!ky_is_type(T))
        n->data.~T();
    this->destroy((T*)n);
}
// C++11
#if kyLanguage >= kyLanguage11
template< typename T , typename Alloc>
ky_linked<T, Alloc>::ky_linked(const ky_linked<T, Alloc> &&rhs):
    impl((_ky_linked_*)_ky_linked_::null())
{
    *this = rhs;
}
template< typename T , typename Alloc>
ky_linked<T, Alloc>& ky_linked<T, Alloc>::operator = (ky_linked<T, Alloc> &&rhs)
{
    if(this == &rhs)
        return *this;

    if (!is_null() && header()->lessref())
        free(impl);
    impl = rhs.impl;
    rhs.impl = (_ky_linked_*)_ky_linked_::null();

    return *this;
}
template< typename T , typename Alloc>
ky_linked<T, Alloc>::ky_linked(const std::initializer_list<T> & il):
    impl((_ky_linked_*)_ky_linked_::null())
{
    clear ();
    foreach (const T & a, il)
        append (a);
}
#endif


// core
template< typename T , typename Alloc>
void ky_linked<T, Alloc>::copy()
{
    if (size())
    {
        union { _ky_linked_ *d; list_node *e; } tmp;
        tmp.d = (_ky_linked_ *)this->alloc(sizeof(_ky_linked_));
        tmp.d->ref.set(ky_ref::refShareableDetach);
        tmp.d->count = this->count();

        list_node *current = list->next;
        list_node *ope = tmp.e;
        list_node *end = list;

        while (current != end)
        {
            ope->next = newnode(current->data);
            ope->next->prev = ope;
            current = current->next;
            ope = ope->next;
        }
        ope->next = tmp.e;
        tmp.e->prev = ope;
        if (header()->lessref())
            free(impl);
        impl = tmp.d;
    }
}
template< typename T , typename Alloc>
void ky_linked<T, Alloc>::detach()
{
    if (is_null())
    {
        impl = (_ky_linked_ *)this->alloc(sizeof(_ky_linked_));
        impl->ref.set(ky_ref::refShareableDetach);
        impl->count = 0;
        list->next = list->prev = list;
    }
    else if (header()->ref.has_detach() && header()->ref.is_shared())
        copy();
}

// STL
template< typename T , typename Alloc>
ky_linked<T, Alloc>::ky_linked():
    impl((_ky_linked_*)_ky_linked_::null())
{
}

template< typename T , typename Alloc>
ky_linked<T, Alloc>::ky_linked( const ky_linked<T, Alloc> &rhs ):
    impl((_ky_linked_*)_ky_linked_::null())
{
    if (impl == rhs.impl)
        return ;
    impl = rhs.impl;
    if (header()->ref.has_shareable())
        header()->addref();
    else
        detach();
}

template< typename T , typename Alloc>
ky_linked<T, Alloc>::~ky_linked()
{
    if (!is_null() && header()->lessref())
        free(impl);
    impl = NULL;
}
template< typename T , typename Alloc>
ky_linked<T, Alloc> &ky_linked<T, Alloc>::operator =(const ky_linked<T, Alloc> &rhs )
{
    if( impl == rhs.impl)
        return *this;

    if (!is_null() && header()->lessref())
        free(impl);
    impl = rhs.impl;
    if (header()->ref.has_shareable())
        header()->addref();
    else
        detach();

    return *this;
}
template< typename T , typename Alloc>
void ky_linked<T, Alloc>::swap(ky_linked<T, Alloc> &rhs)
{
    _ky_linked_ *tmp = impl;
    impl = rhs.impl;
    rhs.impl = tmp;
}
template< typename T , typename Alloc>
void ky_linked<T, Alloc>::sort()
{
    ky_qsort<iterator>(begin(), end());
}
template< typename T , typename Alloc>
void ky_linked<T, Alloc>::reverse()
{
    iterator first = begin();
    iterator last = end();
    while ((first != last) && (first != --last))
        ky_swap (*first++, *last);
}

template< typename T , typename Alloc>
typename ky_linked<T, Alloc>::iterator ky_linked<T, Alloc>::insert(int64 pos, const T& val)
{
    detach();
    list_node *nnode = newnode(val);

    if (pos > size())
        pos = size()-1;

    list_node *p = (begin() + pos).ope;

    nnode->prev = p->prev;
    nnode->next = p;

    p->prev->next = nnode;
    p->prev = nnode;
    ++impl->count;
    return iterator(p->prev);
}
template< typename T , typename Alloc>
void ky_linked<T, Alloc>::insert(int64 pos, const ky_linked<T, Alloc>& val)
{
    //for(iterator temp = val.begin(); temp != val.end(); ++temp)
    foreach (const T &var, val)
        insert(pos++, var);
}

template< typename T , typename Alloc>
void ky_linked<T, Alloc>::clear()
{
    *this = ky_linked<T, Alloc>();
}
template< typename T , typename Alloc>
typename ky_linked<T, Alloc>::iterator ky_linked<T, Alloc>::begin()
{
    detach();
    return iterator(list->next);
}
template< typename T , typename Alloc>
typename ky_linked<T, Alloc>::const_iterator ky_linked<T, Alloc>::begin() const
{
    return iterator(list->next);
}
template< typename T , typename Alloc>
typename ky_linked<T, Alloc>::iterator ky_linked<T, Alloc>::end()
{
    detach();
    return iterator(list);
}
template< typename T , typename Alloc>
typename ky_linked<T, Alloc>::const_iterator ky_linked<T, Alloc>::end() const
{
    return iterator(list);
}
template< typename T , typename Alloc>
T& ky_linked<T, Alloc>::front()
{
    return *begin();
}
template< typename T , typename Alloc>
const T& ky_linked<T, Alloc>::front() const
{
    return *begin();
}
template< typename T , typename Alloc>
T& ky_linked<T, Alloc>::back()
{
    return *(--end());
}
template< typename T , typename Alloc>
const T& ky_linked<T, Alloc>::back() const
{
    return *(--end());
}
template< typename T , typename Alloc>
void ky_linked<T, Alloc>::pop_front()
{
    erase(begin());
}
template< typename T , typename Alloc>
void ky_linked<T, Alloc>::pop_back()
{
    erase(--end());
}
template< typename T , typename Alloc>
#if kyLanguage < kyLanguage11
void ky_linked<T, Alloc>::insert(iterator itr, int64 n, const T &x )
{
    iterator i = itr;
    forever (n--)
        i = insert(i, x);
}
#else
typename ky_linked<T, Alloc>::iterator ky_linked<T, Alloc>::insert(const_iterator itr, int64 n, const T &x )
{
    iterator i = itr;
    forever (n--)
        i = insert(i, x);
    return i;
}

#endif
template< typename T , typename Alloc>
#if kyLanguage < kyLanguage11
void ky_linked<T, Alloc>::splice(iterator pos, ky_linked& x)
#else
void ky_linked<T, Alloc>::splice(const_iterator pos, ky_linked<T, Alloc>& x)
#endif
{
    insert(pos, x);
    x.clear();
}
template< typename T , typename Alloc>
#if kyLanguage < kyLanguage11
void ky_linked<T, Alloc>::splice(iterator pos, ky_linked<T, Alloc>& x, iterator i)
#else
void ky_linked<T, Alloc>::splice(const_iterator pos, ky_linked<T, Alloc>& x, const_iterator i)
#endif
{
    insert(pos, i, x.end());
    x.erase(i, x.end());
}
template< typename T , typename Alloc>
#if kyLanguage < kyLanguage11
void ky_linked<T, Alloc>::splice(iterator pos, ky_linked& x, iterator first, iterator last)
#else
void ky_linked<T, Alloc>::splice(const_iterator pos, ky_linked& x, const_iterator first, const_iterator last)
#endif
{
    insert(pos, first, last);
    x.erase(first, last);
}

#if kyLanguage < kyLanguage11
template< typename T , typename Alloc>
void ky_linked<T, Alloc>::merge(ky_linked& x)
{

}
#endif

#if kyLanguage >= kyLanguage11
template< typename T , typename Alloc>
typename ky_linked<T, Alloc>::iterator ky_linked<T, Alloc>::insert(const_iterator itr, T &&x )
{
    int64 pos = itr - begin();
    return insert(pos, x);
}
template< typename T , typename Alloc>
void ky_linked<T, Alloc>::splice(const_iterator pos, ky_linked&& x)
{

}
template< typename T , typename Alloc>
void ky_linked<T, Alloc>::splice(const_iterator pos, ky_linked&& x, const_iterator i)
{

}
template< typename T , typename Alloc>
void ky_linked<T, Alloc>::splice(const_iterator pos, ky_linked&& x, const_iterator first, const_iterator last)
{

}
template< typename T , typename Alloc>
void ky_linked<T, Alloc>::merge(ky_linked&& x)
{

}
#endif

template< typename T , typename Alloc>
#if kyLanguage < kyLanguage11
typename ky_linked<T, Alloc>::iterator ky_linked<T, Alloc>::insert(iterator itr, const T &x)
#else
typename ky_linked<T, Alloc>::iterator ky_linked<T, Alloc>::insert(const_iterator itr, const T &x)
#endif
{
    int inx = itr -begin();
    detach();

    list_node *i = (begin()+inx).ope;
    list_node *m = newnode(x);
    m->next = i;
    m->prev = i->prev;
    m->prev->next = m;
    i->prev = m;
    ++header()->count;
    return m;
}
template< typename T , typename Alloc>
#if kyLanguage < kyLanguage11
typename ky_linked<T, Alloc>::iterator ky_linked<T, Alloc>::insert(iterator itr, const iterator &x )
#else
typename ky_linked<T, Alloc>::iterator ky_linked<T, Alloc>::insert(const_iterator itr, const iterator &x )
#endif
{
    return insert(itr, *x);
}

template< typename T , typename Alloc>
#if kyLanguage < kyLanguage11
typename ky_linked<T, Alloc>::iterator ky_linked<T, Alloc>::insert(iterator itr, const ky_linked<T, Alloc> &x)
#else
typename ky_linked<T, Alloc>::iterator ky_linked<T, Alloc>::insert(const_iterator itr, const ky_linked<T, Alloc> &x)
#endif
{
    iterator n = itr;
    foreach (const T &val, x)
        n = insert(n, val);
    /*
    for(iterator temp = x.begin(); temp != x.end(); ++temp)
        itr = insert(itr, *temp);*/
    return ++n;
}
template< typename T , typename Alloc>
#if kyLanguage < kyLanguage11
typename ky_linked<T, Alloc>::iterator ky_linked<T, Alloc>::erase(iterator itr)
#else
typename ky_linked<T, Alloc>::iterator ky_linked<T, Alloc>::erase(const_iterator itr)
#endif
{
    int inx =  itr - cbegin();
    remove(inx);
    return begin()+inx;
}
template< typename T , typename Alloc>
#if kyLanguage < kyLanguage11
typename ky_linked<T, Alloc>::iterator ky_linked<T, Alloc>::erase(iterator start, iterator end)
#else
typename ky_linked<T, Alloc>::iterator ky_linked<T, Alloc>::erase(const_iterator start, const_iterator end)
#endif
{
    int c = end - start;
    int inx = start - begin();
    while (c--)
        remove(inx);

    return begin()+inx;
}

template< typename T , typename Alloc>
void ky_linked<T, Alloc>::assign(iterator itr, const T &x )
{
    int inx = itr - begin();
    replace(inx, x);
}
template< typename T , typename Alloc>
void ky_linked<T, Alloc>::assign(iterator itr, const iterator &x )
{
    int inx = itr - begin();
    replace(inx, *x);
}


// base
template< typename T , typename Alloc>
ky_linked<T, Alloc>::ky_linked(const std::list<T> &rhs):
    impl((T*)_ky_linked_::null())
{
    form(rhs);
}
template< typename T , typename Alloc>
std::list<T> ky_linked<T, Alloc>::to_std()
{
    std::list<T> retlist;

    foreach (const iterator &itr , *this)
        retlist.push_back(*itr);
    return retlist;
}
template< typename T , typename Alloc>
void ky_linked<T, Alloc>::form(const std::list<T> &rhs)
{
    detach();
    foreach (const T &var , rhs)
        append(var);
    //std::copy(rhs.begin(), rhs.end(), std::back_inserter(*this));
}

template< typename T , typename Alloc>
ky_linked<T, Alloc>& ky_linked<T, Alloc>::operator = (const std::list<T> &rhs)
{
    form(rhs);
    return *this;
}

template< typename T , typename Alloc>
ky_linked<T, Alloc> &ky_linked<T, Alloc>::operator +=(const ky_linked<T, Alloc> &rhs)
{
    foreach (const T &var , rhs)
        push_back(var);

    return *this;
}
template< typename T , typename Alloc>
ky_linked<T, Alloc> &ky_linked<T, Alloc>::operator +=(const T &val)
{
    push_back(val);
    return *this;
}
template< typename T , typename Alloc>
ky_linked<T, Alloc> ky_linked<T, Alloc>::operator +(const ky_linked<T, Alloc> &rhs) const
{
    ky_linked<T, Alloc> retlist(*this);
    foreach (const T &var , rhs)
        retlist.push_back(var);
    return retlist;
}
template< typename T , typename Alloc>
ky_linked<T, Alloc> &ky_linked<T, Alloc>::operator << (const T &val)
{
    *this += val;
    return *this;
}
template< typename T , typename Alloc>
ky_linked<T, Alloc> &ky_linked<T, Alloc>::operator <<(const ky_linked<T, Alloc> &rhs)
{
    *this += rhs;
    return *this;
}


template< typename T , typename Alloc>
bool ky_linked<T, Alloc>::is_empty() const
{
    return count() == 0 && begin() == end();
}

template< typename T , typename Alloc>
int64 ky_linked<T, Alloc>::count() const
{
    return impl->count;
}

template< typename T , typename Alloc>
void ky_linked<T, Alloc>::assign(int64 inx, const T &x )
{
    replace(inx, x);
}

template< typename T , typename Alloc>
bool ky_linked<T, Alloc>::operator ==(const ky_linked<T, Alloc> &rhs) const
{
    if (header()->count != rhs.count())
        return false;
    if (list == rhs.list)
        return true;

    list_node *i = list->next;
    list_node *irhs = rhs.list->next;
    while (i != list)
    {
        if (! (i->data == irhs->data))
            return false;
        i = i->next;
        irhs = irhs->next;
    }
    return true;
}
template< typename T , typename Alloc>
void ky_linked<T, Alloc>::append(const T& val)
{
    detach();
    list_node *nnode = newnode(val);
    nnode->next = list;
    nnode->prev = list->prev;
    nnode->prev->next = list->prev = nnode;
    ++header()->count;
}

template< typename T , typename Alloc>
void ky_linked<T, Alloc>::append(const ky_linked<T, Alloc>& val)
{
    foreach (const T &var, val)
        append(var);
}
template< typename T , typename Alloc>
void ky_linked<T, Alloc>::prepend(const T& val)
{
    detach();
    list_node *nnode = newnode(val);
    nnode->data = val;
    nnode->next = list->next;
    nnode->prev = list;
    nnode->next->prev = list->next = nnode;
    ++header()->count;
}
template< typename T , typename Alloc>
void ky_linked<T, Alloc>::prepend(const ky_linked<T, Alloc>& val)
{
    foreach (const T &var, val)
        prepend(var);
}

template< typename T , typename Alloc>
void ky_linked<T, Alloc>::replace(int64 i, const T &t)
{
    detach();
    list_node *n = (this->begin() +i).ope;
    n->data = t;
}
template< typename T , typename Alloc>
void ky_linked<T, Alloc>::move(int64 inx, int64 to)
{
    if (inx == to)
        return ;

    detach();
    list_node *pinx = (this->begin() +inx).ope;
    list_node *toinx = (this->begin() +to).ope;

    if (pinx != list && toinx != list)
    {
        // 断开原来的
        pinx->next->prev = pinx->prev;
        pinx->prev->next = pinx->next;
        // 修改自身的指向
        pinx->next= toinx;
        pinx->prev = toinx->prev;

        // 修改原来位置的指向
        toinx->prev->next = pinx;
        toinx->prev = pinx;
    }
}

template< typename T , typename Alloc>
void ky_linked<T, Alloc>::swap(iterator x, iterator y)
{
    int64 posx = x - begin(), posy = y - begin();
    swap(posx, posy);
}

template< typename T , typename Alloc>
void ky_linked<T, Alloc>::remove(int64 pos)
{
    detach();
    pos = ky_min(pos, size()-1);
    list_node *p = (begin()+pos).ope;

    if (p != list)
    {
        p->next->prev = p->prev;
        p->prev->next = p->next;
        freenode(p);
        -- header()->count;
    }
}

template< typename T , typename Alloc>
void ky_linked<T, Alloc>::swap(int64 i, int64 j)
{
    detach();

    list_node *x = (this->begin() +ky_bound(int64(0), ky_min(i, j), size()-1)).ope;
    list_node *y = (this->begin() +ky_bound(int64(0), ky_max(i, j), size()-1)).ope;
    if (x == y)
        return ;

    x->prev->next = y;
    y->prev->next = x;

    list_node *tmp = x->next;
    x->next = y->next;
    y->next = tmp;
    tmp = x->prev;
    x->prev = y->prev;
    y->prev = tmp;
}
template< typename T , typename Alloc>
bool ky_linked<T, Alloc>::contains(const T &t) const
{
    list_node *i = list;
    while ((i = i->next) != list)
        if (i->data == t)
            return true;
    return false;
}
template< typename T , typename Alloc>
int ky_linked<T, Alloc>::find(const T &t) const
{
    int fi = 0;
    list_node *i = list;
    while ((i = i->next) != list)
    {
        fi++;
        if (i->data == t)
            return fi;
    }
    return -1;
}
template< typename T , typename Alloc>
typename ky_linked<T, Alloc>::opelist_node ky_linked<T, Alloc>::opelist_node::operator +(int c)
{
    opelist_node node = *this;
    if (c > 0)
        while (c--) ++node;
    else
        while (c++) --node;
    return (node);
}
template< typename T , typename Alloc>
const typename ky_linked<T, Alloc>::opelist_node ky_linked<T, Alloc>::opelist_node::operator +(int c)const
{
    opelist_node node = *this;
    if (c > 0)
        while (c--) ++node;
    else
        while (c++) --node;
    return (node);
}

template< typename T , typename Alloc>
const typename ky_linked<T, Alloc>::opelist_node& ky_linked<T, Alloc>::opelist_node::operator++()const
{
    opelist_node *take = (opelist_node *)this;
    take->ope = ope->next;
    return *this;
}
template< typename T , typename Alloc>
const typename ky_linked<T, Alloc>::opelist_node ky_linked<T, Alloc>::opelist_node::operator++(int)const
{
    opelist_node old = *this;
    ope = ope->next;
    return old;
}
template< typename T , typename Alloc>
const typename ky_linked<T, Alloc>::opelist_node& ky_linked<T, Alloc>::opelist_node::operator--()const
{
    opelist_node *take = (opelist_node *)this;
    take->ope = ope->prev;
    return *this;
}
template< typename T , typename Alloc>
const typename ky_linked<T, Alloc>::opelist_node ky_linked<T, Alloc>::opelist_node::operator--(int)const
{
    opelist_node old = *this;
    ope = ope->prev;
    return old;
}
template< typename T , typename Alloc>
typename ky_linked<T, Alloc>::opelist_node& ky_linked<T, Alloc>::opelist_node::operator++()
{
    ope = ope->next;
    return *this;
}
template< typename T , typename Alloc>
typename ky_linked<T, Alloc>::opelist_node ky_linked<T, Alloc>::opelist_node::operator++(int)
{
    opelist_node old = *this;
    ope = ope->next;
    return old;
}
template< typename T , typename Alloc>
typename ky_linked<T, Alloc>::opelist_node& ky_linked<T, Alloc>::opelist_node::operator--()
{
    ope = ope->prev;
    return *this;
}
template< typename T , typename Alloc>
typename ky_linked<T, Alloc>::opelist_node ky_linked<T, Alloc>::opelist_node::operator--(int)
{
    opelist_node old = *this;
    ope = ope->prev;
    return old;
}
template< typename T , typename Alloc>
int ky_linked<T, Alloc>::opelist_node::operator -(const opelist_node &rhs)const
{
    ky_linked<T, Alloc>::opelist_node *take = (ky_linked<T, Alloc>::opelist_node*)this;
    int retval = 0;
    list_node * nt = ope;
    forever (ope != NULL && ope != rhs.ope)
    {
        this->operator --();
        ++retval;
    }
    if (ope == NULL)
    {
        take->ope = nt;
        retval = 0;
        forever (ope != NULL && ope != rhs.ope)
        {
            this->operator ++();
            --retval;
        }
    }
    take->ope = nt;
    return retval;
}


#endif // KY_LINKED_INL
