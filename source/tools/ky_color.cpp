#include "ky_color.h"
#include "maths/ky_mat3x3.h"
const ky_color::Type    HsvUndefined     = -1.0;
const ky_color::Type    HslUndefined     = HsvUndefined;
const ky_color::Type    RgbComponent     = 0.003921568627451;

static ky_color::Type x_gamma(ky_color::Type x)
{
    return x > (ky_color::Type)0.04045 ?
                ky_pow((x + (ky_color::Type)0.055) /
                    (ky_color::Type)1.055,
                    (ky_color::Type)2.4) :
                x / (ky_color::Type)12.92;
}
class FloatColor
{

#define RGB_2_XYZ (mat3(0.4124564, 0.3575761, 0.1804375, \
                        0.2126729, 0.7151522, 0.0721750, \
                        0.0193339, 0.1191920, 0.9503041))
#define XYZ_2_RGB (mat3(3.2404542,-1.5371385,-0.4985314, \
                        -0.9692660, 1.8760108, 0.0415560, \
                        0.0556434,-0.2040259, 1.0572252))

public:
    //! HSV格式
    static ky_color rgb_to_hsv (const ky_color &rgb)
    {
        ky_color hsv;

        ky_color::Type max = ky_max (ky_max(rgb.r, rgb.g), rgb.b);
        ky_color::Type min = ky_min (ky_min(rgb.r, rgb.g), rgb.b);
        ky_color::Type delta = max - min;
        hsv.v = max;

        if (delta > 0.0001)
        {
            ky_color::Type hue = 0.0;
            if (rgb.r == max)
            {
                hue = (rgb.g - rgb.b) / delta;
                if (hue < 0.0)
                    hue += 6.0;
            }
            else if (rgb.g == max)
                hue = 2.0 + (rgb.b - rgb.r) / delta;
            else
                hue = 4.0 + (rgb.r - rgb.g) / delta;

            hue /= 6.0;
            return ky_color(hue, delta / max , max, hsv.a);
        }

        return ky_color(0.f,0.f,max, hsv.a);
    }
    static ky_color hsv_to_rgb (const ky_color &hsv)
    {
        if (!kyEmptyf (hsv.s))
        {
            ky_color::Type hue = (hsv.h == 1.0) ? 0.0 : hsv.h * 6.0;

            int i = (int) hue;
            ky_color::Type f = hue - i;
            ky_color::Type w = hsv.v * (1.0 - hsv.s);
            ky_color::Type q = hsv.v * (1.0 - (hsv.s * f));
            ky_color::Type t = hsv.v * (1.0 - (hsv.s * (1.0 - f)));

            switch (i)
            {
            case 0: return ky_color(hsv.v, t, w, hsv.a);
            case 1: return ky_color(q, hsv.v, w, hsv.a);
            case 2: return ky_color(w, hsv.v, t, hsv.a);
            case 3: return ky_color(w, q, hsv.v, hsv.a);
            case 4: return ky_color(t, w, hsv.v, hsv.a);
            case 5: return ky_color(hsv.v, w, q, hsv.a);
            }
        }
        return ky_color (hsv.v,hsv.v,hsv.v,hsv.a);
    }

    //! HSL格式
    static ky_color rgb_to_hsl (const ky_color &rgb)
    {
        ky_color::Type max = ky_max (ky_max(rgb.r, rgb.g), rgb.b);
        ky_color::Type min = ky_min (ky_min(rgb.r, rgb.g), rgb.b);

        ky_color::Type luminance = (max + min) / 2.0;

        if (max != min)
        {
            ky_color::Type saturation = luminance <= 0.5 ?
                        (max - min) / (max + min):
                        (max - min) / (2.0 - max - min);
            ky_color::Type delta = max - min;

            if (delta == 0.0)
                delta = 1.0;

            ky_color::Type hue = 4.0 + (rgb.r - rgb.g) / delta;
            if (rgb.r == max)
                hue = (rgb.g - rgb.b) / delta;
            else if (rgb.g == max)
                hue = 2.0 + (rgb.b - rgb.r) / delta;

            hue /= 6.0;

            if (hue < 0.0)
                hue += 1.0;
            return ky_color(hue, saturation, luminance, rgb.a);
        }

        return ky_color(HslUndefined, 0.f, luminance, rgb.a);
    }
    static ky_color hsl_to_rgb (const ky_color &hsl)
    {
        struct CLocal
        {
            static inline ky_color::Type hsl_value
                    (ky_color::Type n1, ky_color::Type n2, ky_color::Type hue)
            {
                ky_color::Type val;

                if (hue > 6.0)
                    hue -= 6.0;
                else if (hue < 0.0)
                    hue += 6.0;

                if (hue < 1.0)
                    val = n1 + (n2 - n1) * hue;
                else if (hue < 3.0)
                    val = n2;
                else if (hue < 4.0)
                    val = n1 + (n2 - n1) * (4.0 - hue);
                else
                    val = n1;

                return val;
            }
        };

        if (!kyEmptyf(hsl.s))
        {
            ky_color::Type m2 = hsl.l + hsl.s - hsl.l * hsl.s;
            if (hsl.l <= 0.5)
                m2 = hsl.l * (1.0 + hsl.s);

            ky_color::Type m1 = 2.0 * hsl.l - m2;

            return ky_color(CLocal::hsl_value (m1, m2, hsl.h * 6.0 + 2.0),
                            CLocal::hsl_value (m1, m2, hsl.h * 6.0),
                            CLocal::hsl_value (m1, m2, hsl.h * 6.0 - 2.0),
                            hsl.a);
        }

        return ky_color(hsl.l,hsl.l,hsl.l,hsl.a);
    }

    //! HSI格式
    static ky_color rgb_to_hsi (const ky_color &rgb)
    {
        ky_color hsi;
        const u8 R = rgb.r * 255;
        const u8 G = rgb.g * 255;
        const u8 B = rgb.b * 255;

        const u8 min = ky_min(R, ky_min(G, B));

        hsi.i = (R + B + G) / 3.0;
        hsi.s = (hsi.i < 0.001) ? 1.0 : 1.0 - min / hsi.i;
        hsi.a = rgb.a;

        if (min == B)
        {
            ky_color::Type num = 2.0 * PIf * (G - B);
            ky_color::Type den = 3.0 * (R + G - 2 * B);
            hsi.h = (den < 0.001) ? PIdiv2f : num / den;
        }
        if (min == R)
        {
            ky_color::Type num = 2.0 * PIf * (B - R);
            ky_color::Type den = 3.0 * (G + B - 2 * R);
            hsi.h = num / den + 2 * PIf / 3;
        }
        else
        {
            ky_color::Type num = 2.0 * PIf * (R - G);
            ky_color::Type den = 3.0 * (R + B - 2 * G);
            hsi.h = num / den + 4 * PIf / 3;
        }
        return hsi;
    }
    static ky_color hsi_to_rgb (const ky_color &hsi)
    {
        ky_color rgb;
        if (hsi.h > 0 && hsi.h <= 2*PIf/3)
        {
            rgb.b = hsi.i * (1.0 - hsi.s);
            rgb.r = hsi.i * (1.0 + hsi.s * cos(hsi.h) / cos(PIf/3 - hsi.h));
            rgb.g = 3 * hsi.i - rgb.r - rgb.b;
        }
        else if (hsi.h < 4*PIf / 3)
        {
            rgb.r = hsi.i * (1.0 - hsi.s);
            rgb.g = hsi.i * (1.0 + hsi.s * cos(hsi.h - 2*PIf/3) / cos(PIf - hsi.h));
            rgb.b = 3 * hsi.i - rgb.r - rgb.g;
        }
        else
        {
            rgb.g = hsi.i * (1.0 - hsi.s);
            rgb.b = hsi.i * (1.0 + hsi.s * cos(hsi.h - 4*PIf/3) / cos(5*PIf/3 - hsi.h));
            rgb.r = 3 * hsi.i - rgb.g - rgb.b;
        }
        return ky_color((ky_color::Type)ky_clamp(rgb.r / 255., 0., 1.),
                        (ky_color::Type)ky_clamp(rgb.r / 255., 0., 1.),
                        (ky_color::Type)ky_clamp(rgb.r / 255., 0., 1.),
                        (ky_color::Type)hsi.a);
    }

    //! CMYK格式
    //! \brief rgb_to_cmyk
    //! \param rgb
    //! \param pullout (0-1) 0使rgb转换到CMY，1使rgb黑色转换到最大量
    //! \return
    //!
    static ky_color rgb_to_cmyk (const ky_color &rgb, ky_color::Type pullout)
    {
        ky_color cmyk;
        ky_color::Type c, m, y, k;

        c = 1.0 - rgb.r;
        m = 1.0 - rgb.g;
        y = 1.0 - rgb.b;

        k = 1.0;
        if (c < k)  k = c;
        if (m < k)  k = m;
        if (y < k)  k = y;

        k *= pullout;

        if (k < 1.0)
        {
            cmyk.c = (c - k) / (1.0 - k);
            cmyk.m = (m - k) / (1.0 - k);
            cmyk.y = (y - k) / (1.0 - k);
        }
        else
        {
            cmyk.c = 0.0;
            cmyk.m = 0.0;
            cmyk.y = 0.0;
        }

        cmyk.k = k;
        return cmyk;
    }
    static ky_color cmyk_to_rgb (const ky_color &cmyk)
    {
        ky_color rgb;
        ky_color::Type c, m, y, k;
        k = cmyk.k;

        if (k < 1.0)
        {
            c = cmyk.c * (1.0 - k) + k;
            m = cmyk.m * (1.0 - k) + k;
            y = cmyk.y * (1.0 - k) + k;
        }
        else
        {
            c = 1.0;
            m = 1.0;
            y = 1.0;
        }

        rgb.r = 1.0 - c;
        rgb.g = 1.0 - m;
        rgb.b = 1.0 - y;
        return rgb;
    }

    //! HWB格式
    //! \brief rgb_to_hwb
    //! 当w == 1 - b，h是不确定的。
    //! \param rgb
    //! \return
    //!
    static ky_color rgb_to_hwb (const ky_color &rgb)
    {
        ky_color::Type R = rgb.r, G = rgb.g, B = rgb.b;

        ky_color::Type v = ky_max (ky_max(rgb.r, rgb.g), rgb.b);
        ky_color::Type w = ky_min (ky_min(rgb.r, rgb.g), rgb.b);
        ky_color::Type b = 1.0 - v;

        if (v != w)
        {
            ky_color::Type f = (R == w) ? G - B : ((G == w) ? B - R : R - G);
            int i = (R == w) ? 3.0 : ((G == w) ? 5.0 : 1.0);

            return ky_color((ky_color::Type)(360.0 / 6.0) * (i - f / (v - w)), w, b, rgb.a);
        }
        return  ky_color(HsvUndefined, w, b, rgb.a);
    }
    static ky_color hwb_to_rgb (const ky_color &hwb)
    {
#define RETURN_RGB(x, y, z) return ky_color (x, y ,z)

        ky_color::Type h = hwb.h, w = hwb.w, b = hwb.b;
        ky_color::Type v = 1.0 - b;
        h = 6.0 * h/ 360.0;

        if (h != HsvUndefined)
        {
            int i = ky_ifloor (h);
            ky_color::Type f = h - i;

            if (i & 1)
                f = 1.0 - f;  /* if i is odd */

            ky_color::Type n = w + f * (v - w);     /* linear interpolation between w and v */
            switch (i)
            {
            case 6:
            case 0: RETURN_RGB (v, n, w);
            case 1: RETURN_RGB (n, v, w);
            case 2: RETURN_RGB (w, v, n);
            case 3: RETURN_RGB (w, n, v);
            case 4: RETURN_RGB (n, w, v);
            case 5: RETURN_RGB (v, w, n);
            }
        }
#undef RETURN_RGB
        return ky_color(v,v,v,hwb.a);
    }

    //! XYZ格式
    //! \brief rgb_to_xyz
    //! \param rgb
    //! \return
    //!
    static ky_color rgb_to_xyz (const ky_color &rgb)
    {
        vec3 xyz = RGB_2_XYZ * vec3(rgb.r,rgb.g,rgb.b);
        return ky_color(xyz.x, xyz.y, xyz.z, rgb.a);
    }
    static ky_color xyz_to_rgb (const ky_color &xyz)
    {
        vec3 rgb = XYZ_2_RGB * vec3(xyz.x, xyz.y, xyz.z);
        return ky_color(rgb.r, rgb.g, rgb.b, xyz.a);
    }

    //! LAB格式
    static ky_color xyz_to_lab (const ky_color &xyz)
    {
        vec3 nXYZ = vec3(xyz.x, xyz.y, xyz.z) / vec3(0.950456f, 1.0f, 1.088753f);
        float x = nXYZ.x > 0.008856f ? pow(nXYZ.x, 0.333333f) : (7.787f * nXYZ.x +0.137931f);
        float y = nXYZ.y > 0.008856f ? pow(nXYZ.y, 0.333333f) : (7.787f * nXYZ.y +0.137931f);
        float z = nXYZ.z > 0.008856f ? pow(nXYZ.z, 0.333333f) : (7.787f * nXYZ.z +0.137931f);

        return ky_color(nXYZ.y > 0.008856f ? (116.0f * y - 16.0f) : (903.3f * nXYZ.y),
                        500.f * (x - y), 200.f * (y - z), xyz.a);
    }
    static ky_color lab_to_xyz (const ky_color &lab)
    {
        vec3 xyz;
        float fY = (lab.x + 16.0f) / 116.0f;
        float fX = lab.y / 500.0f + fY;
        float fZ = fY - lab.z / 200.0f;

        xyz.x = fX > 0.206893f ? fX * fX * fX : (fX - 0.137931f) / 7.787f;
        xyz.y = fY > 0.206893f ? fY * fY * fY : (fY - 0.137931f) / 7.787f;
        xyz.z = fZ > 0.206893f ? fZ * fZ * fZ : (fZ - 0.137931f) / 7.787f;
        xyz *= vec3(0.950456f, 1.0f, 1.088753f);
        return ky_color(xyz.x, xyz.y, xyz.z, lab.a);
    }
    static ky_color rgb_to_lab (const ky_color &rgb)
    {
        ky_color xyz = rgb_to_xyz (rgb);
        return xyz_to_lab (xyz);
    }
    static ky_color lab_to_rgb (const ky_color &lab)
    {
        ky_color xyz = lab_to_xyz (lab);
        return xyz_to_rgb (xyz);
    }

    //! HEX格式
    static ky_color hex_to_rgba(ky_color32 hex)
    {
        ky_color rgba;
        rgba.r = (1.0 / 255.0)*((hex >> 24) & 0xff);
        rgba.g = (1.0 / 255.0)*((hex >> 16) & 0xff);
        rgba.b = (1.0 / 255.0)*((hex >> 8) & 0xff);
        rgba.a = (1.0 / 255.0)* (hex & 0xff);
        return rgba;
    }
    static ky_color32 rgba_to_hex(ky_color::Type R, ky_color::Type G,
                                  ky_color::Type B, ky_color::Type A)
    {
        return (((ky_color32)ky_clamp(R*255.0, 0.0, 255.0))<<24) |
                (((ky_color32)ky_clamp(G*255.0, 0.0, 255.0))<<16) |
                (((ky_color32)ky_clamp(B*255.0, 0.0, 255.0))<<8) |
                ((ky_color32)ky_clamp(A*255.0, 0.0, 255.0));
    }

    //! YCbCr格式
    static ky_color rgb_to_ycbcr_bt601(const ky_color &rgb)
    {
        static const ky_color::Type Wr = 0.2988390, Wb = 0.1143500, Wg = 1 - Wr - Wb;
        //static const flt32 Umax = 0.436, Vmax = 0.615;

        ky_color ycc;
        ycc.y = Wr * rgb.r + Wg * rgb.g + Wb * rgb.b;
        //u_ = Umax * ( rgb.b - ycc.y )/( 1- Wb );
        //v_ = Vmax * ( rgb.r - ycc.y )/( 1- Wr );
        ycc.cb = ((rgb.b - ycc.y) / (1 - Wb) + ky_color::Type(1)) / ky_color::Type(2);
        ycc.cr = ((rgb.r - ycc.y) / (1 - Wr) + ky_color::Type(1)) / ky_color::Type(2);
        ycc.a = rgb.a;
        return ycc;
    }
    static ky_color rgb_to_ycbcr_bt709(const ky_color &rgb)
    {
        static const ky_color::Type Wr = 0.2126729, Wb = 0.0721750, Wg = 1 - Wr - Wb;
        //static const flt32 Umax = 0.436, Vmax = 0.615;
        ky_color ycc;
        ycc.y = Wr * rgb.r + Wg * rgb.g + Wb * rgb.b;
        //u_ = Umax * ( rgb.b - ycc.y )/( 1- Wb );
        //v_ = Vmax * ( rgb.r - ycc.y )/( 1- Wr );
        ycc.cb = ((rgb.b - ycc.y) / (1 - Wb) + flt32(1)) / flt32(2);
        ycc.cr = ((rgb.r - ycc.y) / (1 - Wr) + flt32(1)) / flt32(2);
        ycc.a = rgb.a;
        return ycc;
    }
    static ky_color ycbcr_to_rgb_bt601(const ky_color &ycc)
    {
        static const ky_color::Type Wr = 0.2988390, Wb = 0.1143500, Wg = 1- Wr -Wb;
        static const ky_color::Type Umax = 0.436, Vmax = 0.615;
        static ky_color::Type b11= 1, b12= 0,                   b13= (1- Wr) /Vmax;
        static ky_color::Type b21= 1, b22= -Wb*(1-Wb) /Umax/Wg, b23= -Wr*(1-Wr) /Vmax/Wg;
        static ky_color::Type b31= 1, b32= ((1- Wb) /Umax),     b33= 0;

        ky_color rgb;
        const ky_color::Type cb = (ycc.cb - ky_color::Type(0.5)) * ky_color::Type(2) * Umax;
        const ky_color::Type cr = (ycc.cr - ky_color::Type(0.5)) * ky_color::Type(2) * Vmax;

        rgb.r = ycc.y*b11 + cb*b12 + cr*b13;
        rgb.g = ycc.y*b21 + cb*b22 + cr*b23;
        rgb.b = ycc.y*b31 + cb*b32 + cr*b33;
        rgb.a = ycc.a;
        return rgb;
    }
    static ky_color ycbcr_to_rgb_bt709(const ky_color &ycc)
    {
        static const flt32 Wr = 0.2126729, Wb = 0.0721750, Wg = 1 - Wr - Wb;
        static const ky_color::Type Umax = 0.436, Vmax = 0.615;
        static ky_color::Type b11= 1, b12= 0,                   b13= (1- Wr) /Vmax;
        static ky_color::Type b21= 1, b22= -Wb*(1-Wb) /Umax/Wg, b23= -Wr*(1-Wr) /Vmax/Wg;
        static ky_color::Type b31= 1, b32= ((1- Wb) /Umax),     b33= 0;

        ky_color rgb;
        const ky_color::Type cb = (ycc.cb - ky_color::Type(0.5)) * ky_color::Type(2) * Umax;
        const ky_color::Type cr = (ycc.cr - ky_color::Type(0.5)) * ky_color::Type(2) * Vmax;

        rgb.r = ycc.y*b11 + cb*b12 + cr*b13;
        rgb.g = ycc.y*b21 + cb*b22 + cr*b23;
        rgb.b = ycc.y*b31 + cb*b32 + cr*b33;
        rgb.a = ycc.a;
        return rgb;
    }

    //! YCoCg格式
    static ky_color ycocg_to_rgb(const ky_color &ycc)
    {
        ky_color rgb;
        rgb.r = ycc.y + ycc.co - ycc.cg;
        rgb.g = ycc.y          + ycc.cg;
        rgb.b = ycc.y - ycc.co - ycc.cg;
        rgb.a = ycc.a;
        return rgb;
    }
    static ky_color rgb_to_ycocg(const ky_color &rgb)
    {
        ky_color result;
        result.y =  rgb.r / ky_color::Type(4) + rgb.g / ky_color::Type(2) +
                rgb.b / ky_color::Type(4);
        result.co = rgb.r / ky_color::Type(2) + rgb.g * ky_color::Type(0) -
                rgb.b / ky_color::Type(2);
        result.cg = -rgb.r / ky_color::Type(4) + rgb.g / ky_color::Type(2) -
                rgb.b / ky_color::Type(4);
        return result;
    }
    //! YCoCg-R格式
    static ky_color ycocgr_to_rgb(const ky_color &ycc)
    {
        ky_color rgb;
        ky_color::Type tmp = ycc.y - (ycc.cg / ky_color::Type(2));
        rgb.g = ycc.cg + tmp;
        rgb.b = tmp - (ycc.co / ky_color::Type(2));
        rgb.r = rgb.b + ycc.co;
        rgb.a = ycc.a;
        return rgb;
    }
    static ky_color rgb_to_ycocgr(const ky_color &rgb)
    {
        ky_color result;
        result.y  = rgb.g / ky_color::Type(2) + (rgb.r + rgb.b) / ky_color::Type(4);
        result.co = rgb.r - rgb.b;
        result.cg = rgb.g - (rgb.r + rgb.b) / ky_color::Type(2);
        return result;
    }
};
class IntColor
{
public:
    //! HSV格式
    //! \brief rgb_to_hsv
    //! \param red
    //! \param green
    //! \param blue
    //! \return
    //!
    static ky_color rgb_to_hsv (int red, int green, int blue)
    {
#define ROUND(x) ((int) ((x) + 0.5))
        ky_color hsv;
        ky_color::Type  r, g, b;
        ky_color::Type  h, s, v;
        int     mins;
        ky_color::Type  delta;

        r = red; g = green; b = blue;

        if (r > g)
        {
            v = std::max (r, b);
            mins = std::min (g, b);
        }
        else
        {
            v = std::max (g, b);
            mins = std::min (r, b);
        }

        delta = v - mins;

        if (v == 0.0)
            s = 0.0;
        else
            s = delta / v;

        if (s == 0.0)
        {
            h = 0.0;
        }
        else
        {
            if (r == v)
                h = 60.0 * (g - b) / delta;
            else if (g == v)
                h = 120 + 60.0 * (b - r) / delta;
            else
                h = 240 + 60.0 * (r - g) / delta;

            if (h < 0.0)
                h += 360.0;

            if (h > 360.0)
                h -= 360.0;
        }

        hsv.h   = ROUND (h);
        hsv.s = ROUND (s * 255.0);
        hsv.v  = ROUND (v);

        if (hsv.h == 360)
            hsv.h = 0;
        #undef ROUND
        return hsv;
    }
    static ky_color hsv_to_rgb (int hue, int saturation, int value)
    {
        ky_color rgb;
#define ROUND(x) ((int) ((x) + 0.5))
        ky_color::Type h, s, v, h_temp;
        ky_color::Type f, p, q, t;
        int i;

        if (saturation == 0)
        {
            hue        = value;
            saturation = value;
            value      = value;
        }
        else
        {
            h = hue;
            s = saturation / 255.0;
            v = value      / 255.0;

            if (h == 360)
                h_temp = 0;
            else
                h_temp = h;

            h_temp = h_temp / 60.0;
            i = ky_ifloor (h_temp);
            f = h_temp - i;
            p = v * (1.0 - s);
            q = v * (1.0 - (s * f));
            t = v * (1.0 - (s * (1.0 - f)));

            switch (i)
            {
            case 0:
                rgb.r = ROUND (v * 255.0);
                rgb.g = ROUND (t * 255.0);
                rgb.b = ROUND (p * 255.0);
                break;

            case 1:
                rgb.r = ROUND (q * 255.0);
                rgb.g = ROUND (v * 255.0);
                rgb.b = ROUND (p * 255.0);
                break;

            case 2:
                rgb.r = ROUND (p * 255.0);
                rgb.g = ROUND (v * 255.0);
                rgb.b = ROUND (t * 255.0);
                break;

            case 3:
                rgb.r = ROUND (p * 255.0);
                rgb.g = ROUND (q * 255.0);
                rgb.b = ROUND (v * 255.0);
                break;

            case 4:
                rgb.r = ROUND (t * 255.0);
                rgb.g = ROUND (p * 255.0);
                rgb.b = ROUND (v * 255.0);
                break;

            case 5:
                rgb.r = ROUND (v * 255.0);
                rgb.g = ROUND (p * 255.0);
                rgb.b = ROUND (q * 255.0);
                break;
            }
        }
#undef ROUND
        return rgb;
    }

