
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
 * @file     ky_random.h
 * @brief    随机数生成类
 *
 * @author   kunyang
 * @email    kunyang.yk@gmail.com
 * @version  1.0.0.1
 * @date     2014/04/09
 * @license  GNU General Public License (GPL)
 *
 * Change History :
 *    Date    |  Version  |  Author  |   Description
 * 2018/11/29 | 1.0.0.1   | kunyang  | 创建文件
 */
#ifndef KY_RANDOM_H
#define KY_RANDOM_H

#include "ky_maths.h"
#include "ky_bigint.h"

class ky_random
{
public:
    //!
    //! \brief rand 均匀分布的随机数
    //! \param seed
    //! \return
    //!
    static real rand(real seed);

    //!
    //! \brief normality 正态分布的随机数
    //! \param seed
    //! \return
    //!
    static real normality(real u, real t, real seed);
    //!
    //! \brief gauss 高斯分布的随机数(中心极限定理)
    //! \param seed
    //! \return
    //!
    static real gauss(real seed);

    //!
    //! \brief box_muller Box-Muller正态分布的随机数
    //! \param seed
    //! \return
    //!
    static real box_muller(real seed);

    //!
    //! \brief marsaglia Marsaglia正态分布的随机数
    //! \param seed
    //! \return
    //!
    static real marsaglia(real seed);
protected:

private:
};

#endif
