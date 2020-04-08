

template <int Count, typename T>
__bit_bridge<Count, T> &__bit_bridge<Count, T>::operator = (bool b)
{
    if (b)
        v |= ((1 << i) & ~(0));
    else
        v &= ~((1 << i) & ~(0));
    return *this;
}
template <int Count, typename T>
bool __bit_bridge<Count, T>::operator == (bool b)const
{
    return ((v >> i) & 0x01) == b;
}
template <int Count, typename T>
bool __bit_bridge<Count, T>::operator != (bool b)const
{
    return ((v >> i) & 0x01) != b;
}


template <int Count, typename T>
ky_bitset<Count, T>::ky_bitset(T v)
{
    mu[0].i = 0;
    mu[0].v = v;
}

template <int Count, typename T>
ky_bitset<Count, T>::ky_bitset(eBinarys v)
{
    mu[0].i = 0;
    mu[0].v = (T)v;
}
template <int Count, typename T>
ky_bitset<Count, T>::ky_bitset(const ky_binary &v)
{
    form_string (v.v);
}

template <int Count, typename T>
ky_bitset<Count, T>::ky_bitset(const ky_bitset<Count, T> &rhs)
{
    *this = rhs;
}

template <int Count, typename T>
ky_bitset<Count, T> &ky_bitset<Count, T>::operator = (const eBinarys &rhs)
{
    mu[0].i = 0;
    mu[0].v = rhs;
    return *this;
}

template <int Count, typename T>
ky_bitset<Count, T> &ky_bitset<Count, T>::operator = (const ky_binary &v)
{
    form_string (v.v);
    return *this;
}

template <int Count, typename T>
ky_bitset<Count, T>  &ky_bitset<Count, T>::operator = (const ky_bitset<Count, T> &rhs)
{
    for (int i = 0; i < __group; ++i)
    {
        mu[i].i = 0;
        mu[i].v = rhs.mu[i].v;
    }
    return *this;
}

template <int Count, typename T>
void ky_bitset<Count, T>::__convert_array (const __bridge_t *mu,uint8 *arr)
{
    for (int i = 0; i < Count; ++i)
    {
        const uint8 x = i & __smod;
        const uint8 u = i >> __shift;
        arr[i] = (mu[u].v >> x) & 0x01;
    }
}
template <int Count, typename T>
void ky_bitset<Count, T>::__convert_bridge (__bridge_t *mu, uint8 *arr, int idx)
{
    const int cut = idx > 0 ? Count - idx : Count;
    const int sut = idx < 0 ? -idx : 0;
    for (int i = sut ; i < cut; ++i)
    {
        const uint8 x = i & __smod;
        const uint8 u = i >> __shift;
        mu[u].v |= (arr[i] << x);
    }
}