    //! HSL格式
    static ky_color rgb_to_hsl (int red, int green, int blue)
    {
        ky_color hsl;
#define ROUND(x) ((int) ((x) + 0.5))
        int    r, g, b;
        ky_color::Type h, s, l;
        int    mins, maxs;
        int    delta;

        r = red; g = green; b = blue;

        if (r > g)
        {
            maxs = std::max (r, b);
            mins = std::min (g, b);
        }
        else
        {
            maxs = std::max (g, b);
            mins = std::min (r, b);
        }

        l = (maxs + mins) / 2.0;

        if (maxs == mins)
        {
            s = 0.0;
            h = 0.0;
        }
        else
        {
            delta = (maxs - mins);

            if (l < 128)
                s = 255 * (ky_color::Type) delta / (ky_color::Type) (maxs + mins);
            else
                s = 255 * (ky_color::Type) delta / (ky_color::Type) (511 - maxs - mins);

            if (r == maxs)
                h = (g - b) / (ky_color::Type) delta;
            else if (g == maxs)
                h = 2 + (b - r) / (ky_color::Type) delta;
            else
                h = 4 + (r - g) / (ky_color::Type) delta;

            h = h * 42.5;

            if (h < 0)
                h += 255;
            else if (h > 255)
                h -= 255;
        }

        hsl.h   = ROUND (h);
        hsl.s = ROUND (s);
        hsl.l  = ROUND (l);
#undef ROUND
        return hsl;
    }
    static ky_color hsl_to_rgb (int hue, int saturation, int lightness)
    {
        struct CLocal
        {
            static int hsl_value (ky_color::Type n1, ky_color::Type n2, ky_color::Type hue)
            {
#define ROUND(x) ((int) ((x) + 0.5))
              ky_color::Type value;

              if (hue > 255)
                hue -= 255;
              else if (hue < 0)
                hue += 255;

              if (hue < 42.5)
                value = n1 + (n2 - n1) * (hue / 42.5);
              else if (hue < 127.5)
                value = n2;
              else if (hue < 170)
                value = n1 + (n2 - n1) * ((170 - hue) / 42.5);
              else
                value = n1;

              return ROUND (value * 255.0);
#undef ROUND
            }
        };
        ky_color rgb;
        ky_color::Type h, s, l;

        h = hue; s = saturation; l = lightness;

        if (s == 0)
        {
            rgb.r = l;
            rgb.g = l;
            rgb.b = l;
        }
        else
        {
            ky_color::Type m1, m2;

            if (l < 128)
                m2 = (l * (255 + s)) / 65025.0;
            else
                m2 = (l + s - (l * s) / 255.0) / 255.0;

            m1 = (l / 127.5) - m2;

            rgb.r = CLocal::hsl_value (m1, m2, h + 85);
            rgb.g = CLocal::hsl_value (m1, m2, h);
            rgb.b = CLocal::hsl_value (m1, m2, h - 85);
        }
        return rgb;
    }

