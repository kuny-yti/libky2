
/**
 * Basic tool library
 * Copyright (C) 2014 kunyang kunyang.yk@gmail.com
 *
 * @file     ky_atomic.h
 * @brief    系统原子操作，支持x86,arm,gcc,clang
 *       1.完整gcc原子操作并兼容clang
 *       2.支持1,2,4,8 byte的类型操作
 *       3.支持指针类型操作
 *       4.可在编译阶段断言系统是否支持以上类型
 *       5.同时支持arm和x86原子操作
 *       6.内存序操作保证(内存屏障)
 *
 * @author   kunyang
 * @email    kunyang.yk@gmail.com
 * @version  1.3.3.1
 * @date     2012/04/01
 * @license  GNU General Public License (GPL)
 *
 * Change History :
 *    Date    |  Version  |  Author  |   Description
 * 2012/04/01 | 1.0.0.1   | kunyang  | 创建文件
 * 2012/04/13 | 1.0.1.0   | kunyang  | 使用gcc完成简单的原子操作
 * 2013/08/10 | 1.1.0.1   | kunyang  | 修改gcc原子操作加入内存屏障
 * 2014/02/05 | 1.1.1.0   | kunyang  | 修改gcc原子操作兼容clang并简化原子运算符
 * 2014/05/25 | 1.1.2.1   | kunyang  | 加入原子指针操作和8 byte原子操作
 * 2015/08/20 | 1.2.0.1   | kunyang  | 重构原子操作类加入通用原子类，定义了内存序模式
 * 2015/08/23 | 1.2.1.1   | kunyang  | 加入x86原子操作汇编
 * 2016/10/07 | 1.3.0.1   | kunyang  | 加入arm原子操作并修改了原子指针模板类
 * 2017/08/20 | 1.3.1.1   | kunyang  | 在类中加入编译器编译断言
 * 2018/02/22 | 1.3.2.1   | kunyang  | 修改比较交换引用错误
 * 2018/03/09 | 1.3.2.2   | kunyang  | 修改windows下编译时setne指令错误
 * 2019/02/22 | 1.3.3.1   | kunyang  | 修复arm原子操作地址作为值应用的错误
 */

#ifndef KY_ATOMIC
#define KY_ATOMIC

#include "ky_barrier.h"
#include "ky_type.h"


typedef enum
{
    Fence_Relaxed,         ///< 宽松的内存屏障
    Fence_Acquire,         ///< 对读取有保证的内存屏障,在之前不做任何重新排序
    Fence_Release,         ///< 对写入有保证的内存屏障,在之后不做任何重新排序
    Fence_AcquireRelease,  ///< 对写入读取有保证的内存屏障
} eMemoryFences;

template<int> struct atomic_has { enum{has = 0};};

template<> struct atomic_has<1> { enum{has = 1};};
template<> struct atomic_has<2> { enum{has = 1};};
template<> struct atomic_has<4> { enum{has = 1};};

//native support for 64-bit types
#if kyArchIs64Bit
template<> struct atomic_has<8> { enum{has = 1};};
#endif

template <typename T>
struct atomic_add{typedef T type; static const int size = 1;};
template <typename T>
struct atomic_add <T *> {typedef intptr type; static const int size = sizeof(T*);};

