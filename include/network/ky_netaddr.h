
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
 * @file     ky_netaddr.h
 * @brief    socket 网络地址信息类
 *       1.支持IPv4和IPv6.
 *       2.支持检测地址信息的合法性、域名的合法性
 *
 * @author   kunyang
 * @email    kunyang.yk@gmail.com
 * @version  1.2.1.1
 * @date     2012/07/26
 * @license  GNU General Public License (GPL)
 *
 * Change History :
 *    Date    |  Version  |  Author  |   Description
 * 2012/07/26 | 1.0.0.1   | kunyang  | 创建文件实现IPv4地址类
 * 2013/04/10 | 1.0.1.0   | kunyang  | 修改地址类
 * 2013/05/03 | 1.0.2.0   | kunyang  | 将地址类修改为虚类加实现
 * 2014/05/30 | 1.1.0.1   | kunyang  | 加入IPv6和ANY地址类实现
 * 2016/03/09 | 1.1.2.0   | kunyang  | 修改IPv6地址无效判断方法
 * 2017/06/21 | 1.2.0.1   | kunyang  | 将虚类加实现模式修改为netaddr一类来完成并加入有效性检测
 * 2018/04/05 | 1.2.1.1   | kunyang  | 加入主机字节序构造
 */
#ifndef ky_netaddr_H
#define ky_netaddr_H

#include "tools/ky_string.h"
//!
//! \brief The ky_netaddr class ipv4和ipv6地址类
//!
class ky_netaddr
{
public:
    //!
    //! \brief local_addr 返回本地地址信息
    //! \return
    //!
    static ky_string local_addr();
    //!
    //! \brief is_legality 是否合法ipv4,ipv6
    //! \param ip
    //! \return        true为合法,否则不合法
    //!
    static bool is_legality(const ky_string &ip);
    //!
    //! \brief is_domain 域名是否合法
    //! \param ip
    //! \return true为合法,否则不合法
    //!
    static bool is_domain(const ky_string &domain);

public:
    //!
    //! \brief ky_netaddr 构造函数any地址
    //! \param port
    //! \param is_ipv6
    //!
    ky_netaddr (u16 port = 0, bool is_ipv6 = false);
    //!
    //! \brief ky_netaddr 拷贝构造
    //! \param rhs
    //!
    ky_netaddr (const ky_netaddr &rhs);
    //!
    //! \brief ky_netaddr 根据协议地址构造
    //! \param sa
    //! \param sa_len
    //!
    ky_netaddr (const sockaddr *sa, int sa_len);
    //!
    //! \brief ky_netaddr 根据地址和端口构造
    //! \param ip
    //! \param port
    //!
    ky_netaddr (const ky_string &ip, u16 port);

    //!
    //! \brief ky_netaddr IP和PORT都以HOST BYTE传递
    //! \param ipHostByteOrder
    //! \param port
    //!
    ky_netaddr(u32 ipHostByteOrder, u16 port);

    virtual ~ky_netaddr();

    //!
    //! \brief address 返回ip地址
    //! \return
    //!
    ky_string address()const;

    //!
    //! \brief port 返回端口号
    //! \return
    //!
    u16 port()const;

    //!
    //! \brief socklen 返回socket()返回的指针长度
    //! \return
    //!
    int socklen() const;
    //!
    //! \brief socket 返回socket需要使用的地址信息
    //! \return
    //!
    sockaddr *socket() const;

    //!
    //! \brief is_ipv4 是否ipv4
    //! \return        true为支持,否则不支持
    //!
    bool is_ipv4()const;
    //!
    //! \brief is_ipv6 是否ipv6
    //! \return        true为支持,否则不支持
    //!
    bool is_ipv6()const;
    //!
    //! \brief is_any 是否为any地址
    //! \return
    //!
    bool is_any()const ;
    //!
    //! \brief is_valid 判断socket()返回的是否是有效地址
    //! \return         true为有效,否则无效
    //!
    bool is_valid()const;

    //!
    //! \brief operator == 判断i的地址是否和本地址相同
    //! \param i
    //! \return            true为相同,否则不相同
    //!
    bool operator == (const ky_netaddr &i);
    bool operator != (const ky_netaddr &i){return !this->operator == (i);}
    //!
    //! \brief operator = 将i的地址赋值给本地址
    //! \param i
    //! \return           返回本地址
    //!
    ky_netaddr &operator= (const ky_netaddr &i);

    //!
    //! \brief scope_id 范围id(在IPV6中才使用)
    //! \param id
    ulong scope_id() const;

    //!
    //! \brief flow_info 信息流(在IPV6中才使用)
    //! \param info
    //!
    ulong flow_info() const;

protected:
    union uAddr
    {
        sockaddr     generic;
        sockaddr_in  ipv4;
        sockaddr_in6 ipv6;
    } addr;
};
#endif // ky_netaddr_H
