
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
 * @file     iio.h
 * @brief    所有io类型操作的抽象接口
 *       1.抽象了所有io的基本操作基础接口
 *       2.支持错误描述
 *       3.支持非阻塞操作
 *       4.支持异步操作
 *
 * @author   kunyang
 * @email    kunyang.yk@gmail.com
 * @version  1.2.0.1
 * @date     2011/12/11
 * @license  GNU General Public License (GPL)
 *
 * Change History :
 *    Date    |  Version  |  Author  |   Description
 * 2011/12/11 | 1.0.0.1   | kunyang  | 创建文件
 * 2012/02/19 | 1.0.1.1   | kunyang  | 修改抽象的Io模式定义
 * 2014/10/07 | 1.0.2.1   | kunyang  | 加入io类型定义
 * 2015/02/23 | 1.1.0.1   | kunyang  | 重构类并加入reset功能
 * 2018/03/09 | 1.1.1.1   | kunyang  | 加入文件的随机性操作接口
 * 2018/04/10 | 1.2.0.1   | kunyang  | 加入非阻塞操作,异步操作
 * 2018/12/14 | 1.2.0.2   | kunyang  | 加入事件响应标签
 */
#ifndef KY_IO_H
#define KY_IO_H

#include "ky_flags.h"
#include "tools/ky_path.h"
#include "tools/ky_signal.h"

//!
//! \brief The eIoTypes enum 当前Io的类型
//!
typedef enum IoType
{
    Io_TypeFile     = 0x0100,   ///< 文件 io
    Io_TypeMemory   = 0x0200,   ///< 内存 io
    Io_TypeSocket   = 0x0400,   ///< 套接字 io
    Io_TypeDevice   = 0x0800,   ///< 硬件 io
    Io_TypeStream   = 0x1000,   ///< 流式 io
    Io_TypeDirectory= 0x2000,   ///< 目录 io

    Io_TypeSeral   = 0x0001 | Io_TypeDevice,  ///< 串口 io
    Io_TypeUsb     = 0x0002 | Io_TypeDevice,  ///< USB io
    Io_TypeMedia   = 0x0001 | Io_TypeStream,  ///< 媒体 io

    Io_TypeArchive = 0x0001 | Io_TypeFile,    ///< 归档 io
    Io_TypeTcp     = 0x0001 | Io_TypeSocket,  ///< TCP io
    Io_TypeUdp     = 0x0002 | Io_TypeSocket,  ///< UDP io
    Io_TypeHttp    = 0x0003 | Io_TypeSocket,  ///< HTTP io
    Io_TypeHttps   = 0x0004 | Io_TypeSocket,  ///< Ftp io
    Io_TypeFtp     = 0x0005 | Io_TypeSocket,  ///< Ftp io
}eIoTypes;
kyDeclareFlags(eIoTypes, eIoTypeFlags);

//!
//! \brief The eIoModes enum 当前Io的操作模式
//!
typedef enum IoMode
{
    //! 必选
    Io_ReadOnly    = 0x01, ///< 只读模式打开
    Io_WriteOnly   = 0x02, ///< 只写模式打开
    Io_ReadWrite   =       ///< 读写模式打开
                     Io_ReadOnly | Io_WriteOnly,

    Io_Append      = 0x04, ///< 附加模式打开, 默认覆盖
    Io_Text        = 0x08, ///< 文本模式打开, 默认二进制

    Io_NonBlocking = 0x10, ///< 非阻塞模式,默认阻塞模式
    Io_ASync       = 0x20, ///< 异步io模式,默认同步模式,异步下采用非阻塞实现
    Io_Atomic      = 0x40, ///< 原子io模式,默认非原子模式,除非系统支持

    Io_Accept      = 0x80, ///< io接收响应
    Io_Close       = 0x100,///< io关闭响应
    Io_Ignored     = 0x200,///< io忽略响应

}eIoModes;
kyDeclareFlags(eIoModes, eIoModeFlags);

