#include "ky_image.h"

#include "image/stb/stb_image.h"
#include "image/stb/stb_image_write.h"
#include "image/stb/stb_image_resize.h"

#include "image/impl_image_scaled.h"

namespace impl  {

struct pixel_image_io : ky_ref
{
    int width;
    int height;
    int depth;
    int raww;
    int rawh;
    int rawd;
    int osx;
    int osy;
    int osz;
    bool empty;
    bool compress;
    bool alpha;
    eImageFlags    flag;
    eImagePixels  format;

    static const pixel_image_io shared_null;
    static intptr null()
    {
        return (intptr)&shared_null;
    }
    static bool is_null(intptr p)
    {
        return p == (intptr)&shared_null;
    }
};
const pixel_image_io pixel_image_io::shared_null = {};

static int pixelbyte(int w, int h, int d, eImagePixels f)
{
    switch ((int)f)
    {
    case Pixel_S3TC1:
        return ((w + 3) / 4) * ((h + 3) / 4) * 8 * d;
    case Pixel_S3TC2:
    case Pixel_S3TC3:
    case Pixel_S3TC4:
    case Pixel_S3TC5:
        return ((w + 3) / 4) * ((h + 3) / 4) * 16 * d;

    case Pixel_PVRTCRGB2:
    case Pixel_PVRTCRGBA2:
        return (ky_max(w, 16) * ky_max(h, 8) * 2 + 7) / 8;
    case Pixel_PVRTCRGB4:
    case Pixel_PVRTCRGBA4:
        return (ky_max(w, 8) * ky_max(h, 8) * 2 + 7) / 8;
    case Pixel_ETC1RGB8:
    case Pixel_ETC2RGB8:
    case Pixel_ETC2RGBA8:
    case Pixel_ETC2RGB8A1:
        return ((w * h) >> 1);

    case Pixel_ATITCRGB:
        return ((w + 3) / 4) * ((h + 3) / 4) * 8;
    case Pixel_ATITCRGBAe:
    case Pixel_ATITCRGBAi:
        return ((w + 3) / 4) * ((h + 3) / 4) * 16;
    default:
        break;
    }

    return w * h * d * PixelInfo[f].byte;
}

}

impl::pixel_image_io *ky_image::header()
{
    impl::pixel_image_io * h = (impl::pixel_image_io*)data;
    if (h && !impl::pixel_image_io::is_null((intptr)h)) --h;
    return h;
}
const impl::pixel_image_io *ky_image::header()const
{
    impl::pixel_image_io * h = (impl::pixel_image_io*)data;
    if (h && !impl::pixel_image_io::is_null((intptr)h)) --h;
    return h;
}

ky_image::ky_image():
    data((uchar*)impl::pixel_image_io::null ())
{

}

ky_image::~ky_image()
{
    destroy();
}

ky_image::ky_image(int w, int h, int d, eImagePixels f, uchar *md, eImageFlags flag):
    data(create(w, h, d, f, flag))
{
    if (md && (flag == Image_Copy))
    {
        ky_memory::copy (data, md, bytecount());
        header ()->empty = false;
    }
    else if (md && (flag & Image_Ref))
    {
        uchar **dptr = (uchar **)data;
        *dptr = md;
        header ()->empty = false;
    }

}

ky_image::ky_image(int w, int h, eImagePixels f, uchar *md, eImageFlags flag):
    data(create(w, h, 1, f, flag))
{
    if (md && (flag == Image_Copy))
    {
        ky_memory::copy (data, md, bytecount());
        header ()->empty = false;
    }
    else if (md && (flag & Image_Ref))
    {
        uchar **dptr = (uchar **)data;
        *dptr = md;
        header ()->empty = false;
    }

}

ky_image::ky_image(const isize3 &dim, eImagePixels f, uchar *md, eImageFlags flag):
    data(create(dim.width, dim.height, dim.depth, f, flag))
{
    if (md && (flag == Image_Copy))
    {
        ky_memory::copy (data, md, bytecount());
        header ()->empty = false;
    }
    else if (md && (flag & Image_Ref))
    {
        uchar **dptr = (uchar **)data;
        *dptr = md;
        header ()->empty = false;
    }
}

