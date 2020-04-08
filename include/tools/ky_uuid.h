
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
 * @file     ky_uuid.h
 * @brief    uuid 类
 *
 * @author   kunyang
 * @email    kunyang.yk@gmail.com
 * @version  1.0.0.1
 * @date     2011/12/28
 * @license  GNU General Public License (GPL)
 *
 * Change History :
 *    Date    |  Version  |  Author  |   Description
 * 2011/12/28 | 1.0.0.1   | kunyang  | 创建文件
 */
#ifndef ky_UUID
#define ky_UUID

#include "ky_define.h"
#include "ky_string.h"
#include "ky_date.h"
#include "ky_variant.h"

struct ky_uuid
{
    uint32   data1;
    uint16   data2;
    uint16   data3;
    uchar    data4[8];

    ky_string to_string();
    ky_datetime to_timestamp();
    bool is_uuid(const char *uuid);

    void swap(ky_uuid &b)
    {
        ky_uuid tmp = b;
        b = *this;
        *this = tmp;
    }
    friend void ky_swap(ky_uuid &a, ky_uuid &b) {a.swap(b);}

    bool operator ==(const ky_uuid &rhs)const
    {
        return data1 == rhs.data1 &&
                data2 == rhs.data2 &&
                data3 == rhs.data3 &&
                data4[0] == rhs.data4[0] &&
                data4[1] == rhs.data4[1] &&
                data4[2] == rhs.data4[2] &&
                data4[3] == rhs.data4[3] &&
                data4[4] == rhs.data4[4] &&
                data4[5] == rhs.data4[5] &&
                data4[6] == rhs.data4[6] &&
                data4[7] == rhs.data4[7] ;
    }
    bool operator !=(const ky_uuid &rhs)const
    {
        return !(*this == rhs);
    }
    ky_uuid &operator =(const ky_uuid &rhs)
    {
         data1 = rhs.data1;
         data2 = rhs.data2;
         data3 = rhs.data3 ;
         data4[0] = rhs.data4[0];
         data4[1] = rhs.data4[1];
         data4[2] = rhs.data4[2];
         data4[3] = rhs.data4[3];
         data4[4] = rhs.data4[4];
         data4[5] = rhs.data4[5];
         data4[6] = rhs.data4[6] ;
         data4[7] = rhs.data4[7] ;
         return *this;
    }

    bool operator < (const ky_uuid &rhs)const
    {
        return data1 < rhs.data1 ? true :
               data2 < rhs.data2 ? true :
               data3 < rhs.data3 ? true :
               data4[0] < rhs.data4[0] ? true :
               data4[1] < rhs.data4[1] ? true :
               data4[2] < rhs.data4[2] ? true :
               data4[3] < rhs.data4[3] ? true :
               data4[4] < rhs.data4[4] ? true :
               data4[5] < rhs.data4[5] ? true :
               data4[6] < rhs.data4[6] ? true :
               data4[7] < rhs.data4[7] ? true : false;
    }
    bool operator > (const ky_uuid &rhs)const
    {
        return !this->operator < (rhs);
    }

    static ky_uuid create();
    static ky_string create_string();
    static ky_uuid  create_external(const ky_string &external);
};

const ky_uuid uuid_null = {0,0,0,{0,0,0,0,0,0,0,0}};
const ky_uuid uuid_max = {0xFFFFFFFF, 0xFFFF, 0xFFFF, {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};
const fourwd_t kyUuidCode = kyFourWord(uuid);

ky_streamb &operator << (ky_streamb &in, const ky_uuid &v);
ky_streamb &operator >> (ky_streamb &out, ky_uuid &v);
ky_streamt &operator << (ky_streamt &in, const ky_uuid &v);
ky_streamt &operator >> (ky_streamt &out, ky_uuid &v);
ky_variant &operator << (ky_variant &va, const ky_uuid &uid);
ky_variant &operator >> (ky_variant &va, ky_uuid &uid);

#endif // ky_UUID

