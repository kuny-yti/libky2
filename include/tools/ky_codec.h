/**
 * Basic tool library
 * Copyright (C) 2014 kunyang  kunyang.@gmail.com.
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
 * @file     ky_codec.h
 * @brief 编解码算法类
 *        1.支持HEX格式编解码
 *        1.支持BASE64格式编解码
 *
 * @author   kunyang
 * @email    kunyang.yk@gmail.com
 * @version  1.0.0.1
 * @date     2019/02/12
 * @license  GNU General Public License (GPL)
 *
 * Change History :
 *    Date    |  Version  |  Author  |   Description
 * 2019/02/12 | 1.0.0.1   | kunyang  | 创建文件
 */

#ifndef KY_CODEC_H
#define KY_CODEC_H

#include "ky_byte.h"

class ky_codec
{
public:
    static ky_byte to_hex(const ky_byte &, bool is_lower = false);
    static ky_byte to_base64(const ky_byte &, bool is_wrap = true);
    static ky_byte from_hex(const ky_byte &);
    static ky_byte from_base64(const ky_byte &);

};

#endif
