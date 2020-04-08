#include "tools/ky_stream.h"
#include "interface/iio.h"
#include "tools/ky_array.h"
#include "tools/ky_string.h"
#include "ky_number_double.h"
#include "ky_number_longlong.h"

ky_stream::ky_stream (const iio *io, eIoModes mode):
    _mode(mode),
    _io(io)
{

}

ky_stream::ky_stream(ky_stream &rhs):
    _mode(rhs._mode),
    _io(rhs._io)
{

}

ky_stream::~ky_stream()
{

}

i64 ky_stream::size()const
{

}

ky_stream & ky_stream::operator << (ky_stream &, bool)
{

}
ky_stream & ky_stream::operator << (ky_stream &, bool_t)
{

}
ky_stream & ky_stream::operator << (ky_stream &, i8)
{

}
ky_stream & ky_stream::operator << (ky_stream &, u8)
{

}
ky_stream & ky_stream::operator << (ky_stream &, i16)
{

}
ky_stream & ky_stream::operator << (ky_stream &, u16)
{

}
ky_stream & ky_stream::operator << (ky_stream &, i32)
{

}
ky_stream & ky_stream::operator << (ky_stream &, u32)
{

}
ky_stream & ky_stream::operator << (ky_stream &, i64)
{

}
ky_stream & ky_stream::operator << (ky_stream &, u64)
{

}

ky_stream & ky_stream::operator << (ky_stream &, f16)
{

}
ky_stream & ky_stream::operator << (ky_stream &, f32)
{

}
ky_stream & ky_stream::operator << (ky_stream &, f64)
{

}

ky_stream & ky_stream::operator >> (ky_stream &, bool & )
{

}
ky_stream & ky_stream::operator >> (ky_stream &, bool_t & )
{

}
ky_stream & ky_stream::operator >> (ky_stream &, i8 & )
{

}
ky_stream & ky_stream::operator >> (ky_stream &, u8 & )
{

}
ky_stream & ky_stream::operator >> (ky_stream &, i16 & )
{

}
ky_stream & ky_stream::operator >> (ky_stream &, u16 & )
{

}
ky_stream & ky_stream::operator >> (ky_stream &, i32 & )
{

}
ky_stream & ky_stream::operator >> (ky_stream &, u32 & )
{

}
ky_stream & ky_stream::operator >> (ky_stream &, i64 & )
{

}
ky_stream & ky_stream::operator >> (ky_stream &, u64 & )
{

}

ky_stream & ky_stream::operator >> (ky_stream &, f16 & )
{

}
ky_stream & ky_stream::operator >> (ky_stream &, f32 & )
{

}
ky_stream & ky_stream::operator >> (ky_stream &, f64 & )
{

}


ky_streamb::ky_streamb(const ky_byte &ay):
    ky_stream<tStreamBinarys>(ay)
{
}
ky_streamb::ky_streamb(ky_byte *ay):
    ky_stream<tStreamBinarys>(ay)
{
}

ky_streamt::ky_streamt(const ky_byte &v):
    ky_stream<tStreamTexts>(v)
{
}
ky_streamt::ky_streamt(ky_byte *v):
    ky_stream<tStreamTexts>(v)
{
}

