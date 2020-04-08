
/**
 * Basic tool library
 * Copyright (C) 2014 kunyang kunyang.yk@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * You should have received a copy of the GNU General Public License.
 * If not, see <http://www.gnu.org/licenses/>.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file     ky_datetim.h
 * @brief    日期时间类
 *       1.定义了使用的类型.
 *       2.支持多种颜色空间
 *
 * @author   kunyang
 * @email    kunyang.yk@gmail.com
 * @version  1.0.0.1
 * @date     2014/01/02
 * @license  GNU General Public License (GPL)
 *
 * Change History :
 *    Date    |  Version  |  Author  |   Description
 * 2014/01/02 | 1.0.0.1   | kunyang  | 创建文件
 *
 */
#ifndef ky_TIME_H
#define ky_TIME_H

#include "ky_define.h"
#include "ky_string.h"
#include "ky_stream.h"
#include "ky_variant.h"
static const int kyTimeZone = 8;

//!
//! \brief The ky_timer class
//!
class ky_time
{
public:
    ky_time();
    ~ky_time();
    //!
    //! \brief ky_timer 微妙构造时间
    //! \param microsec
    //! \param timezone
    //!
    ky_time(u64 microsec, const i32 timezone = kyTimeZone);
    //!
    //! \brief ky_timer 根据时间构造
    //! \param hour
    //! \param minute
    //! \param second
    //! \param millisec
    //!
    ky_time(i32 hour, i32 minute, i32 second, i32 millisec = 0);

    //!
    //! \brief hour 小时
    //! \return
    //!
    inline int hour()const   {return _hour;}
    //!
    //! \brief minute 分钟
    //! \return
    //!
    inline int minute()const {return _minute;}
    //!
    //! \brief second 秒
    //! \return
    //!
    inline int second()const {return _second;}
    //!
    //! \brief msec 毫秒
    //! \return
    //!
    inline int msec()const   {return _millisec;}

public:
    bool operator ==(const ky_time &);
    bool operator !=(const ky_time &);

    bool operator >(const ky_time &);
    bool operator <(const ky_time &);
    bool operator >=(const ky_time &);
    bool operator <=(const ky_time &);

    ky_time operator +(const ky_time &);
    ky_time operator -(const ky_time &);
    ky_time &operator +=(const ky_time &);
    ky_time &operator -=(const ky_time &);

    void swap(ky_time &b);
    friend void ky_swap(ky_time &a, ky_time &b) {a.swap(b);}

    //!
    //! \brief to_millisec 转换为毫秒
    //! \return
    //!
    u64 to_millisec() const;
    //!
    //! \brief to_string 转换为字符串
    //! \param form
    //! \return
    //! \note
    //!
    ky_string to_string(const ky_string &form = ky_string("hh:mm:ss:ms")) const;
    //!
    //! \brief from_string
    //! \param s
    //! \param form
    //!
    void from_string(const ky_string &s, const ky_string &form = ky_string("hh:mm:ss:ms"));

public:
    //!
    //! \brief millisec 系统当前毫秒时间戳
    //! \return
    //!
    static u64 millisec();
    //!
    //! \brief convert_timespec
    //! \param millisec
    //! \return
    //!
    static timespec convert_timespec(const u64 millisec);
    //!
    //! \brief convert_timespec
    //! \param time
    //! \return
    //!
    static timespec convert_timespec(const ky_time &time);

    //!
    //! \brief current 当前时间
    //! \return
    //!
    static ky_time current();

private:
    i32 _millisec;
    i32 _second;
    i32 _minute;
    i32 _hour;
};
const fourwd_t kyTimeCode = kyFourWord(time);

ky_stream &operator << (ky_stream &in, const ky_time &v);
ky_stream &operator >> (ky_stream &out, ky_time &v);

ky_variant &operator << (ky_variant &va, const ky_time &col);
ky_variant &operator >> (ky_variant &va, ky_time &col);

#endif // UTIME_H
