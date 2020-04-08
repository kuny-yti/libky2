
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
 * @file     ky_color.h
 * @brief    颜色对象定义，几乎包含所有常用颜色空间转换
 *       1.定义了使用的类型.
 *       2.支持多种颜色空间
 *         RGBA, HSL, HSV, HWB, HSB, HSI, CMYK, LAB, YCoCg, YCrCb
 *       3.支持RGB颜色的运算
 *         相加、相减、相乘、差值
 *       4.支持RGB颜色的插值
 *         线性插值、双线性插值
 *       5.支持RGB颜色的合成及混合
 *       6.支持伽马校正、亮度值计算、强度值计算
 *
 * @author   kunyang
 * @email    kunyang.yk@gmail.com
 * @version  1.4.0.1
 * @date     2012/01/02
 * @license  GNU General Public License (GPL)
 *
 * Change History :
 *    Date    |  Version  |  Author  |   Description
 * 2012/01/02 | 1.0.0.1   | kunyang  | 创建文件
 * 2012/01/10 | 1.0.1.0   | kunyang  | 加入RGB组合模式
 * 2012/03/22 | 1.1.0.0   | kunyang  | 加入HSL、HSV、CMYK颜色转换
 * 2014/07/28 | 1.1.1.0   | kunyang  | 修改内部存储结构
 * 2015/04/11 | 1.2.0.0   | kunyang  | 加入颜色运算、颜色插值等计算
 * 2016/08/09 | 1.3.0.0   | kunyang  | 加入CIE-LAB颜色转换
 * 2017/04/13 | 1.3.1.0   | kunyang  | 修改内部算法
 * 2017/08/03 | 1.3.1.1   | kunyang  | 修改HEX颜色定义
 * 2018/01/16 | 1.4.0.1   | kunyang  | 加入YCoCg,YCoCg-R,YCrCb颜色转换
 *
 */
#ifndef ky_COLOR_H
#define ky_COLOR_H
#include "maths/ky_maths.h"
#include "tools/ky_stream.h"
#include "tools/ky_variant.h"

typedef u32 ky_color32; // R8-G8-B8-A8

const ky_color32 kyColorBlack     = 0x000000ff;   // Black
const ky_color32 kyColorWhite     = 0xffffffff;   // White
const ky_color32 kyColorZero      = 0x00000000;   // Zero
const ky_color32 kyColorRed       = 0xff0000ff;   // Red
const ky_color32 kyColorGreen     = 0x00ff00ff;   // Green
const ky_color32 kyColorBlue      = 0x0000ffff;   // Blue


/*!
 * @brief The ky_color class 简单的颜色处理类
 * @class ky_color
 */
struct ky_color
{
#if defined(kyColor16Bit)
    typedef flt16 Type ;
#elif defined(kyColor64Bit)
    typedef flt64 Type ;
#else
    typedef flt32 Type ;
#endif
    /*!
     * @brief The eRGBCompositeModes enum RGB颜色合成模式
     * @enum eRGBCompositeModes
     */
    typedef enum
    {
        RGB_CompositeNone = 0,
        RGB_CompositeNormal,
        RGB_CompositeBehind
    } eRGBCompositeModes;
    enum {size = 4, sizeOf = sizeof(Type)};

    union
    {
        // xyz
        struct {
            Type x; Type z; Type xyz_y; Type _w;
        };
        // rgb
        struct {
            //![0.0..1.0] or [0.0..255.0]
            //!@r: red @g: green @b: blue @a: alpha
            Type r; Type g; Type b;  Type a;
        };
        // hsb
        struct {
            Type h; Type s; Type _hsb_b; Type _hsb_a;
        };
        // hsv
        struct {
            //!H[0, 360] S[0.0-1.0] V[0.0-1.0]
            //!@h: hue @s: saturation @v: value @a: alpha
            Type _hsv_h; Type _hsv_s; Type v; Type _hsv_a;
        };
        // hsl
        struct {
            //!H[0, 360] S[0.0-1.0] L[0.0-1.0]
            //!@h: hue @s: saturation @l: lightness @a: alpha
            Type _hsl_h; Type _hsl_s; Type l; Type _hsl_a;
        };
        // hsi
        struct {
            //!H[0, 360] S[0.0-1.0] I[0.0-1.0]
            //!@h: hue @s: saturation @i: i32ensity @a: alpha
            Type _hsi_h; Type _hsi_s; Type i; Type _hsi_a;
        };
        // hwb
        struct {
            //! @h :hue (色调0-6) @w :whiteness (白度0-1) @b :blackness (黑度0-1)
            Type _hwb_h; Type w; Type _hwb_b; Type _hwb_a;
        };
        // CIE-LAB
        struct {
            //! @L :Luminosity (0-100)
            //! @A :Green - Magenta (-128 - +127)
            //! @B :Blue - yellow (-128 - +127)
            Type L; Type A; Type B; Type _lab_a;
        };
        // cmyk
        struct {
            //![0.0..1.0]
            //!@c: cyan @m: magenta @y: yellow @k: black @a: alpha
            Type c; Type m; Type y; Type k;
        };
        // YCrCb
        struct {
            //![0.0..1.0]
            //!@cb: Chroma @cr: Chrominance @y: Luminance  @a: alpha
            Type cb; Type cr; Type ycrcb_y; Type ycrcb_a;
        };
        // YCoCg
        struct {
            //![0.0..1.0]
            //!@cg: chrominance green @co: chrominance orange @y: Luminance
            Type cg; Type co; Type ycocg_y; Type ycocg_a;
        };

