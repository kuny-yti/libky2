
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
 * @file     ky_image.h
 * @brief    像素图像类
 *           1.内存采用引用计数模式
 *           2.支持图像缩放
 *           2.支持图像格式转换
 *           2.支持图像裁剪
 *           2.支持 OpenGL 像素格式计算
 *           2.支持三维图像
 *
 * @author   kunyang
 * @email    kunyang.yk@gmail.com
 * @version  1.3.2.1
 * @date     2013/12/10
 * @license  GNU General Public License (GPL)
 *
 * Change History :
 *    Date    |  Version  |  Author  |   Description
 * 2013/12/10 | 1.0.0.1   | kunyang  | 创建文件
 * 2014/06/19 | 1.1.0.1   | kunyang  | 图像内存修改为内存引用模式
 * 2014/09/20 | 1.1.1.1   | kunyang  | 实现CPU图像的缩放
 * 2015/01/06 | 1.1.2.1   | kunyang  | 重构类并将像素格式分离
 * 2017/09/23 | 1.2.0.1   | kunyang  | 加入图像的裁剪和转换
 * 2018/10/11 | 1.3.0.1   | kunyang  | 加入三维纹理图像支持
 *
 */
#ifndef KY_IMAGE_H
#define KY_IMAGE_H

#include "ky_define.h"
#include "ky_color.h"
#include "ky_string.h"
#include "maths/ky_vec3.h"

typedef enum
{
    Image_NOT,
    Image_TGA,
    Image_PNG,
    Image_BMP,
    Image_JPEG,
    Image_PSD,
    Image_HDR,
    Image_PIC,
    Image_GIF,
    Image_PNM
}eImageFormats;

