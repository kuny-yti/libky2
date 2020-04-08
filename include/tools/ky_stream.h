
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
 * @file     ky_stream.h
 * @brief    数据流类
 *
 * @author   kunyang
 * @email    kunyang.yk@gmail.com
 * @version  1.0.0.1
 * @date     2014/05/02
 * @license  GNU General Public License (GPL)
 *
 * Change History :
 *    Date    |  Version  |  Author  |   Description
 * 2014/05/02 | 1.0.0.1   | kunyang  | 创建文件
 * 2016/03/26 | 1.0.1.0   | kunyang  | 加入数据格式
 * 2020/02/02 | 1.1.0.0   | kunyang  | 修改使用对象为iio
 */
#ifndef KY_STREAM_H
#define KY_STREAM_H

#include "interface/iio.h"

/*!
 * @brief The ky_stream class
 * @class ky_stream
 */
class ky_stream
{
private:
    iio *_io;
    i32  _order;

public :
    //!
    //! \brief ky_stream
    //! \param io
    //!
    explicit ky_stream (const iio *io);
    //!
    //! \brief ky_stream
    //! \param byte
    //! \param mf
    //!
    ky_stream (ky_byte &byte, eIoModeFlags mf);
    //!
    //! \brief ky_stream
    //! \param rhs
    //!
    explicit ky_stream(ky_stream &rhs);

    virtual ~ky_stream();

    i64 size()const;

public:
    friend ky_stream & operator << (ky_stream &, bool);
    friend ky_stream & operator << (ky_stream &, bool_t);
    friend ky_stream & operator << (ky_stream &, i8);
    friend ky_stream & operator << (ky_stream &, u8);
    friend ky_stream & operator << (ky_stream &, i16);
    friend ky_stream & operator << (ky_stream &, u16);
    friend ky_stream & operator << (ky_stream &, i32);
    friend ky_stream & operator << (ky_stream &, u32);
    friend ky_stream & operator << (ky_stream &, i64);
    friend ky_stream & operator << (ky_stream &, u64);
    friend ky_stream & operator << (ky_stream &, f16);
    friend ky_stream & operator << (ky_stream &, f32);
    friend ky_stream & operator << (ky_stream &, f64);

    friend ky_stream & operator >> (ky_stream &, bool & );
    friend ky_stream & operator >> (ky_stream &, bool_t & );
    friend ky_stream & operator >> (ky_stream &, i8 & );
    friend ky_stream & operator >> (ky_stream &, u8 & );
    friend ky_stream & operator >> (ky_stream &, i16 & );
    friend ky_stream & operator >> (ky_stream &, u16 & );
    friend ky_stream & operator >> (ky_stream &, i32 & );
    friend ky_stream & operator >> (ky_stream &, u32 & );
    friend ky_stream & operator >> (ky_stream &, i64 & );
    friend ky_stream & operator >> (ky_stream &, u64 & );
    friend ky_stream & operator >> (ky_stream &, f16 & );
    friend ky_stream & operator >> (ky_stream &, f32 & );
    friend ky_stream & operator >> (ky_stream &, f64 & );
};

#include "ky_stream.inl"


#endif // ky_stream_H
