
/**
 * Basic tool library
 * Copyright (C) 2014 kunyang kunyang.yk@gmail.com
 *
 * @file     ky_flags.h
 * @brief    装配枚举类型为旗语标志
 *
 * @author   kunyang
 * @email    kunyang.yk@gmail.com
 * @version  1.0.1.1
 * @date     2018/08/01
 * @license  GNU General Public License (GPL)
 *
 * Change History :
 *    Date    |  Version  |  Author  |   Description
 * 2018/08/01 | 1.0.0.1   | kunyang  | 创建文件
 *
 */
#ifndef KY_FLAGS_H
#define KY_FLAGS_H

#include "ky_define.h"

#define kyDeclareFlags(T, N) \
    typedef ky_flags<T> N

//!
//! \brief The ky_flag class 将枚举类型包装
//!
template<typename Enum>
class ky_flags
{
    kyCompilerAssert(sizeof(Enum) <= sizeof(int));
    kyCompilerAssert(is_enum<Enum>());
public:
    ky_flags(){flags = 0;}
    ky_flags(Enum e){flags = e;}
    ky_flags(int e){flags = e;}

    inline ky_flags &operator &= (int mask)   {flags &= mask; return *this;}
    inline ky_flags &operator &= (Enum mask)  {flags &= (int)mask; return *this;}
    inline ky_flags &operator |= (ky_flags f) {flags |= f.flags; return *this;}
    inline ky_flags &operator |= (Enum f)     {flags |= (int)f; return *this;}
    inline ky_flags &operator ^= (ky_flags f) {flags ^= f.flags; return *this;}
    inline ky_flags &operator ^= (Enum f)     {flags ^= (int)f; return *this;}

    inline operator int() const  {return flags;}

    inline ky_flags operator |(ky_flags f) const {return ky_flags(flags | f.flags);}
    inline ky_flags operator |(Enum f) const     {return ky_flags(flags | (int)f);}
    inline ky_flags operator ^(ky_flags f) const {return ky_flags(flags ^ f.i);}
    inline ky_flags operator ^(Enum f) const     {return ky_flags(flags ^ (int)f);}
    inline ky_flags operator &(int mask) const   {return ky_flags(flags & mask);}
    inline ky_flags operator &(Enum f) const     {return ky_flags(flags & (int)f);}
    inline ky_flags operator ~() const           {return ky_flags(~flags);}

    inline bool operator !() const  {return !flags;}

    inline bool operator == (Enum f) const
    {
        return ((flags & (int)f) == (int)f) &&
               ((int)f != 0 || flags == (int)f);
    }
    inline bool operator == (ky_flags f) const
    {
        return flags == f.flags;
    }

    inline ky_flags &set(Enum f, bool on = true)
    {
        return on ? (*this |= f) : (*this &= ~f);
    }
    inline bool is(Enum f)
    {
        return (*this & f) == f;
    }

private:
    int  flags;
};

#endif // KY_FLAG_H
