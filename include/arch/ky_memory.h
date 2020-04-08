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
 * @file     ky_memory.h
 * @brief    内存的操作定义
 *       1.ky_alloc 简单的内存分配器
 *       2.ky_memory 内存的拷贝及清理的快速实现，可以评估那种加速方式更高效
 *
 * @author   kunyang
 * @email    kunyang.yk@gmail.com
 * @version  1.0.1.1
 * @date     2013/05/01
 * @license  GNU General Public License (GPL)
 *
 * Change History :
 *    Date    |  Version  |  Author  |   Description
 * 2013/05/01 | 1.0.0.1   | kunyang  | 创建文件
 * 2014/01/10 | 1.0.0.2   | kunyang  | 加入快速评估加速指令的选择
 * 2014/02/20 | 1.0.1.0   | kunyang  | 建立ky_alloc
 * 2018/03/10 | 1.0.1.1   | kunyang  | 加入内存增长计算
 *
 */

#ifndef KY_MEMORY_H
#define KY_MEMORY_H

#include "ky_define.h"
#include "ky_utils.h"
#include "ky_ref.h"

typedef enum
{
    Memcpy_Default = 0,
    Memcpy_SSE = 1 << 0,
    Memcpy_MMX = 1 << 2,
    Memcpy_MMX2 = 1 << 3,
    MemCoy_3DNOW = 1 << 4,
    Memcpy_Auto = 1 << 5
}eMemcpyModes;

template<typename T>
struct ky_alloc
{
    static T* alloc(int64 size)
    {
        return (T*)kyMalloc (size);
    }
    static T* realloc(T* mem, int64 size)
    {
        return (T*)kyRealloc((void*)mem, size);
    }
    static void destroy(void *mem)
    {
        kyFree (mem);
    }
};

//!
//! \brief The ky_memory class
//! \note
//!   1.内部为内存池机构，扩展出对象池机构
//!   2.可快速分配地址和对象
//!   3.自动回收垃圾机制
//!   4.内存分析机制：
//!     a.那些对象经常申请或释放内存
//!     b.每个对象使用的地址范围
//!     c.每个对象使用的内存情况
//!     d.在出现异常时可以快速确定异常的对象和内存操作的位置
//!
class ky_memory
{
public:
    enum PageType
    {
        Large,     // 使用大页
        Small,     // 使用小页
        Default	   // 启发式
    };
    enum PagePort
    {
        None = 0,  // 无法访问页面
        Read = 1,  // 可以读取页面
        Write = 2, // 可以写入页面
        Exec = 4   // 可以执行页面
    };

    //!
    //! \brief The growth_linear struct 线性增长
    //! \note O(N) 分配比 O(1) 要浪费空间
    //!
    template<uint64 increment = kyMmuPageSize>
    struct growth_linear
    {
        uint64 operator()(uint64 size) const
        {
            return size + increment;
        }
    };

    //!
    //! \brief The growth_exp struct 指数增长
    //! \note O(log r) 分配比 O(N) 要浪费空间
    //!       注意：通常选择扩展因子 r = 2
    //!
    template<uint64 multiplier = 21, uint64 divisor = 16>
    struct growth_exp
    {
        uint64 operator()(uint64 size) const
        {
            const uint64 product = size * multiplier;
            kyASSERT(product >= size);
            return product / divisor;
        }
    };

    //!
    //! \brief The dynaddr struct 动态数组形式的地址
    //! \note Algorithm complexity:
    //!        1.insert   O(n)
    //!        2.append   O(1)-near
    //!        3.prepend  O(1)-near
    //!        4.at       O(1)
    //!
    struct dynaddr
    {
        typedef struct header_t header_t;
        typedef struct {void *value;} node_t;
        enum{NodeSize = sizeof(node_t)};

        header_t *header;

