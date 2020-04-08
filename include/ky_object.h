
/**
 * Basic tool library
 * Copyright (C) 2014 kunyang kunyang.yk@gmail.com
 *
 * @file     ky_object.h
 * @brief    支持托管及信号槽模式的连接继承对象，并管理子父对象
 *       1.支持类的RTTI.
 *       2.可完成信号槽模式的对象间调用机制
 *       3.支持子父对象的管理及遍历
 *       4.支持对象间的引用或指针间的引用机制
 *
 * @author   kunyang
 * @email    kunyang.yk@gmail.com
 * @version  1.3.2.1
 * @date     2011/10/11
 * @license  GNU General Public License (GPL)
 *
 * Change History :
 *    Date    |  Version  |  Author  |   Description
 * 2011/10/11 | 1.0.0.1   | kunyang  | 创建文件实现空对象类
 * 2011/10/20 | 1.0.1.0   | kunyang  | 完善对象类实现
 * 2012/03/13 | 1.1.0.1   | kunyang  | 添加类的RTTI机制
 * 2012/07/27 | 1.1.1.1   | kunyang  | 修改对象类的子父对象管理
 * 2014/05/08 | 1.2.0.1   | kunyang  | 添加指针间引用计数机制
 * 2015/02/28 | 1.2.1.0   | kunyang  | 修改指针间引用机制为原子操作模式
 * 2016/03/08 | 1.3.0.0   | kunyang  | 实现信号槽调用机制
 * 2016/05/14 | 1.3.1.0   | kunyang  | 修改引用计数机制并兼容对象类
 * 2017/04/12 | 1.3.2.0   | kunyang  | 修改字符对象管理机制并重构信号槽机制
 * 2018/03/26 | 1.3.2.1   | kunyang  | 将模板对象是否为继承ky_object进行枚举定义
 * 2018/06/17 | 1.3.3.0   | kunyang  | 修改对象线程安全继承
 * 2018/11/08 | 1.4.0.1   | kunyang  | 加入线程底层事件派遣接口
 * 2019/04/20 | 1.4.0.2   | kunyang  | 将模板对象实现写入inl文件中
 */
#ifndef KY_OBJECT_H
#define KY_OBJECT_H

#include "ky_define.h"
#include "ky_flags.h"
#include "tools/ky_typeinfo.h"
#include "tools/ky_string.h"
#include "tools/ky_list.h"
#include "tools/ky_map.h"
#include "tools/ky_signal.h"
#include "thread/ky_thread.h"
#include "interface/ievent.h"

namespace impl {class object;}

//!
//! \brief The eIoNotifys enum Io的通知
//!
typedef enum
{
    Notify_Not    = 0,

    Notify_Read   = 0x01,             ///< 通知读事件
    Notify_Write  = 0x02,             ///< 通知写事件
    Notify_Accept = 0x04,             ///< 通知连接事件
    Notify_Close  = 0x08,             ///< 通知关闭事件

    Notify_Msg    = 0x80,             ///< 消息通知
    Notify_Io     = 0x40,             ///< Io通知(异步IO、套接字、非阻塞IO)
    Notify_Timer  = 0x20,             ///< 计时器
    Notify_Signal = 0x10,             ///< 信号通知

    Notify_Always = 0x200,            ///< 总是通知
    Notify_Socket = 0x100 | Notify_Io,///< 套接字Io通知
}eIoNotifys;
kyDeclareFlags(eIoNotifys, eNotifyFlags);

#define kyObject(type) public:\
    virtual ky_string name() const{return ky_typeinfo<type>();} \
    virtual bool is(ky_object *rhs)const \
    {\
        type *tmps = kyAsCast(type *)(rhs);\
        if (tmps != 0)\
            return true;\
        return false; \
    }private:

class ky_object : public ky_threadsafe
{
    kyObject(ky_object)

    template<typename T>
    friend class ky_ptrref;
    friend class thread_layer;

protected:
    //!
    //! \brief 由ky_ptrref引用者管理对象的生存周期
    //!
    void lessref();
    void addref();

public:
    explicit ky_object(ky_object *parent = 0);