        Type elem[size];
    };

public:
    ky_color();
    //!
    //! \brief ky_color
    //! \param hex
    //!
    explicit ky_color(ky_color32 hex);
    //!
    //! \brief ky_color
    //! \param init
    //!
    explicit ky_color(f64 init);
    //!
    //! \brief ky_color
    //! \param init
    //!
    explicit ky_color(const f64* init);
    //!
    //! \brief ky_color
    //! \param init
    //!
    explicit ky_color(Type init);
    //!
    //! \brief ky_color
    //! \param init
    //!
    explicit ky_color(const Type* init);
    //!
    //! \brief ky_color
    //! \param init
    //!
    explicit ky_color(u8 init);
    //!
    //! \brief ky_color
    //! \param init
    //!
    explicit ky_color(const u8* init);
    //!
    //! \brief ky_color
    //! \param init
    //!
    explicit ky_color(const i32* init);

    //!
    //! \brief ky_color
    //! \param t1
    //! \param t2
    //! \param t3
    //! \param t4
    //!
    ky_color(f64 t1, f64 t2, f64 t3, f64 t4 = 1.0);
    //!
    //! \brief ky_color
    //! \param t1
    //! \param t2
    //! \param t3
    //! \param t4
    //!
    ky_color(Type t1, Type t2, Type t3, Type t4 = 1.0);
    //!
    //! \brief ky_color
    //! \param t1
    //! \param t2
    //! \param t3
    //! \param t4
    //!
    ky_color(u8 t1, u8 t2, u8 t3, u8 t4 = 255);
    //!
    //! \brief ky_color
    //! \param t1
    //! \param t2
    //! \param t3
    //! \param t4
    //!
    ky_color(i32 t1, i32 t2, i32 t3, i32 t4 = 255);

    //!
    //! \brief ky_color
    //! \param v
    //!
    ky_color(const ky_color & v);
    //!
    //! \brief operator =
    //! \param v
    //! \return
    //!
    ky_color &operator = (const ky_color &v);

    const Type& operator [] (int index) const;
    Type & operator [] (int index);

    //!
    //! \brief form_hex
    //! \param hex
    //!
    void form_hex(ky_color32 hex);
    //!
    //! \brief form_hsl
    //! \param hsl
    //!
    void form_hsl (const ky_color &hsl);
    //!
    //! \brief form_hsv
    //! \param hsv
    //!
    void form_hsv (const ky_color &hsv);
    //!
    //! \brief form_hsi
    //! \param hsi
    //!
    void form_hsi (const ky_color &hsi);
    //!
    //! \brief form_hwb
    //! \param hwb
    //!
    void form_hwb (const ky_color &hwb);
    //!
    //! \brief form_cmyk
    //! \param cmyk
    //!
    void form_cmyk (const ky_color &cmyk);
    //!
    //! \brief form_lab
    //! \param lab
    //!
    void form_lab (const ky_color &lab);
    //!
    //! \brief form_xyz
    //! \param xyz
    //!
    void form_xyz (const ky_color &xyz);
    //!
    //! \brief form_ycbcr
    //! \param yuv
    //!
    void form_ycbcr(const ky_color &yuv);
    //!
    //! \brief form_ycocg
    //! \param ycc
    //!
    void form_ycocg(const ky_color &ycc);
    //!
    //! \brief form_ycocgr
    //! \param ycc
    //!
    void form_ycocgr(const ky_color &ycc);

    //!
    //! \brief to_hex
    //! \return
    //!
    ky_color32 to_hex()const;
    //!
    //! \brief to_hsl
    //! \return
    //!
    ky_color to_hsl()const;
    //!
    //! \brief to_hsv
    //! \return
    //!
    ky_color to_hsv()const;
    //!
    //! \brief to_hsi
    //! \return
    //!
    ky_color to_hsi()const;
    //!
    //! \brief to_hwb
    //! \return
    //!
    ky_color to_hwb()const;
    //!
    //! \brief to_cmyk
    //! \return
    //!
    ky_color to_cmyk()const;
    //!
    //! \brief to_lab
    //! \return
    //!
    ky_color to_lab()const;
    //!
    //! \brief to_xyz
    //! \return
    //!
    ky_color to_xyz()const;
    //!
    //! \brief to_ycbcr
    //! \return
    //!
    ky_color to_ycbcr()const;
    //!
    //! \brief to_ycocg
    //! \return
    //!
    ky_color to_ycocg()const;
    //!
    //! \brief to_ycocgr
    //! \return
    //!
    ky_color to_ycocgr()const;

public:
    //!
    //! \brief gamma
    //! \param gamma
    //!
    inline void gamma(Type gamma){*this = rgb_gamma(*this, gamma);}
    //!
    //! \brief luminance
    //! \return
    //!
    inline Type luminance()const{return rgb_luminance(*this);}
    //!
    //! \brief intensity
    //! \return
    //!
    inline Type intensity()const{return rgb_intensity(*this);}
    //!
    //! \brief composite
    //! \param col
    //! \param mode
    //!
    inline void composite(const ky_color &col, eRGBCompositeModes  mode)
    {
        *this = rgb_composite(*this, col, mode);
    }
    //!
    //! \brief distance
    //! \param col
    //! \return
    //!
    inline Type distance(const ky_color &col){return rgba_distance(*this, col);}