ky_image::ky_image(const ky_image &pd, const isize3 &dim, const ivec3 &offset):
    data((uchar*)impl::pixel_image_io::null ())
{
    if (!pd.is_null ())
    {
        data = pd.data;
        header ()->addref ();
        if (dim != isize3(-1))
        {
            header ()->width = dim.width;
            header ()->height = dim.height;
            header ()->depth = dim.depth;
            header ()->osx = offset.x;
            header ()->osy = offset.y;
            header ()->osz = offset.z;
        }
    }
}

ky_image & ky_image::operator = (const ky_image &rhs)
{
    destroy();
    if (!rhs.is_null ())
    {
        data = rhs.data;
        header ()->addref ();
    }
    return *this;
}

ky_image::ky_image(uchar *data, int64 dlen, eImageFormats rforms ):
    data((uchar*)impl::pixel_image_io::null ())
{
    if (data && dlen)
        load(data, dlen, rforms);
}

isize  ky_image::size()const
{
    return isize(width(), height ());
}

isize3 ky_image::dim()const
{
    return isize3(width(), height(), depth ());
}

int ky_image::width()const
{
    return header()->width;
}
int ky_image::height()const
{
    return header()->height;
}
int ky_image::depth()const
{
    return header()->depth;
}

isize3 ky_image::raw_dim()const
{
    return isize3(header ()->raww, header ()->rawh, header ()->rawd);
}

ivec3 ky_image::offset()const
{
    return ivec3(header ()->osx, header ()->osy, header ()->osz);
}

eImagePixels ky_image::format() const
{
    return header ()->format;
}

bool ky_image::is_empty() const
{
    return header ()->empty;
}

bool ky_image::is_compress() const
{
    return header ()->compress;
}

bool ky_image::is_alpha()const
{
    return header ()->alpha;
}

bool ky_image::is_null() const
{
    return impl::pixel_image_io::is_null ((intptr)data);
}

int ky_image::bytevec() const
{
    return impl::PixelInfo[format ()].byte;
}

int64 ky_image::bytecount()const
{
    return impl::pixelbyte(width(), height(), depth(), format());
}

uchar* ky_image::bits()
{
    return header()->flag & Image_Ref ? *(uchar **)data : data;
}
const uchar* ky_image::bits() const
{
    return header()->flag & Image_Ref ? *(uchar **)data : data;
}

ky_color ky_image::pixel(const ipoint3 &pos) const
{
    if (!is_null ())
        return ky_color();
    const uchar *temp = bits () +((pos.y * size().width * bytevec()) +
                                  (pos.x * bytevec()) +
                                  (pos.z * slice () * bytevec ()));

    switch ((int)format())
    {
    case Pixel_RGBA:
        return ky_color(temp[0],
                        bytevec() >= 2?temp[1]:uchar(0),
                        bytevec() >= 3?temp[2]:uchar(0),
                        bytevec() >= 4?temp[3]:uchar(0));
    case Pixel_RGB:
        return ky_color(temp[0],
                        bytevec() >= 2?temp[1]:uchar(0),
                        bytevec() >= 3?temp[2]:uchar(0),
                        uchar(0));
    default:
        break;
    }
    return ky_color();
}
ky_color ky_image::color(const ipoint3 &pos) const
{
    return pixel(pos);
}

void ky_image::pixel(const ipoint3 &pos, const ky_color &col)
{
    if (!is_null())
        return ;
    uchar *temp = bits () +((pos.y * size().width * bytevec()) +
                            (pos.x * bytevec()) +
                            (pos.z * slice () * bytevec ()));
    temp[0] = col.r;
    if (bytevec() >= 2)
        temp[1] = col.g;
    if (bytevec() >= 3)
        temp[2] = col.b;
    if (bytevec() >= 4)
        temp[3] = col.a;
}
void ky_image::color(const ipoint3 &pos, const ky_color &col)
{
    pixel(pos, col);
}

void ky_image::resize(int width, int height, int depth, eImagePixels f)
{
    destroy ();
    data = create (width, height, depth, f);
}
void ky_image::resize(const isize3 &s, eImagePixels f)
{
    resize(s.width, s.height, s.depth, f);
}