    //! CMYK格式
    //! \brief rgb_to_cmyk
    //! \param red
    //! \param green
    //! \param blue
    //! \param pullout (0-100)
    //! \return
    //!
    static ky_color rgb_to_cmyk (int red, int green, int blue, int pullout)
    {
        ky_color cmyk;
        int c, m, y;

        c = 255 - red; m = 255 - green; y = 255 - blue;

        if (pullout == 0)
        {
            red   = c;
            green = m;
            blue  = y;
        }
        else
        {
            int k = 255;

            if (c < k)  k = c;
            if (m < k)  k = m;
            if (y < k)  k = y;

            k = (k * ky_clamp(pullout, 0, 100)) / 100;

            cmyk.c   = ((c - k) << 8) / (256 - k);
            cmyk.m = ((m - k) << 8) / (256 - k);
            cmyk.y  = ((y - k) << 8) / (256 - k);
            cmyk.k = k;
        }
        return cmyk;
    }
    static ky_color cmyk_to_rgb (int cyan, int magenta, int yellow, int black)
    {
        ky_color rgb;
        int c, m, y, k;

        c = cyan; m = magenta; y = yellow; k = black;

        if (k)
        {
            c = ((c * (256 - k)) >> 8) + k;
            m = ((m * (256 - k)) >> 8) + k;
            y = ((y * (256 - k)) >> 8) + k;
        }

        rgb.r = 255 - c;
        rgb.g = 255 - m;
        rgb.b = 255 - y;
        return rgb;
    }