template <int Count, typename T>
ky_bitset<Count, T> ky_bitset<Count, T>::operator & (const ky_bitset<Count, T> &rhs)const
{
    ky_bitset<Count, T> out;
    for (int i = 0; i < __group; ++i)
        out.mu[i].v = mu[i].v & rhs.mu[i].v;
    return out;
}
template <int Count, typename T>
ky_bitset<Count, T> ky_bitset<Count, T>::operator | (const ky_bitset<Count, T> &rhs)const
{
    ky_bitset<Count, T> out;
    for (int i = 0; i < __group; ++i)
        out.mu[i].v = mu[i].v | rhs.mu[i].v;
    return out;
}
template <int Count, typename T>
ky_bitset<Count, T> ky_bitset<Count, T>::operator ~ ()const
{
    ky_bitset<Count, T> out;
    for (int i = 0; i < __group; ++i)
        out.mu[i].v = ~mu[i].v;
    return out;
}
template <int Count, typename T>
ky_bitset<Count, T> ky_bitset<Count, T>::operator ^ (const ky_bitset<Count, T> &rhs)const
{
    ky_bitset<Count, T> out;
    for (int i = 0; i < __group; ++i)
        out.mu[i].v = mu[i].v ^ rhs.mu[i].v;
    return out;
}
template <int Count, typename T>
ky_bitset<Count, T> &ky_bitset<Count, T>::operator &= (const ky_bitset<Count, T> &rhs)
{
    for (int i = 0; i < __group; ++i)
        mu[i].v &= rhs.mu[i].v;
    return *this;
}
template <int Count, typename T>
ky_bitset<Count, T> &ky_bitset<Count, T>::operator |= (const ky_bitset<Count, T> &rhs)
{
    for (int i = 0; i < __group; ++i)
        mu[i].v |= rhs.mu[i].v;
    return *this;
}
template <int Count, typename T>
ky_bitset<Count, T> &ky_bitset<Count, T>::operator ~ ()
{
    for (int i = 0; i < __group; ++i)
        mu[i].v = ~mu[i].v;
    return *this;
}
template <int Count, typename T>
ky_bitset<Count, T> &ky_bitset<Count, T>::operator ^= (const ky_bitset<Count, T> &rhs)
{
    for (int i = 0; i < __group; ++i)
        mu[i].v ^= rhs.mu[i].v;
    return *this;
}
template <int Count, typename T>
ky_bitset<Count, T> ky_bitset<Count, T>::operator << (uint b)const
{
    ky_bitset<Count, T> out;
    if (b < Count)
    {
        uint8 _bool[Count];
        __convert_array(mu, _bool);
        memmove(&_bool[b], _bool, Count - b);
        __convert_bridge(out.mu, _bool, -b);
    }

    return out;
}
template <int Count, typename T>
ky_bitset<Count, T> ky_bitset<Count, T>::operator >> (uint b)const
{
    ky_bitset<Count, T> out;
    if (b < Count)
    {
        uint8 _bool[Count];
        __convert_array(mu, _bool);
        memmove(_bool, &_bool[b], Count - b);
        __convert_bridge(out.mu, _bool, b);
    }
    return out;
}
template <int Count, typename T>
ky_bitset<Count, T> &ky_bitset<Count, T>::operator <<= (uint b)
{
    ky_bitset<Count, T> x = *this << b;
    *this = x;
    return *this;
}
template <int Count, typename T>
ky_bitset<Count, T> &ky_bitset<Count, T>::operator >>= (uint b)
{
    ky_bitset<Count, T> x = *this >> b;
    *this = x;
    return *this;
}
template <int Count, typename T>
bool ky_bitset<Count, T>::operator || (const ky_bitset<Count, T> &rhs)const
{
    return any() || rhs.any();
}
template <int Count, typename T>
bool ky_bitset<Count, T>::operator && (const ky_bitset<Count, T> &rhs)const
{
    return any() && rhs.any();
}
template <int Count, typename T>
bool ky_bitset<Count, T>::operator ! ()const
{
    return !any();
}

