
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
 *
 * @author   kunyang
 * @email    kunyang.yk@gmail.com
 * @version  1.1.0.1
 * @date     2014/01/02
 * @license  GNU General Public License (GPL)
 *
 * Change History :
 *    Date    |  Version  |  Author  |   Description
 * 2014/01/02 | 1.0.0.1   | kunyang  | 创建文件
 * 2020/03/26 | 1.1.0.1   | kunyang  | 分离时间类
 *
 */
#ifndef ky_DATA_H
#define ky_DATA_H

#include "ky_define.h"
#include "ky_string.h"
#include "ky_stream.h"
#include "ky_variant.h"
#include "ky_time.h"

static const i32 kyDateBase = 1970;

class ky_date
{
public:
    ky_date();
    //!
    //! \brief ky_date
    //! \param microsec
    //!
    ky_date(u64 microsec, const i32 timezone = kyTimeZone);
    //!
    //! \brief ky_date
    //! \param year
    //! \param month
    //! \param day
    //! \param time
    //!
    ky_date(i32 year, i32 month, i32 day, const ky_time &time = ky_time());
    //!
    //! \brief ky_date
    //! \param dt
    //!
    ky_date(const ky_date &dt);
    ~ky_date();

    //!
    //! \brief time 时间
    //! \return
    //!
    inline ky_time time() const{return _time;}
    //!
    //! \brief year 年
    //! \return
    //!
    inline int year() const{return _year;}

    //!
    //! \brief month 月
    //! \return
    //!
    inline int month() const{return _month;}

    //!
    //! \brief day 日
    //! \return
    //!
    inline int day() const{return _day;}

    //!
    //! \brief yday 第几天
    //! \return
    //!
    int yday() const;
    //!
    //! \brief wday 星期几
    //! \return
    //!
    int wday() const;

    //!
    //! \brief week 第几周
    //! \return
    //!
    int week() const;

    //!
    //! \brief is_yleap 是否闰年
    //! \return
    //!
    bool is_yleap() const;

    //!
    //! \brief to_millisec 转换为毫秒
    //! \return
    //!
    u64 to_millisec()const;

    //!
    //! \brief to_lunar 转换到农历
    //! \return
    //!
    ky_date to_lunar() const;

public:
    //!
    //! \brief to_string
    //! \param form
    //! \return
    //!
    ky_string to_string(const ky_string &form = ky_string("yy-mm-dd hh:mm:ss:ms"))const;
    //!
    //! \brief from_string
    //! \param s
    //! \param form
    //!
    void from_string(const ky_string &s,
                     const ky_string &form = ky_string("yy-mm-dd hh:mm:ss:ms"));

public:
    bool operator !=(const ky_date &);
    bool operator ==(const ky_date &);

    bool operator >(const ky_date &);
    bool operator <(const ky_date &);
    bool operator >=(const ky_date &);
    bool operator <=(const ky_date &);

    ky_date operator +(const ky_date &);
    ky_date operator -(const ky_date &);
    ky_date &operator +=(const ky_date &);
    ky_date &operator -=(const ky_date &);

    void swap(ky_date &b);
    friend void ky_swap(ky_date &a, ky_date &b) {a.swap(b);}

public:
    //!
    //! \brief current 当前系统日期
    //! \param timezone
    //! \return
    //!
    static ky_date current(const i32 timezone = kyTimeZone);

private:
    ky_time _time;
    i32     _year;
    i32     _month;
    i32     _day;
};

const fourwd_t kyDateCode = kyFourWord(date);
ky_stream &operator << (ky_stream &in, const ky_date &v);
ky_stream &operator >> (ky_stream &out, ky_date &v);

ky_variant &operator << (ky_variant &va, const ky_date &col);
ky_variant &operator >> (ky_variant &va, ky_date &col);
#endif // UTIME_H