    //! HEX格式
    static ky_color hex_to_rgba(ky_color32 hex)
    {
        ky_color rgba;
        rgba.r = ((hex >> 24) & 0xff);
        rgba.g = ((hex >> 16) & 0xff) ;
        rgba.b = ((hex >> 8) & 0xff);
        rgba.a = (hex & 0xff) ;
        return rgba;
    }
    static ky_color32 rgba_to_hex(u8 R, u8 G, u8 B, u8 A)
    {
        return (((ky_color32)ky_clamp(R, 0, 255))<<24) |
                (((ky_color32)ky_clamp(G, 0, 255))<<16) |
                (((ky_color32)ky_clamp(B, 0, 255))<<8) |
                ((ky_color32)ky_clamp(A, 0, 255));
    }
};

//!双线性插值。
class BilinearColor
{
public:
    ky_color::Type bilinear (ky_color::Type  x, ky_color::Type  y, ky_color::Type *values)
    {
      ky_color::Type m0, m1;

      x = ky_fmod (x, 1.0);
      y = ky_fmod (y, 1.0);

      if (x < 0.0)
        x += 1.0;
      if (y < 0.0)
        y += 1.0;

      m0 = (1.0 - x) * values[0] + x * values[1];
      m1 = (1.0 - x) * values[2] + x * values[3];

      return (1.0 - y) * m0 + y * m1;
    }
    u8 bilinear_8 (ky_color::Type x, ky_color::Type y, u8 *values)
    {
      ky_color::Type m0, m1;

      x = ky_fmod (x, 1.0);
      y = ky_fmod (y, 1.0);

      if (x < 0.0)
        x += 1.0;
      if (y < 0.0)
        y += 1.0;

      m0 = (1.0 - x) * values[0] + x * values[1];
      m1 = (1.0 - x) * values[2] + x * values[3];

      return (u8) ((1.0 - y) * m0 + y * m1);
    }
    u16 bilinear_16 (ky_color::Type  x, ky_color::Type  y, u16 *values)
    {
      f64 m0, m1;

      x = ky_fmod (x, 1.0);
      y = ky_fmod (y, 1.0);

      if (x < 0.0)
        x += 1.0;
      if (y < 0.0)
        y += 1.0;

      m0 = (1.0 - x) * values[0] + x * values[1];
      m1 = (1.0 - x) * values[2] + x * values[3];

      return (u16) ((1.0 - y) * m0 + y * m1);
    }
    u32 bilinear_32 (ky_color::Type  x, ky_color::Type  y, u32 *values)
    {
      f64 m0, m1;

      x = ky_fmod (x, 1.0);
      y = ky_fmod (y, 1.0);

      if (x < 0.0)
        x += 1.0;
      if (y < 0.0)
        y += 1.0;

      m0 = (1.0 - x) * values[0] + x * values[1];
      m1 = (1.0 - x) * values[2] + x * values[3];

      return (u32) ((1.0 - y) * m0 + y * m1);
    }
    ky_color bilinear_rgb (ky_color::Type  x, ky_color::Type  y, ky_color *values)
    {
      ky_color::Type m0, m1;
      ky_color::Type ix, iy;
      ky_color v(0.0f);

      x = ky_fmod(x, 1.0);
      y = ky_fmod(y, 1.0);

      if (x < 0)
        x += 1.0;
      if (y < 0)
        y += 1.0;

      ix = 1.0 - x;
      iy = 1.0 - y;

      /* Red */

      m0 = ix * values[0].r + x * values[1].r;
      m1 = ix * values[2].r + x * values[3].r;

      v.r = iy * m0 + y * m1;

      /* Green */

      m0 = ix * values[0].g + x * values[1].g;
      m1 = ix * values[2].g + x * values[3].g;

      v.g = iy * m0 + y * m1;

      /* Blue */

      m0 = ix * values[0].b + x * values[1].b;
      m1 = ix * values[2].b + x * values[3].b;

      v.b = iy * m0 + y * m1;

      return v;
    }
    ky_color bilinear_rgba (ky_color::Type  x, ky_color::Type  y, ky_color *values)
    {
      ky_color::Type m0, m1;
      ky_color::Type ix, iy;
      ky_color::Type a0, a1, a2, a3, alpha;
      ky_color v(0.0f);

      x = ky_fmod (x, 1.0);
      y = ky_fmod (y, 1.0);

      if (x < 0)
        x += 1.0;
      if (y < 0)
        y += 1.0;

      ix = 1.0 - x;
      iy = 1.0 - y;

      a0 = values[0].a;
      a1 = values[1].a;
      a2 = values[2].a;
      a3 = values[3].a;

      /* Alpha */

      m0 = ix * a0 + x * a1;
      m1 = ix * a2 + x * a3;

      alpha = v.a = iy * m0 + y * m1;

      if (alpha > 0)
        {
          /* Red */

          m0 = ix * a0 * values[0].r + x * a1 * values[1].r;
          m1 = ix * a2 * values[2].r + x * a3 * values[3].r;

          v.r = (iy * m0 + y * m1)/alpha;

          /* Green */

          m0 = ix * a0 * values[0].g + x * a1 * values[1].g;
          m1 = ix * a2 * values[2].g + x * a3 * values[3].g;

          v.g = (iy * m0 + y * m1)/alpha;

          /* Blue */

          m0 = ix * a0 * values[0].b + x * a1 * values[1].b;
          m1 = ix * a2 * values[2].b + x * a3 * values[3].b;

          v.b = (iy * m0 + y * m1)/alpha;
        }

      return v;
    }