    //!
    //! \brief as 对象转换为继承对象
    //! \return
    //!
    template <typename T>
    T* as() {return kyAsCast(T *)(this);}

    //!
    //! \brief thread 获取对象所在的线程
    //! \return
    //!
    ky_thread *thread() const;

    //!
    //! \brief move_thread 对象移到其他线程
    //! \param t
    //! \return
    //!
    bool move_thread(ky_thread *t);

    //!
    //! \brief event 继承者可接收事件派遣、通知的到达
    //! \param e
    //! \return
    //!
    virtual bool event(ievent *e);

    //!
    //! \brief registered 注册通知
    //! \param fd
    //! \param mode
    //! \return
    //!
    void registered(intptr fd, eNotifyFlags flag = Notify_Not);
    //!
    //! \brief unregister 注销通知
    //! \param fd
    //!
    void unregister(intptr fd);
    //!
    //! \brief modify 修改通知标志
    //! \param fd
    //! \param flag
    //! \param active
    //!
    void modify(intptr fd, eNotifyFlags flag, bool active);

public:
    typedef ky_list <ky_object*>::iterator child;

    //!
    //! \brief child_count 返回本对象下的子对象数
    //! \return
    //!
    uint child_count()const;
    //!
    //! \brief parent 返回对象的父对象
    //! \return
    //!
    ky_object *parent()const;
    //!
    //! \brief set_parent 设置父对象
    //! \param p
    //!
    void set_parent(const ky_object *p);

protected:
    virtual ~ky_object();

protected:
    //!
    //! \brief append 添加子对象
    //! \param obj
    //!
    void append(ky_object *obj);
    //!
    //! \brief remove 删除子对象
    //! \param it
    //!
    void remove(const child &it);
    void remove(ky_object *obj);
    //!
    //! \brief begin 子对象的开始节点
    //! \return
    //!
    child begin();
    const child begin()const;
    //!
    //! \brief end 子对象的结束节点
    //! \return
    //!
    child end();
    const child end()const;

private:
    impl::object *impl;

private:
    typedef typename ky_map<intptr, int>::iterator slot;
    ky_map<intptr, int> slot_map;

public:
#include "ky_object.inl"
};


template<typename T>
class ky_ptrref
{
    enum {Derived = (typeid(T) == typeid(ky_object))};
public:
    ky_ptrref():ptr(0), ref(0){}
    ky_ptrref (T *p, ky_ref::eRefFlags c = ky_ref::Shareable);

    ky_ptrref(const ky_ptrref<T> &rhs);

#if kyLanguage >= kyLanguage11
    ky_ptrref(ky_ptrref<T> &&rhs);
    ky_ptrref<T>& operator =(ky_ptrref<T>&& rhs);
#endif

    //! 其他类到本类转换
    template <typename U>
    ky_ptrref(const ky_ptrref <U> &rhs)
    {
        ptr = kyAsCast(T *)(rhs.ptr);
        if (ptr)
        {
            ref = rhs.ref;
            addref();
        }
        else
            ref = 0;
    }

    //! 其他类到本类转换
    template <typename U>
    inline ky_ptrref<T>& operator =(const ky_ptrref<U>& rhs)
    {
        lessref();
        ptr = (kyAsCast(T *)(rhs.ptr));
        if (ptr)
        {
            ref = rhs.ref;
            addref();
        }
        else
            ref = 0;
        return *this;
    }

    //! 作为其他类使用
    template <typename U>
    inline ky_ptrref <U> as()const {return ky_ptrref <U>(*this);}
    template <typename U>
    inline ky_ptrref <U> as() {return ky_ptrref <U>(*this);}


    virtual ~ky_ptrref();

    ky_ptrref<T>& operator =(const ky_ptrref<T>& rhs);
    ky_ptrref<T>& operator = (T *otp);

    bool operator ==(const ky_ptrref<T> &rhs)const;
    bool operator !=(const ky_ptrref<T> &rhs)const;
    bool operator ==(const T* rhs) const;
    bool operator !=(const T* rhs) const;

    T* operator->();
    T* operator->() const ;
    T* get();
    const T* get() const;
    T& operator*();
    const T& operator*() const;
    operator T* ();
    operator bool() const;