//!
//! \brief The eIoStatus enum 当前Io的操作状态
//!
typedef enum IoStatu
{
    Io_StateNot     = 0x00, ///< 无动作
    Io_StateRead    = 0x01, ///< 正在读取
    Io_StateWrite   = 0x02, ///< 正在写入
    Io_StateWait    = 0x03, ///< 等待系统完成
    Io_StatePrepare = 0x04, ///< 准备操作
    Io_StateFinish  = 0x05, ///< 系统完成
    Io_StateBusy    = 0x06, ///< 系统忙

}eIoStatus;
kyDeclareFlags(eIoStatus, eIoStatuFlags);

//!
//! \brief The eIoErrors enum 当前Io的操作错误
//!
typedef enum IoError
{
    Io_ErrorNot = 0,           ///< 没有错误
    Io_ErrorStat = INT32_MIN,  ///< 状态操作错误
    Io_ErrorNotReady = -1,     ///< 未就绪错误
    Io_ErrorOpen,              ///< 打开错误
    Io_ErrorRead,              ///< 读取错误
    Io_ErrorWrite,             ///< 写入错误
    Io_ErrorClose,             ///< 关闭错误
    Io_ErrorNotOpen,           ///< 未打开错误
    Io_ErrorBusy,              ///< 操作正忙
    Io_ErrorPathEmpty,         ///< 指定path为空
    Io_ErrorAlsoWR,            ///< 同时读写错误
    Io_ErrorNotDirect,         ///< 不支持直接操作
    Io_ErrorRegularFile,       ///< 打开的不是常规文件
    Io_ErrorDirectory,         ///< 打开的不是目录
    Io_ErrorUnknown,           ///< 未知错误
}eIoErrors;
kyDeclareFlags(eIoErrors, eIoErrorFlags);

///! 采用异步IO时，使用者需要管理缓冲，定义缓冲区大小
//const static int kyASyncIoCacheSize = 512 *1024;


/*!
 * @brief The iio class 统一io操作接口
 * @class iio
 * 1.当指定非阻塞时,当io未就绪时则返回错误。
 * 2.当指定异步时,当io就绪后会调用就绪信号
 * 3.内部无缓存io操作,使用者需要管理缓存
 */