    /**
     * bilinear_pixels_8:
     * @dest: Pixel, where interpolation result is to be stored.
     * @x: x-coordinate (0.0 to 1.0).
     * @y: y-coordinate (0.0 to 1.0).
     * @bpp: Bytes per pixel.  @dest and each @values item is an array of
     *    @bpp bytes.
     * @has_alpha: %TRUE if the last channel is an alpha channel.
     * @values: Array of four pointers to pixels.
     *
     * Computes bilinear interpolation of four pixels.
     *
     * When @has_alpha is %FALSE, it's identical to bilinear_8() on
     * each channel separately.  When @has_alpha is %TRUE, it handles
     * alpha channel correctly.
     *
     * The pixels in @values correspond to corner x, y coordinates in the
     * following order: [0,0], [1,0], [0,1], [1,1].
     **/
    void bilinear_pixels_8 (u8    *dest,
                            ky_color::Type    x,
                            ky_color::Type    y,
                            uint      bpp,
                            bool      has_alpha,
                            u8    **values)
    {
        uint i;

        x = ky_fmod (x, 1.0);
        y = ky_fmod (y, 1.0);

        if (x < 0.0)
            x += 1.0;
        if (y < 0.0)
            y += 1.0;

        if (has_alpha)
        {
            uint   ai     = bpp - 1;
            ky_color::Type alpha0 = values[0][ai];
            ky_color::Type alpha1 = values[1][ai];
            ky_color::Type alpha2 = values[2][ai];
            ky_color::Type alpha3 = values[3][ai];
            ky_color::Type alpha  = ((1.0 - y) * ((1.0 - x) * alpha0 + x * alpha1)
                                     + y * ((1.0 - x) * alpha2 + x * alpha3));

            dest[ai] = (u8) alpha;
            if (dest[ai])
            {
                for (i = 0; i < ai; i++)
                {
                    ky_color::Type m0 = ((1.0 - x) * values[0][i] * alpha0
                            + x * values[1][i] * alpha1);
                    ky_color::Type m1 = ((1.0 - x) * values[2][i] * alpha2
                            + x * values[3][i] * alpha3);

                    dest[i] = (u8) (((1.0 - y) * m0 + y * m1) / alpha);
                }
            }
        }
        else
        {
            for (i = 0; i < bpp; i++)
            {
                ky_color::Type m0 = (1.0 - x) * values[0][i] + x * values[1][i];
                ky_color::Type m1 = (1.0 - x) * values[2][i] + x * values[3][i];

                dest[i] = (u8) ((1.0 - y) * m0 + y * m1);
            }
        }
    }

};