typedef enum
{
    ///< 8-bit/16-bit Luminance
    Pixel_L8,
    Pixel_L16,

    ///< 8-bit Alpha
    Pixel_A8,

    ///< 8-bit Alpha, 8-bit Luminance
    Pixel_LA8,

    ///< 5-bit Red, 6-bit Green, 5-bit Blue
    Pixel_RGB565, ///< packed RGB 5:6:5, 16bpp, (msb)   5R 6G 5B(lsb)

    ///< 4-bit Alpha, 4-bit Red, 4-bit Green, 4-bit Blue
    Pixel_RGBA4, ///< 16-bit Pixel

    ///< 1-bit Alpha, 5-bit Red, 5-bit Green, 5-bit Blue
    Pixel_RGB5A1, ///< 16-bit Pixel

    ///< 2-bit Alpha, 10-bit Red, 10-bit Green, 10-bit Blue
    Pixel_RGB10A2, ///< 32-bit Pixel

    ///< float 11-bit Red, 11-bit Green, 10-bit Blue
    Pixel_RG11B10f,///< 32-bit Pixel
    ///<
    Pixel_RGB9E5,

    ///< 8-bit Alpha, 8-bit Red, 8-bit Green, 8-bit Blue
    /// unsigned char
    Pixel_R8,
    Pixel_RG8,
    Pixel_RGB8,  ///< 24-bit Pixel
    Pixel_RGB = Pixel_RGB8,
    Pixel_RGB24 = Pixel_RGB,     ///< packed RGB 8:8:8, 24bpp, RGBRGB...
    Pixel_RGBA8, ///< 32-bit Pixel
    Pixel_RGBA = Pixel_RGBA8, ///< packed RGBA 8:8:8:8, 32bpp, RGBARGBA...

    Pixel_R8sn,
    Pixel_RG8sn,
    Pixel_RGB8sn,
    Pixel_RGBA8sn,

    Pixel_sRGB8,
    Pixel_sRGBA8,

    ///< 8-bit Red, 8-bit Green, 8-bit Blue, 8-bit Alpha
    /// unsigned char
    Pixel_R8i,
    Pixel_RG8i,
    Pixel_RGB8i,
    Pixel_RGBA8i,

    ///< 16-bit Red, 16-bit Green, 16-bit Blue, 16-bit Alpha
    /// unsigned int
    Pixel_R16i,    ///< 16-bit Pixel
    Pixel_RG16i,   ///< 32-bit Pixel
    Pixel_RGB16i,  ///< 48-bit Pixel
    Pixel_RGB48 = Pixel_RGB16i,     ///< packed RGB 16:16:16, 48bpp, 16R, 16G, 16B
    Pixel_RGBA16i, ///< 64-bit Pixel
    Pixel_RGBA64 = Pixel_RGBA16i,    ///< packed RGBA 16:16:16:16, 64bpp, 16R, 16G, 16B, 16A

    ///< 32-bit Red, 32-bit Green, 32-bit Blue, 32-bit Alpha
    /// unsigned int
    Pixel_R32i,    ///< 32-bit Pixel
    Pixel_RG32i,   ///< 64-bit Pixel
    Pixel_RGB32i,  ///< 96-bit Pixel
    Pixel_RGBA32i, ///< 128-bit Pixel

    ///< 16-bit float
    Pixel_R16f,    ///< 16-bit Pixel
    Pixel_RG16f,   ///< 32-bit Pixel
    Pixel_RGB16f,  ///< 48-bit Pixel
    Pixel_RGBA16f, ///< 64-bit Pixel

    ///< 32-bit float
    Pixel_R32f,    ///< 32-bit Pixel
    Pixel_RG32f,   ///< 64-bit Pixel
    Pixel_RGB32f,  ///< 96-bit Pixel
    Pixel_RGBA32f, ///< 128-bit Pixel

    ///< Ericcson texture compression)
    Pixel_ETC1RGB8, ///< RGB888 Not Alpha
    Pixel_ETC2RGB8,
    Pixel_ETC2RGBA8,
    Pixel_ETC2RGB8A1,

    ///< PowerVR texture compression
    Pixel_PVRTCRGB4, ///< RGB 4 bit per pixel
    Pixel_PVRTCRGB2,
    Pixel_PVRTCRGBA4,
    Pixel_PVRTCRGBA2,
    Pixel_PVRTCRGBA4V2,
    Pixel_PVRTCRGBA2V2,

    ///< ATI texture compression
    Pixel_ATITCRGB,
    Pixel_ATITCRGBAe,
    Pixel_ATITCRGBAi,

    ///< S3 texture compression (DXT)
    Pixel_S3TC1, ///< DXT1
    Pixel_S3TC2, ///< DXT2
    Pixel_S3TC3, ///< DXT3
    Pixel_S3TC4, ///< DXT4
    Pixel_S3TC5, ///< DXT5

    ///< Adaptive Scalable Texture Compression

    ///< Depth
    Pixel_Depth,

    Pixel_BGR8,      ///< packed BGR 8:8:8, 24bpp, BGRBGR...
    Pixel_BGR = Pixel_BGR8,
    Pixel_ARGB,      ///< packed ARGB 8:8:8:8, 32bpp, ARGBARGB...
    Pixel_ABGR,      ///< packed ABGR 8:8:8:8, 32bpp, ABGRABGR...
    Pixel_BGRA,      ///< packed BGRA 8:8:8:8, 32bpp, BGRABGRA...

    Pixel_XRGB4,     ///< packed RGB 4:4:4, 16bpp, (msb)4X 4R 4G 4B(lsb)
    Pixel_XRGB5,     ///< packed RGB 5:5:5, 16bpp, (msb)1X 5R 5G 5B(lsb)
    Pixel_BGR16,     ///< packed RGB 16:16:16, 48bpp, 16B, 16G, 16R
    Pixel_XBGR4,     ///< packed BGR 4:4:4, 16bpp, (msb)4X 4B 4G 4R(lsb)
    Pixel_XBGR5,     ///< packed BGR 5:5:5, 16bpp, (msb)1X 5B 5G 5R(lsb)
    Pixel_BGR565,    ///< packed BGR 5:6:5, 16bpp, (msb)   5B 6G 5R(lsb)
    Pixel_BGRA16,    ///< packed RGBA 16:16:16:16, 64bpp, 16B, 16G, 16R, 16A

    Pixel_AYUV16,    ///< packed AYUV 4:4:4,64bpp, (1 Cr & Cb sample per 1x1 Y & A samples)
    Pixel_YUYV422,   ///< packed YUV 4:2:2, 16bpp, Y0 Cb Y1 Cr
    Pixel_UYVY422,   ///< packed YUV 4:2:2, 16bpp, Cb Y0 Cr Y1
    Pixel_YVYU422,   ///< packed YUV 4:2:2, 16bpp, Y0 Cr Y1 Cb
    Pixel_UYYVYY411, ///< packed YUV 4:1:1, 12bpp, Cb Y0 Y1 Cr Y2 Y3
    Pixel_XYZ12,     ///< packed XYZ 4:4:4, 36 bpp, (msb) 12X, 12Y, 12Z (lsb)

    Pixel_YUV410P,   ///< planar YUV 4:1:0,  9bpp, (1 Cr & Cb sample per 4x4 Y samples)
    Pixel_YUV411P,   ///< planar YUV 4:1:1, 12bpp, (1 Cr & Cb sample per 4x1 Y samples)
    Pixel_YUV420P,   ///< planar YUV 4:2:0, 12bpp, (1 Cr & Cb sample per 2x2 Y samples)
    Pixel_YUV422P,   ///< planar YUV 4:2:2, 16bpp, (1 Cr & Cb sample per 2x1 Y samples)
    Pixel_YUV440P,   ///< planar YUV 4:4:0, (1 Cr & Cb sample per 1x2 Y samples)
    Pixel_YUV444P,   ///< planar YUV 4:4:4, 24bpp, (1 Cr & Cb sample per 1x1 Y samples)

    Pixel_YUVA420P,  ///< planar YUV 4:2:0, 20bpp, (1 Cr & Cb sample per 2x2 Y & A samples)
    Pixel_YUVA422P,  ///< planar YUV 4:2:2, 24bpp, (1 Cr & Cb sample per 2x1 Y & A samples)
    Pixel_YUVA444P,  ///< planar YUV 4:4:4, 32bpp, (1 Cr & Cb sample per 1x1 Y & A samples)

    Pixel_YUV420P9,  ///< planar YUV 4:2:0, 13.5bpp, (1 Cr & Cb sample per 2x2 Y samples)
    Pixel_YUV422P9,  ///< planar YUV 4:2:2, 18bpp, (1 Cr & Cb sample per 2x1 Y samples)
    Pixel_YUV444P9,  ///< planar YUV 4:4:4, 27bpp, (1 Cr & Cb sample per 1x1 Y samples)

    Pixel_YUV420P10, ///< planar YUV 4:2:0, 15bpp, (1 Cr & Cb sample per 2x2 Y samples)
    Pixel_YUV422P10, ///< planar YUV 4:2:2, 20bpp, (1 Cr & Cb sample per 2x1 Y samples)
    Pixel_YUV440P10, ///< planar YUV 4:4:0, 20bpp, (1 Cr & Cb sample per 1x2 Y samples)
    Pixel_YUV444P10, ///< planar YUV 4:4:4, 30bpp, (1 Cr & Cb sample per 1x1 Y samples)

    Pixel_YUV420P12, ///< planar YUV 4:2:0, 18bpp, (1 Cr & Cb sample per 2x2 Y samples)
    Pixel_YUV422P12, ///< planar YUV 4:2:2, 24bpp, (1 Cr & Cb sample per 2x1 Y samples)
    Pixel_YUV440P12, ///< planar YUV 4:4:0, 24bpp, (1 Cr & Cb sample per 1x2 Y samples)
    Pixel_YUV444P12, ///< planar YUV 4:4:4, 36bpp, (1 Cr & Cb sample per 1x1 Y samples)

    Pixel_YUV420P14, ///< planar YUV 4:2:0, 21bpp, (1 Cr & Cb sample per 2x2 Y samples)
    Pixel_YUV422P14, ///< planar YUV 4:2:2, 28bpp, (1 Cr & Cb sample per 2x1 Y samples)
    Pixel_YUV444P14, ///< planar YUV 4:4:4, 42bpp, (1 Cr & Cb sample per 1x1 Y samples)

    Pixel_YUV420P16, ///< planar YUV 4:2:0, 24bpp, (1 Cr & Cb sample per 2x2 Y samples)
    Pixel_YUV422P16, ///< planar YUV 4:2:2, 32bpp, (1 Cr & Cb sample per 2x1 Y samples)
    Pixel_YUV444P16, ///< planar YUV 4:4:4, 48bpp, (1 Cr & Cb sample per 1x1 Y samples)

    Pixel_YUVA420P9, ///< planar YUV 4:2:0 22.5bpp, (1 Cr & Cb sample per 2x2 Y & A samples)
    Pixel_YUVA422P9, ///< planar YUV 4:2:2 27bpp, (1 Cr & Cb sample per 2x1 Y & A samples)
    Pixel_YUVA444P9, ///< planar YUV 4:4:4 36bpp, (1 Cr & Cb sample per 1x1 Y & A samples)

    Pixel_YUVA420P10,///< planar YUV 4:2:0 25bpp, (1 Cr & Cb sample per 2x2 Y & A samples)
    Pixel_YUVA422P10,///< planar YUV 4:2:2 30bpp, (1 Cr & Cb sample per 2x1 Y & A samples)
    Pixel_YUVA444P10,///< planar YUV 4:4:4 40bpp, (1 Cr & Cb sample per 1x1 Y & A samples)

    Pixel_YUVA420P16,///< planar YUV 4:2:0 40bpp, (1 Cr & Cb sample per 2x2 Y & A samples)
    Pixel_YUVA422P16,///< planar YUV 4:2:2 48bpp, (1 Cr & Cb sample per 2x1 Y & A samples)
    Pixel_YUVA444P16,///< planar YUV 4:4:4 64bpp, (1 Cr & Cb sample per 1x1 Y & A samples)

    Pixel_NV12,      ///< planar YUV 4:2:0, 12bpp, 1 plane for Y and 1 plane for the UV components,
                     ///< which are interleaved (first byte U and the following byte V)
    Pixel_NV21,      ///< as above, but U and V bytes are swapped
    Pixel_NV16,      ///< interleaved chroma YUV 4:2:2, 16bpp, (1 Cr & Cb sample per 2x1 Y samples)
    Pixel_NV20,      ///< interleaved chroma YUV 4:2:2, 20bpp, (1 Cr & Cb sample per 2x1 Y samples)

    Pixel_Count
}eImagePixels;