        //!
        //! \brief dynaddr 地址数组构造
        //! \note 头为空
        //!
        dynaddr();
        //!
        //! \brief dynaddr 地址数组构造
        //! \param h 引用头
        //! \note
        //!
        explicit dynaddr(const header_t *);
        //!
        //! \brief ~dynaddr 地址数组析构
        //! \note 不释放内存
        //!
        virtual ~dynaddr();

        //!
        //! \brief refer 返回引用计数器
        //! \return
        //!
        inline ky_ref &refer(){return *(ky_ref*)header;}
        inline ky_ref &refer()const{return *(ky_ref*)header;}
        //!
        //! \brief addref 引用计数器操作
        //!
        inline void addref(){refer ().addref ();}
        inline bool lessref(){return refer ().lessref ();}

        //!
        //! \brief is_nul 当前内存块是否为默认
        //! \return
        //!
        bool is_nul ();
        bool is_nul()const;

        //!
        //! \brief null 返回默认内存块
        //! \return
        //!
        header_t* nul();

        //!
        //! \brief reserve 根据size创建内存块
        //! \param size
        //! \return 返回旧的内存块
        //! \note 外部使用时，若出现多次引用需要将其分离在调用
        //!
        header_t *reserve(int64 size);

        //!
        //! \brief destroy 释放当前内存块
        //! \note 任何时候不需要内存块时必须用它销毁
        //!
        void destroy();

        //!
        //! \brief expand 扩充内存块
        //! \param growth 需要增长的值
        //! \return 是否扩充成功
        //!
        bool expand(int64 growth);

        //!
        //! \brief detach 分离
        //! \param idx   内部写入后续的索引号
        //! \param count 分离时需要多余的内存数
        //! \return 返回旧的内存块
        //!
        header_t *detach(int *idx, int count);

        //!
        //! \brief append 尾部添加n个元素
        //! \param n
        //! \return 返回内存块开始位置
        //!
        node_t *append(int n);

        //!
        //! \brief prepend 头部添加n个元素
        //! \param n
        //! \return 返回内存块开始位置
        //! \note 注意返回内存为正序
        //!
        node_t *prepend(int n);

        //!
        //! \brief insert 任意位置插入n个元素
        //! \param i 插入位置
        //! \param n
        //! \return 返回内存块开始位置
        //!
        node_t *insert(int i, int n);

        //!
        //! \brief remove 任意位置删除n个元素
        //! \param i 删除位置
        //! \param n
        //! \note 注意若为对象时，对象内存需要释放
        //!
        void remove(int i, int n);
        void remove(int i);

        //!
        //! \brief erase 擦除一块内存，并返回下一块内存地址
        //! \param xi
        //! \return
        //!
        node_t *erase(node_t *xi);

        //!
        //! \brief move 将from位置开始的n个元素移动到to位置处
        //! \param from
        //! \param n
        //! \param to
        //! \note 注意当from+n超过to时则不移动
        //!
        void move(int from, int64 n, int to);

        //!
        //! \brief count 元素数量
        //! \return
        //!
        int64 count() const;
        int64 capacity()const;

        //!
        //! \brief is_empty 内存块中元素为空
        //! \return
        //!
        bool is_empty() const;

        //!
        //! \brief at 取出指定位置的元素
        //! \param i
        //! \return
        //!
        node_t *at(int i);
        node_t *at(int i)const;

        //!
        //! \brief begin 返回元素的开始地址
        //! \return
        //!
        node_t *begin();
        node_t *begin()const;

        //!
        //! \brief end 返回元素的结束地址
        //! \return
        //!
        node_t *end();
        node_t *end()const;

    };

    //!
    //! \brief The dynaddr struct 动态数组形式的内存
    //! \note Algorithm complexity:
    //!        1.insert   O(n)
    //!        2.append   O(1)-near
    //!        3.prepend  O(1)-near
    //!        4.at       O(1)
    //!
    //!
    struct dynarray
    {
        typedef struct header_t header_t;

        header_t * header;

        dynarray();
        explicit dynarray(const header_t *);
        virtual ~dynarray();