////////////////////////////////////////////////////
ky_color::ky_color()
{
    for (int i = 0; i < size; ++i)
        elem[i] = (Type)0;
}

ky_color::ky_color(f64 init)
{
    for (int i = 0; i < size; ++i)
        elem[i] = ky_clamp(init, 0., 1.);
}
ky_color::ky_color(const f64 *init)
{
    for (int i = 0; i < size; ++i)
        elem[i] = ky_clamp(init[i], 0., 1.);
}

ky_color::ky_color(Type init)
{
    for (int i = 0; i < size; ++i)
        elem[i] = ky_clamp(init, (Type)0., (Type)1.);
}
ky_color::ky_color(const Type* init)
{
    for (int i = 0; i < size; ++i)
        elem[i] = ky_clamp(init[i], (Type)0., (Type)1.);
}

ky_color::ky_color(u8 init)
{
    for (int i = 0; i < size; ++i)
        elem[i] = ky_clamp((Type)init * RgbComponent, (Type)0, (Type)1);
}
ky_color::ky_color(const u8 *init)
{
    for (int i = 0; i < size; ++i)
        elem[i] = ky_clamp((Type)init[i] * RgbComponent, (Type)0., (Type)1.);
}
ky_color::ky_color(const i32 *init)
{
    for (int i = 0; i < size; ++i)
        elem[i] = (Type)(ky_clamp(init[i], 0, 255)) * RgbComponent;
}