bool ky_image::load(uchar *ld, int64 dlen, eImageFormats )
{
    int width = 0, height = 0, dbyte = 0;
    destroy ();

    stbi_uc *lmd = stbi_load_from_memory (ld, dlen, &width, &height, &dbyte, 4);
    if (!lmd) return false;

    data = create (width, height, 1, Pixel_RGBA, Image_RefFree);
    uchar **rptr = (uchar **)data;
    *rptr = lmd;
    header ()->empty = false;

    return true;
}
bool ky_image::load(const ky_string &name, eImageFormats)
{
    int width = 0, height = 0, dbyte = 0;
    destroy ();

    stbi_set_unpremultiply_on_load(1);
    stbi_convert_iphone_png_to_rgb(1);

    stbi_uc *lmd = stbi_load((char*)name.to_utf8().data(), &width, &height, &dbyte, 4);
    if (!lmd) return false;

    data = create (width, height, 1, Pixel_RGBA, Image_RefFree);
    uchar **rptr = (uchar **)data;
    *rptr = lmd;
    header ()->empty = false;

    return true;
}

bool ky_image::save(uchar **sd, int64 *dlen, eImageFormats form )
{
    if (is_null() && !sd && !dlen)
        return false;
    switch ((int)form)
    {
    case Image_TGA :
        break;
    case Image_PNG :
        *sd = stbi_write_png_to_mem(bits (), width() * bytevec(), width(),
                                    height(), bytevec(), (int*)dlen);
        return true;
    case Image_BMP :
        break;
    case Image_HDR :
        break;
    default:
        break;
    }
    return false;
}
bool ky_image::save(const ky_string &name, eImageFormats form )
{
    int ret = 0;
    if (is_null())
        return false;
    switch ((int)form)
    {
    case Image_TGA :
        ret = stbi_write_tga((char*)name.to_utf8().data(), width(), height(), bytevec(), bits ());
        break;
    case Image_PNG :
         ret = stbi_write_png((char*)name.to_utf8().data(), width(), height(),
                              bytevec(), bits(), width() * bytevec());
        break;
    case Image_BMP :
         ret = stbi_write_bmp((char*)name.to_utf8().data(), width(), height(),
                              bytevec(), bits());
        break;
    case Image_HDR :
    {
        float * dat = stbi__ldr_to_hdr(bits(), width(), height(), bytevec());
        ret = stbi_write_hdr((char*)name.to_utf8().data(), width(), height(),
                             bytevec(), dat);
        if (dat)
            stbi_image_free(dat);
    }
    break;
    default:
        break;
    }
    return ret;
}


void ky_image::fill(const ky_color &pixel)
{
    if (is_null())
        return ;
    uchar *_data = bits ();
    for (int s = 0; s < depth (); ++s)
    {
        for (int i = 0; i < height(); i++)
        {
            for (int j = 0; j < width() * bytevec(); j += bytevec())
            {
                int64 inx = (i * width() * bytevec()) + j + (s * slice () * bytevec ());
                _data[inx] = pixel.r;
                if (bytevec() >= 2)
                    _data[inx +1] = pixel.g;
                if (bytevec() >= 3)
                    _data[inx +2] = pixel.b;
                if (bytevec() >= 4)
                    _data[inx +3] = pixel.a;
            }
        }
    }
}
void ky_image::fill(const char val)
{
    if (is_null())
        return ;
    ky_memory::zero (bits (), bytecount (), val);
}
bool ky_image::operator ==(const ky_image &rhs)
{
    return rhs.data == data;
}
bool ky_image::operator !=(const ky_image &rhs)
{
    return !((*this) == rhs);
}

uchar *ky_image::create(int w, int h, int d, eImagePixels f, eImageFlags flag)
{
    d = d<1 ? 1 : d;
    const int64 bs = !(flag & Image_Ref) ? impl::pixelbyte(w, h, d, f) : sizeof(uchar **);
    impl::pixel_image_io *phd = (impl::pixel_image_io *)kyMalloc (bs + sizeof(impl::pixel_image_io));
    phd->set (ky_ref::refShareable);
    phd->depth = phd->rawd = d;
    phd->empty = true;
    phd->width = phd->raww = w;
    phd->height = phd->rawh = h;
    phd->format = f;
    phd->compress = impl::PixelInfo[f].is_compress;
    phd->alpha = impl::PixelInfo[f].is_alpha;
    phd->flag = flag;
    phd->osx = phd->osy = phd->osz = 0;
    return (uchar*)(++phd);
}
void ky_image::destroy()
{
    if (!is_null () && header ()->lessref ())
    {
        impl::pixel_image_io *h = header();
        if (h->flag & Image_Ref)
        {
            if ((h->flag & Image_RefFree) == Image_RefFree)
                kyFree (bits ());
            else if ((h->flag & Image_RefDel) == Image_RefDel)
                kyDelete(bits ());
        }
        kyFree (h);
    }
    data = (uchar*)impl::pixel_image_io::null ();
}

