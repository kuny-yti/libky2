
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
 * @file     ky_stack.h
 * @brief    采用ky_list链表实现堆栈
 *
 * @author   kunyang
 * @email    kunyang.yk@gmail.com
 * @version  1.0.0.1
 * @date     2014/05/02
 * @license  GNU General Public License (GPL)
 *
 * Change History :
 *    Date    |  Version  |  Author  |   Description
 * 2014/05/02 | 1.0.0.1   | kunyang  | 创建文件
 */
#ifndef ky_STACK
#define ky_STACK
#include <stack>
#include "ky_list.h"

template <typename T>
class ky_stack
{
public:
    ky_stack(): sequence() {}
    ky_stack(const ky_stack<T> &s): sequence(s.sequence) {}
    ky_stack(const std::stack<T> &s):sequence() {form(s);}

    std::stack<T> &to_std(){return sequence.to_std ();}
    void form (const std::stack<T> &rhs)
    {

    }

    T &top(){return sequence.back ();}
    const T &top() const{return sequence.back ();}

    void push(const T& val){sequence.push_back (val);}
#if kyLanguage >= kyLanguage11
    void push(T && v){sequence.push_back (v);}
#endif
    T pop(){T out = sequence.back (); remove ();return out;}
    void remove(){sequence.pop_back();}
    void swap(ky_stack& s){ky_swap (sequence, s.sequence);}
    friend void ky_swap (ky_stack &a, ky_stack &b){a.swap (b);}

    int64 size() const{return sequence.size ();}
    int64 count() const {return size();}

    bool empty() const {return sequence.empty ();}
    bool is_empty() const{return empty ();}
protected:
    ky_list <T> sequence;
};

#endif // ky_STACK