extern struct ky_atomic_arch
{
    //!
    //! \brief load 将_v地址内的值加载到nv内
    //! \param _v
    //! \param nv
    //! \param mf 内存栅模式
    //!
    void (*load)(void volatile * volatile _v, void *nv, eMemoryFences ) ;
    //!
    //! \brief store 将nv的值存储到_v地址中
    //! \param _v
    //! \param nv
    //! \param mf 内存栅模式
    //!
    void (*store)(void volatile * volatile _v, void * nv, eMemoryFences) ;
    //!
    //! \brief increase 将_v地址内的值自加1
    //! \param _v
    //!
    bool (*increase)(void volatile * volatile _v) ;
    //!
    //! \brief reduction 将_v地址内的值自减1
    //! \param _v
    //!
    bool (*reduction)(void volatile * volatile _v) ;
    //!
    //! \brief fetch_store 将_v地址内的值取出到fv中，并将ev内的值存储到_v地址中
    //! \param _v
    //! \param fv
    //! \param ev
    //!
    void (*fetch_store)(void volatile * volatile _v, void * fv, void * ev) ;
    //!
    //! \brief fetch_add 将_v地址内的值取出到fv中，并将ev内的值和_v地址内的值相加存储到_v中
    //! \param _v
    //! \param fv
    //! \param ev
    //!
    void (*fetch_add)(void volatile * volatile _v, void * fv, void * ev) ;
    //!
    //! \brief compare_exchange 将_v地址内的值和ev的值比较，若相等则把nv内的值存储到_v地址内，并返回true
    //!                         若不等则不存储，并返回false
    //! \param _v
    //! \param ev
    //! \param nv
    //! \return
    //!
    bool (*compare_exchange)(void volatile * volatile _v, void * ev, void * nv) ;
    //!
    //! \brief logic_and 将_v地址内的值和va的值进行相与,并存储到_v地址内
    //! \param _v
    //! \param va
    //!
    void (*logic_and)(void volatile * volatile _v, void *va) ;
    //!
    //! \brief logic_or 将_v地址内的值和va的值进行相或,并存储到_v地址内
    //! \param _v
    //! \param va
    //!
    void (*logic_or)(void volatile * volatile _v, void * va) ;
    //!
    //! \brief logic_xor 将_v地址内的值和va的值进行异或,并存储到_v地址内
    //! \param _v
    //! \param va
    //!
    void (*logic_xor)(void volatile * volatile _v, void * va) ;
    //!
    //! \brief logic_add 将_v地址内的值和va的值进行相加,并存储到_v地址内
    //! \param _v
    //! \param va
    //!
    void (*logic_add)(void volatile * volatile _v, void * va) ;
    //!
    //! \brief logic_not 将_v地址内的值取反，并存储到_v地址中
    //! \param _v
    //!
    void (*logic_not)(void volatile * volatile _v) ;
    //!
    //! \brief logic_inc 将_v地址内的值自加1，并存储到_v地址中
    //! \param _v
    //!
    void (*logic_inc)(void volatile * volatile _v) ;
    //!
    //! \brief logic_inc 将_v地址内的值自减1，并存储到_v地址中
    //! \param _v
    //!
    void (*logic_dec)(void volatile * volatile _v) ;

    //!
    //! \brief pause 暂停指令
    //!
    void (*pause)(void) ;
    //!
    //! \brief memory_fence 内存栅操作
    //! \param mf
    //!
    void (*memory_fence)(eMemoryFences) ;

}* const kyArchAtomics[9];

struct atomic_base
{
    template <typename T>
    static bool increase(volatile T & _v)
    {
        return kyArchAtomics[sizeof(T)]->increase (&_v);
    }
    template <typename T>
    static bool reduction(volatile T &_v)
    {
        return kyArchAtomics[sizeof(T)]->reduction (&_v);
    }

    template <typename T>
    static bool compare_exchange(volatile T &_v, T ev, T nv)
    {
        return kyArchAtomics[sizeof(T)]->compare_exchange (&_v, &ev, &nv);
    }

    template <typename T>
    static T fetch_store(volatile T &_v, T nv)
    {
        T fv;
        kyArchAtomics[sizeof(T)]->fetch_store (&_v, &fv, &nv);

        return fv;
    }
    template <typename T>
    static T fetch_add(volatile T &_v, T va)
    {
         T fv;
         kyArchAtomics[sizeof(T)]->fetch_add (&_v, &fv, &va);
        return fv;
    }

    template <typename T>
    static T logic_and(volatile T &_v, T va)
    {
        kyArchAtomics[sizeof(T)]->logic_and (&_v,  &va);
        return _v;
    }
    template <typename T>
    static T logic_or(volatile T &_v, T va)
    {
        kyArchAtomics[sizeof(T)]->logic_or (&_v,  &va);
        return _v;
    }
    template <typename T>
    static T logic_xor(volatile T &_v, T va)
    {
        kyArchAtomics[sizeof(T)]->logic_xor (&_v,  &va);
        return _v;
    }
    template <typename T>
    static T logic_add(volatile T &_v, T va)
    {
        kyArchAtomics[sizeof(T)]->logic_add (&_v,  &va);
        return _v;
    }