void ky_image::cutting(const ivec3 &offset, const isize3 &dim)
{
    this->cutting (offset.x, offset.y, offset.z, dim.width, dim.height, dim.depth);
}

ky_image ky_image::convert(eImagePixels form) const
{
    this->convert (form);
    return *this;
}

void ky_image::scaled(const isize3 &s, eImageScaleds is)
{
    scaled (s.width, s.height, s.depth, is);
}

ky_image ky_image::scaled(const isize3 &s, eImageScaleds is)const
{
    ky_image *take = (ky_image*)this;
    take->scaled (s.width, s.height, s.depth, is);
    return *this;
}

ky_image ky_image::rotate(int angle)const
{
    this->rotate (angle);
    return *this;
}


void ky_image::convert(eImagePixels form)
{

}


void ky_image::scaled(int w, int h, int d, eImageScaleds is)
{
    if (w < 1 || h < 1)
        return ;

    if (this->depth () == 1)
    {
        ky_image simg(w, h, 1, format());
        stbir_edge edge = STBIR_EDGE_CLAMP;
        stbir_filter filter = STBIR_FILTER_DEFAULT;
        d = d < 1 ? 1 : d;
        switch ((int)is)
        {
        case Image_Bilinear:
            filter = STBIR_FILTER_TRIANGLE;
            break;
        case Image_Box:
            filter = STBIR_FILTER_BOX;
        case Image_CatmullRom:
            filter = STBIR_FILTER_CATMULLROM;
        case Image_Mitchell:
            filter = STBIR_FILTER_MITCHELL;
        case Image_Gaussian:
            filter = STBIR_FILTER_CUBICBSPLINE;
            break;
        default:
        {
            if (ImageScaled[is].is_valid && ImageScaled[is].scaled &&
                    ImageScaled[is].is_valid(format()))
            {
                ImageScaled[is].scaled(this->bits (), width(), height(), depth(), format (),
                                       simg.bits (), w, h, d, format());
            }
            *this = simg;
            return ;
        }
        }
        stbir_resize ((const void*)bits(), width(), height(), 0, simg.bits (), w, h, 0,
                      STBIR_TYPE_UINT8, bytevec(), STBIR_ALPHA_CHANNEL_NONE, 0,
                      edge, edge, filter, filter, STBIR_COLORSPACE_LINEAR, 0);
        *this = simg;
    }
}

void ky_image::rotate(int angle)
{

}
/*
void PicRotary1(const TPicRegion& Dst,const TPicRegion& Src,double RotaryAngle,double ZoomX,double ZoomY,double move_x,double move_y)
{
   if ( (fabs(ZoomX*Src.width)<1.0e-4) || (fabs(ZoomY*Src.height)<1.0e-4) ) return; //太小的缩放比例认为已经不可见
   double rZoomX=1.0/ZoomX;
   double rZoomY=1.0/ZoomY;
   double sinA=sin(RotaryAngle);
   double cosA=cos(RotaryAngle);
   double Ax=(rZoomX*cosA);
   double Ay=(rZoomX*sinA);
   double Bx=(-rZoomY*sinA);
   double By=(rZoomY*cosA);
   double rx0=Src.width*0.5;  //(rx0,ry0)为旋转中心
   double ry0=Src.height*0.5;
   double Cx=(-(rx0+move_x)*rZoomX*cosA+(ry0+move_y)*rZoomY*sinA+rx0);
   double Cy=(-(rx0+move_x)*rZoomX*sinA-(ry0+move_y)*rZoomY*cosA+ry0);

   TARGB32* pDstLine=Dst.pdata;
   double srcx0_f=(Cx);
   double srcy0_f=(Cy);
   for (long y=0;y<Dst.height;++y)
   {
       double srcx_f=srcx0_f;
       double srcy_f=srcy0_f;
       for (long x=0;x<Dst.width;++x)
       {
           long srcx=(long)(srcx_f);
           long srcy=(long)(srcy_f);
           if (PixelsIsInPic(Src,srcx,srcy))
               pDstLine[x]=Pixels(Src,srcx,srcy);
           srcx_f+=Ax;
           srcy_f+=Ay;
       }
       srcx0_f+=Bx;
       srcy0_f+=By;
       ((TUInt8*&)pDstLine)+=Dst.byte_width;
   }
}*/