typedef enum
{
    Image_Not = 0,
    Image_Copy = Image_Not,
    Image_Ref = 0x10,            ///< 图像数据为引用外部指针
    Image_RefFree = 0x11,        ///< 引用指针则需要使用free函数释放
    Image_RefDel = 0x12,         ///< 引用指针则需要使用delete函数释放
    Image_RefUser= 0x13,

}eImageFlags;

typedef enum
{
    Image_Nearest = 0,  ///< 近邻取样插值
    Image_Linear,       ///< 二次线性插值
    Image_Bilinear,     ///< 双线性插值
    Image_Box,          ///<
    Image_CatmullRom,   ///< 内插三次样条
    Image_Mitchell,     ///< Mitchell-Netrevalli [B=1/3, C=1/3]
    Image_Gaussian,     ///< Mitchell-Netrevalli 类似高斯 [B=1,C=0]

    Scaled_Count
}eImageScaleds;

namespace impl {
struct pixel_image_info{
    eImagePixels format;   ///< 像素格式
    int is_alpha;          ///< 是否支持透明通道
    int is_compress;       ///< 是否为压缩格式
    int is_planar;         ///< 是否为平面格式
    int component;         ///< 分量数
    int byte;              ///< 分量数的字节

    int width;            ///< 格式中计算分量的宽度
    int height;           ///< 格式中计算分量的高度