ky_color::ky_color(f64 t1, f64 t2, f64 t3, f64 t4)
{
    elem[0] = ky_clamp(t1, 0., 1.);
    elem[1] = ky_clamp(t2, 0., 1.);
    elem[2] = ky_clamp(t3, 0., 1.);
    elem[3] = ky_clamp(t4, 0., 1.);
}
ky_color::ky_color(Type t1, Type t2, Type t3, Type t4)
{
    elem[0] = ky_clamp(t1, (Type)0., (Type)1.);
    elem[1] = ky_clamp(t2, (Type)0., (Type)1.);
    elem[2] = ky_clamp(t3, (Type)0., (Type)1.);
    elem[3] = ky_clamp(t4, (Type)0., (Type)1.);
}

ky_color::ky_color(u8 t1, u8 t2, u8 t3, u8 t4)
{
    elem[0] = ky_clamp((Type)t1 * RgbComponent, (Type)0., (Type)1.);
    elem[1] = ky_clamp((Type)t2 * RgbComponent, (Type)0., (Type)1.);
    elem[2] = ky_clamp((Type)t3 * RgbComponent, (Type)0., (Type)1.);
    elem[3] = ky_clamp((Type)t4 * RgbComponent, (Type)0., (Type)1.);
}
ky_color::ky_color(i32 t1, i32 t2, i32 t3, i32 t4)
{
    elem[0] = (Type)(ky_clamp(t1, 0, 255)) * RgbComponent;
    elem[1] = (Type)(ky_clamp(t2, 0, 255)) * RgbComponent;
    elem[2] = (Type)(ky_clamp(t3, 0, 255)) * RgbComponent;
    elem[3] = (Type)(ky_clamp(t4, 0, 255)) * RgbComponent;
}

ky_color::ky_color(const ky_color & v){
    for (int i = 0; i < v.size; ++i)
        elem[i] = v.elem[i];
}

ky_color &ky_color::operator = (const ky_color &v)
{
    for (int i = 0; i < v.size; ++i)
        elem[i] = v.elem[i];
    return *this;
}

const ky_color::Type& ky_color::operator [] (int index) const
{
    kyASSERT(index >= size);
    return elem[index];
}
ky_color::Type & ky_color::operator [] (int index)
{
    kyASSERT(index >= size);
    return elem[index];
}

ky_color::ky_color(ky_color32 hex)
{
    *this = IntColor::hex_to_rgba(hex);
}
ky_color32 ky_color::to_hex()const
{
    return FloatColor::rgba_to_hex(r,g,b,a);
}

void ky_color::form_hex(ky_color32 hex)
{
    *this = IntColor::hex_to_rgba(hex);
}
void ky_color::form_hsl (const ky_color &hsl)
{
    *this = FloatColor::hsl_to_rgb(hsl);
}
void ky_color::form_hsv (const ky_color &hsv)
{
    *this = FloatColor::hsv_to_rgb(hsv);
}
void ky_color::form_hsi (const ky_color &hsi)
{
    *this = FloatColor::hsi_to_rgb(hsi);
}
void ky_color::form_hwb (const ky_color &hwb)
{
    *this = FloatColor::hwb_to_rgb(hwb);
}
void ky_color::form_cmyk (const ky_color &cmyk)
{
    *this = FloatColor::cmyk_to_rgb(cmyk);
}
void ky_color::form_lab (const ky_color &lab)
{
    *this = FloatColor::lab_to_rgb (lab);
}
void ky_color::form_xyz (const ky_color &xyz)
{
    *this = FloatColor::xyz_to_rgb (xyz);
}

void ky_color::form_ycbcr(const ky_color &yuv)
{
    *this = FloatColor::ycbcr_to_rgb_bt601(yuv);
}
void ky_color::form_ycocg(const ky_color &ycc)
{
    *this = FloatColor::ycocg_to_rgb(ycc);
}
void ky_color::form_ycocgr(const ky_color &ycc)
{
    *this = FloatColor::ycocgr_to_rgb(ycc);
}