void ky_image::cutting(int x, int y, int z, int w, int h, int d)
{
    if(x < 0 || y < 0 || w < 1 || h < 1 || (w+x) > width () || (h+y) > height ())
        return ;

    const int64 dstrow = w * bytevec();
    const int64 srcx = x * bytevec ();
    const int64 srcrow = (width () - x) * bytevec ();

    ky_image simg(w, h, d, format());
    uint8 *dst = simg.bits ();
    uint8 *src = bits();
    src += y * (srcrow + srcx);
    for (int line = 0; line < h; ++line)
    {
        src += srcx;
        ky_memory::copy (dst, src, dstrow);

        src += srcrow;
        dst += dstrow;
    }
    *this = simg;
}



class yuv2rgb
{
    //I420是yuv420格式，是3个plane，排列方式为(Y)(U)(V)
    static int* I420ToRGB(uchar* src, int width, int height)
    {
        int numOfPixel = width * height;
        int positionOfV = numOfPixel;
        int positionOfU = numOfPixel/4 + numOfPixel;
        int *rgb = new int[numOfPixel*3];

        for(int i = 0; i < height; i++)
        {
            int startY = i*width;
            int step = (i/2)*(width/2);
            int startU = positionOfV + step;
            int startV = positionOfU + step;
            for(int j = 0; j < width; j++)
            {
                int Y = startY + j;
                int U = startU + j/2;
                int V = startV + j/2;
                int index = Y*3;
                RGB tmp = yuv_to_rgb(src[Y], src[U], src[V]);
                rgb[index+R] = tmp.r;
                rgb[index+G] = tmp.g;
                rgb[index+B] = tmp.b;
            }
        }

        return rgb;
    }

    //YV16是yuv422格式，是三个plane，(Y)(U)(V)
    static int* YV16ToRGB(uchar* src, int width, int height)
    {
        int numOfPixel = width * height;
        int positionOfU = numOfPixel;
        int positionOfV = numOfPixel/2 + numOfPixel;
        int *rgb = new int[numOfPixel*3];

        for(int i = 0; i < height; i++)
        {
            int startY = i*width;
            int step = i*width/2;
            int startU = positionOfU + step;
            int startV = positionOfV + step;
            for(int j = 0; j < width; j++)
            {
                int Y = startY + j;
                int U = startU + j/2;
                int V = startV + j/2;
                int index = Y*3;
                //rgb[index+R] = (int)((src[Y]&0xff) + 1.4075 * ((src[V]&0xff)-128));
                //rgb[index+G] = (int)((src[Y]&0xff) - 0.3455 * ((src[U]&0xff)-128) - 0.7169*((src[V]&0xff)-128));
                //rgb[index+B] = (int)((src[Y]&0xff) + 1.779 * ((src[U]&0xff)-128));
                RGB tmp = yuv_to_rgb(src[Y], src[U], src[V]);
                rgb[index+R] = tmp.r;
                rgb[index+G] = tmp.g;
                rgb[index+B] = tmp.b;
            }
        }
        return rgb;
    }

    //YV12是yuv420格式，是3个plane，排列方式为(Y)(V)(U)
    static int* YV12ToRGB(uchar* src, int width, int height)
    {
        int numOfPixel = width * height;
        int positionOfV = numOfPixel;
        int positionOfU = numOfPixel/4 + numOfPixel;
        int *rgb = new int[numOfPixel*3];

        for(int i = 0; i < height; i++)
        {
            int startY = i*width;
            int step = (i/2)*(width/2);
            int startV = positionOfV + step;
            int startU = positionOfU + step;
            for(int j = 0; j < width; j++)
            {
                int Y = startY + j;
                int V = startV + j/2;
                int U = startU + j/2;
                int index = Y*3;

                //rgb[index+R] = (int)((src[Y]&0xff) + 1.4075 * ((src[V]&0xff)-128));
                //rgb[index+G] = (int)((src[Y]&0xff) - 0.3455 * ((src[U]&0xff)-128) - 0.7169*((src[V]&0xff)-128));
                //rgb[index+B] = (int)((src[Y]&0xff) + 1.779 * ((src[U]&0xff)-128));
                RGB tmp = yuv_to_rgb(src[Y], src[U], src[V]);
                rgb[index+R] = tmp.r;
                rgb[index+G] = tmp.g;
                rgb[index+B] = tmp.b;
            }
        }
        return rgb;
    }

