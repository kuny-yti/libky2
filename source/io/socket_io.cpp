#include "socket_io.h"
#include "ky_vector.h"
#include "ky_debug.h"
#include "ky_memory.h"
#include "ky_hash.h"

#if kyOSIsLinux
#include <arpa/inet.h>
#include <netinet/in.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/tcp.h>
#endif


namespace impl
{

#if kyOSIsWin32
#  include <winsock2.h>
#  include <ws2tcpip.h>


#  ifndef AF_INET6
#    define AF_INET6        23              /* Internetwork Version 6 */
#  endif

#  ifndef SOL_TCP
#    define SOL_TCP IPPROTO_TCP
#  endif

#  ifndef SOL_IP
#    define SOL_IP IPPROTO_IP
#  endif

#  ifndef WSA_FLAG_NO_HANDLE_INHERIT
#    define WSA_FLAG_NO_HANDLE_INHERIT 0x80
#  endif

#  ifndef SIO_UDP_CONNRESET
#    ifndef IOC_VENDOR
#      define IOC_VENDOR 0x18000000
#    endif
#    ifndef _WSAIOW
#      define _WSAIOW(x,y) (IOC_IN|(x)|(y))
#    endif
#    define SIO_UDP_CONNRESET _WSAIOW(IOC_VENDOR,12)
#  endif

#endif

struct tGlobalSockets
{
    struct SockBase
    {
        sockhd              sock;
        eNetworkSocketFlags mode;
        SockBase(){}
        SockBase(sockhd shd, const eNetworkSocketFlags &ns):sock(shd),mode(ns){}
    };

    ky_hash_map<sockhd, SockBase> pool;

    eNetworkSocketFlags mode(sockhd h)
    {
        return pool.at (h).mode;
    }

    bool is_exist(sockhd h){return pool.contains (h);}

    void append (sockhd h, const eNetworkSocketFlags & m)
    {
        pool.append (h, SockBase(h, m));
    }

    void remove (sockhd h)
    {
        if (pool.contains (h))
            pool.remove (h);
    }

