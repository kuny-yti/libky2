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
 * @file     ky_rbtree.h
 * @brief    模板链表数据空间容器类(支持C++11)
 *       1.支持初始化列表初始化
 *       2.内部算法使用ky_memory::array_addr
 *       3.支持引用计数和写时拷贝
 *       4.采用连续地址指针模式实现快速链表遍历
 *
 * @author   kunyang
 * @email    kunyang.yk@gmail.com
 * @version  1.2.1.1
 * @date     2010/05/08
 * @license  GNU General Public License (GPL)
 *
 * Change History :
 *    Date    |  Version  |  Author  |   Description
 * 2010/06/01 | 1.0.0.1   | kunyang  | 创建文件
 * 2011/02/16 | 1.0.1.0   | kunyang  | 加入ky_alloc模板分配类
 * 2014/08/10 | 1.0.2.0   | kunyang  | 加入对象引用计数，同时加入C++11支持
 * 2015/03/06 | 1.0.2.2   | kunyang  | 修改将头信息和链表地址共用
 * 2017/06/12 | 1.1.0.1   | kunyang  | 将原有指针型链表修改为ky_linked类，采样连续地址指针模式
 * 2018/02/27 | 1.2.0.1   | kunyang  | 将连续地址指针思想重构并移入ky_memory::array_addr
 * 2018/03/18 | 1.2.1.1   | kunyang  | 将模板对象分为可构造来优化速度
 */

#ifndef KY_RBTREE_H
#define KY_RBTREE_H

#include "ky_define.h"
#include "ky_algorlthm.h"
#include "ky_typeinfo.h"
#include "arch/ky_memory.h"

struct rb_node
{
    enum  {cBlack = 0, cRed = 1, cParent = 0, cLeft = 1, cRight = 2, cCount = 3};

    char     color;
    rb_node *child[cCount];

    //!
    //! \brief left 左节点
    //! \return
    //!
    inline rb_node *left() const { return child[cLeft];}
    //!
    //! \brief right 右节点
    //! \return
    //!
    inline rb_node *right() const { return child[cRight];}
    //!
    //! \brief parent 父节点
    //! \return
    //!
    inline rb_node *parent() const { return child[cParent];}

    //!
    //! \brief next  下一个节点
    //! \return
    //!
    rb_node *next();
    //!
    //! \brief prev 上一个节点
    //! \return
    //!
    rb_node *prev();

    //!
    //! \brief compare 比较节点
    //! \param rhs
    //! \return >0: 大于rhs, <0: 小于rhs， ==0:相等
    //! \note 需要实现节点的比较，不然树无法插入
    //!
    virtual char compare (rb_node * kyUnused(rhs)) const {return 0;}
};

struct rb_tree
{
    rb_node  head;
    rb_node *most_left;

    //!
    //! \brief left_rotate 对红黑树的节点(x)进行左旋转
    //! \param x
    //!
    void left_rotate(rb_node* x);

    //!
    //! \brief right_rotate 对红黑树的节点(y)进行右旋转
    //! \param y
    //!
    void right_rotate(rb_node* y);

    //!
    //! \brief insert_rebalance 插入节点对红黑树进行平衡修正
    //! \param x
    //!
    void insert_rebalance(rb_node *x);

    //!
    //! \brief remove_rebalance 删除节点对红黑树进行平衡修正
    //! \param z
    //! \param parent
    //!
    void remove_rebalance(rb_node *z, rb_node *parent);

    //!
    //! \brief remove_node
    //! \param z
    //!
    void remove (rb_node *z);

    //!
    //! \brief insert
    //! \param z
    //! \param parent
    //! \param is_left
    //!
    void insert (rb_node *z);
 };

template<typename T, typename Alloc = ky_alloc<T>>
class ky_rbtree : protected rb_tree, private Alloc
{
private:
    i64       node_count;
    enum {tComplex = is_complex<T>::value};

public:
    class node : protected rb_node
    {
        T key;
    public:
        inline node *left() const ;
        inline node *right() const ;
        inline node *next() ;
        inline node *prev() ;

        virtual char compare (rb_node *rhs) const;
        inline void destroy();
    protected:
        inline void destroy_child(false_type_t);
        inline void destroy_child(true_type_t);
    };
    typedef const node const_node;

public:
    ky_rbtree();
    virtual ~ky_rbtree();

    //!
    //! \brief root
    //! \return
    //!
    inline node *root() const;
    //!
    //! \brief end
    //! \return
    //!
    inline const_node *end() const;

    //!
    //! \brief end
    //! \return
    //!
    inline node *end();
    //!
    //! \brief begin
    //! \return
    //!
    inline const_node *begin() const;
    //!
    //! \brief begin
    //! \return
    //!
    inline node *begin();

    //!
    //! \brief find
    //! \param k
    //! \return
    //!
    inline node *find (const T &k)const;
    //!
    //! \brief search
    //! \param k
    //! \return
    //!
    inline node *search(const T &k)const;

    //!
    //! \brief insert
    //! \param v
    //!
    void insert(const T &v);

    //!
    //! \brief remove
    //! \param v
    //!
    void remove(const T &v);

    //!
    //! \brief minimum
    //! \return
    //!
    const T minimum()const;
    //!
    //! \brief maximum
    //! \return
    //!
    const T maximum()const;

protected:
    node *create_node(false_type_t, const T &v);
    node *create_node(true_type_t, const T &v);
    void destory_node(false_type_t, node *n);
    void destory_node(true_type_t, node *n);
};

#include "ky_rbtree.inl"
#endif