    //YUY2是YUV422格式，排列是(YUYV)，是1 plane
    static int* YUY2ToRGB(uchar* src, int width, int height)
    {
        int numOfPixel = width * height;
        int *rgb = new int[numOfPixel*3];
        int lineWidth = 2*width;

        for(int i = 0; i < height; i++)
        {
            int startY = i*lineWidth;
            for(int j = 0; j < lineWidth; j+=4)
            {
                int Y1 = j + startY;
                int Y2 = Y1+2;
                int U = Y1+1;
                int V = Y1+3;
                int index = (Y1>>1)*3;
                RGB tmp = yuv_to_rgb(src[Y1], src[U], src[V]);
                rgb[index+R] = tmp.r;
                rgb[index+G] = tmp.g;
                rgb[index+B] = tmp.b;
                index += 3;
                tmp = yuv_to_rgb(src[Y2], src[U], src[V]);
                rgb[index+R] = tmp.r;
                rgb[index+G] = tmp.g;
                rgb[index+B] = tmp.b;
            }
        }
        return rgb;
    }

    //UYVY是YUV422格式，排列是(UYVY)，是1 plane
    static int* UYVYToRGB(uchar* src, int width, int height)
    {
        int numOfPixel = width * height;
        int *rgb = new int[numOfPixel*3];
        int lineWidth = 2*width;

        for(int i = 0; i < height; i++)
        {
            int startU = i*lineWidth;
            for(int j = 0; j < lineWidth; j += 4)
            {
                int U = j + startU;
                int Y1 = U+1;
                int Y2 = U+3;
                int V = U+2;
                int index = (U>>1)*3;
                RGB tmp = yuv_to_rgb(src[Y1], src[U], src[V]);
                rgb[index+R] = tmp.r;
                rgb[index+G] = tmp.g;
                rgb[index+B] = tmp.b;
                index += 3;
                tmp = yuv_to_rgb(src[Y2], src[U], src[V]);
                rgb[index+R] = tmp.r;
                rgb[index+G] = tmp.g;
                rgb[index+B] = tmp.b;
            }
        }
        return rgb;
    }

    //NV21是YUV420格式，排列是(Y), (VU)，是2 plane
    static int* NV21ToRGB(uchar* src, int width, int height)
    {
        int numOfPixel = width * height;
        int positionOfV = numOfPixel;
        int *rgb = new int[numOfPixel*3];

        for(int i = 0; i < height; i++)
        {
            int startY = i*width;
            int step = i/2*width;
            int startV = positionOfV + step;
            for(int j = 0; j < width; j++)
            {
                int Y = startY + j;
                int V = startV + j/2;
                int U = V + 1;
                int index = Y*3;
                RGB tmp = yuv_to_rgb(src[Y], src[U], src[V]);
                rgb[index+R] = tmp.r;
                rgb[index+G] = tmp.g;
                rgb[index+B] = tmp.b;
            }
        }
        return rgb;
    }

    //NV12是YUV420格式，排列是(Y), (UV)，是2 plane
    static int* NV12ToRGB(uchar* src, int width, int height)
    {
        int numOfPixel = width * height;
        int positionOfU = numOfPixel;
        int *rgb = new int[numOfPixel*3];

        for(int i=0; i<height; i++)
        {
            int startY = i*width;
            int step = i/2*width;
            int startU = positionOfU + step;
            for(int j = 0; j < width; j++)
            {
                int Y = startY + j;
                int U = startU + j/2;
                int V = U + 1;
                int index = Y*3;
                RGB tmp = yuv_to_rgb(src[Y], src[U], src[V]);
                rgb[index+R] = tmp.r;
                rgb[index+G] = tmp.g;
                rgb[index+B] = tmp.b;
            }
        }
        return rgb;
    }

