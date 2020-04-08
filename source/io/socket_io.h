#ifndef SOCKET_IO_H
#define SOCKET_IO_H
#include "interface/isocket.h"
#include "ky_vector.h"

class socket_setup : public isocket::setup
{
public:
    explicit socket_setup(sockhd sd);
    virtual ~socket_setup(){}

    //!
    //!
    //! \brief set_nonblocking 阻塞模式
    //! \param nb          true为非阻塞模式，否则阻塞模式
    //! \return            true设置成功，否则失败
    //!
    virtual bool set_nonblocking(bool nb = true);
    virtual bool nonblocking() ;

    // option so
public:
    //!  允许发送广播数据
    virtual bool set_broadcast(bool s= true);
    virtual bool broadcast();

    //! 使能调试跟踪
    virtual bool set_debug(bool s = true);
    virtual bool debug();

    //! \brief dont_route 不查找路由
    virtual bool set_dontroute(bool s = true);
    virtual bool dontroute();

    //! 获取错误状态并清除。 only get
    virtual int error();
    //! 取得套接口类型 only get
    virtual int type();

    //! 周期性测试连接是否存活
    virtual bool set_keepalive(int idle, int interval, int count = 3);
    virtual bool keepalive(int *idle, int *interval, int *count);

    //! 若有数据待发送则延迟关闭
    virtual bool set_linger(int ms);
    virtual int linger ();

    //! 让接收到的带外数据继续在线存放
    virtual bool set_oobinline(bool s = true);
    virtual bool oobinline ();

    //! 设置缓冲区大小
    //! bufsize == 0 不经历由系统缓冲区到socket缓冲区的拷贝
    virtual bool set_rb(int bufsize);
    virtual int rb();
    virtual bool set_sb(int bufsize);
    virtual int sb();

    //! 接收超时
    virtual bool set_rt(int ms);
    virtual int rt();
    //! 发送超时
    virtual bool set_st(int ms);
    virtual int st();

    //! 允许重用本地地址和端口
    virtual bool set_reuse(bool saddr = true, bool sport = false);
    virtual bool reuse(bool *aaddr, bool *sport);

#if !kyOSIsWin32
    virtual bool set_binddevice(char *dev);
    virtual char * binddevice ();
#endif
    //! 设置套接字的优先级别
    virtual bool set_priority(bool s = true);
    virtual bool priority ();

    //! 调用closesocket后强制关闭，不经历TIME_WAIT的过程
    virtual bool set_dontlinger(bool s = true);

    //! 非阻塞模式下的socket在connect()的过程中可以设置connect()延时,直到accpet()被呼叫
    //! 非阻塞使用
    virtual bool set_condaccept(bool s= true);

    // ip
public:
    virtual bool set_tos(eTOSPrioritys tp, eTOSSubfields ts);
    virtual uchar tos ();

protected:
    bool set_(int level, int opt, void *value, int len);
    bool get_(int level, int opt, void *value, int *len);

protected:
    sockhd sock;
    uint32 option;
    int keepalive_idle;
    int keepalive_interval;
    int keepalive_count;
    int linger;
    int bufsiz_send;
    int bufsiz_recv;
    int timeout_send;
    int timeout_recv;
    uchar tos;
#if !kyOSIsWin32
    char devbind[IFNAMSIZ];
#endif
};


class socket_io : public isocket
{
    friend class ky_server;
    friend class ky_socket;
public:
    explicit socket_io(sockhd sd = -1);
    virtual ~socket_io();

    virtual bool connect(const ky_netaddr &ip);

    virtual bool listen(const ky_netaddr &ip, int count = 20);
    virtual bool listen(int count = 20);

    virtual sockhd accept(ky_netaddr *addrs) ;

    //! 打开套接字
    //! url fromat "tcp[v4,listen]:192.168.1.1:456"
    virtual bool open(const ky_url &, int = Socket_TCPv4) ;
    //! addr 192.168.1.1  mode = eNetworkSockets
    virtual bool open(const ky_string &addr, int mode = Socket_TCPv4);
    //! 只打开socket
    virtual bool open(const eIoModeFlags &mode = Socket_TCPv4);

    virtual void close() ;

    virtual int64 read(void *data, int64 maxlen) const;
    virtual int64 write(void *data, int64 len)const;
    virtual int64 read(void *data, int64 maxlen, ky_netaddr *ip);
    virtual int64 write(const void *data, int64 len, const ky_netaddr &ip);

    virtual ky_string errors()const;
    virtual setup *option() {return sock_option;}

public :
    static pisocket create_socket(sockhd sd = -1);

protected:
    static sockhd sys_create_raw(const eNetworkSocketFlags &mode);
    static sockhd sys_create(const eNetworkSocketFlags &mode);
    static void sys_close(sockhd sd);
    static bool sys_connect(sockhd sd, const ky_netaddr &ip);
    static bool sys_bind(sockhd sd, const ky_netaddr &ip);
    static bool sys_listen(sockhd sd,int backlog);
    static sockhd sys_accept(sockhd sd, ky_netaddr* ad);

private:
    ky_netaddr local_addr;
    bool not_bind;
    setup *sock_option;
};

#endif // SOCKET_IO_H