ky_streamb & operator << (ky_streamb &in, bool v)
{
    in.put((i8*)&v, sizeof(v));
    return in;
}
ky_streamb & operator << (ky_streamb &in, bool_t v)
{
    in.put((i8*)&v, sizeof(bool));
    return in;
}
ky_streamb & operator << (ky_streamb &in, i8 v)
{
    in.put((i8*)&v, sizeof(v));
    return in;
}
ky_streamb & operator << (ky_streamb &in, ui8 v)
{
    in.put((i8*)&v, sizeof(v));
    return in;
}
ky_streamb & operator << (ky_streamb &in, short v)
{
    in.put((i8*)&v, sizeof(v));
    return in;
}
ky_streamb & operator << (ky_streamb &in, ushort v)
{
    in.put((i8*)&v, sizeof(v));
    return in;
}
ky_streamb & operator << (ky_streamb &in, int v)
{
    in.put((i8*)&v, sizeof(v));
    return in;
}
ky_streamb & operator << (ky_streamb &in, uint v)
{
    in.put((i8*)&v, sizeof(v));
    return in;
}
ky_streamb & operator << (ky_streamb &in, long v)
{
    in.put((i8*)&v, sizeof(v));
    return in;
}
ky_streamb & operator << (ky_streamb &in, ulong v)
{
    in.put((i8*)&v, sizeof(v));
    return in;
}
ky_streamb & operator << (ky_streamb &in, longlong v)
{
    in.put((i8*)&v, sizeof(v));
    return in;
}
ky_streamb & operator << (ky_streamb &in, ulonglong v)
{
    in.put((i8*)&v, sizeof(v));
    return in;
}
ky_streamb & operator << (ky_streamb &in, float v)
{
    in.put((i8*)&v, sizeof(v));
    return in;
}
ky_streamb & operator << (ky_streamb &in, half_float v)
{
    in.put((i8*)&v, sizeof(float));
    return in;
}
ky_streamb & operator << (ky_streamb &in, real v)
{
    in.put((i8*)&v, sizeof(v));
    return in;
}



ky_streamb & operator >> (ky_streamb &out, bool & v)
{
    int len = sizeof(v);
    v = *(bool*)out.take(&len);
    return out;
}
ky_streamb & operator >> (ky_streamb &out, bool_t & v)
{
    int len = sizeof(bool);
    bool vt = *(bool*)out.take(&len);
    v = vt;
    return out;
}
ky_streamb & operator >> (ky_streamb &out,  i8& v)
{
    int len = sizeof(v);
    v = *(i8*)out.take(&len);
    return out;
}
ky_streamb & operator >> (ky_streamb &out,  ui8& v)
{
    int len = sizeof(v);
    v = *(ui8*)out.take(&len);
    return out;
}
ky_streamb & operator >> (ky_streamb &out,  short& v)
{
    int len = sizeof(v);
    v = *(short*)out.take(&len);
    if (out.border != kyByteOrder)
        out.swap_endian(v);
    return out;
}
ky_streamb & operator >> (ky_streamb &out,  ushort& v)
{
    int len = sizeof(v);
    v = *(ushort*)out.take(&len);
    if (out.border != kyByteOrder)
        out.swap_endian(v);
    return out;
}
ky_streamb & operator >> (ky_streamb &out,  int& v)
{
    int len = sizeof(v);
    v = *(int*)out.take(&len);
    if (out.border != kyByteOrder)
        out.swap_endian(v);
    return out;
}
ky_streamb & operator >> (ky_streamb &out,  uint& v)
{
    int len = sizeof(v);
    v = *(uint*)out.take(&len);
    if (out.border != kyByteOrder)
        out.swap_endian(v);
    return out;
}
ky_streamb & operator >> (ky_streamb &out,  long& v)
{
    int len = sizeof(v);
    v = *(long*)out.take(&len);
    if (out.border != kyByteOrder)
        out.swap_endian(v);
    return out;
}
ky_streamb & operator >> (ky_streamb &out,  ulong& v)
{
    int len = sizeof(v);
    v = *(ulong*)out.take(&len);
    if (out.border != kyByteOrder)
        out.swap_endian(v);
    return out;
}
ky_streamb & operator >> (ky_streamb &out,  longlong& v)
{
    int len = sizeof(v);
    v = *(longlong*)out.take(&len);
    if (out.border != kyByteOrder)
        out.swap_endian(v);
    return out;
}
ky_streamb & operator >> (ky_streamb &out,  ulonglong& v)
{
    int len = sizeof(v);
    v = *(ulonglong*)out.take(&len);
    if (out.border != kyByteOrder)
        out.swap_endian(v);
    return out;
}
ky_streamb & operator >> (ky_streamb &out,  float& v)
{
    int len = sizeof(v);
    v = *(float*)out.take(&len);
    if (out.border != kyByteOrder)
        out.swap_endian(v);
    return out;
}
ky_streamb & operator >> (ky_streamb &out,  half_float& v)
{
    int len = sizeof(float);
    float vt = *(float*)out.take(&len);
    if (out.border != kyByteOrder)
        out.swap_endian(vt);
    v = vt;
    return out;
}
ky_streamb & operator >> (ky_streamb &out,  real& v)
{
    int len = sizeof(v);
    v = *(real*)out.take(&len);
    if (out.border != kyByteOrder)
        out.swap_endian(v);
    return out;
}