    union {
        struct {int yt, ut, vt, at;};
        struct {int rt, gt, bt, _t;};
        int step[4];      ///< 格式中计算下一个分量的开始
    };
    union {
        struct {int yf, uf, vf, af;};
        struct {int rf, gf, bf, _f;};
        int offset[4];    ///< 格式中计算每个分量之间的偏移
    };
    union {
        struct {int ys, us, vs, as;};
        struct {int rs, gs, bs, _s;};
        int shift[4];    ///< 格式中计算每个分量通过移位获取,偏移完后移位
    };
    union{
        struct {int red, green, blue, alpha;};
        struct {int  y,    u,     v,    a;};
        struct {int lum,  _1b,   _2b,  _3b;};
        int bits[4];      ///< 像素位数
    };
    const char*const name;///< 像素名称
};

extern pixel_image_info PixelInfo[Pixel_Count];

struct pixel_image_io;
}

/*!
 * \brief The ky_image class 像素纹理图像
 * @class ky_image
 */
class ky_image
{
public:
    ky_image();
    virtual ~ky_image();

    //!
    //! \brief ky_pximg 根据尺寸和格式构造图像
    //! \param w 图像的宽度
    //! \param h 图像的高度
    //! \param d 在3D图像中此代表图像的片数,在2D图像中永远为1.
    //! \param f 存储的图像像素格式
    //! \param md 引用或源图像的地址
    //! \param flag 可根据标志设置引用或是拷贝以及释放方法,默认为拷贝图像数据
    //!
    ky_image(int w, int h, int d, eImagePixels f, uchar *md = 0, eImageFlags flag = Image_Copy);
    ky_image(int w, int h, eImagePixels f, uchar *md = 0, eImageFlags flag = Image_Copy);
    ky_image(const isize3 &dim, eImagePixels f, uchar *md = 0, eImageFlags flag = Image_Copy);