        //!
        //! \brief refer 返回引用计数器
        //! \return
        //!
        ky_ref &refer() ;
        void addref();
        bool lessref();

        //!
        //! \brief is_null 当前内存块是否为默认
        //! \return
        //!
        bool is_nul ();
        bool is_nul ()const;
        header_t * nul();

        //!
        //! \brief count 元素数量
        //! \return
        //!
        int64 count() const;
        int64 capacity()const;

        //!
        //! \brief is_empty 内存块中元素为空
        //! \return
        //!
        bool is_empty() const;

        //!
        //! \brief reserve 根据size创建内存块
        //! \param size
        //! \param align 对齐字节数
        //! \return 返回旧的内存块
        //! \note 外部使用时，若出现多次引用需要将其分离在调用
        //!
        header_t *reserve(int64 size, int align);
        void resize(int64 size, int align);

        //!
        //! \brief destroy 销毁内存块
        //!
        void destroy();

        //!
        //! \brief expand 根据growth增长值扩充内存块
        //! \param growth
        //! \return
        //!
        bool expand(int64 growth);

        //!
        //! \brief detach 分离
        //!
        header_t *detach(int *idx, int count, int align);

        //!
        //! \brief append 尾部添加n个元素
        //! \param c
        //! \return
        //!
        void* append(int c);

        //!
        //! \brief prepend 头部添加n个元素
        //! \param n
        //! \return
        //! \note 注意返回内存为正序
        //!
        void* prepend(int c);

        //!
        //! \brief insert 任意位置插入n个元素
        //! \param i
        //! \param n
        //! \return
        //!
        void* insert(int i, int c);

        //!
        //! \brief remove 任意位置删除n个元素
        //! \param i
        //! \param n
        //! \note 注意若为对象时，对象内存需要释放
        //!
        void remove(int i, int c);

        //!
        //! \brief move 将from位置开始的n个元素移动到to位置处
        //! \param from
        //! \param n
        //! \param to
        //! \note 注意当from+n超过to时则不移动
        //!
        void move(int from, int n, int to);

        //!
        //! \brief at 取出指定位置的元素
        //! \param i
        //! \return
        //!
        void* at(int i);
        void* at(int i)const;

        //!
        //! \brief begin 返回元素的开始地址
        //! \return
        //!
        void* begin();
        void* begin()const;

        //!
        //! \brief end 返回元素的结束地址
        //! \return
        //!
        void* end();
        void* end()const;
    };


    struct virtual_memory
    {
        //!
        //! \brief ReserveAddressSpace 保留地址空间并为以后的任何按需提交设置参数
        //! \param size 所需的字节数 ,最后一页中的任何其他空间也可以访问.
        //! \param commit 每次要提交多少
        //! \param pageType 在大/小页面之间进行选择
        //! \param prot 内存保护标志用于新提交的页面
        //! \return 返回基地址（与相应的页面大小对齐）；如果地址空间/描述符存储已用尽，则返回0
        //! \note 1.commit 较大的值以增加内部碎片为代价，减少了页面错误的数量。
        //!         除非pageType == kSmall，否则必须为largePageSize的倍数
        //!       2.必须通过ReleaseAddressSpace释放
        //!
        void* ReserveAddressSpace(uint64 size, uint64 commit = kyMmuLargePageSize,
                                  PageType type = Default, int prot = Read | Write);

        /**
         * 释放地址空间并取消使用任何内存
         *
         * @param p 一个先前由ReserveAddressSpace返回的指针
         * @param size 是POSIX实现所必需的，而在Windows上则被忽略。 它还确保与UniqueRange兼容
         **/
        void ReleaseAddressSpace(void* p, uint64 size = 0);

        //!
        //! \brief Commit 将物理内存映射到以前保留的地址空间
        //! \param address 大小不需要对齐，但是此函数将提交与该间隔相交的所有页面
        //! \param size
        //! \param type
        //! \param prot
        //! \return  内存是否成功提交
        //! \note 仅提交会映射虚拟页面，实际上不会分配页面框架。
        //!
        //!
        bool Commit(uintptr address, uint64 size, PageType type = Default,
                    int prot = Read | Write);