void ky_streamt::numput(longlong val, int bs)
{
    i8 *str = longlong_string(val, bs);
    put(str, strlen(str));
}
void ky_streamt::unumput(ulonglong val, int bs )
{
    i8 *str = ulonglong_string(val, bs);
    put(str, strlen(str));
}
void ky_streamt::fltput(real val, int prec)
{

    int decpt = 0, sign = 0, slen = 0;
    ky_array<i8> refs;
    i8 *dadd = NULL;
    double_string(val, 0, prec, &decpt, &sign, NULL, &dadd, &slen);

    refs.append(dadd, slen);
    kyFree (dadd);

    if (decpt != slen)
    {
        if (decpt != 0)
            refs.insert(decpt, '.');
        else
            refs.insert(0, "0.", 2);
    }

    if (sign != 0 )
        refs.insert(0, '-');
    put (refs.data(), strlen(refs.data()));
}

template<typename T>
T ky_streamt::unumtake(int bs)
{
    i8 *str = take();
    return toIntegral_helper<T>(str, strlen(str), bs);
}
template<typename T>
T ky_streamt::numtake(int bs )
{
    i8 *str = take();
    return toUIntegral_helper<T>(str, strlen(str), bs);
}

ky_streamt & operator << (ky_streamt&in, bool v)
{
    in.numput(v);
    return in;
}
ky_streamt & operator << (ky_streamt&in, bool_t v)
{
    in.numput((bool)v);
    return in;
}
ky_streamt & operator << (ky_streamt&in, i8 v)
{
    in.put(&v, 1);
    return in;
}
ky_streamt & operator << (ky_streamt&in, ui8 v)
{
    in.unumput(v);
    return in;
}
ky_streamt & operator << (ky_streamt&in, short v)
{
    in.numput(v);
    return in;
}
ky_streamt & operator << (ky_streamt&in, ushort v)
{
    in.unumput(v);
    return in;
}
ky_streamt & operator << (ky_streamt&in, int v)
{
    in.numput(v);
    return in;
}
ky_streamt & operator << (ky_streamt&in, uint v)
{
    in.unumput(v);
    return in;
}
ky_streamt & operator << (ky_streamt&in, long v)
{
    in.numput(v);
    return in;
}
ky_streamt & operator << (ky_streamt&in, ulong v)
{
    in.unumput(v);
    return in;
}
ky_streamt & operator << (ky_streamt&in, longlong v)
{
    in.numput(v);
    return in;
}
ky_streamt & operator << (ky_streamt&in, ulonglong v)
{
    in.unumput(v);
    return in;
}
ky_streamt & operator << (ky_streamt&in, float v)
{
    in.fltput(v);
    return in;
}
ky_streamt & operator << (ky_streamt&in, half_float v)
{
    in.fltput((float)v);
    return in;
}
ky_streamt & operator << (ky_streamt&in, real v)
{
    in.fltput(v);
    return in;
}

ky_streamt & operator << (ky_streamt&in, i8 *v)
{
    in.put(v, strlen(v));
    return in;
}
ky_streamt & operator << (ky_streamt&in, const i8 *v)
{
    in.put((i8*)v, strlen(v));
    return in;
}