    //!
    //! \brief ky_image 默认作为构造赋值,当指定dim和offset时作为引用源对象并需要进行裁剪
    //! \param pd
    //! \param dim
    //! \param offset
    //!
    ky_image(const ky_image &pd, const isize3 &dim = isize3(-1), const ivec3 &offset = ivec3(0));
    ky_image & operator = (const ky_image &rhs);

    ky_image(uchar *data, int64 dlen, eImageFormats rforms );

    //!
    //! \brief raw_dim 返回图像原始尺寸
    //! \return
    //! \note 引用了其他图像对象时此为引用的原始尺寸，若不是引用则为实际尺寸
    //!
    isize3 raw_dim()const;
    //!
    //! \brief dim 返回图像尺寸[3D]
    //! \return
    //!
    isize3 dim()const;
    int width()const;
    int height()const;
    int depth()const;

    //!
    //! \brief size 返回图像尺寸[2D]
    //! \return
    //!
    isize  size()const;
    //!
    //! \brief offset 返回图像在原始图像的偏移值
    //! \return
    //! \note 当引用其他图像后本图像作为截取中间部分时所使用的偏移值
    //!
    ivec3 offset()const;

    //!
    //! \brief slice 源图像的片尺寸
    //! \return
    //! \note 在3D图像中返回一层图像的总像素数，称之为片
    //!
    int slice()const{return raw_dim ().width*raw_dim ().height;}
    //!
    //! \brief format 返回图像像素格式
    //! \return
    //!
    eImagePixels format() const;