    tGlobalSockets()
    {
        pool.clear ();
#if kyOSIsWin32
        static WSAData wsadata;
        if (::WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
        {
            log_warning("WinSock v2.2 initialization failed.");
            if (::WSAStartup(MAKEWORD(2,0), &wsadata) != 0)
                log_err("WinSock v2.0 initialization failed.");
            else
                log_warning("WinSock v2.0 initialization success.");
        }
#endif
    }
    ~tGlobalSockets()
    {
#if kyOSIsWin32
        ::WSACleanup();
#endif
        pool.clear ();
    }
};
static tGlobalSockets __global_socket_init__;
static tGlobalSockets *GlobalSocket = &__global_socket_init__;

}

socket_io::socket_io(sockhd sd)
{
    sock_option = 0;
    not_bind = true;
    reset ();
    hd = sd;
    if (sd != -1)
    {
        sock_option = kyNew(socket_setup(sd));
        io_open = true;
    }
}
socket_io::~socket_io()
{
    if (is_open ())
        close();
    reset ();
}

bool socket_io::connect(const ky_netaddr &ip)
{
    if (!is_open () || !ip.is_valid ())
        return false;
    if ((mode() & Socket_Listen) && not_bind)
    {
        io_mode &= ~Socket_Listen;
        return sys_connect(hd, ip);
    }
    return false;
}

bool socket_io::listen(const ky_netaddr &ip, int count)
{
    if (is_open () && !not_bind && local_addr != ip)
    {
        close ();
        if (!open (mode ()))
            return false;
    }
    local_addr = ip;
    return listen (count);
}
bool socket_io::listen(int count)
{
    if (!is_open () || !( io_mode & Socket_Listen))
        return false;
    if (not_bind && !local_addr.is_valid ())
    {
        log_err("Socket failed to bind, may not be valid");
        return false;
    }
    else if (not_bind)
    {
        if (!sys_bind (hd, local_addr))
        {
            sys_close (hd);
            io_open = false;
            return false;
        }
        not_bind = false;
    }
    return sys_listen(hd, count);
}

sockhd socket_io::accept(ky_netaddr *addrs)
{
    if (!is_open () || !addrs)
        return -1;
    sockhd asock = (sockhd)sys_accept(hd, addrs);
    if (asock == -1)
        return -1;
    impl::GlobalSocket->append (asock, (eNetworkSocketFlags)mode());
    return asock;
}

//! url fromat "socket://tcp-v4-listen:192.168.1.1:456"
bool socket_io::open(const ky_url &url, int)
{
    if (url.protocol () != Protocol_Socket)
        return false;

    ky_string addr = url.path ();
    int ope_mode = 0;
    addr.lower ();
    if (addr.contains ("tcp"))
        ope_mode = Socket_TCP;
    else if (addr.contains ("udp"))
        ope_mode = Socket_UDP;
    else if (addr.contains ("raw"))
        ope_mode = Socket_Raw;
    else
        return false;

    if (addr.contains ("v6"))
        ope_mode |= Socket_IPv6;
    else
        ope_mode |= Socket_IPv4;

    if (addr.contains ("listen"))
        ope_mode |= Socket_Listen;
    ky_string_list addrs = addr.split (":");

    for (int i =0 ; i < addrs.count (); ++i)
    {
        ky_string cns = addrs.at (i);
        if (cns.contains ("tcp") || cns.contains ("udp") || cns.contains ("raw"))
            addrs.remove (i);
    }
    addr = ky_string();
    for (int i = 0; i < addrs.count (); ++i)
        addr.append (addrs.at (i) + ":");

    return open(addr, ope_mode);
}

//! addr 192.168.1.1:456  mode = eNetworkSockets
bool socket_io::open(const ky_string &addr, int mode)
{
    if (io_url.path ()== addr && mode == io_mode)
        return true;
    else if ((io_url.path () != addr || mode != io_mode) && is_open ())
        close ();
    ky_string_list ap = addr.split (":");
    ky_string address;
    short port = -1;
    for (int i = 0; i < ap.count (); ++i)
    {
        if (ap.at (i).contains ("."))
            address = ap.at (i);
        else if (ap.at (i).is_number ())
            port = ap.at (i).to_short ();
    }
    if (!ky_netaddr::ip_legality(address))
        return false;

    local_addr = ky_netaddr(address, port);
    if ((mode & Socket_IPv6) && !local_addr.is_ipv6 ())
    {
        mode &= ~Socket_IPv6;
        mode |= Socket_IPv4;
    }
    return open(mode);
}

//! 只打开socket
bool socket_io::open(const eIoModeFlags &mode)
{
    if (is_open ())
        close ();

    sockhd sd = sys_create((eNetworkSocketFlags)mode);
    if (sd < 0)
        return false;

    io_open = true;
    io_mode = mode;
    io_type = mode & Socket_TCP ? Io_TypeTcp : mode & Socket_UDP ? Io_TypeUdp : 0;
    hd = sd;
    sock_option = kyNew(socket_setup(hd));
    return true;
}

void socket_io::close()
{
    if (!is_open ())
        return ;
    sys_close(hd);
    if (sock_option)
        kyDelete(sock_option);
    sock_option = 0;
    io_open = false;
    not_bind = true;
    hd = -1;
}

int64 socket_io::read(void *data, int64 maxlen)const
{
    socket_io *take = (socket_io *)this;
    if (!is_open ())
    {
        take->io_error = Socket_ErrorNotOpen;
        return -1;
    }
    int64 ret = ::recv(hd, (char *)data, maxlen, 0);
    if (ret == -1)
        take->io_error = Socket_ErrorDisconnect;
    else if (ret < 0)
        take->io_error = Socket_ErrorUnknown;
    return ret;
}
int64 socket_io::write(void *data, int64 len)const
{
    socket_io *take = (socket_io *)this;
    if (!is_open ())
    {
        take->io_error = Socket_ErrorNotOpen;
        return -1;
    }

    int64 ret = ::send(hd, (const char *)data, len, 0);
    if (ret == -1)
        take->io_error = Socket_ErrorDisconnect;
    else if (ret < 0)
        take->io_error = Socket_ErrorUnknown;

    return ret;
}
int64 socket_io::read(void *data, int64 maxlen, ky_netaddr *ip)
{
    socket_io *take = (socket_io *)this;
    if (!is_open ())
    {
        take->io_error = Socket_ErrorNotOpen;
        return -1;
    }

    if (ip == 0)
        return -1;

    int ad_len = 0;
    static union {
        sockaddr_in ipv4;
        sockaddr_in6 ipv6;
    }addr;
    int ret = ::recvfrom(hd, (char *)data, maxlen, 0, (sockaddr*)&addr, (socklen_t*)&ad_len);
    if (ret == -1)
        take->io_error = Socket_ErrorDisconnect;
    else if (ret < 0)
        take->io_error = Socket_ErrorUnknown;
    else
        *ip = ky_netaddr((sockaddr*)&addr, ad_len);

    return ret;
}

int64 socket_io::write(const void *data, int64 len,const ky_netaddr &ip)
{
    socket_io *take = (socket_io *)this;
    if (!is_open ())
    {
        take->io_error = Socket_ErrorNotOpen;
        return -1;
    }
    if (!ip.is_valid ())
    {
        take->io_error = Socket_ErrorNotAddress;
        return -1;
    }

    int64 ret = ::sendto(hd, (const char *)data, len, 0, ip.socket (), ip.socklen());
    if (ret == -1)
        take->io_error = Socket_ErrorDisconnect;
    else if (ret < 0)
        take->io_error = Socket_ErrorUnknown;
    return ret;
}

ky_string socket_io::errors()const
{
    if ((int)this->io_error == Socket_ErrorNot)
        return ky_string();
    else if ((int)this->io_error == Socket_ErrorNotOpen)
        return "socket not open";
    else if ((int)this->io_error == Socket_ErrorDisconnect)
        return "socket disconnect";
    else if ((int)this->io_error == Socket_ErrorNotAddress)
        return "Unspecified IP";
    return "socket unknown error";
}

pisocket socket_io::create_socket(sockhd sd)
{
    return ky_malloc (socket_io, sd);
}

sockhd socket_io::sys_create_raw(const eNetworkSocketFlags & mode)
{
    return -1;
}

sockhd socket_io::sys_create(const eNetworkSocketFlags &mode)
{
    sockhd sd = -1;
    if (mode & Socket_Raw)
        sd = sys_create_raw (mode);
    else
    {
        const int prot = mode & Socket_IPv6 ? AF_INET6 : AF_INET;
        const int type = (mode & Socket_UDP) ? SOCK_DGRAM : SOCK_STREAM ;
        const int ipport = (mode & Socket_UDP) ? IPPROTO_UDP: IPPROTO_TCP;
        sd = ::socket(prot, type, ipport);
    }

    if (sd == -1)
    {
        log_err("create socket failed(%s-%d)!",
        #if kyOSIsWin32
                " ", GetLastError()
        #else
                strerror(errno),errno
        #endif
                );
        return sd;
    }

    impl::GlobalSocket->append (sd, mode);
    return sd;
}

void socket_io::sys_close(sockhd sd)
{
    if (impl::GlobalSocket->is_exist(sd))
    {
        impl::GlobalSocket->remove (sd);
#if kyOSIsWin32
        ::closesocket(sd);
#else
        ::close (sd);
#endif
    }
}

bool socket_io::sys_connect(sockhd sd, const ky_netaddr &ip)
{
    if (!ip.is_valid () || !impl::GlobalSocket->is_exist(sd))
        return false;

    if (ip.is_ipv6() && !(impl::GlobalSocket->mode (sd) & Socket_IPv6))
        return false;
    if (ip.is_ipv4() && !(impl::GlobalSocket->mode (sd) & Socket_IPv4))
        return false;

    if (::connect(sd, (sockaddr*)ip.socket(), ip.socklen()) < 0)
    {
        log_err("connect error (%s-%d)!",
        #if kyOSIsWin32
                " ", WSAGetLastError()
        #else
                strerror(errno),errno
        #endif
                );
        return false;
    }
    return true;
}

bool socket_io::sys_bind(sockhd sd, const ky_netaddr &ip)
{
    int ipv6only = 0;
    if (!ip.is_valid () || !impl::GlobalSocket->is_exist (sd))
        return false;
    if (ip.is_ipv6())
    {
        ipv6only = 1;
        ipv6only = ::setsockopt(sd, IPPROTO_IPV6, IPV6_V6ONLY,
                                (char*)&ipv6only, sizeof(ipv6only) );
    }
    if (ipv6only < 0)
        log_warn("IPv6 socket needs to be set to fail only in IPv6 mode!");

    int r = ::bind(sd, (sockaddr*)ip.socket(), ip.socklen());
    if (r < 0)
    {
        log_err("bind socket error(%s-%d)!",
        #if kyOSIsWin32
                " ", WSAGetLastError()
        #else
                strerror(errno),errno
        #endif
                );

        return false;
    }

    return true;
}

bool socket_io::sys_listen(sockhd sd,int backlog)
{
    if (!impl::GlobalSocket->is_exist (sd))
        return false;

    if (backlog < 1)
        backlog = 1;
    if (::listen(sd, backlog) < 0)
    {
        log_err("listen socket error(%s-%d)!",
        #if kyOSIsWin32
                " ", WSAGetLastError()
        #else
                strerror(errno),errno
        #endif
                );
        return false;
    }
    return true;
}

sockhd socket_io::sys_accept(sockhd sd, ky_netaddr* ad )
{
    int adds_len = 0;
    if (!impl::GlobalSocket->is_exist (sd) || ad == 0)
        return -1;

    sockhd ret_sd = ::accept(sd, (sockaddr*)ad->socket(), (socklen_t*)&adds_len);
    if ((int)ret_sd < 0)
    {
        log_err("accept error(%s-%d)!",
        #if kyOSIsWin32
                " ", WSAGetLastError()
        #else
                strerror(errno),errno
        #endif
                );
    }

    return ret_sd;
}



#define optSet(vat) do{(s) ? option |= (vat) : option &=~(vat);}while(0)
#define optGet(vat) (option & (vat))

socket_setup::socket_setup(sockhd sd):
    sock(sd)
{
    option = 0;
    keepalive_idle = 0;
    keepalive_interval = 0;
    keepalive_count = 0;
    bufsiz_recv = 0;
    bufsiz_send = 0;
    timeout_recv = 0;
    timeout_send = 0;
    tos = 0;

    int value = 0, len = 0;
    bool s = false;

    get_(SOL_SOCKET ,SO_RCVBUF, &bufsiz_recv, &len);
    get_(SOL_SOCKET ,SO_SNDBUF, &bufsiz_send, &len);
    get_(SOL_SOCKET ,SO_RCVTIMEO, &timeout_recv, &len);
    get_(SOL_SOCKET ,SO_SNDTIMEO, &timeout_send, &len);
#  if !kyOSIsWin32
    ky_memory::zero (devbind, IFNAMSIZ);
    get_(SOL_SOCKET, SO_BINDTODEVICE, devbind, &len);
#  endif
    get_(SOL_IP, IP_TOS, &tos, &len);


    get_(SOL_SOCKET, SO_BROADCAST, &value, &len);
    s = value;
    optSet(1 << 1);

    get_(SOL_SOCKET, SO_DEBUG, &value, &len);
    s = value;
    optSet(1 << 2);

    get_(SOL_SOCKET, SO_DONTROUTE, &value, &len);
    s = value;
    optSet(1 << 3);

    get_(SOL_SOCKET, SO_KEEPALIVE, &value, &len);
    s = value;
    optSet(1 << 4);

#  if !kyOSIsWin32
    if (s)
    {
        get_(SOL_TCP, TCP_KEEPIDLE, &keepalive_idle, &len);
        get_(SOL_TCP, TCP_KEEPINTVL, &keepalive_interval,  &len);
        get_(SOL_TCP, TCP_KEEPCNT, &keepalive_count,  &len);
    }
#  endif

    struct _linger
    {
        u_short l_onoff;
        u_short l_linger;
    } sLinger = {0};
    get_(SOL_SOCKET, SO_LINGER, &sLinger, &len);
    s = sLinger.l_onoff;
    optSet(1 << 5);
    if (s)
        linger = sLinger.l_linger;

    get_(SOL_SOCKET, SO_OOBINLINE, &value, &len);
    s = value;
    optSet(1 << 6);

    get_(SOL_SOCKET ,SO_REUSEADDR, &value, &len);
    s = value;
    optSet(1 << 7);

#  if !kyOSIsWin32
    get_(SOL_SOCKET ,SO_REUSEPORT, &value, &len);
    s = value;
    optSet(1 << 8);

    get_(SOL_SOCKET, SO_PRIORITY,  &value, &len);
    s = value;
    optSet(1 << 9);
#  endif
}
bool socket_setup::set_nonblocking(bool s )
{
    ulong l = s;
    int n =
        #if kyOSIsWin32
            ioctlsocket (sock, FIONBIO, &l);
#else
            ioctl (sock, FIONBIO, &l);
#endif
    if (n != 0)
        return false;
    optSet(1 << 0);
    return true;
}

bool socket_setup::get_nonblocking()
{
    return optGet((1 << 0));
}

//!  允许发送广播数据
bool socket_setup::set_broadcast(bool s)
{
    int bc = s;
    if (!set_(SOL_SOCKET, SO_BROADCAST, &bc, sizeof(int)))
        return false;
    optSet(1 << 1);
    return true;
}
bool socket_setup::get_broadcast()
{
    return optGet(1 << 1);
}

//! 使能调试跟踪
bool socket_setup::set_debug(bool s )
{
    int ds = s;
    if (!set_(SOL_SOCKET, SO_DEBUG, &ds, sizeof(int)))
        return false;
    optSet(1 << 2);
    return true;
}
bool socket_setup::get_debug ()
{
    return optGet(1 << 2);
}

//! \brief dont_route 不查找路由
bool socket_setup::set_dontroute(bool s )
{
    int bc = s;
    if (!set_(SOL_SOCKET, SO_DONTROUTE, &bc, sizeof(int)))
        return false;
    optSet(1 << 3);
    return true;
}
bool socket_setup::get_dontroute()
{
    return optGet(1 << 3);
}

//! 获取错误状态并清除。 only get
int socket_setup::get_error()
{
    int err = 0, len = 0;
    get_(SOL_SOCKET, SO_ERROR, &err, &len);
    return err;
}
//! 取得套接口类型 only get
int socket_setup::get_type()
{
    int value = -1, len = 0;
    get_(SOL_SOCKET, SO_TYPE, &value, &len);
    return value;
}

//! 周期性测试连接是否存活
bool socket_setup::set_keepalive(int idle, int interval, int count)
{
#if kyOSIsWin32
    return false;
#else
    // 开启keepalive属性
    int keepAlive = (interval > 0 && interval > 0 && count > 0) ? 1 : 0;
    bool s = set_(SOL_SOCKET, SO_KEEPALIVE, &keepAlive, sizeof(int));
    //int keepIdle = idle; // 该连接在idle秒内没有任何数据往来,则进行探测
    //int keepInterval = interval; // 探测时发包的时间间隔为interval秒
    //int keepCount = 3; // 探测尝试的次数.如果第1次探测包就收到响应了,则后2次的不再发.
    if (s)
    {
        s &= set_(SOL_TCP, TCP_KEEPIDLE, &idle, sizeof(int));
        s &= set_(SOL_TCP, TCP_KEEPINTVL, &interval, sizeof(int));
        s &= set_(SOL_TCP, TCP_KEEPCNT, &count, sizeof(int));
        if (s)
        {
            keepalive_idle = idle;
            keepalive_interval = interval;
            keepalive_count = count;
        }
    }
    optSet(1 << 4);
    return s;
#endif
}
bool socket_setup::get_keepalive (int *idle, int *intercal, int *count)
{
    if (idle)
        *idle = keepalive_idle;
    if (intercal)
        *intercal = keepalive_interval;
    if (count)
        *count = keepalive_count;
    return keepalive_idle != 0 || keepalive_interval != 0 || keepalive_count != 0;
}

//! 若有数据待发送则延迟关闭
bool socket_setup::set_linger(int ms)
{
    struct _linger
    {
        ushort l_onoff; //(在closesocket()调用时还有数据没发送完毕则容许逗留)
        // 如果l_onoff = 0和set_dont_linger作用相同;
        ushort l_linger; //(容许逗留的时间)
    } sLinger =
    {
        .l_onoff = (ushort)((ms > 0) ? 1 : 0),
        .l_linger = (ushort)ms
    };
    if (!set_(SOL_SOCKET, SO_LINGER, &sLinger, sizeof(sLinger)))
        return false;
    linger = ms;
    bool s = sLinger.l_onoff;
    optSet(1 << 5);
    return true;
}
int socket_setup::get_linger ()
{
    return linger;
}

//! 让接收到的带外数据继续在线存放
bool socket_setup::set_oobinline(bool s )
{
    int os = s;
    if (!set_(SOL_SOCKET, SO_OOBINLINE, &os, sizeof(int)))
        return false;
    optSet(1 << 6);
    return true;
}
bool socket_setup::get_oobinline ()
{
    return optGet(1 << 6);
}
//! 设置缓冲区大小
//! bufsize == 0 不经历由系统缓冲区到socket缓冲区的拷贝
bool socket_setup::set_rb(int bufsize)
{
    if (!set_(SOL_SOCKET ,SO_RCVBUF, &bufsize,sizeof(bufsize)))
        return false;
    bufsiz_recv = bufsize;
    return true;
}
bool socket_setup::set_sb(int bufsize)
{
    if (!set_(SOL_SOCKET ,SO_SNDBUF, &bufsize,sizeof(bufsize)))
        return false;
    bufsiz_send = bufsize;
    return true;
}
int socket_setup::get_rb ()
{
    return bufsiz_recv;
}
int socket_setup::get_sb ()
{
    return bufsiz_send;
}

//! 接收超时
bool socket_setup::set_rt(int ms)
{
    if (!set_(SOL_SOCKET ,SO_RCVTIMEO, &ms,sizeof(ms)))
        return false;
    timeout_send = ms;
    return true;
}
//! 发送超时
bool socket_setup::set_st(int ms)
{
    if (!set_(SOL_SOCKET ,SO_SNDTIMEO, &ms,sizeof(ms)))
        return false;
    timeout_send = ms;
    return true;
}
int socket_setup::get_rt ()
{
    return timeout_recv;
}
int socket_setup::get_st ()
{
    return timeout_send;
}

//! 允许重用本地地址和端口
bool socket_setup::set_reuse(bool saddr , bool sport)
{
    int ra = saddr;
    bool r = set_(SOL_SOCKET ,SO_REUSEADDR, &ra, sizeof(ra));
    bool s = !r ? false : saddr;
    optSet(1 << 7);

#if !kyOSIsWin32
    ra = sport;
    r &= set_(SOL_SOCKET ,SO_REUSEPORT, &ra, sizeof(ra));
    s = !r ? false : sport;
    optSet(1 << 8);
#endif
    return r;
}
bool socket_setup::get_reuse (bool *saddr, bool *sport)
{
    if (saddr)
        *saddr = optGet(1 << 7);
    if (sport)
        *sport = optGet(1 << 8);
    return true;
}

#if !kyOSIsWin32
bool socket_setup::set_binddevice(char *dev)
{
    if (!set_(SOL_SOCKET, SO_BINDTODEVICE, dev, strlen (dev)))
        return false;
    ky_memory::copy (devbind, dev, strlen (dev));
    return true;
}
char * socket_setup::get_binddevice ()
{
    return devbind;
}
#endif
//! 设置套接字的优先级别
bool socket_setup::set_priority(bool s )
{
#if !kyOSIsWin32
    int ps = s;
    if (!set_(SOL_SOCKET, SO_PRIORITY, &ps, sizeof(int)))
        return false;
    optSet(1 << 9);
    return true;
#endif
    return false;
}
bool socket_setup::get_priority ()
{
    return optGet(1 << 9);
}

bool socket_setup::set_tos(eTOSPrioritys tp, eTOSSubfields ts)
{
    uchar  service_type = tp | ts;
    if (!set_(SOL_IP, IP_TOS, &service_type, sizeof(uchar)))
        return false;
    tos = service_type;
    return true;
}
uchar socket_setup::get_tos ()
{
    return tos;
}

//! 调用closesocket后强制关闭，不经历TIME_WAIT的过程
bool socket_setup::set_dontlinger(bool s )
{
    int dl = s;
#ifdef SO_DONTLINGER
    return set_(SOL_SOCKET, SO_DONTLINGER, &dl, sizeof(int));
#else
    return false;
#endif
}

//! 非阻塞模式下的socket在connect()的过程中可以设置connect()延时,直到accpet()被呼叫
//! 非阻塞使用
bool socket_setup::set_condaccept(bool s)
{
    int ca = s;
#ifdef SO_CONDITIONAL_ACCEPT
    ::setsockopt(sock, SOL_SOCKET, SO_CONDITIONAL_ACCEPT,
                 (const char*)&ca, sizeof(int));
    return set_(SOL_SOCKET, SO_CONDITIONAL_ACCEPT, &ca, sizeof(int));
#else
    return false;
#endif
}

bool socket_setup::set_(int level, int opt, void *value, int len)
{
    if (::setsockopt(sock, level, opt, (const char*)&value, len) == -1)
        return false;
    return true;
}
bool socket_setup::get_(int level, int opt, void *value, int *len)
{
    if (::getsockopt(sock, level, opt, (char*)&value, (socklen_t*)len) == -1)
        return false;
    return true;
}
