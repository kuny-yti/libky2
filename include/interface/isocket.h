
/**
 * Universal interface header
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
 * @file     isocket.h
 * @brief    根据标准io抽象的套接字通用接口
 *       1.支持套接字选项设置.
 *       2.支持IPv4和IPv6协议
 *       3.暂时未实现原始套接字
 *
 * @author   kunyang
 * @email    kunyang.yk@gmail.com
 * @version  1.1.0.1
 * @date     2014/03/27
 * @license  GNU General Public License (GPL)
 *
 * Change History :
 *    Date    |  Version  |  Author  |   Description
 * 2014/03/27 | 1.0.0.1   | kunyang  | 创建文件
 * 2014/06/10 | 1.0.1.0   | kunyang  | 实现套接字抽象
 * 2016/04/04 | 1.0.2.0   | kunyang  | 套接字修改为抽象接口
 * 2016/07/21 | 1.0.3.0   | kunyang  | 加入非阻塞模式设置
 * 2017/04/28 | 1.0.4.0   | kunyang  | 将接口继承自iio接口
 * 2018/03/29 | 1.1.0.1   | kunyang  | 重构接口并加入选项设置
 */
#ifndef KY_ISOCKET_H
#define KY_ISOCKET_H

#include "ky_define.h"
#include "tools/ky_string.h"
#include "iio.h"
#include "network/ky_netaddr.h"

//! Socket 模式
typedef enum NetworkSocket
{
    Socket_TCP  = 0x01,     ///< TCP socket
    Socket_UDP  = 0x02,     ///< UDP socket
    Socket_Raw  = 0x04,     ///< Raw socket
    Socket_IPv4 = 0x10,     ///< IPv4 protocol
    Socket_IPv6 = 0x20,     ///< IPv6 protocol
    Socket_Listen = 0x40,   ///< Listen Mode

    ///< Socket Mode
    Socket_TCPv4 = Socket_TCP | Socket_IPv4,
    Socket_TCPv6 = Socket_TCP | Socket_IPv6,
    Socket_UDPv4 = Socket_UDP | Socket_IPv4,
    Socket_UDPv6 = Socket_UDP | Socket_IPv6,

    ///< Listen Mode
    Socket_TCPv4Listen = Socket_TCPv4 | Socket_Listen,
    Socket_TCPv6Listen = Socket_TCPv6 | Socket_Listen,
    Socket_UDPv4Listen = Socket_UDPv4 | Socket_Listen,
    Socket_UDPv6Listen = Socket_UDPv6 | Socket_Listen,
}eNetworkSockets;
kyDeclareFlags(eNetworkSockets, eNetworkSocketFlags);

//! Socket 错误
typedef enum
{
    Socket_ErrorNot = Io_ErrorNot,             ///< Socket 没有错误
    Socket_ErrorNotOpen = Io_ErrorNotOpen,     ///< Socket 创建错误
    Socket_ErrorNotAddress = Io_ErrorUnknown+1,///< Socket 地址无效错误
    Socket_ErrorDisconnect ,                   ///< Socket 断开错误
    Socket_ErrorUnknown                        ///< Socket 未知错误
}eNetworkErrors;
kyDeclareFlags(eNetworkErrors, eNetworkErrorFlags);

///< socket 句柄
typedef int sockhd;