    //!
    //! \brief interpolate
    //! \param initial
    //! \param final
    //! \param pos
    //! \return
    //!
    static ky_color interpolate (const ky_color &initial, const ky_color &final,
                                 Type pos);
    //!
    //! \brief blend
    //! \param _src
    //! \param _dst
    //! \param _S
    //! \return
    //!
    static ky_color blend(const ky_color &_src ,const ky_color &_dst, Type _S);

public:
    //!
    //! \brief operator +
    //! \param _src
    //! \return
    //!
    inline ky_color operator +(const ky_color &_src){return rgba_add(*this, _src);}
    //!
    //! \brief operator -
    //! \param _src
    //! \return
    //!
    inline ky_color operator -(const ky_color &_src){return rgba_subtract(*this, _src);}
    //!
    //! \brief operator *
    //! \param factor
    //! \return
    //!
    inline ky_color operator *(Type  factor){return rgba_multiply(*this, factor);}
    //!
    //! \brief operator +=
    //! \param _src
    //!
    inline void operator +=(const ky_color &_src){*this = rgba_add(*this, _src);}
    //!
    //! \brief operator -=
    //! \param _src
    //!
    inline void operator -=(const ky_color &_src){*this = rgba_subtract(*this, _src);}
    //!
    //! \brief operator *=
    //! \param factor
    //!
    inline void operator *=(Type  factor){*this = rgba_multiply(*this, factor);}

    //!
    //! \brief operator ==
    //! \param rhs
    //! \return
    //!
    inline bool operator == (const ky_color &rhs)const
    {
        return kyEqualf(r, rhs.r) &&
                kyEqualf(g, rhs.g) &&
                kyEqualf(b, rhs.b) &&
                kyEqualf(a, rhs.a);
    }
    //!
    //! \brief operator !=
    //! \param rhs
    //! \return
    //!
    inline bool operator != (const ky_color &rhs)const{return !((*this) == rhs);}
    //!
    //! \brief swap
    //! \param v
    //!
    void swap(ky_color &v)
    {
        ky_color tmp = v;
        for (int i = 0; i < this->size; ++i)
        {
            v.elem[i] = this->elem[i];
            this->elem[i] = tmp.elem[i];
        }
    }
    friend void ky_swap(ky_color &a, ky_color &b) {a.swap(b);}

private:
    //!
    //! \brief rgb_gamma 伽马校正
    //! \param rgb
    //! \param gamma
    //! \return
    //!
    static ky_color rgb_gamma (ky_color &rgb, Type  gamma);
    //!
    //! \brief rgb_luminance 亮度值
    //! \param rgb
    //! \return
    //!
    static Type rgb_luminance (const ky_color &rgb);
    //!
    //! \brief rgb_i32ensity 强度值
    //! \param rgb
    //! \return
    //!
    static Type rgb_intensity (const ky_color &rgb);
    //!
    //! \brief rgb_composite
    //! \param color1
    //! \param color2
    //! \param mode
    //! \return
    //!
    static ky_color rgb_composite (ky_color &color1, const ky_color &color2,
                        eRGBCompositeModes  mode);
    //!
    //! \brief rgba_add
    //! \param rgba1
    //! \param rgba2
    //! \return
    //!
    static ky_color rgba_add (ky_color &rgba1, const ky_color &rgba2);
    //!
    //! \brief rgba_subtract
    //! \param rgba1
    //! \param rgba2
    //! \return
    //!
    static ky_color rgba_subtract (ky_color &rgba1, const ky_color &rgba2);
    //!
    //! \brief rgba_multiply
    //! \param rgba
    //! \param factor
    //! \return
    //!
    static ky_color rgba_multiply (ky_color &rgba ,Type  factor);
    //!
    //! \brief rgba_distance
    //! \param rgba1
    //! \param rgba2
    //! \return
    //!
    static Type rgba_distance (const ky_color &rgba1, const ky_color &rgba2);

};

typedef ky_color color;
const fourwd_t kyColorCode = kyFourWord(colr);

ky_stream &operator << (ky_stream &in, const ky_color &v);
ky_stream &operator >> (ky_stream &out, ky_color &v);
ky_variant &operator << (ky_variant &va, const ky_color &col);
ky_variant &operator >> (ky_variant &va, ky_color &col);

#endif // COLOR_H
