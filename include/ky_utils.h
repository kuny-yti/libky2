
/**
 * Basic tool library
 * Copyright (C) 2014 kunyang kunyang.yk@gmail.com
 *
 * @file     ky_utils.h
 * @brief    无依赖关系的工具类实现
 *       1.ky_noncopy禁止继承者实现拷贝构造及默认赋值.
 *       2.ky_singleton实现全局单例对象类
 *
 * @author   kunyang
 * @email    kunyang.yk@gmail.com
 * @version  1.0.3.1
 * @date     2014/02/12
 * @license  GNU General Public License (GPL)
 *
 * Change History :
 *    Date    |  Version  |  Author  |   Description
 * 2014/02/12 | 1.0.0.1   | kunyang  | 创建文件并实现单例对象
 * 2014/06/04 | 1.0.1.0   | kunyang  | 修改单例对象加入互斥
 * 2015/03/27 | 1.0.2.0   | kunyang  | 加入ky_noncopy类
 * 2016/07/02 | 1.0.2.1   | kunyang  | 修改单例对象指针引用方法
 * 2018/02/22 | 1.0.3.1   | kunyang  | 修改单例模式继承ky_noncopy
 */

#ifndef ky_UTILS_H
#define ky_UTILS_H

#include "ky_define.h"

struct ky_noncopy
{
    ky_noncopy() {}
    ky_noncopy(const ky_noncopy&) = delete;
    ky_noncopy& operator = (const ky_noncopy&) = delete;
};

#ifdef kyHasSingletonMutex
#include "ky_thread.h"
template <typename T>
class ky_singleton : public ky_noncopy
{
public:
    static inline T* instance()
    {
        if (0 == ptr)
        {
            mutex.lock ();
            if (0 == ptr)
            {
                T *t = kyNew(T);
                ptr = t;
            }
            mutex.unlock ();
        }
        return ptr;
    }

protected:
    ky_singleton(){}
    virtual ~ky_singleton()
    {
        mutex.lock ();
        if (0 != ptr)
            kyDelete (ptr);
        ptr = 0;
        mutex.unlock ();
    }

private:
    static ky_mutex  mutex;
    static T*        ptr;
};

template <typename T>
ky_mutex ky_singleton<T>::mutex ;

template <typename T>
T* ky_singleton<T>::ptr = 0;
#else
template <typename T>
class ky_singleton : public ky_noncopy
{
public:
    static inline T* instance()
    {
        pthread_once(&once_create, &ky_singleton::__create);
        return ptr;
    }

protected:
    ky_singleton(){}
    virtual ~ky_singleton()
    {
        pthread_once(&once_destroy, &ky_singleton::__destroy);
    }

    static void __create()
    {
        ptr = kyNew(T());
    }
    static void __destroy()
    {
        kyCompilerAssert(sizeof(T));
        kyDelete(ptr);
        ptr = 0;
    }
private:
    static pthread_once_t  once_create;
    static pthread_once_t  once_destroy;
    static T*              ptr;
};

template <typename T>
pthread_once_t ky_singleton<T>::once_create = PTHREAD_ONCE_INIT;
template <typename T>
pthread_once_t ky_singleton<T>::once_destroy = PTHREAD_ONCE_INIT;
template <typename T>
T* ky_singleton<T>::ptr = 0;
#endif

#endif // ky_UTILS_H