//! Socket 抽象接口
kyPackage isocket : iio
{
    virtual ~isocket(){}

    //! 覆盖iio的读写接口
    //!
    //! \brief read
    //! \param buf
    //! \param len
    //! \return
    //!
    virtual int64 read(void *buf, int64 len) const = 0;
    //!
    //! \brief write
    //! \param buf
    //! \param len
    //! \return
    //!
    virtual int64 write(void *buf, int64 len) const = 0;

    //!
    //! \brief read   从socket读取maxlen长度的数据存储到data中
    //! \param data   存储接受的数据(指针需要先申请)
    //! \param maxlen 存储数据的空间(申请时的长度)
    //! \param ip     数据从ip中发来的
    //! \return       读取到的长度
    //!
    virtual int64 read(void *data, int64 maxlen, ky_netaddr *ip)=0;
    //!
    //! \brief write 向socket中发送len长度的data
    //! \param data  需要发送的数据
    //! \param len   需要发送的数据的长度
    //! \param ip    将数据发送到ip目标
    //! \return      返回发送的长度
    //!
    virtual int64 write(const void *data, int64 len, const ky_netaddr &ip)=0;

    //!
    //! \brief connect 链接服务端
    //! \param ip      服务端地址信息和端口号(内部区分IPV4和IPV6)
    //! \return        true 成功, 否则失败
    //!
    virtual bool connect(const ky_netaddr &ip) = 0;

    //!
    //! \brief listen 监听Socket
    //! \param count  同时能够被Accept接受的客户端数
    //! \return       true 成功, 否则失败
    //!
    virtual bool listen(const ky_netaddr &ip, int count = 20) = 0;
    //!
    //! \brief listen
    //! \param count
    //! \return
    //!
    virtual bool listen(int count = 20) = 0;

    //!
    //! \brief accept 接受客户端的链接
    //! \param addrs  接受到的客户端地址信息
    //! \return       链接后分配的socket句柄
    //!
    virtual sockhd accept(ky_netaddr *addrs) = 0;

    //! 打开套接字
    //!
    //! \brief open
    //! \param s
    //! \return
    //!
    virtual bool open(const eNetworkSocketFlags &s = Socket_TCPv4)
    {
        return open((eIoModeFlags)s);
    }

    //! url fromat "tcp[v4,listen]:192.168.1.1:456"
    //!
    //! \brief open
    //! \return
    //!
    virtual bool open(const ky_url &, int = Socket_TCPv4) = 0;

    //! addr 192.168.1.1  mode = eNetworkSockets
    //!
    //! \brief open
    //! \param addr
    //! \param mode
    //! \return
    //!
    virtual bool open(const ky_string &addr, int mode = Socket_TCPv4) = 0;

    //! 只打开socket
    //!
    //! \brief open
    //! \param mode
    //! \return
    //!
    virtual bool open(const eIoModeFlags & mode = Socket_TCPv4) = 0;

    //!
    //! \brief close  关闭创建的socket
    //!
    virtual void close() = 0;

    //!
    //! \brief The setup class
    //! 对socket的选项进行设置
    //!
    struct setup
    {
        virtual ~setup(){}

        //!
        //! \brief nonblocking 阻塞模式
        //! \param nb          true为非阻塞模式，否则阻塞模式
        //! \return            true设置成功，否则失败
        //!
        virtual bool set_nonblocking(bool nb = true) = 0;
        virtual bool nonblocking() = 0;

        // option so
        //!
        //! \brief set_broadcast 允许发送广播数据
        //! \param s true 允许发送
        //! \return
        //!
        virtual bool set_broadcast(bool s= true) = 0;
        virtual bool broadcast()= 0;

        //!
        //! \brief set_debug 使能调试跟踪
        //! \param s true 使能调试
        //! \return
        //!
        virtual bool set_debug(bool s = true)= 0;
        virtual bool debug() = 0;

        //!
        //! \brief set_dontroute 不查找路由
        //! \param s
        //! \return
        //!
        virtual bool set_dontroute(bool s = true)= 0;
        virtual bool dontroute()= 0;

        ///[only get]
        //!
        //! \brief get_error 获取错误状态并清除。
        //! \return
        //!
        virtual int error()= 0;
        //!
        //! \brief get_type 取得套接口类型
        //! \return
        //!
        virtual int type()= 0;
        ///[only get]

        //!
        //! \brief set_keepalive 周期性测试连接是否存活
        //! \param idle
        //! \param interval
        //! \param count
        //! \return
        //!
        virtual bool set_keepalive(int idle, int interval, int count = 3)= 0;
        virtual bool keepalive(int *idle, int *interval, int *count) = 0;

        //!
        //! \brief set_linger 若有数据待发送则延迟关闭
        //! \param sec
        //! \return
        //!
        virtual bool set_linger(int sec)= 0;
        virtual int linger() = 0;

        //!
        //! \brief set_oobinline 让接收到的带外数据继续在线存放
        //! \param s
        //! \return
        //!
        virtual bool set_oobinline(bool s = true)= 0;
        virtual bool oobinline() = 0;

        //!
        //! \brief set_rb 设置接受缓冲区大小
        //! \param bufsize
        //! \return
        //! \note bufsize == 0 不经历由系统缓冲区到socket缓冲区的拷贝
        //!
        virtual bool set_rb(int bufsize)= 0;
        virtual int rb() = 0;
        //!
        //! \brief set_sb 设置发送缓冲区大小
        //! \param bufsize
        //! \return
        //! \note bufsize == 0 不经历由系统缓冲区到socket缓冲区的拷贝
        //!
        virtual bool set_sb(int bufsize)= 0;
        virtual int sb() = 0;

        //!
        //! \brief set_rt 设置接收超时
        //! \param ms
        //! \return
        //!
        virtual bool set_rt(int ms)= 0;
        virtual int rt() = 0;

        //!
        //! \brief set_st 设置发送超时
        //! \param ms
        //! \return
        //!
        virtual bool set_st(int ms)= 0;
        virtual int st() = 0;

        //!
        //! \brief set_reuse 允许重用本地地址和端口
        //! \param saddr
        //! \param sport
        //! \return
        //!
        virtual bool set_reuse(bool saddr = true, bool sport = false)= 0;
        virtual bool reuse(bool *aaddr, bool *sport) = 0;

        //!
        //! \brief set_binddevice 设置socket绑定到指定设备
        //! \param dev
        //! \return
        //!
        virtual bool set_binddevice(char *){}
        virtual char* binddevice() {return 0;}

        //!
        //! \brief set_priority 设置套接字的优先级别
        //! \param s
        //! \return
        //!
        virtual bool set_priority(bool s = true)= 0;
        virtual bool priority() = 0;

        // ip
        // 3bit的8个优先级的定义如下：
        typedef enum
        {
            // 优先级6和7一般保留给网络控制数据使用，如路由。
            // 111--Network Control（网络控制）；
            TOSNetworkControl = 0xe0,
            // 110--Internetwork Control（网间控制）；
            TOSInternetworkControl = 0xc0,
            // 优先级5推荐给语音数据使用。
            // 101--Critic（关键）；
            TOSCritic = 0xa0,
            // 优先级4由视频会议和视频流使用。
            // 100--Flash Override（疾速）；
            TOSFlashOverride = 0x80,
            // 优先级3给语音控制数据使用。
            //011--Flash（闪速）；
            TOSFlash = 0x60,
            // 优先级1和2给数据业务使用。
            //010--Immediate（快速）；
            TOSImmediate = 0x40,
            //001--Priority（优先）；
            TOSPriority = 0x20,
            // 优先级0为默认标记值。
            //000--Routine（普通）。
            TOSRoutine = 0x00
        }eTOSPrioritys;

        // 4bit的TOS子字段
        // 4bit中只能置其中1bit,如果所有4bit均为0是一般服务.
        typedef enum
        {
            // Telnet、Rlogin这两个交互应用要求最小的传输时延
            // 最小时延
            TOSLowDelay = 0x10,        ///< 1 = Low Delay; 0 = Normal Delay
            // FTP文件传输要求最大吞吐量
            // 最大吞吐量
            TOSHighThroughput = 0x08,  ///< 1 = High Throughput; 0 = Normal Throughput
            // 最高可靠性是指网络管理（SNMP）和路由选择协议
            // 最高可靠性, IPTOS_RELIABILITY
            TOSHighReliability = 0x04, ///< 1 = High Reliability; 0 = Normal Reliability
            // 用户网络新闻要求最小费用
            // 最小费用
            TOSMincost = 0x02,         ///< 1 = Minimise monetary cost (RFC 1349)
        }eTOSSubfields;

        //!
        //! \brief set_tos
        //! \param tp
        //! \param ts
        //! \return
        //!
        virtual bool set_tos(eTOSPrioritys tp, eTOSSubfields ts) = 0;
        virtual uchar tos() = 0;
    };

    //!
    //! \brief option
    //!
    virtual setup *option() = 0;

    //! 覆盖不使用接口
    virtual bool seek(int64){return false;}

    sockhd hd;
};
#define iSocket ky_ptr(isocket)
typedef iSocket pisocket;
#undef iSocket
#endif // ISOCKET