template <int Count, typename T>
bool ky_bitset<Count, T>::operator == (const ky_bitset<Count, T> &rhs)
{
    bool ret = true;
    for (int i = 0; i < __group; ++i)
        ret &= mu[i].v == rhs.mu[i].v;
    return ret;
}
template <int Count, typename T>
bool ky_bitset<Count, T>::operator != (const ky_bitset<Count, T> &rhs)
{
    return !this->operator == (rhs);
}
template <int Count, typename T>
typename ky_bitset<Count, T>::__bridge_t &ky_bitset<Count, T>::operator [](uint bit)
{
    static __bridge_t sv;
    if (bit < Count)
    {
        const uint8 x = bit & __smod;
        const uint8 u = bit >> __shift;
        mu[u].i = x;
        return mu[u];
    }
    return sv;
}
template <int Count, typename T>
typename ky_bitset<Count, T>::__bridge_t ky_bitset<Count, T>::operator [](uint bit)const
{
    __bridge_t sv;
    if (bit < Count)
    {
        const uint8 x = bit & __smod;
        const uint8 u = bit >> __shift;
        sv.i = x;
        sv.v = mu[u].v;
        return sv;
    }
    return sv;
}
template <int Count, typename T>
void ky_bitset<Count, T>::set()
{
    for (int i = 0; i < __group; ++i)
        mu[i].v = ~(0);
}
template <int Count, typename T>
void ky_bitset<Count, T>::set(uint bit)
{
    this->operator [] (bit) = True;
}
template <int Count, typename T>
void ky_bitset<Count, T>::unset()
{
    for (int i = 0; i < __group; ++i)
        mu[i].v = 0;
}
template <int Count, typename T>
void ky_bitset<Count, T>::unset(uint bit)
{
    this->operator [] (bit) = False;
}
template <int Count, typename T>
bool ky_bitset<Count, T>::any()const
{
    bool ret = false;
    for (int i = 0; i < __group; ++i)
        ret |= mu[i].v & ~(0);
    return ret;
}
template <int Count, typename T>
bool ky_bitset<Count, T>::none()const
{
    return !any();
}
template <int Count, typename T>
int ky_bitset<Count, T>::count()const
{
    int num = 0;
    for (int i = 0; i < Count; ++i)
        if (this->operator [] (i) == True)
            num++;
    return num;
}

template <int Count, typename T>
void ky_bitset<Count, T>::flip()
{
    this->operator ~ ();
}
template <int Count, typename T>
void ky_bitset<Count, T>::flip(uint bit)
{
    if (this->operator [] (bit))
        unset(bit);
    else
        set(bit);
}

template <int Count, typename T>
void ky_bitset<Count, T>::swap(ky_bitset &b)
{
    for (int i = 0; i < __group; ++i)
        ky_swap(mu[i].v, b.mu[i].v);
}
template <int Count, typename T>
T ky_bitset<Count, T>::extract(uint lo_idx, uint hi_idx)const
{
    if (lo_idx > hi_idx)
        return T(0);
    if (hi_idx > Count)
        hi_idx = Count-1;
    const uint nbit = (hi_idx - lo_idx)+1;
    if (nbit > __sbit)
        return T(0);
    ky_bitset<Count, T> nbs = *this >> lo_idx;
    return T(nbs.mu[0].v & bit_mask<T>(nbit));
}
template <int Count, typename T>
template <int nCount> ky_bitset<nCount, T> ky_bitset<Count, T>::extract (uint lo_idx, uint hi_idx)const
{
    if (lo_idx > hi_idx)
        return ky_bitset<nCount, T>();
    if (hi_idx > Count)
        hi_idx = Count-1;

    const uint nbit = (hi_idx - lo_idx)+1;

    ky_bitset<nCount, T> result;
    uint8 _bool[Count];
    __convert_array(mu, _bool);
    memmove(_bool, &_bool[lo_idx], nbit);
    result.__convert_bridge(result.mu, _bool);
    return result;
}
template <int Count, typename T>
template <typename U>
U ky_bitset<Count, T>::bit_mask(uint num)
{
    const U bitInT = sizeof(U)*sizeof(char);
    if(num >= bitInT)
        return (U)~U(0);
    return (U)((U(1) << num)-1);
}
template <int Count, typename T>
template<typename U>
inline U ky_bitset<Count, T>::bit_extract(U num, uint lo_idx, uint hi_idx)
{
    const U numBits = (hi_idx - lo_idx)+1;
    return U(U(num >> lo_idx) & bit_mask<U>(numBits));
}

template <int Count, typename T>
ky_string ky_bitset<Count, T>::to_string()const
{
    ky_string out("b");
    out.from (val.v, 2);
    return out;
}

template <int Count, typename T>
void ky_bitset<Count, T>::form_string(const ky_string &s)
{
    ky_string tmp(s);
    if (tmp[0] == ky_char('b'))
        tmp.remove(0,1);
    else if (tmp.is_empty ())
        return ;
    val.v = tmp.to_ulonglong (2);
}

