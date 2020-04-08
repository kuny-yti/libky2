#include "impl_image_scaled.h"

static bool check_format_valid(eImagePixels pix)
{
    switch ((int)pix)
    {
    case Pixel_ABGR:
    case Pixel_ARGB:
    case Pixel_BGRA:
    case Pixel_RGBA:
        return true;
    default:
        break;
    }
    return false;
}

static void *image_nearest(void *src, int w, int h, int d, eImagePixels pix,
                           void *dst, int dw, int dh, int dd, eImagePixels dpix)
{
    if (!src || w < 1 || h < 1 || d < 1 || !dst || dw < 1 || dh < 1 || dd < 1 ||
            !check_format_valid (pix) || !check_format_valid (dpix))
        return (void*)0;

    uint32 xf16 = (w << 16) / dw +1;
    uint32 yf16 = (h << 16) / dh +1;

    uint8 *srcp = (uint8*)src;
    uint32 *dstline = (uint32*)dst;
    uint32 srcy16 = 0;
    for (int32 y = 0; y < dh; ++y)
    {
        uint32 *srcline = (uint32*)(srcp + (w * impl::PixelInfo[pix].byte * (srcy16 >> 16)));
        uint32 srcx16 = 0;
        for (int32 x = 0; x < dw; ++x)
        {
            dstline[x] = srcline[srcx16 >> 16];
            srcx16 += xf16;
        }
        srcy16 += yf16;
        ((uint8*&)dstline) += dw * impl::PixelInfo[dpix].byte ;
    }
    return dst;
}

// ABGR
static uint32 bilinear_fast_common(uint32* c0, uint32* c1, uint32 u8, uint32 v8)
{
    const uint32 pm3_8 = (u8 * v8) >> 8;
    const uint32 pm2_8 = u8 - pm3_8;
    const uint32 pm1_8 = v8 - pm3_8;
    const uint32 pm0_8 = 256 - pm1_8 - pm2_8 - pm3_8;

    uint32 color = *c0;
    uint32 BR = (color & 0x00FF00FF) * pm0_8;
    uint32 GA = ((color & 0xFF00FF00) >> 8) * pm0_8;
    color = c0[1];
    GA += ((color & 0xFF00FF00) >> 8) * pm2_8;
    BR += (color & 0x00FF00FF) * pm2_8;
    color = *c1;
    GA += ((color & 0xFF00FF00) >> 8) * pm1_8;
    BR += (color & 0x00FF00FF) * pm1_8;
    color = c1[1];
    GA += ((color & 0xFF00FF00) >> 8) * pm3_8;
    BR += (color & 0x00FF00FF) * pm3_8;

    return (GA & 0xFF00FF00) | ((BR & 0xFF00FF00) >> 8);
}