        //!
        //! \brief Decommit 取消映射物理内存
        //! \param address
        //! \param size
        //! \return  操作是否成功
        //!
        bool Decommit(uintptr address, uint64 size);

        //!
        //! \brief Protect 为与给定间隔相交的所有页面设置内存保护标志
        //! \param address
        //! \param size
        //! \param prot 内存保护标志
        //! \return
        //! \note 这些页面当前必须已提交
        //!
        bool Protect(uintptr address, uint64 size, int prot);

        //!
        //! \brief Allocate 保留地址空间并提交内存
        //! \param size 分配
        //! \param type
        //! \param prot
        //! \return 零初始化的内存与相应的页面大小对齐
        //!
        void* Allocate(uint64 size, PageType type = Default, int prot = Read | Write);

        //!
        //! \brief Free 取消内存并释放地址空间
        //! \param p 先前由分配返回的指针
        //! \param size 是POSIX实现所必需的，而在Windows上则被忽略。 它还确保与UniqueRange兼容
        //! \note 这与ReleaseAddressSpace不同，后者必须考虑对largePageSize进行额外的填充/对齐
        //!
        void Free(void* p, uint64 size = 0);

        //!
        //! \brief BeginOnDemandCommits 安装一个处理程序，该处理程序在遇到读/写页面错误时尝试提交内存。 线程安全的
        //!
        void BeginOnDemandCommits();

        //!
        //! \brief EndOnDemandCommits 减少由BeginOnDemandCommit开始的引用计数，并在页面错误处理程序达到0时将其移除。线程安全
        //!
        void EndOnDemandCommits();

        void DumpStatistics();
    };
private:
    friend class ky_singleton<ky_memory>;
    ky_memory();
    ~ky_memory();

public:
    //!
    //! \brief copy 快速内存拷贝
    //! \param dst
    //! \param src
    //! \param len
    //! \return
    //!
    static void *copy(void *dst, const void* src, int64 len);
    //!
    //! \brief move 快速内存移动
    //! \param dst
    //! \param src
    //! \param len
    //! \return
    //!
    static void *move(void *dst, const void* src, int64 len);
    //!
    //! \brief compare 内存比较
    //! \param dst
    //! \param src
    //! \param len
    //! \return
    //!
    static int compare(const void *dst, const void* src, int64 len);
    //!
    //! \brief zero 内存清零
    //! \param dst
    //! \param len
    //! \param fill
    //!
    static void zero(void *dst, int64 len, int fill = 0);

    //!
    //! \brief block_size 根据大小和对齐计算内存尺寸
    //! \param size   需要分配的尺寸
    //! \param align  内存的字节对齐
    //! \param header 内存的头字节
    //! \return 返回计算内存大小
    //!
    static int64 block_size(int64 size, int align, int64 header);
    //!
    //! \brief block_growing 根据大小和对齐计算内存的增长
    //! \param size   需要分配的尺寸
    //! \param align  内存的字节对齐
    //! \param header 内存的头字节
    //! \param count  返回的申请到的尺寸
    //! \return 返回计算内存大小
    //!
    static int64 block_growing(int64 size, int align, int64 header,
                                int64 *count = 0);

    //!
    //! \brief aligned_alloc 对齐方式向系统申请内存
    //! \param size 申请多少
    //! \param align 对齐字节
    //! \return
    //!
    static void* aligned_alloc (uint64 size, uint64 align);
    //!
    //! \brief aligned_free 释放申请的对齐内存
    //! \param ptr 内存地址
    //!
    static void aligned_free (void* ptr);

    //!
    //! \brief query_size 查询操作系统报告的物理内存大小[MB]
    //! \return
    //!
    static uint64 query_size();
    //!
    //! \brief query_available 查询当前可用内存量[MB]
    //! \return
    //!
    static uint64 query_available();
};


#endif // ky_MEMCPY