    template <typename T>
    static T logic_not(volatile T &_v)
    {
        kyArchAtomics[sizeof(T)]->logic_not (&_v);
        return _v;
    }
    template <typename T>
    static T logic_inc(volatile T &_v)
    {
        kyArchAtomics[sizeof(T)]->logic_inc (&_v);
        return _v;
    }
    template <typename T>
    static T logic_dec(volatile T &_v)
    {
        kyArchAtomics[sizeof(T)]->logic_dec (&_v);
        return _v;
    }

    template <typename T>
    static T load(volatile T &_v, eMemoryFences mf= Fence_Relaxed)
    {
        T lv;
        kyArchAtomics[sizeof(T)]->load (&_v, &lv, mf);
        return lv;
    }

    template <typename T>
    static void store(volatile T &_v, T nv, eMemoryFences mf= Fence_Relaxed)
    {
        kyArchAtomics[sizeof(T)]->store (&_v, &nv, mf);
    }

    static void pause(void) {
        return kyArchAtomics[sizeof(int)]->pause ();
    }

    static void memory_fence(eMemoryFences fence = Fence_Acquire){
        return kyArchAtomics[sizeof(int)]->memory_fence (fence);
    }
};

template <typename T>
class ky_atomic : public atomic_base
{
    kyCompilerAssert(is_int<T>::value);
    kyCompilerAssert(atomic_has<sizeof(T)>::has);

public:
    ky_atomic() {_v = T(0);}
    ky_atomic(T v) {atomic_base::store(_v, v, Fence_Release);}

    inline T operator = (T value)
    {
        atomic_base::store (_v, value, Fence_Release);
        return value;
    }

    inline T value()const {return _v;}
    inline operator T () {return _v;}
    inline operator const T ()const {return _v;}

    inline ky_atomic<T> & operator = (const ky_atomic<T> &rhs)
    {
        atomic_base::store (_v, rhs.value (), Fence_Release);
        return *this;
    }

public:
    // 后置
    inline T operator++(int) {return atomic_base::fetch_add(_v, 1); }
    inline T operator--(int) {return atomic_base::fetch_add(_v, -1); }

    // 前置
    inline T operator++() {return atomic_base::logic_add(_v, (T)1);}
    inline T operator--() {return atomic_base::logic_add(_v, (T)-1);}

    inline T operator+=(T v) { return atomic_base::logic_add(_v, v); }
    inline T operator-=(T v) { return atomic_base::logic_add(_v, -v); }
    inline T operator&=(T v) { return atomic_base::logic_and(_v, v); }
    inline T operator|=(T v) { return atomic_base::logic_or(_v, v); }
    inline T operator^=(T v) { return atomic_base::logic_xor(_v, v); }

public:
    inline T load(eMemoryFences mo = Fence_Relaxed){return atomic_base::load(_v, mo);}
    inline void store(T nv, eMemoryFences mo = Fence_Relaxed){atomic_base::store(_v, nv, mo);}

    inline bool increase(){return atomic_base::increase(_v);}
    inline bool reduction(){return atomic_base::reduction(_v);}

    inline bool compare_exchange (T ev, T nv)
    {
        return atomic_base::compare_exchange(_v, ev, nv);
    }

    inline T fetch_store(T nv)
    {
        return atomic_base::fetch_store(_v, nv);
    }

    inline T fetch_add(T va)
    {
        return atomic_base::fetch_add(_v, va);
    }

    inline T fetch_less(T va)
    {
        return atomic_base::fetch_add(_v, -va);
    }