ky_color ky_color::to_hsl()const
{
    return FloatColor::rgb_to_hsl(*this);
}
ky_color ky_color::to_hsv()const
{
    return FloatColor::rgb_to_hsv(*this);
}
ky_color ky_color::to_hsi()const
{
   return FloatColor::rgb_to_hsi(*this);
}
ky_color ky_color::to_hwb()const
{
    return FloatColor::rgb_to_hwb(*this);
}
ky_color ky_color::to_cmyk()const
{
    return FloatColor::rgb_to_cmyk(*this, 0);
}

ky_color ky_color::to_lab()const
{
    return FloatColor::rgb_to_lab (*this);
}
ky_color ky_color::to_xyz()const
{
    return FloatColor::rgb_to_xyz (*this);
}

ky_color ky_color::to_ycbcr()const
{
    return FloatColor::rgb_to_ycbcr_bt601(*this);
}
ky_color ky_color::to_ycocg()const
{
    return FloatColor::rgb_to_ycocg(*this);
}
ky_color ky_color::to_ycocgr()const
{
    return FloatColor::rgb_to_ycocgr(*this);
}

ky_color ky_color::interpolate (const ky_color &initial, const ky_color &final,
                                Type pos)
{
    ky_color out;
    out.r = initial.r + (final.r - initial.r) * pos;
    out.g = initial.g + (final.g - initial.g) * pos;
    out.b = initial.b + (final.b - initial.b) * pos;
    out.a = initial.a + (final.a - initial.a) * pos;
    return out;
}
ky_color ky_color::blend(const ky_color &_src ,const ky_color &_dst, Type _S)
{
    ky_color out;
    Type t = 1.0f - _S;
    out.a = (t*_src.a +_S*_dst.a );
    out.r = (t*_src.r +_S*_dst.r );
    out.g = (t*_src.g +_S*_dst.g );
    out.b = (t*_src.b +_S*_dst.b );
    return out;
}

//!
//! \brief rgb_gamma 伽马校正
//! \param rgb
//! \param gamma
//! \return
//!
ky_color ky_color::rgb_gamma (ky_color &rgb, Type  gamma)
{
  Type ig = 0.0;

  if (!kyEmptyf(gamma))
    ig = 1.0 / gamma;

  rgb.r = ky_pow (rgb.r, ig);
  rgb.g = ky_pow (rgb.g, ig);
  rgb.b = ky_pow (rgb.b, ig);
  return rgb;
}
//!
//! \brief rgb_luminance 亮度值
//! \param rgb
//! \return
//!
ky_color::Type ky_color::rgb_luminance (const ky_color &rgb)
{
#define RGB_LUMINANCE(r,g,b)((r) *0.2126 + (g) *0.7152 + (b) *0.0722)
  Type luminance = RGB_LUMINANCE (rgb.r, rgb.g, rgb.b);
#undef RGB_LUMINANCE
  return ky_clamp(luminance, (Type)0.0, (Type)1.0);
}
//!
//! \brief rgb_intensity 强度值
//! \param rgb
//! \return
//!
ky_color::Type ky_color::rgb_intensity (const ky_color &rgb)
{
#define RGB_INTENSITY(r,g,b) ((r) *0.30 + (g) *0.59 + (b) *0.11)
  Type intensity = RGB_INTENSITY (rgb.r, rgb.g, rgb.b);
#undef RGB_INTENSITY
  return ky_clamp (intensity, (Type)0.0, (Type)1.0);
}

ky_color ky_color::rgb_composite (ky_color &color1, const ky_color &color2,
                                  eRGBCompositeModes  mode)
{
    switch (mode)
    {
    case RGB_CompositeNone:
        break;

    case RGB_CompositeNormal:
        /*  put color2 on top of color1  */
        if (kyEqualf(color2.a, 1.0))
            color1 = color2;
        else
        {
            Type factor = color1.a * (1.0 - color2.a);

            color1.r = color1.r * factor + color2.r * color2.a;
            color1.g = color1.g * factor + color2.g * color2.a;
            color1.b = color1.b * factor + color2.b * color2.a;
            color1.a = factor + color2.a;
        }
        break;

    case RGB_CompositeBehind:
        /*  put color2 below color1  */
        if (color1.a < 1.0)
        {
            Type factor = color2.a * (1.0 - color1.a);

            color1.r = color2.r * factor + color1.r * color1.a;
            color1.g = color2.g * factor + color1.g * color1.a;
            color1.b = color2.b * factor + color1.b * color1.a;
            color1.a = factor + color1.a;
        }
        break;
    }
    return color1;
}
ky_color ky_color::rgba_add (ky_color &rgba1, const ky_color &rgba2)
{
    rgba1.r += rgba2.r;
    rgba1.g += rgba2.g;
    rgba1.b += rgba2.b;
    rgba1.a += rgba2.a;
    return rgba1;
}
ky_color ky_color::rgba_subtract (ky_color &rgba1, const ky_color &rgba2)
{
    rgba1.r -= rgba2.r;
    rgba1.g -= rgba2.g;
    rgba1.b -= rgba2.b;
    rgba1.a -= rgba2.a;
    return rgba1;
}
ky_color ky_color::rgba_multiply (ky_color &rgba ,Type factor)
{
  rgba.r *= factor;
  rgba.g *= factor;
  rgba.b *= factor;
  rgba.a *= factor;
  return rgba;
}
ky_color::Type ky_color::rgba_distance (const ky_color &rgba1, const ky_color &rgba2)
{
  return (ky_fabs (rgba1.r - rgba2.r) +
          ky_fabs (rgba1.g - rgba2.g) +
          ky_fabs (rgba1.b - rgba2.b) +
          ky_fabs (rgba1.a - rgba2.a));
}

ky_stream &operator << (ky_stream &in, const ky_color &v)
{
    for (int i = 0;i < v.size; ++i)
        in << v.elem[i];
    return in;
}
ky_stream &operator >> (ky_stream &out, ky_color &v)
{
    for (int i = 0;i < v.size; ++i)
        out >> v.elem[i];
    return out;
}

ky_variant &operator << (ky_variant &va, const ky_color &col)
{
    va.set(kyColorCode, 0, (u8*)&col, sizeof(ky_color));
    return va;
}
ky_variant &operator >> (ky_variant &va, ky_color &col)
{
    col = va.get<ky_color>(kyColorCode);
    return va;
}