kyPackage iio
{
    virtual ~iio(){}

    //!
    //! \brief ready 采用异步模式时，当IO处理完成后此信号响应
    //! \param status
    //! \param mem
    //!
    Signal<void (const eIoStatuFlags &status, const void *mem)> ready;

    //!
    //! \brief is_async 是否为异步IO
    //! \return
    //!
    inline bool is_async()const{return io_mode & Io_ASync;}

    //!
    //! \brief is_nonblocking 是否为非阻塞
    //! \return
    //!
    inline bool is_nonblocking()const{return io_mode & Io_NonBlocking;}

    //!
    //! \brief is_ready 是否操作就绪
    //! \return
    //!
    inline bool is_ready()const{return (io_statu & Io_StateFinish) || (io_statu == Io_StateNot);}

    //!
    //! \brief is_open 当前io是否打开
    //! \return
    //!
    inline bool is_open() const{return io_open;}

    //!
    //! \brief mode 返回当前操作的模式
    //! \return
    //!
    inline eIoModeFlags mode()const{return io_mode;}
    //!
    //! \brief type 返回当前操作的模式类型
    //! \return
    //!
    inline eIoTypeFlags type()const{return io_type;}

    //!
    //! \brief error 返回当前错误代码
    //! \return
    //!
    inline eIoErrorFlags error()const{return io_error;}

    //!
    //! \brief path 返回当前操作的io路径-包括io文件名
    //! \return
    //!
    inline ky_path path() const{return io_path;}

    //!
    //! \brief available 当前打开的io剩余字节数
    //! \return
    //!
    inline i64 available()const{return io_size - io_pos;}

    //!
    //! \brief size 当前打开的io大小
    //! \return
    //!
    inline i64 size()const{return io_size;}

    //!
    //! \brief is_eof 是结束
    //! \return
    //!
    inline bool is_eof()const{return at_end ();}

    //!
    //! \brief at_end 是否遇到结束标志
    //! \return
    //!
    virtual bool at_end()const { return io_eof; }

    //!
    //! \brief tell 当前打开的io位置
    //! \return
    //!
    virtual i64 tell()const{return io_pos;}

    //!
    //! \brief errors 返回当前错误描述
    //! \return
    //!
    virtual ky_string errors()const = 0;
    //!
    //! \brief open 打开io
    //! \param mode
    //! \return
    //!
    virtual bool open(const eIoModeFlags &mode = Io_ReadWrite) = 0;
    //!
    //! \brief close 关闭io
    //!
    virtual void close() = 0;

    //!
    //! \brief seek 移动当前打开的io位置
    //! \param pos
    //! \return
    //!
    virtual bool seek(i64 pos) = 0;
    //!
    //! \brief read 读取io数据
    //! \param buf 存放地址
    //! \param len 存放大小
    //! \param offset 当前位置的偏移
    //! \return 返回读取的实际大小
    //! \note 1.同步阻塞io操作(默认),操作完成则返回操作的字节数.
    //!         读取数据存放在buf里
    //!       2.同步非阻塞io操作,操作后未完成则会立即返回0.
    //!         可通过查询状态得知是否完成,读取数据存放在buf里(buf内存需要外部管理，不可使用临时内存)
    //!       3.异步io操作,操作后则会立即返回0.
    //!         当完成时会通知io_ready信号,读取数据存放在buf里(buf内存需要外部管理，不可使用临时内存)
    //!       4.异步和非阻塞模式所有buf内存需要外部管理，只有本次请求完成，才可清理buf内存,
    //!
    virtual i64 read(void *buf, i64 len)const = 0;
    //!
    //! \brief write 写入io数据
    //! \param buf 写入数据的地址
    //! \param len 写入的长度
    //! \param offset 当前位置的偏移
    //! \return 返回实际写入的长度
    //! \note 1.同步阻塞io操作(默认),操作完成则返回操作的字节数.
    //!       2.同步非阻塞io操作,操作后未完成则会立即返回0.
    //!         可通过查询状态得知是否完成,数据存放buf内存需要外部管理，不可使用临时内存
    //!       3.异步io操作,操作后则会立即返回0.
    //!         数据存放buf内存需要外部管理，不可使用临时内存
    //!       4.异步和非阻塞模式所有buf内存需要外部管理，只有本次请求完成，才可清理buf内存
    //!
    virtual i64 write(void *buf, i64 len)const = 0;

    //!
    //! \brief open URL格式打开io
    //! \param path
    //! \param mode
    //! \return
    //!
    virtual bool open(const ky_path &, const eIoModeFlags & = Io_ReadWrite) {return false;}
    //!
    //! \brief open 字符串格式打开io
    //! \param name
    //! \param mode
    //! \return
    //!
    virtual bool open(const ky_string &name, const eIoModeFlags &mode = Io_ReadWrite)
    {
        return open (ky_path(name), mode);
    }

    //!
    //! \brief cancel 取消io操作,对非阻塞或异步有效
    //! \return
    //!
    virtual int cancel () {return 0;}
    //!
    //! \brief submit 异步io提交请求的操作
    //! \return
    //!
    virtual int submit () {return 0;}

    //!
    //! \brief read 随机性读取io数据
    //! \param buf 存放地址
    //! \param len 存放大小
    //! \param offset 当前位置的偏移
    //! \return 返回读取的实际大小
    //! \note 内存要求同上
    //!
    virtual i64 read(void *, i64, i64)const{return -1;}
    //!
    //! \brief write 随机性写入io数据
    //! \param buf 写入数据的地址
    //! \param len 写入的长度
    //! \param offset 当前位置的偏移
    //! \return 返回实际写入的长度
    //! \note 内存要求同上
    //!
    virtual i64 write(void *, i64, i64)const{return -1;}

    virtual void reset()
    {
        io_open = false;
        io_eof = false;
        io_error = Io_ErrorNot;
        io_mode = Io_ReadWrite;
        io_type = Io_TypeFile;
        io_statu = Io_StateNot;
        io_size = 0;
        io_pos = 0;
    }

    bool          io_open;
    bool          io_eof;
    eIoErrorFlags io_error;
    eIoModeFlags  io_mode;
    eIoTypeFlags  io_type;
    eIoStatuFlags io_statu;
    i64           io_size;
    i64           io_pos;
    ky_path       io_path;
};

#endif