    inline T logic_and(T a)
    {
        return atomic_base::logic_and(_v, a);
    }
    inline T logic_or( T a)
    {
        return atomic_base::logic_or(_v, a);
    }
    inline T logic_xor(T a)
    {
        return atomic_base::logic_xor(_v, a);
    }
    inline T logic_add(T a)
    {
        return atomic_base::logic_add(_v, a);
    }

    inline T logic_not()
    {
        return atomic_base::logic_not(_v);
    }
    inline T logic_inc()
    {
        return atomic_base::logic_inc(_v);
    }
    inline T logic_dec()
    {
        return atomic_base::logic_dec(_v);
    }
private:
    volatile T _v;
};

template <typename T>
class ky_atomic <T*>: public atomic_base
{
    typedef T* PT;
    kyCompilerAssert(is_pointer<PT>::value);
    kyCompilerAssert(atomic_has<sizeof(PT)>::has);

public:
    ky_atomic() {_v = PT(0);}
    ky_atomic(PT v) {atomic_base::store(_v, v, Fence_Release);}

    inline PT operator = (PT value)
    {
        atomic_base::store (_v, value, Fence_Release);
        return value;
    }

    inline PT value()const {return _v;}
    inline operator PT () {return _v;}
    inline operator PT ()const {return _v;}
    inline T *operator ->()  { return _v;}
    inline T *operator ->() const { return _v;}

    inline ky_atomic & operator = (const ky_atomic &rhs)
    {
        atomic_base::store (_v, rhs.value (), Fence_Release);
        return *this;
    }

public:
    // 后置
    inline T operator++(int) {return atomic_base::fetch_add(_v, 1);}
    inline T operator--(int) {return atomic_base::fetch_add(_v, -1);}

    // 前置
    inline T operator++() {return atomic_base::logic_add(_v, 1) ;}
    inline T operator--() {return atomic_base::logic_add(_v, -1) ;}

    inline PT operator+=(intptr v) { return atomic_base::logic_add(_v, v) ; }
    inline PT operator-=(intptr v) { return atomic_base::logic_add(_v, -v) ; }
    inline PT operator&=(intptr v) { return atomic_base::logic_and(_v, v) ; }
    inline PT operator|=(intptr v) { return atomic_base::logic_or(_v, v); }
    inline PT operator^=(intptr v) { return atomic_base::logic_xor(_v, v); }

public:
    inline PT load(eMemoryFences mo = Fence_Relaxed){return atomic_base::load(_v, mo);}
    inline void store(PT nv, eMemoryFences mo = Fence_Relaxed){atomic_base::store(_v, nv, mo);}

    inline bool increase(){return atomic_base::increase(_v);}
    inline bool reduction(){return atomic_base::reduction(_v);}

    inline bool compare_exchange (PT ev, PT nv)
    {
        return atomic_base::compare_exchange(_v, ev, nv);
    }

    inline PT fetch_store(PT nv)
    {
        return atomic_base::fetch_store(_v, nv);
    }

    inline PT fetch_add(typename atomic_add<PT>::type va)
    {
        return atomic_base::fetch_add(_v, va);
    }
    inline PT fetch_less(typename atomic_add<PT>::type va)
    {
        return atomic_base::fetch_add(_v, -va);
    }

    inline PT logic_and(typename enif_t <is_pointer<PT>::value, PT>::type ov)
    {
        return atomic_base::logic_and(_v, ov);
    }
    inline PT logic_or(typename enif_t <is_pointer<PT>::value, PT>::type ov)
    {
        return atomic_base::logic_or(_v, ov);
    }
    inline PT logic_xor(typename enif_t <is_pointer<PT>::value, PT>::type ov)
    {
        return atomic_base::logic_xor(_v, ov);
    }
    inline PT logic_add(typename enif_t <is_pointer<PT>::value, PT>::type ov)
    {
        return atomic_base::logic_add(_v, ov);
    }

    inline PT logic_not()
    {
        return atomic_base::logic_not(_v);
    }
    inline PT logic_inc()
    {
        return atomic_base::logic_inc(_v);
    }
    inline PT logic_dec()
    {
        return atomic_base::logic_dec(_v);
    }

private:
    volatile PT _v;
};

#endif // KY_ATOMIC

