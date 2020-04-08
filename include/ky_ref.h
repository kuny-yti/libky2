
/**
 * Basic tool library
 * Copyright (C) 2014 kunyang kunyang.yk@gmail.com
 *
 * @file     ky_ref.h
 * @brief   引用计数类
 *       引用计数器，对象间的赋值及使用的基本管理
 *
 * @author   kunyang
 * @email    kunyang.yk@gmail.com
 * @version  1.0.1.1
 * @date     2012/01/02
 * @license  GNU General Public License (GPL)
 *
 * Change History :
 *    Date    |  Version  |  Author  |   Description
 * 2012/01/02 | 1.0.0.1   | kunyang  | 创建文件并实现单例对象
 * 2020/03/27 | 1.0.1.0   | kunyang  | 将类分离为单独文件
 */

#ifndef KY_REF_H
#define KY_REF_H

#include "ky_define.h"

///< 具备引用计数功能
#ifndef kyHasReferCount
# error "not enable kyHasReferCount macro!"
#else
#include "arch/ky_atomic.h"

/*!
 * \brief The ky_ref struct
 *       使用时需要将ky_ref和被标示引用者绑定到一个对象指针中，
 *       以免修改时其他引用者未修改实际引用数
 */
struct ky_ref
{
    /*!
     * \brief 引用标志定义
     */
    enum eRefFlags
    {
        Static = -1,                          ///< 静态模式，不支持引用
        Shareable = 0,                        ///< 可共享模式，用于对象间引用
        One = 1,                              ///< 将当前引用置为1，
        ShareableOne = Shareable | One,       ///< 可共享模式引用置为1，
        Detach = 0x40000000,                  ///< 支持分离
        Mask = 0xbfffffff,                    ///< 标志提取掩码
        ShareableDetach = Shareable | Detach, ///< 可共享且可分离
        OneDetach = Detach | One,             ///< 可共享、可分离并且将引用置为1
    };

    ky_ref(){ref.store(OneDetach);}
    explicit ky_ref(eRefFlags flag){ref.store (flag);}
    ~ky_ref(){}

    inline ky_ref &operator = (const ky_ref&rhs)
    {
        ref = rhs.ref;
        return *this;
    }

    //!
    //! \brief count 返回引用的次数
    //! \return
    //!
    inline int count()const {return (ref & int(Mask));}
    inline operator int()const{return (ref & int(Mask));}
    inline operator int(){return (ref & int(Mask));}
    //!
    //! \brief set 设置引用标志
    //! \param flag
    //!
    inline void set(eRefFlags flag = Static)
    {ref.store ((int)flag, Fence_Release);}
    inline ky_ref & operator = (eRefFlags flags)
    {ref.store ((int)flags, Fence_Release);return *this;}
    //!
    //! \brief is_shared 返回是否有引用共享
    //! \return
    //!
    inline bool is_shared()const
    {
        return ((ref & int(Mask)) > (int)Shareable);
    }
    //!
    //! \brief is_static 返回是否为静态引用
    //! \return
    //!
    inline bool is_static()const{return ref == (int)Static;}
    //!
    //! \brief has_shareable 返回是否启用引用共享
    //! \return
    //!
    inline bool has_shareable()const
    {
        return (ref & int(Mask)) >= (int)Shareable;
    }

    //!
    //! \brief has_detach 返回是否支持分离
    //! \return
    //!
    inline bool has_detach()const{return ref & (int)Detach;}

    //!
    //! \brief addref 添加引用计数
    //!
    inline void addref() {if (has_shareable()) ++ref;}
    //!
    //! \brief lessref 释放引用计数
    //! \return true 无引用
    //!
    inline bool lessref()
    {
        if (!(ref & int(Mask)) || (--ref) < 0)
            return true;
        return false;
    }
    //!
    //! \brief is_destroy 返回是否需要释放对象
    //! \return true 可销毁
    //!
    inline bool is_destroy()
    {
        if (!(ref & int(Mask)) || ref < 0)
            return true;
        return false;
    }
    inline ky_ref &operator ++(){++ref;return *this;}
    inline ky_ref operator ++(int){ky_ref tret = *this; ref++;return tret;}
    inline ky_ref &operator --(){--ref; return *this;}
    inline ky_ref operator --(int){ky_ref tret = *this;ref--; return tret;}

    ky_atomic<int> ref;
};

#endif

#endif
