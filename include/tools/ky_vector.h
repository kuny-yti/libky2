/**
 * Basic tool library
 * Copyright (C) 2014 kunyang kunyang.yk@gmail.com
 *
 * @file     ky_vector.h
 * @brief    模板连续数据空间容器类(支持C++11)
 *       ky_vector 类主要从ky_array 继承而来，主要加入STL接口和迭代接口
 *
 * @author   kunyang
 * @email    kunyang.yk@gmail.com
 * @version  1.0.1.5
 * @date     2010/05/08
 * @license  GNU General Public License (GPL)
 *
 * Change History :
 *    Date    |  Version  |  Author  |   Description
 * 2010/05/08 | 1.0.0.1   | kunyang  | 创建文件
 * 2010/08/10 | 1.0.1.0   | kunyang  | 将原有继承std::vector完成类修改为ky_array
 * 2012/04/02 | 1.0.1.2   | kunyang  | 将迭代接口进行重新编写
 * 2014/03/09 | 1.0.1.5   | kunyang  | 加入C++11支持
 */

#ifndef VECTOR_H
#define VECTOR_H

#include "ky_define.h"
#include "ky_array.h"
#include "ky_algorlthm.h"

#ifdef kyHasSTL
#include <vector>
#endif

/*!
 * \brief The ky_vector class
 */
template <typename T >
class ky_vector : public ky_array<T>
{
    typedef ky_array<T> VecBase;
public:
    class opevec_node;
    typedef opevec_node  iterator;
    typedef const iterator  const_iterator;

    // STL
public:
    ky_vector();
    explicit ky_vector(int64 count);
    ky_vector(const ky_vector& rhs);
    ~ky_vector(){}

    ky_vector& operator =(const ky_vector& rhs);

    iterator begin();
    iterator end();
    const_iterator begin()const;
    const_iterator end()const;
    inline const_iterator cbegin()const{return begin();}
    inline const_iterator cend()const{return end();}

    bool empty() const;

    T &front();
    const T &front()const;

    T &back();
    const T &back()const;

    void push_back(const T& v);
    inline void push_front(const T& v){prepend(v);}
    void pop_back();
    inline void pop_front(){remove(0);}

#if kyLanguage < kyLanguage11
    iterator insert(iterator pos, const T& v);
    void insert(iterator pos, int64 count, const T& v);

    iterator erase(iterator pos);
    iterator erase(iterator first, iterator last);
#endif

    // C++11
#if kyLanguage >= kyLanguage11
public:
    ky_vector(ky_vector&& rhs);
    ky_vector(const std::initializer_list<T> & il);
    void push_back(T&& v);

    iterator insert(const_iterator pos, const T& v);
    iterator insert(const_iterator pos, T&& v);
    iterator insert(const_iterator pos, int64 count, const T& v);

    iterator erase(const_iterator pos);
    iterator erase(const_iterator first, const_iterator last);
#endif

    // base
public:
    ky_vector(const std::vector<T> &rhs);

    std::vector<T> to_std();
    void form(const std::vector<T> &rhs);
    ky_vector& operator =(const std::vector<T>& rhs);

    inline bool operator!=(const ky_vector<T> &v) const { return !(*this == v); }
    bool operator==(const ky_vector<T> &v) const;
    bool contains(const T &t) const;

    inline ky_vector<T> &operator +=(const ky_vector<T> &l) {return append(l.data(), l.size());}
    inline ky_vector<T> operator +(const ky_vector<T> &l) const { ky_vector n = *this; n += l; return n; }
    inline ky_vector<T> &operator +=(const T &t) { append(t); return *this; }
    inline ky_vector<T> &operator << (const T &t) { append(t); return *this; }
    inline ky_vector<T> &operator <<(const ky_vector<T> &l) { *this += l; return *this; }

    int64 count()const;

    ky_vector<T> &prepend(T c);
    ky_vector<T> &prepend(const T *s, int len);
    ky_vector<T> &prepend(const ky_vector<T> &a);

    ky_vector<T> &append(T c);
    ky_vector<T> &append(const T *s, int64 len);
    ky_vector<T> &append(const ky_vector<T> &a);

    ky_vector<T> &insert(int64 i, T c);
    ky_vector<T> &insert(int64 i, const T *s, int64 len);
    ky_vector<T> &insert(int64 i, const ky_vector<T> &a);

    void remove(int64 index, int64 len = 1);

    ky_vector<T> &replace(int64 index, int64 len, const T *s, int64 alen);
    ky_vector<T> &replace(int64 index, int64 len, const ky_vector<T> &s);

    inline ky_vector<T> mid(int64 pos, int len) const {return extract(pos, len);}
    ky_vector<T> extract( int pos, int count);
    ky_vector<T> extract( int pos );
    ky_vector<T> start( int count);
    ky_vector<T> end( int count);

    void sort(){ky_qsort<iterator>(begin(), end());}
    //!
    //! \brief swap 覆盖ky_array后重写
    //! \param rhs
    //!
    void swap(ky_vector<T> &rhs);
    friend void ky_swap(ky_vector<T> &a, ky_vector<T> &b) {a.swap(b);}

public:
    class opevec_node
    {
        friend class ky_vector;
    public:

        opevec_node():ope(NULL){}
        opevec_node(T *p): ope(p){}
        opevec_node(const opevec_node &ope_v): ope(ope_v.ope){}
        ~opevec_node(){}

        inline T* operator->(){return &ope;}
        inline T& operator*() {return *ope;}
        inline const T& operator*() const{return *ope;}

        inline bool operator ==(const opevec_node &rhs) const { return ope == rhs.ope; }
        inline bool operator !=(const opevec_node &rhs) const { return ope != rhs.ope; }
        inline bool operator <(const opevec_node& rhs) const { return ope < rhs.ope; }
        inline bool operator <=(const opevec_node& rhs) const { return ope <= rhs.ope; }
        inline bool operator >(const opevec_node& rhs) const { return ope > rhs.ope; }
        inline bool operator >=(const opevec_node& rhs) const { return ope >= rhs.ope; }

        opevec_node operator +(int c);
        const opevec_node operator +(int c)const;
        inline opevec_node operator -(int c){return operator+(-c);}
        inline const opevec_node operator -(int c)const{return operator+(-c);}

        int operator - (const opevec_node &rhs);

        const opevec_node& operator++()const;
        const opevec_node operator++(int)const;

        const opevec_node& operator--()const;
        const opevec_node operator--(int)const;

        opevec_node& operator++();
        opevec_node operator++(int);

        opevec_node& operator--();
        opevec_node operator--(int);

        inline opevec_node &operator +=(int c){return *this = *this + c; }
        inline const opevec_node &operator +=(int c)const{return *this = *this + c; }
        inline opevec_node &operator -=(int c){return *this = *this - c; }
        inline const opevec_node &operator -=(int c)const{return *this = *this - c; }

    private:
        T *ope;
    };
};

#include "ky_vector.inl"
#endif // VECTOR_H