    //NV16是YUV422格式，排列是(Y), (UV)，是2 plane
    static int *NV16ToRGB(uchar* src, int width, int height)
    {
        int numOfPixel = width * height;
        int positionOfU = numOfPixel;
        int *rgb = new int[numOfPixel*3];

        for(int i=0; i<height; i++)
        {
            int startY = i*width;
            int step = i*width;
            int startU = positionOfU + step;
            for(int j = 0; j < width; j++)
            {
                int Y = startY + j;
                int U = startU + j/2;
                int V = U + 1;
                int index = Y*3;
                RGB tmp = yuv_to_rgb(src[Y], src[U], src[V]);
                rgb[index+R] = tmp.r;
                rgb[index+G] = tmp.g;
                rgb[index+B] = tmp.b;
            }
        }
        return rgb;
    }

    //NV61是YUV422格式，排列是(Y), (VU)，是2 plane
    static int *NV61ToRGB(uchar* src, int width, int height){
        int numOfPixel = width * height;
        int positionOfV = numOfPixel;
        int *rgb = new int[numOfPixel*3];

        for(int i =0; i < height; i++)
        {
            int startY = i*width;
            int step = i*width;
            int startV = positionOfV + step;
            for(int j = 0; j < width; j++)
            {
                int Y = startY + j;
                int V = startV + j/2;
                int U = V + 1;
                int index = Y*3;
                RGB tmp = yuv_to_rgb(src[Y], src[U], src[V]);
                rgb[index+R] = tmp.r;
                rgb[index+G] = tmp.g;
                rgb[index+B] = tmp.b;
            }
        }
        return rgb;
    }

    //YVYU是YUV422格式，排列是(YVYU)，是1 plane
    static int *YVYUToRGB(uchar* src, int width, int height)
    {
        int numOfPixel = width * height;
        int *rgb = new int[numOfPixel*3];
        int lineWidth = 2*width;
        for(int i = 0; i < height; i++)
        {
            int startY = i*lineWidth;
            for(int j = 0; j < lineWidth; j+=4)
            {
                int Y1 = j + startY;
                int Y2 = Y1+2;
                int V = Y1+1;
                int U = Y1+3;
                int index = (Y1>>1)*3;
                RGB tmp = yuv_to_rgb(src[Y1], src[U], src[V]);
                rgb[index+R] = tmp.r;
                rgb[index+G] = tmp.g;
                rgb[index+B] = tmp.b;
                index += 3;
                tmp = yuv_to_rgb(src[Y2], src[U], src[V]);
                rgb[index+R] = tmp.r;
                rgb[index+G] = tmp.g;
                rgb[index+B] = tmp.b;
            }
        }
        return rgb;
    }

    //VYUY是YUV422格式，排列是(VYUY)，是1 plane
    static int *VYUYToRGB(uchar* src, int width, int height)
    {
        int numOfPixel = width * height;
        int *rgb = new int[numOfPixel*3];
        int lineWidth = 2*width;
        for(int i =0; i < height; i++)
        {
            int startV = i*lineWidth;
            for(int j = 0; j < lineWidth; j+=4)
            {
                int V = j + startV;
                int Y1 = V+1;
                int Y2 = V+3;
                int U = V+2;
                int index = (U>>1)*3;
                RGB tmp = yuv_to_rgb(src[Y1], src[U], src[V]);
                rgb[index+R] = tmp.r;
                rgb[index+G] = tmp.g;
                rgb[index+B] = tmp.b;
                index += 3;
                tmp = yuv_to_rgb(src[Y2], src[U], src[V]);
                rgb[index+R] = tmp.r;
                rgb[index+G] = tmp.g;
                rgb[index+B] = tmp.b;
            }
        }
        return rgb;
    }

private:
    enum
    {
        R = 0,
        G = 1,
        B = 2
    };
    struct RGB
    {
        int r, g, b;
    };
    static RGB yuv_to_rgb(uchar Y, uchar U, uchar V)
    {
        RGB rgb;
        rgb.r = (int)((Y&0xff) + 1.4075 * ((V&0xff)-128));
        rgb.g = (int)((Y&0xff) - 0.3455 * ((U&0xff)-128) - 0.7169*((V&0xff)-128));
        rgb.b = (int)((Y&0xff) + 1.779 * ((U&0xff)-128));
        rgb.r =(rgb.r<0? 0: rgb.r>255? 255 : rgb.r);
        rgb.g =(rgb.g<0? 0: rgb.g>255? 255 : rgb.g);
        rgb.b =(rgb.b<0? 0: rgb.b>255? 255 : rgb.b);
        return rgb;
    }

};