static void* image_bilinear(void *src, int w, int h, int d, eImagePixels pix,
                            void *dst, int dw, int dh, int dd, eImagePixels dpix)
{
    if (!src || w < 1 || h < 1 || d < 1 || !dst || dw < 1 || dh < 1 || dd < 1 ||
            !check_format_valid (pix) || !check_format_valid (dpix))
        return (void*)0;

    int32 xf16 =((w -1) << 16) / dw;
    int32 yf16 =((h -1) << 16) / dh;

    uint32* dstline = (uint32*)dst;
    int32 src_bytew = w * impl::PixelInfo[pix].byte;
    int32 srcy_16 = 0;
    for (int32 y = 0; y < dh; ++y)
    {
        uint32 v_8 = (srcy_16 & 0xFFFF) >> 8;
        uint32* srclinecol = (uint32*)(((uint8*)src) + src_bytew * (srcy_16 >> 16)) ;
        int32 srcx_16 = 0;
        for (int32 x=0; x < dw; ++x)
        {
            uint32* c0 = &srclinecol[srcx_16 >> 16];
            uint32* c1 = (uint32*)(((uint8*)c0) + src_bytew);
            dstline[x] = bilinear_fast_common(c0, c1, (srcx_16 & 0xFFFF) >> 8, v_8);
            srcx_16 += xf16;
        }
        srcy_16 += yf16;
        ((uint8*&)dstline) += dw * impl::PixelInfo[dpix].byte;
    }
    return dst;
}
/*
static real SinXDivX(real x)
{
    //该函数计算插值曲线sin(x*PI)/(x*PI)的值 //PI=3.1415926535897932385;
    //下面是它的近似拟合表达式
    const float a = -1; //a还可以取 a=-2,-1,-0.75,-0.5等等，起到调节锐化或模糊程度的作用

    if (x<0) x = -x;
    real x2 = x * x;
    real x3 = x2 * x;
    if (x <= 1)
        return (a + 2) * x3 - (a + 3) * x2 + 1;
    else if (x <= 2)
        return a * x3 - (5 * a) * x2 + (8 * a) *x - (4 * a);

    return 0;
}

static long SinXDivX_Table8[(2<<8)+1];
class _init_table_sinxdivx
{
public:
    _init_table_sinxdivx() {
        for (long i = 0; i <= (2 << 8); ++i)
            SinXDivX_Table8[i] = long(0.5 + 256 * SinXDivX(i * (1.0 / (256)))) * 1;
    }
};
static _init_table_sinxdivx __tmp_init_table_sinxdivx;

static uint32 Pixels_Bound(void *src, int w, int h, int byte, int32 x, int32 y)
{
    bool IsInImg = true;
    if (x < 0) {x = 0; IsInImg = false;}
    else if (x >= w) {x = w-1; IsInImg = false;}
    if (y < 0) {y = 0; IsInImg=false; }
    else if (y >= h) {y = h-1; IsInImg = false;}
    uint32 result = (uint32)((uint8*)src + w * byte * y)[x];
    if (!IsInImg)
        result &= 0xffffff00;
    return result;
}
//颜色查表
static uint8 ColorTable [256 * 3];
static const uint8* color_table = &ColorTable[256];
class _init_color_table
{
public:
    _init_color_table() {
        for (int i = 0; i < 256*3; ++i)
            ColorTable[i] = ky_bound(0, i-256, 255);
    }
};
static _init_color_table __tmp_init_color_table;

uint32 ThreeConvolution_Fast_Common(void *src, int w, int h, int d, int32 x_16, int32 y_16)
{
    const int32 srcbytew = w * impl::PixelInfo[pix].byte;
    const uint32 u_8 = (uint8)((x_16) >> 8);
    const uint32 v_8 = (uint8)((y_16) >> 8);
    const uint8* pixel = (uint8*)&(uint32)((uint8)src + srcbytew * ((y_16 >> 16)-1))[(x_16 >> 16)-1];

    int32 au_8[4], av_8[4];

    au_8[0] = SinXDivX_Table8[(1 << 8) + u_8];
    au_8[1] = SinXDivX_Table8[u_8];
    au_8[2] = SinXDivX_Table8[(1 << 8) - u_8];
    au_8[3] = SinXDivX_Table8[(2 << 8) - u_8];
    av_8[0] = SinXDivX_Table8[(1 << 8) + v_8];
    av_8[1] = SinXDivX_Table8[v_8];
    av_8[2] = SinXDivX_Table8[(1 << 8) - v_8];
    av_8[3] = SinXDivX_Table8[(2 << 8) - v_8];

    int32 sR = 0, sG = 0, sB = 0, sA = 0;
    for (int32 i = 0; i < 4; ++i)
    {
        int32 aA = au_8[0] *pixel[0] + au_8[1] *pixel[4] + au_8[2]*pixel[8] + au_8[3] *pixel[12];
        int32 aR = au_8[0] *pixel[1] + au_8[1] *pixel[5] + au_8[2]*pixel[9] + au_8[3] *pixel[13];
        int32 aG = au_8[0] *pixel[2] + au_8[1] *pixel[6] + au_8[2]*pixel[10] + au_8[3] *pixel[14];
        int32 aB = au_8[0] *pixel[3] + au_8[1] *pixel[7] + au_8[2]*pixel[11] + au_8[3] *pixel[15];
        sA += aA *av_8[i];
        sR += aR *av_8[i];
        sG += aG *av_8[i];
        sB += aB *av_8[i];
        pixel += srcbytew;
    }

    return (color_table[sA >> 16] & 0xff) |
           ((color_table[sR >> 16] & 0xff) << 24)|
           ((color_table[sG >> 16] & 0xff) << 16) |
           ((color_table[sB >> 16] & 0xff) << 8) ;
}

uint32 ThreeConvolution_Border_Common(void *src, int w, int h, int d, int byte, int32 x_16, int32 y_16)
{
    int32 x0_sub1 = (x_16 >> 16)-1;
    int32 y0_sub1 = (y_16 >> 16)-1;
    uint32 u_16_add1 = ((uint16)(x_16)) + (1<<16);
    uint32 v_16_add1 = ((uint16)(y_16)) + (1<<16);

    uint32 pixel[16];
    for (int32 i = 0; i < 4; ++i)
    {
        int32 y = y0_sub1+i;
        pixel[i*4+0] = Pixels_Bound(src, w, h, byte, x0_sub1+0, y);
        pixel[i*4+1] = Pixels_Bound(src, w, h, byte, x0_sub1+1, y);
        pixel[i*4+2] = Pixels_Bound(src, w, h, byte, x0_sub1+2, y);
        pixel[i*4+3] = Pixels_Bound(src, w, h, byte, x0_sub1+3, y);
    }

    return ThreeConvolution_Fast_Common(&pixel[0], 4, 4, byte, u_16_add1, v_16_add1);
}

void image_three_convolution(const TPicRegion& Dst,const TPicRegion& Src)
{
    if (  (0==Dst.width)||(0==Dst.height)
          ||(0==Src.width)||(0==Src.height)) return;

    long xrIntFloat_16=((Src.width)<<16)/Dst.width+1;
    long yrIntFloat_16=((Src.height)<<16)/Dst.height+1;
    const long csDErrorX=-(1<<15)+(xrIntFloat_16>>1);
    const long csDErrorY=-(1<<15)+(yrIntFloat_16>>1);

    unsigned long dst_width=Dst.width;

    //计算出需要特殊处理的边界
    long border_y0=((1<<16)-csDErrorY)/yrIntFloat_16+1;//y0+y*yr>=1; y0=csDErrorY => y>=(1-csDErrorY)/yr
    if (border_y0>=Dst.height) border_y0=Dst.height;
    long border_x0=((1<<16)-csDErrorX)/xrIntFloat_16+1;
    if (border_x0>=Dst.width ) border_x0=Dst.width;
    long border_y1=(((Src.height-3)<<16)-csDErrorY)/yrIntFloat_16+1; //y0+y*yr<=(height-3) => y<=(height-3-csDErrorY)/yr
    if (border_y1<border_y0) border_y1=border_y0;
    long border_x1=(((Src.width-3)<<16)-csDErrorX)/xrIntFloat_16+1;;
    if (border_x1<border_x0) border_x1=border_x0;

    TARGB32* pDstLine=Dst.pdata;
    long srcy_16=csDErrorY;
    long y;
    for (y=0;y<border_y0;++y)
    {
        long srcx_16=csDErrorX;
        for (unsigned long x=0;x<dst_width;++x)
        {
            ThreeOrder_Border_Common(Src,srcx_16,srcy_16,&pDstLine[x]); //border
            srcx_16+=xrIntFloat_16;
        }
        srcy_16+=yrIntFloat_16;
        ((TUInt8*&)pDstLine)+=Dst.byte_width;
    }
    for (y=border_y0;y<border_y1;++y)
    {
        long srcx_16=csDErrorX;
        long x;
        for (x=0;x<border_x0;++x)
        {
            ThreeOrder_Border_Common(Src,srcx_16,srcy_16,&pDstLine[x]);//border
            srcx_16+=xrIntFloat_16;
        }
        for (x=border_x0;x<border_x1;++x)
        {
            ThreeOrder_Fast_Common(Src,srcx_16,srcy_16,&pDstLine[x]);//fast  !
            srcx_16+=xrIntFloat_16;
        }
        for (x=border_x1;x<dst_width;++x)
        {
            ThreeOrder_Border_Common(Src,srcx_16,srcy_16,&pDstLine[x]);//border
            srcx_16+=xrIntFloat_16;
        }
        srcy_16+=yrIntFloat_16;
        ((TUInt8*&)pDstLine)+=Dst.byte_width;
    }
    for (y=border_y1;y<Dst.height;++y)
    {
        long srcx_16=csDErrorX;
        for (unsigned long x=0;x<dst_width;++x)
        {
            ThreeOrder_Border_Common(Src,srcx_16,srcy_16,&pDstLine[x]); //border
            srcx_16+=xrIntFloat_16;
        }
        srcy_16+=yrIntFloat_16;
        ((TUInt8*&)pDstLine)+=Dst.byte_width;
    }
}
*/
implImageScaled ImageScaled[] =
{
    {Image_Nearest, check_format_valid, image_nearest},
    {Image_Linear, check_format_valid, image_bilinear},
    {Image_Bilinear, check_format_valid, 0},
    {Image_Box, check_format_valid, 0},
    {Image_CatmullRom, check_format_valid, 0},
    {Image_Mitchell, check_format_valid, 0},
    {Image_Gaussian, check_format_valid, 0},
};
