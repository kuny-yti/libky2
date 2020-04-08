
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
 * @file     ky_queue.h
 * @brief    采用ky_list链表实现队列
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
#ifndef ky_QUEUE
#define ky_QUEUE
#include <queue>
#include "ky_list.h"

template <typename T>
class ky_queue
{
public:
    ky_queue() :sequence() {}
    ky_queue(const ky_queue &v) :sequence(v.sequence) {}
    ky_queue(const std::queue<T> &v) :sequence() {form(v);}

    std::queue<T> &std()
    {
        return sequence.to_std ();
    }
    void form (const std::queue<T> &v)
    {

    }

    T& front(){return sequence.front ();}
    const T& front()const{return sequence.front ();}
    T& back(){return sequence.back ();}
    const T& back()const{return sequence.back ();}

    void push(const T& val) {sequence.push_back (val);}
#if kyLanguage >= kyLanguage11
    void push(T && v){sequence.push_back (v);}
#endif

    void remove() {sequence.pop_front ();}
    void clear() {sequence.clear ();}
    T pop() {T out = sequence.front (); remove (); return out;}
    int64 size()const {return sequence.size ();}
    int64 count()const {return size();}

    bool empty()const {return sequence.empty ();}
    bool is_empty()const {return empty();}

protected:
    ky_list<T> sequence;
};

#endif // ky_QUEUE

