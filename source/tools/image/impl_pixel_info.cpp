#include "ky_image.h"

namespace impl {

///
/// A.像素获取方法
///   1.包格式   :
/// 5 6 5
/// R G B
/// #define
/// R= ((val>>(offset * 8)) >> shift) & ~(-(1 << bit));
/// G= ((val>>(offset * 8)) >> shift) & ~(-(1 << bit));
/// B= ((val>>(offset * 8)) >> shift) & ~(-(1 << bit));
/// RGBARGBARGBA...
///   2.面板格式 :
/// B.
///
///
#define MacroBitMasks(bit) (~(-(1 << (bit))))
#define PixelValue(val, offset, shift, bit) ((((val) >> ((offset) * 8)) >> (shift)) & MacroBitMasks(bit))
#define ImageYUV(val, bit) (-((-(val)) >> (bit)))

pixel_image_info PixelInfo[] =
{
    ///< format        alpha  compress planar component byte width height
    ///
    ///< step         offset        shift          bits          name

    {Pixel_L8,          false, false, false,  1, 1, 0, 0,
     {1, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, { 8,  0,  0,  0}, "Luminance"},
    {Pixel_L16,         false, false, false,  1, 2, 0, 0,
     {2, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {16,  0,  0,  0}, "Luminance"},
    {Pixel_A8,           true, false, false,  1, 1, 0, 0,
     {0, 0, 0, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, { 0,  0,  0,  8}, "Alpha"},
    {Pixel_LA8,          true, false, false,  2, 2, 0, 0,
     {1, 0, 0, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, { 8,  0,  0,  8}, "Luminance Alpha"},

    {Pixel_RGB565,      false, false, false,  3,  2, 0, 0,
     {2, 2, 2, 0}, {1, 0, 0, 0}, {3, 5, 0, 0}, { 5,  6,  5,  0}, "RGB565"},
    {Pixel_RGBA4,        true, false, false,  4, 2, 0, 0,
     {2, 2, 2, 2}, {0, 1, 0, 0}, {4, 0, 4, 0}, { 4,  4,  4,  4}, "RGBA4444"},
    {Pixel_RGB5A1,       true, false, false,  4, 2, 0, 0,
     {2, 2, 2, 2}, {1, 0, 0, 0}, {3, 6, 1, 0}, { 5,  5,  5,  1}, "RGB5A1"},
    {Pixel_RGB10A2,      true, false, false,  4, 4, 0, 0,
     {4, 4, 4, 4}, {2, 1, 1, 0}, {6, 4, 2, 0}, {10, 10, 10,  2}, "RGB10A2"},
    {Pixel_RG11B10f,    false, false, false,  3, 4, 0, 0,
     {4, 4, 4, 4}, {0, 0, 0, 0}, {0, 0, 0, 0}, {11, 11, 10,  0}, "RG11B10Float"},
    {Pixel_RGB9E5,      false, false, false,  4, 4, 0, 0,
     {4, 4, 4, 4}, {0, 0, 0, 0}, {0, 0, 0, 0}, { 9,  9,  9,  0}, "RGB9E5Float"},

    ///< unsigned char 8-bit pixel
    {Pixel_R8,          false, false, false,  1, 1, 0, 0,
     {1, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, { 8,  0,  0,  0}, "R8"},
    {Pixel_RG8,         false, false, false,  2, 2, 0, 0,
     {2, 2, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}, { 8,  8,  0,  0}, "RG8"},
    {Pixel_RGB8,        false, false, false,  3, 3, 0, 0,
     {3, 3, 3, 0}, {0, 1, 2, 0}, {0, 0, 0, 0}, { 8,  8,  8,  0}, "RGB8"},
    {Pixel_RGBA8,        true, false, false,  4, 4, 0, 0,
     {4, 4, 4, 4}, {0, 1, 2, 3}, {0, 0, 0, 0}, { 8,  8,  8,  8}, "RGBA8"},

    {Pixel_R8sn,        false, false, false,  1, 1, 0, 0,
     {1, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, { 8,  0,  0,  0}, "R8 SNORM"},
    {Pixel_RG8sn,       false, false, false,  2, 2, 0, 0,
     {2, 2, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}, { 8,  8,  0,  0}, "RG8 SNORM"},
    {Pixel_RGB8sn,      false, false, false,  3, 3, 0, 0,
     {3, 3, 3, 0}, {0, 1, 2, 0}, {0, 0, 0, 0}, { 8,  8,  8,  0}, "RGB8 SNORM"},
    {Pixel_RGBA8sn,      true, false, false,  4, 4, 0, 0,
     {4, 4, 4, 4}, {0, 1, 2, 3}, {0, 0, 0, 0}, { 8,  8,  8,  8}, "RGBA8 SNORM"},

    ///< Hardware Gamma
    {Pixel_sRGB8,       false, false, false,  3, 3, 0, 0,
     {3, 3, 3, 0}, {0, 1, 2, 0}, {0, 0, 0, 0}, { 8,  8,  8,  0}, "sRGB8"},
    {Pixel_sRGBA8,      false, false, false,  4, 4, 0, 0,
     {4, 4, 4, 4}, {0, 1, 2, 3}, {0, 0, 0, 0}, { 8,  8,  8,  8}, "sRGBA8"},

    ///< unsigned int 8-bit pixel
    {Pixel_R8i,         false, false, false,  1, 1, 0, 0,
     {1, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, { 8,  0,  0,  0}, "R8UInt"},
    {Pixel_RG8i,        false, false, false,  2, 2, 0, 0,
     {2, 2, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}, { 8,  8,  0,  0}, "RG8UInt"},
    {Pixel_RGB8i,       false, false, false,  3, 3, 0, 0,
     {3, 3, 3, 0}, {0, 1, 2, 0}, {0, 0, 0, 0}, { 8,  8,  8,  0}, "RGB8UInt"},
    {Pixel_RGBA8i,       true, false, false,  4, 4, 0, 0,
     {4, 4, 4, 4}, {0, 1, 2, 3}, {0, 0, 0, 0}, { 8,  8,  8,  8}, "RGBA8UInt"},

    ///< unsigned short 16-bit pixel
    {Pixel_R16i,        false, false, false,  1, 2, 0, 0,
     {2, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {16,  0,  0,  0}, "R16UInt"},
    {Pixel_RG16i,       false, false, false,  2, 4, 0, 0,
     {4, 4, 0, 0}, {0, 2, 0, 0}, {0, 0, 0, 0}, {16, 16,  0,  0}, "RG16UInt"},
    {Pixel_RGB16i,      false, false, false,  3, 6, 0, 0,
     {6, 6, 6, 0}, {0, 2, 4, 0}, {0, 0, 0, 0}, {16, 16, 16,  0}, "RGB16UInt"},
    {Pixel_RGBA16i,      true, false, false,  4, 8, 0, 0,
     {8, 8, 8, 8}, {0, 2, 4, 6}, {0, 0, 0, 0}, {16, 16, 16, 16}, "RGBA16UInt"},

    ///< unsigned int 32-bit pixel
    {Pixel_R32i,        false, false, false,  1, 4, 0, 0,
     {4, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {32,  0,  0,  0}, "R32UInt"},
    {Pixel_RG32i,       false, false, false,  2, 8, 0, 0,
     {8, 8, 0, 0}, {0, 4, 0, 0}, {0, 0, 0, 0}, {32, 32,  0,  0}, "RG32UInt"},
    {Pixel_RGB32i,      false, false, false,  3,12, 0, 0,
     {12,12,12,0}, {0, 4, 8, 0}, {0, 0, 0, 0}, {32, 32, 32,  0}, "RGB32UInt"},
    {Pixel_RGBA32i,      true, false, false,  4,16, 0, 0,
     {16,16,16,0}, {0, 4, 8,12}, {0, 0, 0, 0}, {32, 32, 32, 32}, "RGBA32UInt"},

    ///< half float 16-bit pixel
    {Pixel_R16f,        false, false, false,  1, 2, 0, 0,
     {2, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {16,  0,  0,  0}, "R16Float"},
    {Pixel_RG16f,       false, false, false,  2, 4, 0, 0,
     {4, 4, 0, 0}, {0, 2, 0, 0}, {0, 0, 0, 0}, {16, 16,  0,  0}, "RG16Float"},
    {Pixel_RGB16f,      false, false, false,  3, 6, 0, 0,
     {6, 6, 6, 0}, {0, 2, 4, 0}, {0, 0, 0, 0}, {16, 16, 16,  0}, "RGB16Float"},
    {Pixel_RGBA16f,      true, false, false,  4, 8, 0, 0,
     {8, 8, 8, 8}, {0, 2, 4, 6}, {0, 0, 0, 0}, {16, 16, 16, 16}, "RGBA16Float"},

    ///< float 32-bit pixel
    {Pixel_R32f,        false, false, false,  1, 4, 0, 0,
     {4, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {32,  0,  0,  0}, "R32Float"},
    {Pixel_RG32f,       false, false, false,  2, 8, 0, 0,
     {8, 8, 0, 0}, {0, 4, 0, 0}, {0, 0, 0, 0}, {32, 32,  0,  0}, "RG32Float"},
    {Pixel_RGB32f,      false, false, false,  3,12, 0, 0,
     {12,12,12,0}, {0, 4, 8, 0}, {0, 0, 0, 0}, {32, 32, 32,  0}, "RGB32Float"},
    {Pixel_RGBA32f,      true, false, false,  4,16, 0, 0,
     {16,16,16,16},{0, 4, 8,12}, {0, 0, 0, 0}, {32, 32, 32, 32}, "RGBA32Float"},

    ///< format        alpha  compress planar component byte width height
    ///
    ///< step         offset        shift          bits          name

    ///< Ericsson Texture Compression
    {Pixel_ETC1RGB8,    false,  true, false,  3, 0, 0, 0,
     {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, { 0,  0,  0,  0}, "ETC1RGB8"},
    {Pixel_ETC2RGB8,    false,  true, false,  3, 0, 0, 0,
     {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, { 0,  0,  0,  0}, "ETC2RGB8"},
    {Pixel_ETC2RGBA8,    true,  true, false,  4, 0, 0, 0,
     {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, { 0,  0,  0,  0}, "ETC2RGBA8"},
    {Pixel_ETC2RGB8A1,   true,  true, false,  4, 0, 0, 0,
     {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, { 0,  0,  0,  0}, "ETC2RGB8A1"},

    ///< PowerVR Texture Compression
    {Pixel_PVRTCRGB4,   false,  true, false,  3, 0, 0, 0,
     {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, { 0,  0,  0,  0}, "PVRTCRGB4"},
    {Pixel_PVRTCRGB2,   false,  true, false,  3, 0, 0, 0,
     {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, { 0,  0,  0,  0}, "PVRTCRGB2"},
    {Pixel_PVRTCRGBA4,   true,  true, false,  4, 0, 0, 0,
     {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, { 0,  0,  0,  0}, "PVRTCRGBA4V1"},
    {Pixel_PVRTCRGBA2,   true,  true, false,  4, 0, 0, 0,
     {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, { 0,  0,  0,  0}, "PVRTCRGBA2V1"},
    {Pixel_PVRTCRGBA4V2, true,  true, false,  4,  0, 0, 0,
     {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, { 0,  0,  0,  0}, "PVRTCRGBA4V2"},
    {Pixel_PVRTCRGBA2V2, true,  true, false,  4, 0, 0, 0,
     {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, { 0,  0,  0,  0}, "PVRTCRGBA2V2"},

    ///< ATI texture compression
    {Pixel_ATITCRGB,    false,  true, false,  3, 0, 0, 0,
     {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, { 0,  0,  0,  0}, "ATITCRGB"},
    {Pixel_ATITCRGBAe,   true,  true, false,  4, 0, 0, 0,
     {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, { 0,  0,  0,  0}, "ATITCRGBAExplicit"},
    {Pixel_ATITCRGBAi,   true,  true, false,  4, 0, 0, 0,
     {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, { 0,  0,  0,  0}, "ATITCRGBAInerpolated"},

    ///< S3 Texture Compression
    {Pixel_S3TC1,        true,  true, false,  4, 0, 0, 0,
     {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, { 0,  0,  0,  0}, "S3TCDXT1"},
    {Pixel_S3TC2,        true,  true, false,  4, 0, 0, 0,
     {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, { 0,  0,  0,  0}, "S3TCDXT2"},
    {Pixel_S3TC3,        true,  true, false,  4, 0, 0, 0,
     {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, { 0,  0,  0,  0}, "S3TCDXT3"},
    {Pixel_S3TC4,        true,  true, false,  4, 0, 0, 0,
     {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, { 0,  0,  0,  0}, "S3TCDXT4"},
    {Pixel_S3TC5,        true,  true, false,  4, 0, 0, 0,
     {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, { 0,  0,  0,  0}, "S3TCDXT5"},

    ///< Depth
    {Pixel_Depth,       false, false, false,  1, 4, 0, 0,
     {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {16,  0,  0,  0}, "Depth"},

    ///< format        alpha  compress planar component byte width height
    ///
    ///< step         offset        shift          bits          name

    {Pixel_BGR8,        false, false, false,  3, 3, 0, 0,
     {3, 3, 3, 0}, {2, 1, 0, 0}, {0, 0, 0, 0}, { 8,  8,  8,  0}, "BGR8"},
    {Pixel_ARGB,         true, false, false,  4, 4, 0, 0,
     {4, 4, 4, 4}, {1, 2, 3, 0}, {0, 0, 0, 0}, { 8,  8,  8,  8}, "ARGB"},
    {Pixel_ABGR,         true, false, false,  4, 4, 0, 0,
     {4, 4, 4, 4}, {3, 2, 1, 0}, {0, 0, 0, 0}, { 8,  8,  8,  8}, "ABGR"},
    {Pixel_BGRA,         true, false, false,  4, 4, 0, 0,
     {4, 4, 4, 4}, {2, 1, 0, 3}, {0, 0, 0, 0}, { 8,  8,  8,  8}, "BGRA"},

    {Pixel_XRGB4,       false, false, false,  4, 2, 0, 0,
     {2, 2, 2, 0}, {1, 0, 0, 0}, {0, 4, 0, 0}, { 4,  4,  4,  0}, "XRGB4"},
    {Pixel_XRGB5,       false, false, false,  4, 2, 0, 0,
     {2, 2, 2, 0}, {1, 0, 0, 0}, {2, 5, 0, 0}, { 5,  5,  5,  0}, "XRGB5"},
    {Pixel_BGR16,       false, false, false,  3, 6, 0, 0,
     {6, 6, 6, 0}, {4, 2, 0, 0}, {0, 0, 0, 0}, {16, 16, 16,  0}, "BGR16"},
    {Pixel_XBGR4,       false, false, false,  4, 2, 0, 0,
     {2, 2, 2, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}, { 4,  4,  4,  4}, "XBGR4"},
    {Pixel_XBGR5,       false, false, false,  4, 2, 0, 0,
     {2, 2, 2, 0}, {0, 0, 1, 0}, {0, 5, 2, 0}, { 5,  5,  5,  0}, "XBGR5"},
    {Pixel_BGR565,      false, false, false,  3, 2, 0, 0,
     {2, 2, 2, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}, { 5,  6,  5,  0}, "BGR565"},
    {Pixel_BGRA16,       true, false, false,  4, 8, 0, 0,
     {8, 8, 8, 8}, {4, 2, 0, 6}, {0, 0, 0, 0}, {16, 16, 16, 16}, "BGRA16"},

    {Pixel_AYUV16,      false, false, false,  4, 8, 0, 0,
     {8, 8, 8, 8}, {2, 4, 6, 0}, {0, 0, 0, 0}, {16, 16, 16, 16}, "AYUV16"},
    {Pixel_YUYV422,     false, false, false,  3, 2, 1, 0,
     {2, 4, 4, 0}, {0, 1, 3, 0}, {0, 0, 0, 0}, { 8,  8,  8,  0}, "YUYV422"},
    {Pixel_UYVY422,     false, false, false,  3, 2, 1, 0,
     {2, 4, 4, 0}, {1, 0, 2, 0}, {0, 0, 0, 0}, { 8,  8,  8,  0}, "UYVY422"},
    {Pixel_YVYU422,     false, false, false,  3, 2, 1, 0,
     {2, 4, 4, 0}, {0, 3, 1, 0}, {0, 0, 0, 0}, { 8,  8,  8,  0}, "YVYU422"},
    {Pixel_UYYVYY411,   false, false, false,  3, 2, 2, 0,
     {4, 6, 6, 0}, {1, 0, 3, 0}, {0, 0, 0, 0}, { 8,  8,  8,  0}, "UYYVYY411"},
    {Pixel_XYZ12,       false, false, false,  3, 2, 0, 0,
     {6, 6, 6, 0}, {0, 2, 4, 0}, {0, 0, 0, 0}, {12, 12, 12,  0}, "XYZ12"},

    {Pixel_YUV410P,     false, false,  true,  3, 1, 2, 2,
     {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, { 8,  8,  8,  0}, "YUV410P"},
    {Pixel_YUV411P,     false, false,  true,  3, 1, 2, 0,
     {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, { 8,  8,  8,  0}, "YUV411P"},
    {Pixel_YUV420P,     false, false,  true,  3, 1, 1, 1,
     {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, { 8,  8,  8,  0}, "YUV420P"},
    {Pixel_YUV422P,     false, false,  true,  3, 1, 1, 0,
     {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, { 8,  8,  8,  0}, "YUV422P"},
    {Pixel_YUV440P,     false, false,  true,  3, 1, 0, 1,
     {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, { 8,  8,  8,  0}, "YUV440P"},
    {Pixel_YUV444P,     false, false,  true,  3, 1, 0, 0,
     {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, { 8,  8,  8,  0}, "YUV444P"},

    {Pixel_YUVA420P,     true, false,  true,  4, 1, 1, 1,
     {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, { 8,  8,  8,  8}, "YUVA420P"},
    {Pixel_YUVA422P,     true, false,  true,  4, 1, 1, 0,
     {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, { 8,  8,  8,  8}, "YUVA422P"},
    {Pixel_YUVA444P,     true, false,  true,  4, 1, 0, 0,
     {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, { 8,  8,  8,  8}, "YUVA444P"},

    {Pixel_YUV420P9,    false, false,  true,  3, 2, 1, 1,
     {2, 2, 2, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, { 9,  9,  9,  0}, "YUV420P9"},
    {Pixel_YUV422P9,    false, false,  true,  3, 2, 1, 0,
     {2, 2, 2, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, { 9,  9,  9,  0}, "YUV422P9"},
    {Pixel_YUV444P9,    false, false,  true,  3, 2, 0, 0,
     {2, 2, 2, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, { 9,  9,  9,  0}, "YUV444P9"},

    {Pixel_YUV420P10,   false, false,  true,  3, 2, 1, 1,
     {2, 2, 2, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {10, 10, 10,  0}, "YUV420P10"},
    {Pixel_YUV422P10,   false, false,  true,  3, 2, 1, 0,
     {2, 2, 2, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {10, 10, 10,  0}, "YUV422P10"},
    {Pixel_YUV440P10,   false, false,  true,  3, 2, 0, 1,
     {2, 2, 2, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {10, 10, 10,  0}, "YUV440P10"},
    {Pixel_YUV444P10,   false, false,  true,  3, 2, 0, 0,
     {2, 2, 2, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {10, 10, 10,  0}, "YUV444P10"},

    {Pixel_YUV420P12,   false, false,  true,  3, 2, 1, 1,
     {2, 2, 2, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {12, 12, 12,  0}, "YUV420P12"},
    {Pixel_YUV422P12,   false, false,  true,  3, 2, 1, 0,
     {2, 2, 2, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {12, 12, 12,  0}, "YUV422P12"},
    {Pixel_YUV440P12,   false, false,  true,  3, 2, 0, 1,
     {2, 2, 2, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {12, 12, 12,  0}, "YUV440P12"},
    {Pixel_YUV444P12,   false, false,  true,  3, 2, 0, 0,
     {2, 2, 2, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {12, 12, 12,  0}, "YUV444P12"},

    {Pixel_YUV420P14,   false, false,  true,  3, 2, 1, 1,
     {2, 2, 2, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {14, 14, 14,  0}, "YUV420P14"},
    {Pixel_YUV422P14,   false, false,  true,  3, 2, 1, 0,
     {2, 2, 2, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {14, 14, 14,  0}, "YUV422P14"},
    {Pixel_YUV444P14,   false, false,  true,  3, 2, 0, 0,
     {2, 2, 2, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {14, 14, 14,  0}, "YUV444P14"},

    {Pixel_YUV420P16,   false, false,  true,  3, 2, 1, 1,
     {2, 2, 2, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {16, 16, 16,  0}, "YUV420P16"},
    {Pixel_YUV422P16,   false, false,  true,  3, 2, 1, 0,
     {2, 2, 2, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {16, 16, 16,  0}, "YUV422P16"},
    {Pixel_YUV444P16,   false, false,  true,  3, 2, 0, 0,
     {2, 2, 2, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {16, 16, 16,  0}, "YUV444P16"},

    {Pixel_YUVA420P9,    true, false,  true,  4, 2, 1, 1,
     {2, 2, 2, 2}, {0, 0, 0, 0}, {0, 0, 0, 0}, { 9,  9,  9,  9}, "YUVA420P9"},
    {Pixel_YUVA422P9,    true, false,  true,  4, 2, 1, 0,
     {2, 2, 2, 2}, {0, 0, 0, 0}, {0, 0, 0, 0}, { 9,  9,  9,  9}, "YUVA422P9"},
    {Pixel_YUVA444P9,    true, false,  true,  4, 2, 0, 0,
     {2, 2, 2, 2}, {0, 0, 0, 0}, {0, 0, 0, 0}, { 9,  9,  9,  9}, "YUVA444P9"},

    {Pixel_YUVA420P10,   true, false,  true,  4, 2, 1, 1,
     {2, 2, 2, 2}, {0, 0, 0, 0}, {0, 0, 0, 0}, {10, 10, 10, 10}, "YUVA420P10"},
    {Pixel_YUVA422P10,   true, false,  true,  4, 2, 1, 0,
     {2, 2, 2, 2}, {0, 0, 0, 0}, {0, 0, 0, 0}, {10, 10, 10, 10}, "YUVA422P10"},
    {Pixel_YUVA444P10,   true, false,  true,  4, 2, 0, 0,
     {2, 2, 2, 2}, {0, 0, 0, 0}, {0, 0, 0, 0}, {10, 10, 10, 10}, "YUVA444P10"},

    {Pixel_YUVA420P16,   true, false,  true,  4, 2, 1, 1,
     {2, 2, 2, 2}, {0, 0, 0, 0}, {0, 0, 0, 0}, {16, 16, 16, 16}, "YUVA420P16"},
    {Pixel_YUVA422P16,   true, false,  true,  4, 2, 1, 0,
     {2, 2, 2, 2}, {0, 0, 0, 0}, {0, 0, 0, 0}, {16, 16, 16, 16}, "YUVA422P16"},
    {Pixel_YUVA444P16,   true, false,  true,  4, 2, 0, 0,
     {2, 2, 2, 2}, {0, 0, 0, 0}, {0, 0, 0, 0}, {16, 16, 16, 16}, "YUVA444P16"},

    {Pixel_NV12,        false, false,  true,  3, 1, 1, 1,
     {1, 2, 2, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}, { 8,  8,  8,  0}, "NV12"},
    {Pixel_NV21,        false, false,  true,  3,  1, 1, 1,
     {1, 2, 2, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}, { 8,  8,  8,  0}, "NV21"},
    {Pixel_NV16,        false, false,  true,  3,  1, 1, 0,
     {1, 2, 2, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}, { 8,  8,  8,  0}, "NV16"},
    {Pixel_NV20,        false, false,  true,  3,  2, 1, 0,
     {2, 4, 4, 0}, {0, 0, 2, 0}, {0, 0, 0, 0}, {10, 10, 10,  0}, "NV20"},

};


}