    void reset();

public:
    T      *ptr;
    ky_ref *ref;

    void lessref();
    void addref();
};

template<typename T>
ky_ptrref<T>::ky_ptrref (T *p, ky_ref::eRefFlags c)
{
    if (typeid(T) != typeid(ky_object))
        ref = kyNew (ky_ref(c));
    else
        ref = 0;
    ptr = (p);
}

template<typename T>
ky_ptrref<T>::ky_ptrref(const ky_ptrref<T> &rhs):
    ref(rhs.ref)
{
    ptr = (rhs.ptr);
    addref();
}

#if kyLanguage >= kyLanguage11
template<typename T>
ky_ptrref<T>::ky_ptrref(ky_ptrref<T> &&rhs) :
    ref(rhs.ref)
{
    ptr = (rhs.ptr);
    rhs.ptr = 0;
    rhs.ref = 0;
}
template<typename T>
ky_ptrref<T>& ky_ptrref<T>::operator = (ky_ptrref<T>&& rhs)
{
    if (*this != rhs)
    {
        lessref();
        ptr = (rhs.ptr);
        ref = rhs.ref;
        rhs.ptr = NULL;
        rhs.ref = NULL;
    }
    return *this;
}
#endif

template<typename T>
ky_ptrref<T>::~ky_ptrref() {lessref();}

template<typename T>
ky_ptrref<T>& ky_ptrref<T>::operator = (const ky_ptrref<T>& rhs)
{
    if (ptr != rhs.ptr && ref != rhs.ref)
    {
        lessref();
        ptr = (rhs.ptr);
        ref = rhs.ref;
        addref();
    }
    return *this;
}

template<typename T>
ky_ptrref<T>& ky_ptrref<T>::operator = (T *otp)
{
    if (otp != ptr)
    {
        lessref();
        if (typeid(T) != typeid(ky_object))
            ref = kyNew (ky_ref());
        else
            ref = 0;
        ptr = (otp);
    }
    return *this;
}

template<typename T>
bool ky_ptrref<T>::operator ==(const ky_ptrref<T> &rhs)const
{
    return ptr ==rhs.ptr && ref ==rhs.ref;
}

template<typename T>
bool ky_ptrref<T>::operator !=(const ky_ptrref<T> &rhs)const
{
    return !(this->operator ==(rhs));
}

template<typename T>
bool ky_ptrref<T>::operator ==(const T* rhs) const
{
    return ptr == rhs;
}

template<typename T>
bool ky_ptrref<T>::operator !=(const T* rhs) const
{
    return !(this->operator ==(rhs));
}

template<typename T>
T* ky_ptrref<T>::operator->()
{
    return ptr;
}

template<typename T>
T* ky_ptrref<T>::operator->() const
{
    return ptr;
}

template<typename T>
T* ky_ptrref<T>::get()
{
    return ptr;
}

template<typename T>
const T* ky_ptrref<T>::get() const
{
    return ptr;
}

template<typename T>
T& ky_ptrref<T>::operator*()
{
    return *ptr;
}

template<typename T>
const T& ky_ptrref<T>::operator*() const
{
    return *ptr;
}

template<typename T>
ky_ptrref<T>::operator T* ()
{
    return ptr;
}

template<typename T>
ky_ptrref<T>::operator bool() const
{
    return ptr != 0;
}

template<typename T>
void ky_ptrref<T>::reset()
{
    lessref();
    ptr = 0;
    ref = 0;
}

template<typename T>
void ky_ptrref<T>::lessref()
{
    ky_object *toj = kyAsCast(ky_object*)(ptr);
    if (typeid(T) == typeid(ky_object))
        toj->lessref();
    else if (ref && ref->lessref())
    {
        T* t = ptr;
        ptr = (0);
        kyDelete (t);
        kyDelete (ref);
        ptr = 0;
        ref = 0;
    }
}

template<typename T>
void ky_ptrref<T>::addref()
{
    ky_object *toj = kyAsCast(ky_object*)(ptr);
    if (typeid(T) == typeid(ky_object))
        toj->addref();
    else if (ref != 0)
        ref->addref();
}

#endif
