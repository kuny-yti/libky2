
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
 * @file     ky_path.h
 * @brief    路径类
 *
 * @author   kunyang
 * @email    kunyang.yk@gmail.com
 * @version  1.0.0.1
 * @date     2020/02/12
 * @license  GNU General Public License (GPL)
 *
 * Change History :
 *    Date    |  Version  |  Author  |   Description
 * 2020/02/12 | 1.0.0.1   | kunyang  | 创建文件
 */
#ifndef ky_PATH_H
#define ky_PATH_H

#include "ky_string.h"

class ky_path
{
public:
    ky_path():_path(){}
    ~ky_path(){}
    explicit ky_path(const char *path);
    explicit ky_path(const wchar_t *path);
    explicit ky_path(const ky_string &path);

    ky_path (const ky_path &path);
    ky_path& operator = (const ky_path& rhs);

    //!
    //! \brief parent 返回父目录
    //! \return
    //!
    ky_path parent() const;
    //!
    //! \brief before_common 返回两个目录向前公共部分
    //! \param other
    //! \return
    //!
    ky_path before_common(const ky_path &other) const;
    //!
    //! \brief is_valid 路径是否有效
    //! \return
    //!
    bool is_valid()const;
    //!
    //! \brief is_directory 路径是否为目录
    //! \return
    //!
    bool is_directory()const;
    //!
    //! \brief is_url 是否URL
    //! \return
    //!
    bool is_url()const;

    //!
    //! \brief drive 路径的驱动器
    //! \return
    //!
    ky_string drive()const;

    //!
    //! \brief path 路径，不含驱动器
    //! \return
    //!
    ky_string path()const;
    //!
    //! \brief path_all 全路径，含驱动器、路径、文件名、扩展名
    //! \return
    //!
    ky_string path_all()const;
    //!
    //! \brief name 文件名称，包含扩展名
    //! \return
    //!
    ky_string file_name()const;
    //!
    //! \brief name_base 文件名称，不含扩展名
    //! \return
    //!
    ky_string base_name()const;
    //!
    //! \brief suffix 文件的扩展名
    //! \return
    //!
    ky_string suffix()const;
    ky_string suffixs()const;

    ky_path operator / (const ky_path &rhs) const;
    bool operator < (const ky_path& rhs) const;
    bool operator == (const ky_path& rhs) const;
    bool operator != (const ky_path& rhs) const;

private:
    ky_string _path;
    ky_string _filename;
    ky_string _suffix;
    ky_string _drive;
    ky_string _dir;
};

#endif
