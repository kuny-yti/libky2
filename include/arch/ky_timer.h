
/**
 * Basic tool library
 * Copyright (C) 2014 kunyang kunyang.yk@gmail.com
 *
 * @file     ky_timer.h
 * @brief    关于 CPU 定时器
 *
 * @author   kunyang
 * @email    kunyang.yk@gmail.com
 * @version  1.0.0.1
 * @date     2016/07/02
 * @license  GNU General Public License (GPL)
 *
 * Change History :
 *    Date    |  Version  |  Author  |   Description
 * 2016/07/02 | 1.0.0.1   | kunyang  | 创建文件
 */

#ifndef ky_TIMER_H
#define ky_TIMER_H
#include "ky_define.h"
#include "ky_utils.h"

// 允许在64位版本中使用 xadd（比cmpxchg更快）而无需强制转换
#if kyArchIs64Bit
typedef intptr cycles_t;
#else
typedef int64 cycles_t;
#endif

//!
//! \brief The ky_timer class
//!
class ky_timer
{
public:
    typedef enum
    {
        RealTime,   //! 系统实时时间
        Monotonic,  //! 从系统启动到现在的时间
        Process,    //! 从进程启动到现在的时间
        Thread      //! 从线程启动到现在的时间
    }eTypes;

public:
    ky_timer();
    ~ky_timer();

    //!
    //! \brief millisec 当前时间戳(毫秒)
    //! \return
    //!
    static u64 millisec();
    //!
    //! \brief microsec 当前时间戳(微妙)
    //! \return
    //!
    static u64 microsec();
    //!
    //! \brief nanosec 根据类型返回时间戳(纳秒)
    //! \param t
    //! \return
    //!
    static u64 nanosec(eTypes t= RealTime);
};

//!
//! \brief The ky_timer_scope class 范围内计时
//!
class ky_timer_scope : ky_noncopy
{
public:
    ky_timer_scope(volatile u64 &elapsed) ;
    ~ky_timer_scope();

    //!
    //! \brief elapsed 构造本类到调用时，中间过去的时间
    //! \return
    //! \note 单位：纳秒
    //!
    u64 elapsed ()const;

private:
    volatile u64 &_elapsed;
    volatile u64  _t0;
};

//!
//! \brief The ky_timer_unit class
//!
class ky_timer_unit
{
public:
    ky_timer_unit();
    ~ky_timer_unit();

    void zero();

    void set_timer();

    void add_diff(const ky_timer_unit &t0, const ky_timer_unit &t1);

    void sub(const ky_timer_unit &t);

    flt64 to_sec() const;

private:
    cycles_t _cycles;
};

#endif