    //!
    //! \brief is_empty 是否为空图像
    //! \return
    //!
    bool is_empty() const;
    //!
    //! \brief is_compress 是否为压缩格式
    //! \return
    //!
    bool is_compress() const;
    //!
    //! \brief is_alpha 是否有透明通道
    //! \return
    //!
    bool is_alpha()const;
    //!
    //! \brief is_null 当图像为无句柄时此返回true
    //! \return
    //!
    bool is_null() const ;
    //!
    //! \brief bytevec 返回图像像素的字节数
    //! \return
    //!
    int bytevec() const;

    //!
    //! \brief bits 返回图像指针
    //! \return
    //!
    uchar* bits();
    const uchar* bits() const;

    //!
    //! \brief pixel 返回图像指定像素点颜色
    //! \param pos
    //! \return
    //!
    ky_color pixel(const ipoint3 &pos) const ;
    ky_color color(const ipoint3 &pos) const ;
    //!
    //! \brief pixel 修改图像指定像素点值
    //! \param pos 像素位置
    //! \param col 颜色值
    //!
    void pixel(const ipoint3 &pos, const ky_color &col) ;
    void color(const ipoint3 &pos, const ky_color &col) ;

    //!
    //! \brief resize 重新申请内部数据的空间
    //! \param width
    //! \param height
    //! \param depth
    //! \param f 像素格式
    //!
    void resize(int width, int height, int depth, eImagePixels f = Pixel_RGBA);
    void resize(const isize3 &s, eImagePixels f = Pixel_RGBA);

    //!
    //! \brief fill 将数据按照pixel指定像素进行填充
    //! \param pixel
    //!
    void fill(const ky_color &pixel) ;
    //!
    //! \brief fill 将数据按照val指定值进行填充
    //! \param val
    //!
    void fill(const char val) ;

    bool operator ==(const ky_image &rhs);
    bool operator !=(const ky_image &rhs);

    //!
    //! \brief bytecount 返回图像所占内存字节数
    //! \return
    //!
    int64 byte()const{return bytecount ();}
    int64 bytecount()const ;

public:
    //!
    //! \brief load 加载图像文件
    //! \param name
    //! \param form 指定图像的格式
    //! \return true 加载成功，否则失败
    //!
    bool load(const ky_string &name, eImageFormats form = Image_NOT) ;
    bool load(uchar *data, int64 dlen, eImageFormats form  = Image_PNG) ;
    //!
    //! \brief save 将当前数据按照form格式保存到name文件中，
    //! \param name
    //! \param form
    //!
    bool save(const ky_string &name, eImageFormats form = Image_NOT) ;
    bool save(uchar **data, int64 *dlen, eImageFormats form = Image_PNG) ;

public:

    //!
    //! \brief convert 转换对象到form指定的格式并返回
    //! \param form
    //! \return
    //!
    ky_image convert(eImagePixels form) const ;
    //!
    //! \brief convert 将对象的图像转换到form指定的格式
    //! \param form
    //!
    void convert(eImagePixels form) ;

    //!
    //! \brief scaled 将对象图像按照宽高进行缩放
    //! \param width
    //! \param height
    //! \param depth
    //!
    void scaled(int width, int height, int depth, eImageScaleds is = Image_Nearest) ;
    void scaled(const isize3 &s, eImageScaleds is = Image_Nearest) ;
    ky_image scaled(const isize3 &s, eImageScaleds is = Image_Nearest)const ;

    //!
    //! \brief rotate 将对象图像根据angle角度进行旋转
    //! \param angle
    //! \param dev 转换时使用的设备
    //!
    void rotate(int angle);
    ky_image rotate(int angle)const;