ky_streamt & operator >> (ky_streamt &out, bool &v)
{
    v = out.numtake<bool>();
    return out;
}
ky_streamt & operator >> (ky_streamt &out, bool_t &v)
{
    v = out.numtake<bool>();
    return out;
}
ky_streamt & operator >> (ky_streamt &out, i8 &v)
{
    v = *out.take();
    return out;
}
ky_streamt & operator >> (ky_streamt &out, ui8 &v)
{
    v = out.unumtake<ui8>();
    return out;
}
ky_streamt & operator >> (ky_streamt &out, short &v)
{
    v = out.numtake<short>();
    return out;
}
ky_streamt & operator >> (ky_streamt &out, ushort &v)
{
    v = out.unumtake<ushort>();
    return out;
}
ky_streamt & operator >> (ky_streamt &out, int &v)
{
    v = out.numtake<int>();
    return out;
}
ky_streamt & operator >> (ky_streamt &out, uint &v)
{
    v = out.unumtake<uint>();
    return out;
}
ky_streamt & operator >> (ky_streamt &out, long &v)
{
    v = out.numtake<long>();
    return out;
}
ky_streamt & operator >> (ky_streamt &out, ulong &v)
{
    v = out.unumtake<ulong>();
    return out;
}
ky_streamt & operator >> (ky_streamt &out, longlong &v)
{
    v = out.numtake<longlong>();
    return out;
}
ky_streamt & operator >> (ky_streamt &out, ulonglong &v)
{
    v = out.unumtake<ulonglong>();
    return out;
}
ky_streamt & operator >> (ky_streamt &out, float &v)
{
    int len = 68;
    i8 *str = out.take(&len);
    v = string_double(str, NULL);
    return out;
}
ky_streamt & operator >> (ky_streamt &out, half_float & v)
{
    int len = 68;
    i8 *str = out.take(&len);
    v = string_double(str, NULL);
}
ky_streamt & operator >> (ky_streamt &out, real &v)
{
    int len = 68;
    i8 *str = out.take(&len);
    v = string_double(str, NULL);
    return out;
}


ky_streamb & operator << (ky_streamb &in, const ky_streamb &v)
{
    int64 vs = v.size();
    if (vs && v.impl)
    {
        in.put((i8*)&vs, sizeof(int64));
        i8 *tmpbuf = ((ky_streamb*)&v)->take((int*)&vs);
        in.put(tmpbuf, vs);
    }
    return in;
}
ky_streamb & operator >> (ky_streamb &out, ky_streamb &v)
{
    if (out.impl && out.size() && v.impl)
    {
        int len = sizeof(int64);
        int64 vs = *(int64*)out.take(&len);
        v.put((i8*)&vs, len);

        i8 *vd = (i8*)out.take((int*)&vs);
        v.put(vd, vs);
    }
    return out;
}
ky_streamb & operator << (ky_streamb &in, const ky_byte &v)
{
    int64 vs = v.size();
    in.put((i8*)&vs, sizeof(int64));
    in.put((i8*)v.data(), vs);
    return in;
}
ky_streamb & operator >> (ky_streamb &out, ky_byte &v)
{
    if (out.impl && out.size())
    {
        int len = sizeof(int64);
        len = *(int64*) out.take(&len);
        i8 *buf = (i8*)out.take(&len);
        v.append((ky_byte::Type*)buf, len);
    }
    return out;
}

ky_streamt & operator << (ky_streamt &in, const ky_streamt &v)
{
    int64 vs = v.size();
    if (vs && v.impl)
    {
        in.put((i8*)&vs, sizeof(int64));
        i8 *tmpbuf = ((ky_streamt*)&v)->take((int*)&vs);
        in.put(tmpbuf, vs);
    }
    return in;
}
ky_streamt & operator >> (ky_streamt &out, ky_streamt &v)
{
    if (out.impl && out.size() && v.impl)
    {
        int len = sizeof(int64);
        int64 vs = *(int64*)out.take(&len);
        v.put((i8*)&vs, len);

        i8 *vd = out.take((int*)&vs);
        v.put(vd, vs);
    }
    return out;
}
ky_streamt & operator << (ky_streamt &in, const ky_byte &v)
{
    int64 vs = v.size();
    in.put((i8*)&vs, sizeof(int64));
    in.put((i8*)v.data(), vs);
    return in;
}
ky_streamt & operator >> (ky_streamt &out, ky_byte &v)
{
    if (out.impl && out.size())
    {
        int len = sizeof(int64);
        len = *(int64*) out.take(&len);
        i8 *buf = (i8*)out.take(&len);
        v.append((ky_byte::Type*)buf, len);
    }
    return out;
}