    //!
    //! \brief cutting  将对象图像按照指定范围进行裁剪
    //! \param x
    //! \param y
    //! \param z
    //! \param width
    //! \param height
    //! \param depth
    //!
    void cutting(int x, int y, int z, int width, int height, int depth) ;
    void cutting(const ivec3 &offset, const isize3 &dim) ;
    ky_image cutting(const ivec3 &offset, const isize3 &dim) const;

    /*
    //!
    //! \brief blend  将file指定的文件和对象图像进行混合
    //! \param file
    //! \param dev 转换时使用的设备
    //!
     void blend(const ky_string &file,
                       ImageProcessorDevice dev= ImageProcessor_CPU) ;
    //!
    //! \brief blend 将image图像和对象图像进行混合
    //! \param image
    //! \param dev 转换时使用的设备
    //!
     void blend(const iimage &image,
                       ImageProcessorDevice dev= ImageProcessor_CPU) ;
    //!
    //! \brief blend 将src_file1文件src_file2图像进行混合后保存到dst_file中
    //! \param src_file1
    //! \param src_file2
    //! \param dst_file
    //! \param dev 转换时使用的设备
    //! \return true 成功，否则失败
    //!
     bool blend(const ky_string &src_file1, const ky_string &src_file2,
                       const ky_string &dst_file,
                       ImageProcessorDevice dev= ImageProcessor_CPU) ;
    //!
    //! \brief blend 将image_1图像和image_2进行混合并返回
    //! \param image_1
    //! \param image_2
    //! \param dev 转换时使用的设备
    //! \return
    //!
     iimage blend(const iimage &image_1, const iimage &image_2,
                         ImageProcessorDevice dev= ImageProcessor_CPU) const;

    //! 将图像进行调色
*/
public:
    //!
    //! \brief form 将指定的尺寸图像生成image格式并返回
    //! \param width
    //! \param height
    //! \param depth
    //! \param data
    //! \param sforms 指定data的图像格式
    //! \param flag
    //! \return
    //!
    static ky_image form(int width, int height,int depth, uchar *data,
                         eImagePixels sforms, eImageFlags flag = Image_Copy)
    {
        return ky_image(width, height, depth, sforms, data, flag);
    }
    //!
    //! \brief form 将指定的尺寸图像生成iimage格式并返回
    //! \param s
    //! \param data
    //! \param sforms 指定data的图像格式
    //! \param flag
    //! \return
    //!
    static ky_image form(const isize3 &s, uchar *data,
                         eImagePixels sforms, eImageFlags flag = Image_Copy)
    {
        return ky_image(s, sforms, data, flag);
    }

    //!
    //! \brief cutting 将image图像按照ret指定范围进行裁剪并返回
    //! \param image
    //! \param offset
    //! \param dim
    //! \return
    //!
    static ky_image cutting(const ky_image &image, const ivec3 &offset, const isize3 &dim)
    {
        return image.cutting (offset, dim);
    }
    //!
    //! \brief convert 转换image到form指定的格式并返回
    //! \param image
    //! \param form 转换后的格式
    //! \return
    //!
    static ky_image convert(const ky_image &image, eImagePixels form)
    {
        return image.convert (form);
    }
    //!
    //! \brief rotate  将image图像根据angle角度进行旋转并返回
    //! \param image
    //! \param angle
    //! \return
    //!
    static ky_image rotate(const ky_image &image, int angle)
    {
        return image.rotate (angle);
    }
    //!
    //! \brief scaled 将image图像按照宽高进行缩放并返回
    //! \param image
    //! \param s
    //! \return
    //!
    static ky_image scaled(const ky_image &image, const isize3 &s, eImageScaleds is = Image_Nearest)
    {
        return image.scaled (s, is);
    }
protected:
    uchar *create(int w, int h, int d, eImagePixels f, eImageFlags flag = Image_Not);
    void destroy();

    impl::pixel_image_io *header();
    const impl::pixel_image_io *header()const;

private:
    uchar *data;
};


#endif // ky_PIXELIMG_H
