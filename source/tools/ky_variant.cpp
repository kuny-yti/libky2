#include "ky_variant.h"
#include "ky_string.h"
#include "ky_debug.h"
#include "ky_maths.h"

static ky_variant_math *s_int = 0;
static ky_variant_math *s_flt = 0;
static ky_variant_math *s_veci = 0;
static ky_variant_math *s_vecf = 0;
static ky_variant_math *s_str = 0;

ky_variant::~ky_variant()
{
    if (mem && mem->lessref())
    {
        kyFree(mem->data);
        kyDelete(mem);
    }
}
ky_variant::ky_variant():
    mem(0)
{

}
ky_variant::ky_variant(fourwd_int v):
    mem(0)
{
    set(kyIntCode, s_int, &v, sizeof(fourwd_int));
}
ky_variant::ky_variant(fourwd_flt v):
    mem(0)
{
    set(kyFltCode, s_flt, &v, sizeof(fourwd_flt));
}
ky_variant::ky_variant(const fourwd_str &v):
    mem(0)
{
    set(kyStrCode, s_str, v.data(), v.bytecount());
}

ky_variant::ky_variant(const fourwd_veci &v):
    mem(kyNew(layout))
{
    mem->code = kyVecIntCode;
    mem->math = s_veci;
    mem->byte = v.bytecount();
    mem->data = (uint8*)kyMalloc(mem->byte);

    ky_memory::copy(mem->data, v.data(), mem->byte);
}
ky_variant::ky_variant(const fourwd_vecf &v):
    mem(kyNew(layout))
{
    mem->code = kyVecFltCode;
    mem->math = s_vecf;
    mem->byte = v.bytecount();
    mem->data = (uint8*)kyMalloc(mem->byte);
    ky_memory::copy(mem->data, v.data(), mem->byte);
}

ky_variant::ky_variant(const fourwd_t code, struct ky_variant_math *math, void *data, int byte):
    mem(0)
{
    set(code, math, data, byte);
}
ky_variant::ky_variant(const ky_variant &pd):
    mem(0)
{
    mem = pd.mem;
    mem->addref();
}
ky_variant &ky_variant::operator = (const ky_variant &pd)
{
    if (mem && mem->lessref())
    {
        kyFree(mem->data);
        kyDelete(mem);
    }
    mem = pd.mem;
    mem->addref();
    return *this;
}

void ky_variant::set(const fourwd_t code, ky_variant_math *math, void *data, int len)
{
    if (mem && mem->lessref())
    {
        kyFree(mem->data);
        kyDelete(mem);
    }
    mem = kyNew(layout);

    mem->code = code;
    mem->math = math;
    mem->byte = len;
    mem->data = (uint8*)kyMalloc(mem->byte);
    ky_memory::copy(mem->data, data, mem->byte);
}

bool ky_variant::is_set(const fourwd_t code) const
{
    if (is_valid())
        return mem->code == code;
    return false;
}
bool ky_variant::is_valid()const
{
    return mem != 0;
}

fourwd_str ky_variant::dump()const
{
    if (mem && mem->math)
        return mem->math->dump(*this);
    return fourwd_str();
}

ky_variant::operator bool() const
{
    if (mem && mem->math)
        return mem->math->to_bool(*this);
    return false;
}
fourwd_str ky_variant::to_str()const
{
    if (!mem || kyStrCode != mem->code)
        return fourwd_str();

    return fourwd_str((char*)mem->data, mem->byte);
}
fourwd_veci ky_variant::to_veci()const
{
    if (!mem || kyVecIntCode != mem->code)
        return fourwd_veci();
    return fourwd_veci((fourwd_int*)mem->data, mem->byte);
}
fourwd_vecf ky_variant::to_vecf()const
{
    if (!mem || kyVecFltCode != mem->code)
        return fourwd_vecf();
    return fourwd_vecf((fourwd_flt*)mem->data, mem->byte);
}
// 关系运算
ky_variant operator==(const ky_variant &v, const ky_variant &v1)
{
    if (v.mem && v.mem->math)
        return v.mem->math->equal(v, v1);
    return ky_variant(bool(v.mem == v1.mem));
}
ky_variant operator!=(const ky_variant &v, const ky_variant &v1)
{
    if (v.mem && v.mem->math)
        return v.mem->math->not_equal(v, v1);
    return ky_variant(bool(v.mem != v1.mem));
}
ky_variant operator<(const ky_variant &v, const ky_variant &v1)
{
    if (v.mem && v.mem->math)
        return v.mem->math->less(v, v1);
    return ky_variant(false);
}
ky_variant operator<=(const ky_variant &v, const ky_variant &v1)
{
    if (v.mem && v.mem->math)
        return v.mem->math->less_equal(v, v1);
    return ky_variant(bool(v.mem == v1.mem));
}
ky_variant operator>=(const ky_variant &v, const ky_variant &v1)
{
    if (v.mem && v.mem->math)
        return v.mem->math->greater_equal(v, v1);
    return ky_variant(bool(v.mem == v1.mem));
}
ky_variant operator>(const ky_variant &v, const ky_variant &v1)
{
    if (v.mem && v.mem->math)
        return v.mem->math->greater(v, v1);
    return ky_variant(false);
}

// 算术运算
ky_variant operator+(const ky_variant &v, const ky_variant &v1)
{
    if (v.mem && v.mem->math)
        return v.mem->math->plus(v, v1);
    return ky_variant();
}
ky_variant operator-(const ky_variant &v, const ky_variant &v1)
{
    if (v.mem && v.mem->math)
        return v.mem->math->minus(v, v1);
    return ky_variant();
}
ky_variant operator*(const ky_variant &v, const ky_variant &v1)
{
    if (v.mem && v.mem->math)
        return v.mem->math->multiply(v, v1);
    return ky_variant();
}
ky_variant operator/(const ky_variant &v, const ky_variant &v1)
{
    if (v.mem && v.mem->math)
        return v.mem->math->division(v, v1);
    return ky_variant();
}
ky_variant operator%(const ky_variant &v, const ky_variant &v1)
{
    if (v.mem && v.mem->math)
        return v.mem->math->modulo(v, v1);
    return ky_variant();
}
ky_variant operator-(const ky_variant &v)
{
    if (v.mem && v.mem->math)
        return v.mem->math->invert(v);
    return v;
}

// 位运算
ky_variant operator&(const ky_variant &v, const ky_variant &v1)
{
    if (v.mem && v.mem->math)
        return v.mem->math->bit_and(v, v1);
    return ky_variant();
}
ky_variant operator|(const ky_variant &v, const ky_variant &v1)
{
    if (v.mem && v.mem->math)
        return v.mem->math->bit_or(v, v1);
    return ky_variant();
}
ky_variant operator^(const ky_variant &v, const ky_variant &v1)
{
    if (v.mem && v.mem->math)
        return v.mem->math->bit_xor(v, v1);
    return ky_variant();
}
ky_variant operator~(const ky_variant &v)
{
    if (v.mem && v.mem->math)
        return v.mem->math->bit_negate(v);
    return v;
}


// 逻辑运算
ky_variant operator!(const ky_variant &v)
{
    if (v.mem && v.mem->math)
        return v.mem->math->logic_not(v);
    return v;
}
ky_variant operator&&(const ky_variant &v, const ky_variant &v1)
{
    if (v.mem && v.mem->math)
        return v.mem->math->logic_and(v, v1);
    return ky_variant();
}
ky_variant operator||(const ky_variant &v, const ky_variant &v1)
{
    if (v.mem && v.mem->math)
        return v.mem->math->logic_or(v, v1);
    return ky_variant();
}

ky_variant operator<<(const ky_variant &v, const ky_variant &v1)
{
    if (v.mem && v.mem->math)
        return v.mem->math->shift_left(v, v1);
    return ky_variant();
}
ky_variant operator>>(const ky_variant &v, const ky_variant &v1)
{
    if (v.mem && v.mem->math)
        return v.mem->math->shift_right(v, v1);
    return ky_variant();
}

#define intMathLogic(Ope) \
    const fourwd_int tmp = (*(fourwd_int*)v1.mem->data);\
    if (!(v2.mem && v2.mem->math))\
        return ky_variant(tmp);\
\
    const int part = v2.mem->math->component(v2);\
    fourwd_int vi[part] = {0};\
    fourwd_flt vf[part] = {0};\
    int byte = part;\
    if (v2.mem->math->code() == kyIntCode)\
    {\
        byte *= sizeof(fourwd_int);\
        fourwd_int *elem = (fourwd_int*)v2.mem->data;\
        for (int i = 0; i < part; ++i, ++elem)\
            vi[i] = tmp Ope *elem;\
    }\
    else if (v2.mem->math->code() == kyFltCode)\
    {\
        byte *= sizeof(fourwd_flt);\
        fourwd_flt *elem = (fourwd_flt*)v2.mem->data;\
        for (int i = 0; i < part; ++i, ++elem)\
            vf[i] = tmp Ope *elem;\
    }\
    else\
        kyASSERT(true, "And unknown type operations");\
\
    if (v2.mem->code == kyIntCode)\
        return ky_variant(v2.mem->code, v2.mem->math, vi, byte);\
    return ky_variant(v2.mem->code, v2.mem->math, vf, byte);

#define intMathBit(Ope) \
    const fourwd_int tmp = (*(fourwd_int*)v1.mem->data);\
    if (!(v2.mem && v2.mem->math))\
        return ky_variant(tmp);\
\
    const int part = v2.mem->math->component(v2);\
    fourwd_int vi[part] = {0};\
    fourwd_flt vf[part] = {0};\
    int byte = part;\
    if (v2.mem->math->code() == kyIntCode)\
    {\
        byte *= sizeof(fourwd_int);\
        fourwd_int *elem = (fourwd_int*)v2.mem->data;\
        for (int i = 0; i < part; ++i, ++elem)\
            vi[i] = tmp Ope *elem;\
    }\
    else if (v2.mem->math->code() == kyFltCode)\
    {\
        kyASSERT(true, "Float type cannot be bitwise operation");\
    }\
    else\
        kyASSERT(true, "And unknown type operations");\
\
    return ky_variant(v2.mem->code, v2.mem->math, vi, byte);

#define intMathComp(Ope) \
    const fourwd_int tmp = (*(fourwd_int*)v1.mem->data);\
    if (!(v2.mem && v2.mem->math))\
        return ky_variant(false);\
    \
    const int part = v2.mem->math->component(v2);\
    bool comp = 0;\
    if (v2.mem->math->code() == kyIntCode)\
    {\
        fourwd_int suto = 0;\
        fourwd_int *elem = (fourwd_int*)v2.mem->data;\
        for (int i = 0; i < part; ++i, ++elem)\
            suto += *elem;\
        comp = (tmp - suto) Ope 0 ? true : false;\
    }\
    else if (v2.mem->math->code() == kyFltCode)\
    {\
         fourwd_flt suto = 0;\
         fourwd_flt *elem = (fourwd_flt*)v2.mem->data;\
         for (int i = 0; i < part; ++i, ++elem)\
             suto += *elem;\
        comp = (tmp - suto) Ope 0 ? true : false;\
    }\
    else\
        kyASSERT(true, "And unknown type operations");\
    return ky_variant(comp);

static struct math_int : ky_variant_math
{
    math_int()
    {
        s_int = this;
    }
    virtual fourwd_str dump(const ky_variant &v)
    {
        ky_string pm("int[");
        pm += ky_string::number(*(fourwd_int*)v.mem->data);
        pm += "]";
        return fourwd_str(pm.to_latin1(), pm.count());
    }
    virtual bool to_bool(const ky_variant &v)
    {
        return *(fourwd_int*)v.mem->data;
    }
    virtual int component(const ky_variant &) {return 1;}
    virtual fourwd_t code() {return kyIntCode;}

    // 逻辑运算
    virtual ky_variant logic_not  (const ky_variant &v)
    {
        const fourwd_int tmp = !(*(fourwd_int*)v.mem->data);
        return ky_variant(tmp);
    }
    virtual ky_variant logic_and  (const ky_variant &v1, const ky_variant &v2)
    {
        intMathLogic(&&)
    }
    virtual ky_variant logic_or  (const ky_variant &v1, const ky_variant &v2)
    {
        intMathLogic(||)
    }

    // 位运算
    virtual ky_variant bit_and  (const ky_variant &v1, const ky_variant &v2)
    {
        intMathBit(&)
    }
    virtual ky_variant bit_or  (const ky_variant &v1, const ky_variant &v2)
    {
        intMathBit(|)
    }
    virtual ky_variant bit_xor  (const ky_variant &v1, const ky_variant &v2)
    {
        intMathBit(^)
    }
    virtual ky_variant bit_negate (const ky_variant &v)
    {
        const fourwd_int tmp = ~(*(fourwd_int*)v.mem->data);
        return ky_variant(tmp);
    }
    virtual ky_variant shift_left (const ky_variant &v1, const ky_variant &v2)
    {
        const fourwd_int tmp = (*(fourwd_int*)v1.mem->data);
        if (!(v2.mem && v2.mem->math))
            return ky_variant(tmp);

        kyASSERT(v2.mem->math->component(v2) != component(v1),
                 "Shift operation does not support multi-component operations");
        int byte = 0;
        if (v2.mem->math->code() == kyIntCode)
        {
            byte = sizeof(fourwd_int);
            fourwd_int *elem = (fourwd_int*)v2.mem->data;
            return ky_variant(fourwd_int(tmp << *elem));
        }
        else if (v2.mem->math->code() == kyFltCode)
        {
            kyASSERT(true, "Float type cannot be shift operations");
        }
        else
            kyASSERT(true, "And unknown type operations");

        return ky_variant();
    }
    virtual ky_variant shift_right (const ky_variant &v1, const ky_variant &v2)
    {
        const fourwd_int tmp = (*(fourwd_int*)v1.mem->data);
        if (!(v2.mem && v2.mem->math))
            return ky_variant(tmp);

        kyASSERT(v2.mem->math->component(v2) != component(v1),
                 "Shift operation does not support multi-component operations");
        int byte = 0;
        if (v2.mem->math->code() == kyIntCode)
        {
            byte = sizeof(fourwd_int);
            fourwd_int *elem = (fourwd_int*)v2.mem->data;
            return ky_variant(fourwd_int(tmp >> *elem));
        }
        else if (v2.mem->math->code() == kyFltCode)
        {
            kyASSERT(true, "Float type cannot be shift operations");
        }
        else
            kyASSERT(true, "And unknown type operations");

        return ky_variant();
    }

    // 算术运算
    virtual ky_variant multiply  (const ky_variant &v1, const ky_variant &v2)
    {
        intMathLogic(*)
    }
    virtual ky_variant division  (const ky_variant &v1, const ky_variant &v2)
    {
        intMathLogic(/)
    }
    virtual ky_variant modulo  (const ky_variant &v1, const ky_variant &v2)
    {
        const fourwd_int tmp = (*(fourwd_int*)v1.mem->data);
        if (!(v2.mem && v2.mem->math))
            return ky_variant(tmp);

        const int part = v2.mem->math->component(v2);
        fourwd_int vi[part] = {0};
        fourwd_flt vf[part] = {0};
        int byte = part;
        if (v2.mem->math->code() == kyIntCode)
        {
            byte *= sizeof(fourwd_int);
            fourwd_int *elem = (fourwd_int*)v2.mem->data;
            for (int i = 0; i < part; ++i, ++elem)
                vi[i] = tmp % *elem;
        }
        else if (v2.mem->math->code() == kyFltCode)
        {
            byte *= sizeof(fourwd_flt);
            fourwd_flt *elem = (fourwd_flt*)v2.mem->data;
            for (int i = 0; i < part; ++i, ++elem)
                vf[i] = ky_fmod(tmp , *elem);
        }
        else
            kyASSERT(true, "And unknown type operations");

        if (v2.mem->code == kyIntCode)
            return ky_variant(v2.mem->code, v2.mem->math, vi, byte);
        return ky_variant(v2.mem->code, v2.mem->math, vf, byte);
    }
    virtual ky_variant plus  (const ky_variant &v1, const ky_variant &v2)
    {
        intMathLogic(+)
    }
    virtual ky_variant minus  (const ky_variant &v1, const ky_variant &v2)
    {
        intMathLogic(-)
    }
    virtual ky_variant invert  (const ky_variant &v)
    {
        const fourwd_int tmp = -(*(fourwd_int*)v.mem->data);
        return ky_variant(tmp);
    }

    // 关系运算
    virtual ky_variant less  (const ky_variant &v1, const ky_variant &v2)
    {
        intMathComp(<)
    }
    virtual ky_variant less_equal  (const ky_variant &v1, const ky_variant &v2)
    {
        intMathComp(<=)
    }
    virtual ky_variant equal  (const ky_variant &v1, const ky_variant &v2)
    {
        intMathComp(==)
    }
    virtual ky_variant not_equal  (const ky_variant &v1, const ky_variant &v2)
    {
        const fourwd_int tmp = (*(fourwd_int*)v1.mem->data);
        if (!(v2.mem && v2.mem->math))
            return ky_variant(true);

        const int part = v2.mem->math->component(v2);
        bool comp = 0;
        if (v2.mem->math->code() == kyIntCode)
        {
            fourwd_int suto = 0;
            fourwd_int *elem = (fourwd_int*)v2.mem->data;
            for (int i = 0; i < part; ++i, ++elem)
                suto += *elem;
            comp = (tmp - suto) != 0 ? true : false;
        }
        else if (v2.mem->math->code() == kyFltCode)
        {
             fourwd_flt suto = 0;
             fourwd_flt *elem = (fourwd_flt*)v2.mem->data;
             for (int i = 0; i < part; ++i, ++elem)
                 suto += *elem;
            comp = (tmp - suto) != 0 ? true : false;
        }
        else
            kyASSERT(true, "And unknown type operations");
        return ky_variant(comp);
    }
    virtual ky_variant greater_equal  (const ky_variant &v1, const ky_variant &v2)
    {
        intMathComp(>=)
    }
    virtual ky_variant greater  (const ky_variant &v1, const ky_variant &v2)
    {
        intMathComp(>)
    }

}__s_int;

static struct math_flt : ky_variant_math
{
    math_flt()
    {
        s_flt = this;
    }
    virtual ky_array<char> dump(const ky_variant &v)
    {
        ky_string pm("flt[");
        pm += ky_string::number(*(fourwd_flt*)v.mem->data);
        pm += "]";
        return ky_array<char>(pm.to_latin1(), pm.count());
    }
    virtual bool to_bool(const ky_variant &v)
    {
        return *(fourwd_flt*)v.mem->data;
    }
    virtual int component(const ky_variant &) {return 1;}
    virtual fourwd_t code() {return kyFltCode;}

    // 逻辑运算
    virtual ky_variant logic_not  (const ky_variant &v)
    {
        const fourwd_flt tmp = !(*(fourwd_flt*)v.mem->data);
        return ky_variant(tmp);
    }
    virtual ky_variant logic_and  (const ky_variant &v1, const ky_variant &v2)
    {
        const fourwd_flt tmp = (*(fourwd_flt*)v1.mem->data);
        if (!(v2.mem && v2.mem->math))
            return ky_variant(tmp);

        const int part = v2.mem->math->component(v2);
        const int byte = part * sizeof(fourwd_flt);
        fourwd_flt vec[part];
        if (v2.mem->math->code() == kyIntCode)
        {
            fourwd_int *elem = (fourwd_int*)v2.mem->data;
            for (int i = 0; i < part; ++i, ++elem)
                vec[i] = tmp && *elem;
        }
        else if (v2.mem->math->code() == kyFltCode)
        {
            fourwd_flt *elem = (fourwd_flt*)v2.mem->data;
            for (int i = 0; i < part; ++i, ++elem)
                vec[i] = tmp && *elem;
        }
        else
            kyASSERT(true, "And unknown type operations");

        return ky_variant(kyFltCode, s_flt, vec, byte);
    }
    virtual ky_variant logic_or  (const ky_variant &v1, const ky_variant &v2)
    {
        const fourwd_flt tmp = (*(fourwd_flt*)v1.mem->data);
        if (!(v2.mem && v2.mem->math))
            return ky_variant(tmp);

        const int part = v2.mem->math->component(v2);
        const int byte = part * sizeof(fourwd_flt);
        fourwd_flt vec[part];
        if (v2.mem->math->code() == kyIntCode)
        {
            fourwd_int *elem = (fourwd_int*)v2.mem->data;
            for (int i = 0; i < part; ++i, ++elem)
                vec[i] = tmp || *elem;
        }
        else if (v2.mem->math->code() == kyFltCode)
        {
            fourwd_flt *elem = (fourwd_flt*)v2.mem->data;
            for (int i = 0; i < part; ++i, ++elem)
                vec[i] = tmp || *elem;
        }
        else
            kyASSERT(true, "And unknown type operations");

        return ky_variant(kyFltCode, s_flt, vec, byte);
    }

    // 位运算
    virtual ky_variant bit_and  (const ky_variant &, const ky_variant &)
    {
        kyASSERT(true, "Float type cannot be bitwise operation");
        return ky_variant();
    }
    virtual ky_variant bit_or  (const ky_variant &, const ky_variant &)
    {
        kyASSERT(true, "Float type cannot be bitwise operation");
        return ky_variant();
    }
    virtual ky_variant bit_xor  (const ky_variant &, const ky_variant &)
    {
        kyASSERT(true, "Float type cannot be bitwise operation");
        return ky_variant();
    }
    virtual ky_variant bit_negate (const ky_variant &)
    {
        kyASSERT(true, "Float type cannot be bitwise operation");
        return ky_variant();
    }
    virtual ky_variant shift_left (const ky_variant &, const ky_variant &)
    {
        kyASSERT(true, "Float type cannot be shift operations");
        return ky_variant();
    }
    virtual ky_variant shift_right (const ky_variant &, const ky_variant &)
    {
        kyASSERT(true, "Float type cannot be shift operations");
        return ky_variant();
    }

    // 算术运算
    virtual ky_variant multiply  (const ky_variant &v1, const ky_variant &v2)
    {
        const fourwd_flt tmp = (*(fourwd_flt*)v1.mem->data);
        if (!(v2.mem && v2.mem->math))
            return ky_variant(tmp);

        const int part = v2.mem->math->component(v2);
        const int byte = part * sizeof(fourwd_flt);
        fourwd_flt vec[part];
        if (v2.mem->math->code() == kyIntCode)
        {
            fourwd_int *elem = (fourwd_int*)v2.mem->data;
            for (int i = 0; i < part; ++i, ++elem)
                vec[i] = tmp * *elem;
        }
        else if (v2.mem->math->code() == kyFltCode)
        {
            fourwd_flt *elem = (fourwd_flt*)v2.mem->data;
            for (int i = 0; i < part; ++i, ++elem)
                vec[i] = tmp * *elem;
        }
        else
            kyASSERT(true, "And unknown type operations");

        return ky_variant(kyFltCode, s_flt, vec, byte);
    }
    virtual ky_variant division  (const ky_variant &v1, const ky_variant &v2)
    {
        const fourwd_flt tmp = (*(fourwd_flt*)v1.mem->data);
        if (!(v2.mem && v2.mem->math))
            return ky_variant(tmp);

        const int part = v2.mem->math->component(v2);
        const int byte = part * sizeof(fourwd_flt);
        fourwd_flt vec[part];
        if (v2.mem->math->code() == kyIntCode)
        {
            fourwd_int *elem = (fourwd_int*)v2.mem->data;
            for (int i = 0; i < part; ++i, ++elem)
                vec[i] = tmp / *elem;
        }
        else if (v2.mem->math->code() == kyFltCode)
        {
            fourwd_flt *elem = (fourwd_flt*)v2.mem->data;
            for (int i = 0; i < part; ++i, ++elem)
                vec[i] = tmp / *elem;
        }
        else
            kyASSERT(true, "And unknown type operations");

        return ky_variant(kyFltCode, s_flt, vec, byte);
    }
    virtual ky_variant modulo  (const ky_variant &v1, const ky_variant &v2)
    {
        const fourwd_flt tmp = (*(fourwd_flt*)v1.mem->data);
        if (!(v2.mem && v2.mem->math))
            return ky_variant(tmp);

        const int part = v2.mem->math->component(v2);
        const int byte = part * sizeof(fourwd_flt);
        fourwd_flt vec[part];
        if (v2.mem->math->code() == kyIntCode)
        {
            fourwd_int *elem = (fourwd_int*)v2.mem->data;
            for (int i = 0; i < part; ++i, ++elem)
                vec[i] = ky_fmod(tmp, *elem);
        }
        else if (v2.mem->math->code() == kyFltCode)
        {
            fourwd_flt *elem = (fourwd_flt*)v2.mem->data;
            for (int i = 0; i < part; ++i, ++elem)
                vec[i] = ky_fmod(tmp, *elem);
        }
        else
            kyASSERT(true, "And unknown type operations");

        return ky_variant(kyFltCode, s_flt, vec, byte);
    }
    virtual ky_variant plus  (const ky_variant &v1, const ky_variant &v2)
    {
        const fourwd_flt tmp = (*(fourwd_flt*)v1.mem->data);
        if (!(v2.mem && v2.mem->math))
            return ky_variant(tmp);

        const int part = v2.mem->math->component(v2);
        const int byte = part * sizeof(fourwd_flt);
        fourwd_flt vec[part];
        if (v2.mem->math->code() == kyIntCode)
        {
            fourwd_int *elem = (fourwd_int*)v2.mem->data;
            for (int i = 0; i < part; ++i, ++elem)
                vec[i] = tmp + *elem;
        }
        else if (v2.mem->math->code() == kyFltCode)
        {
            fourwd_flt *elem = (fourwd_flt*)v2.mem->data;
            for (int i = 0; i < part; ++i, ++elem)
                vec[i] = tmp + *elem;
        }
        else
            kyASSERT(true, "And unknown type operations");

        return ky_variant(kyFltCode, s_flt, vec, byte);
    }
    virtual ky_variant minus  (const ky_variant &v1, const ky_variant &v2)
    {
        const fourwd_flt tmp = (*(fourwd_flt*)v1.mem->data);
        if (!(v2.mem && v2.mem->math))
            return ky_variant(tmp);

        const int part = v2.mem->math->component(v2);
        const int byte = part * sizeof(fourwd_flt);
        fourwd_flt vec[part];
        if (v2.mem->math->code() == kyIntCode)
        {
            fourwd_int *elem = (fourwd_int*)v2.mem->data;
            for (int i = 0; i < part; ++i, ++elem)
                vec[i] = tmp - *elem;
        }
        else if (v2.mem->math->code() == kyFltCode)
        {
            fourwd_flt *elem = (fourwd_flt*)v2.mem->data;
            for (int i = 0; i < part; ++i, ++elem)
                vec[i] = tmp - *elem;
        }
        else
            kyASSERT(true, "And unknown type operations");

        return ky_variant(kyFltCode, s_flt, vec, byte);
    }
    virtual ky_variant invert  (const ky_variant &v)
    {
        const fourwd_flt tmp = -(*(fourwd_flt*)v.mem->data);
        return ky_variant(tmp);
    }

    // 关系运算
    virtual ky_variant less  (const ky_variant &v1, const ky_variant &v2)
    {
        const fourwd_flt tmp = (*(fourwd_flt*)v1.mem->data);
        if (!(v2.mem && v2.mem->math))
            return ky_variant(false);

        const int part = v2.mem->math->component(v2);
        bool comp = 0;
        if (v2.mem->math->code() == kyIntCode)
        {
            fourwd_flt suto = 0;
            int64 *elem = (int64*)v2.mem->data;
            for (int i = 0; i < part; ++i, ++elem)
                suto += *elem;
            comp = (tmp - suto) < 0 ? true : false;
        }
        else if (v2.mem->math->code() == kyFltCode)
        {
            fourwd_flt suto = 0;
            fourwd_flt *elem = (fourwd_flt*)v2.mem->data;
            for (int i = 0; i < part; ++i, ++elem)
                suto += *elem;
            comp = (tmp - suto) < 0 ? true : false;
        }
        else
            kyASSERT(true, "And unknown type operations");

        return ky_variant(comp);
    }
    virtual ky_variant less_equal  (const ky_variant &v1, const ky_variant &v2)
    {
        const fourwd_flt tmp = (*(fourwd_flt*)v1.mem->data);
        if (!(v2.mem && v2.mem->math))
            return ky_variant(false);

        const int part = v2.mem->math->component(v2);
        bool comp = 0;
        if (v2.mem->math->code() == kyIntCode)
        {
            fourwd_flt suto = 0;
            int64 *elem = (int64*)v2.mem->data;
            for (int i = 0; i < part; ++i, ++elem)
                suto += *elem;
            comp = (tmp - suto) <= 0 ? true : false;
        }
        else if (v2.mem->math->code() == kyFltCode)
        {
            fourwd_flt suto = 0;
            fourwd_flt *elem = (fourwd_flt*)v2.mem->data;
            for (int i = 0; i < part; ++i, ++elem)
                suto += *elem;
            comp = (tmp - suto) <= 0 ? true : false;
        }
        else
            kyASSERT(true, "And unknown type operations");

        return ky_variant(comp);
    }
    virtual ky_variant equal  (const ky_variant &v1, const ky_variant &v2)
    {
        const fourwd_flt tmp = (*(fourwd_flt*)v1.mem->data);
        if (!(v2.mem && v2.mem->math))
            return ky_variant(false);

        const int part = v2.mem->math->component(v2);
        bool comp = 0;
        if (v2.mem->math->code() == kyIntCode)
        {
            fourwd_flt suto = 0;
            int64 *elem = (int64*)v2.mem->data;
            for (int i = 0; i < part; ++i, ++elem)
                suto += *elem;
            comp = (tmp - suto) == 0 ? true : false;
        }
        else if (v2.mem->math->code() == kyFltCode)
        {
            fourwd_flt suto = 0;
            fourwd_flt *elem = (fourwd_flt*)v2.mem->data;
            for (int i = 0; i < part; ++i, ++elem)
                suto += *elem;
            comp = (tmp - suto) == 0 ? true : false;
        }
        else
            kyASSERT(true, "And unknown type operations");

        return ky_variant(comp);
    }
    virtual ky_variant not_equal  (const ky_variant &v1, const ky_variant &v2)
    {
        const fourwd_flt tmp = (*(fourwd_flt*)v1.mem->data);
        if (!(v2.mem && v2.mem->math))
            return ky_variant(true);

        const int part = v2.mem->math->component(v2);
        bool comp = 0;
        if (v2.mem->math->code() == kyIntCode)
        {
            fourwd_flt suto = 0;
            int64 *elem = (int64*)v2.mem->data;
            for (int i = 0; i < part; ++i, ++elem)
                suto += *elem;
            comp = (tmp - suto) != 0 ? true : false;
        }
        else if (v2.mem->math->code() == kyFltCode)
        {
            fourwd_flt suto = 0;
            fourwd_flt *elem = (fourwd_flt*)v2.mem->data;
            for (int i = 0; i < part; ++i, ++elem)
                suto += *elem;
            comp = (tmp - suto) != 0 ? true : false;
        }
        else
            kyASSERT(true, "And unknown type operations");

        return ky_variant(comp);
    }
    virtual ky_variant greater_equal  (const ky_variant &v1, const ky_variant &v2)
    {
        const fourwd_flt tmp = (*(fourwd_flt*)v1.mem->data);
        if (!(v2.mem && v2.mem->math))
            return ky_variant(false);

        const int part = v2.mem->math->component(v2);
        bool comp = 0;
        if (v2.mem->math->code() == kyIntCode)
        {
            fourwd_flt suto = 0;
            int64 *elem = (int64*)v2.mem->data;
            for (int i = 0; i < part; ++i, ++elem)
                suto += *elem;
            comp = (tmp - suto) >= 0 ? true : false;
        }
        else if (v2.mem->math->code() == kyFltCode)
        {
            fourwd_flt suto = 0;
            fourwd_flt *elem = (fourwd_flt*)v2.mem->data;
            for (int i = 0; i < part; ++i, ++elem)
                suto += *elem;
            comp = (tmp - suto) >= 0 ? true : false;
        }
        else
            kyASSERT(true, "And unknown type operations");

        return ky_variant(comp);
    }
    virtual ky_variant greater  (const ky_variant &v1, const ky_variant &v2)
    {
        const fourwd_flt tmp = (*(fourwd_flt*)v1.mem->data);
        if (!(v2.mem && v2.mem->math))
            return ky_variant(false);

        const int part = v2.mem->math->component(v2);
        bool comp = 0;
        if (v2.mem->math->code() == kyIntCode)
        {
            fourwd_flt suto = 0;
            int64 *elem = (int64*)v2.mem->data;
            for (int i = 0; i < part; ++i, ++elem)
                suto += *elem;
            comp = (tmp - suto) > 0 ? true : false;
        }
        else if (v2.mem->math->code() == kyFltCode)
        {
            fourwd_flt suto = 0;
            fourwd_flt *elem = (fourwd_flt*)v2.mem->data;
            for (int i = 0; i < part; ++i, ++elem)
                suto += *elem;
            comp = (tmp - suto) > 0 ? true : false;
        }
        else
            kyASSERT(true, "And unknown type operations");

        return ky_variant(comp);
    }

}__s_flt;

static struct math_veci : ky_variant_math
{
    math_veci()
    {
        s_veci = this;
    }
    virtual ky_array<char> dump(const ky_variant &v)
    {
        ky_string pm("veci[");
        const int count = v.mem->math->component(v);
        fourwd_int *vd = (fourwd_int *)v.mem->data;
        for (int i = 0; i < count; ++i)
        {
            pm += ky_string::number(vd[i]);
            if (i+1 != count)
              pm += ", ";
        }

        pm += "]";
        return ky_array<char>(pm.to_latin1(), pm.count());
    }
    virtual bool to_bool(const ky_variant &v)
    {
        const int count = v.mem->math->component(v);
        fourwd_int *vd = (fourwd_int *)v.mem->data;
        fourwd_int retusl = 0;
        for (int i = 0; i < count; ++i)
            retusl += vd[i];
        return retusl;
    }
    virtual int component(const ky_variant &v) {return v.mem->byte / sizeof(fourwd_int);}
    virtual fourwd_t code() {return kyIntCode;}

    // 逻辑运算
    virtual ky_variant logic_not  (const ky_variant &v)
    {
        fourwd_veci veci;veci.resize(v.mem->math->component(v));
        fourwd_int *vd = (fourwd_int *)v.mem->data;
        for (int i = 0; i < v.mem->math->component(v); ++i)
            veci[i] = !vd[i];
        return ky_variant(veci);
    }
    virtual ky_variant logic_and  (const ky_variant &v1, const ky_variant &v2)
    {
        if (!(v2.mem && v2.mem->math))
            return v1;

        const int part1 = v1.mem->math->component(v1);
        const int part2 = v2.mem->math->component(v2);
        const int part_max = part1 > part2 ? part1 : part2;

        fourwd_veci retusli;retusli.resize(part_max);
        fourwd_vecf retuslf;retuslf.resize(part_max);
        ky_memory::copy(retusli.data(), v1.mem->data, v1.mem->byte);

        fourwd_t code = 0;
        if (v2.mem->math->code() == kyIntCode)
        {
            code = kyVecIntCode;
            fourwd_int *elem = (fourwd_int*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                retusli[i] = retusli[i] && *elem;
        }
        else if (v2.mem->math->code() == kyFltCode)
        {
            code = kyVecFltCode;
            fourwd_flt *elem = (fourwd_flt*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                retuslf[i] = retusli[i] && *elem;
        }
        else
            kyASSERT(true, "And unknown type operations");
        if (code == kyVecIntCode)
            return ky_variant(retusli);
        return ky_variant(retuslf);
    }
    virtual ky_variant logic_or  (const ky_variant &v1, const ky_variant &v2)
    {
        if (!(v2.mem && v2.mem->math))
            return v1;

        const int part1 = v1.mem->math->component(v1);
        const int part2 = v2.mem->math->component(v2);
        const int part_max = part1 > part2 ? part1 : part2;

        fourwd_veci retusli;retusli.resize(part_max);
        fourwd_vecf retuslf;retuslf.resize(part_max);
        ky_memory::copy(retusli.data(), v1.mem->data, v1.mem->byte);

        fourwd_t code = 0;
        if (v2.mem->math->code() == kyIntCode)
        {
            code = kyVecIntCode;
            fourwd_int *elem = (fourwd_int*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                retusli[i] = retusli[i] || *elem;
        }
        else if (v2.mem->math->code() == kyFltCode)
        {
            code = kyVecFltCode;
            fourwd_flt *elem = (fourwd_flt*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                retuslf[i] = retusli[i] || *elem;
        }
        else
            kyASSERT(true, "And unknown type operations");
        if (code == kyVecIntCode)
            return ky_variant(retusli);
        return ky_variant(retuslf);
    }

    // 位运算
    virtual ky_variant bit_and  (const ky_variant &v1, const ky_variant &v2)
    {
        if (!(v2.mem && v2.mem->math))
            return v1;

        const int part1 = v1.mem->math->component(v1);
        const int part2 = v2.mem->math->component(v2);
        const int part_max = part1 > part2 ? part1 : part2;

        fourwd_veci retusli;retusli.resize(part_max);
        ky_memory::copy(retusli.data(), v1.mem->data, v1.mem->byte);

        if (v2.mem->math->code() == kyIntCode)
        {
            fourwd_int *elem = (fourwd_int*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                retusli[i] = retusli[i] & *elem;
        }
        else if (v2.mem->math->code() == kyFltCode)
        {
            kyASSERT(true, "Float type cannot be bitwise operation");
        }
        else
            kyASSERT(true, "And unknown type operations");
        return ky_variant(retusli);
    }
    virtual ky_variant bit_or  (const ky_variant &v1, const ky_variant &v2)
    {
        if (!(v2.mem && v2.mem->math))
            return v1;

        const int part1 = v1.mem->math->component(v1);
        const int part2 = v2.mem->math->component(v2);
        const int part_max = part1 > part2 ? part1 : part2;

        fourwd_veci retusli;retusli.resize(part_max);
        ky_memory::copy(retusli.data(), v1.mem->data, v1.mem->byte);

        if (v2.mem->math->code() == kyIntCode)
        {
            fourwd_int *elem = (fourwd_int*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                retusli[i] = retusli[i] | *elem;
        }
        else if (v2.mem->math->code() == kyFltCode)
        {
            kyASSERT(true, "Float type cannot be bitwise operation");
        }
        else
            kyASSERT(true, "And unknown type operations");
        return ky_variant(retusli);
    }
    virtual ky_variant bit_xor  (const ky_variant &v1, const ky_variant &v2)
    {
        if (!(v2.mem && v2.mem->math))
            return v1;

        const int part1 = v1.mem->math->component(v1);
        const int part2 = v2.mem->math->component(v2);
        const int part_max = part1 > part2 ? part1 : part2;

        fourwd_veci retusli;retusli.resize(part_max);
        ky_memory::copy(retusli.data(), v1.mem->data, v1.mem->byte);

        if (v2.mem->math->code() == kyIntCode)
        {
            fourwd_int *elem = (fourwd_int*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                retusli[i] = retusli[i] ^ *elem;
        }
        else if (v2.mem->math->code() == kyFltCode)
        {
            kyASSERT(true, "Float type cannot be bitwise operation");
        }
        else
            kyASSERT(true, "And unknown type operations");
        return ky_variant(retusli);
    }
    virtual ky_variant bit_negate (const ky_variant &v)
    {
        fourwd_veci veci;veci.resize(v.mem->math->component(v));
        fourwd_int *vd = (fourwd_int *)v.mem->data;
        for (int i = 0; i < v.mem->math->component(v); ++i)
            veci[i] = ~vd[i];
        return ky_variant(veci);
    }
    virtual ky_variant shift_left (const ky_variant &, const ky_variant &)
    {
        kyASSERT(true, "Shift operation does not support multi-component operations");
        return ky_variant();
    }
    virtual ky_variant shift_right (const ky_variant &, const ky_variant &)
    {
        kyASSERT(true, "Shift operation does not support multi-component operations");
        return ky_variant();
    }

    // 算术运算
    virtual ky_variant multiply  (const ky_variant &v1, const ky_variant &v2)
    {
        if (!(v2.mem && v2.mem->math))
            return v1;

        const int part1 = v1.mem->math->component(v1);
        const int part2 = v2.mem->math->component(v2);
        const int part_max = part1 > part2 ? part1 : part2;

        fourwd_veci retusli;retusli.resize(part_max);
        fourwd_vecf retuslf;retuslf.resize(part_max);
        ky_memory::copy(retusli.data(), v1.mem->data, v1.mem->byte);

        fourwd_t code = 0;
        if (v2.mem->math->code() == kyIntCode)
        {
            code = kyVecIntCode;
            fourwd_int *elem = (fourwd_int*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                retusli[i] = retusli[i] * *elem;
        }
        else if (v2.mem->math->code() == kyFltCode)
        {
            code = kyVecFltCode;
            fourwd_flt *elem = (fourwd_flt*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                retuslf[i] = retusli[i] * *elem;
        }
        else
            kyASSERT(true, "And unknown type operations");
        if (code == kyVecIntCode)
            return ky_variant(retusli);
        return ky_variant(retuslf);
    }
    virtual ky_variant division  (const ky_variant &v1, const ky_variant &v2)
    {
        if (!(v2.mem && v2.mem->math))
            return v1;

        const int part1 = v1.mem->math->component(v1);
        const int part2 = v2.mem->math->component(v2);
        const int part_max = part1 > part2 ? part1 : part2;

        fourwd_veci retusli;retusli.resize(part_max);
        fourwd_vecf retuslf;retuslf.resize(part_max);
        ky_memory::copy(retusli.data(), v1.mem->data, v1.mem->byte);

        fourwd_t code = 0;
        if (v2.mem->math->code() == kyIntCode)
        {
            code = kyVecIntCode;
            fourwd_int *elem = (fourwd_int*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                retusli[i] = retusli[i] / *elem;
        }
        else if (v2.mem->math->code() == kyFltCode)
        {
            code = kyVecFltCode;
            fourwd_flt *elem = (fourwd_flt*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                retuslf[i] = retusli[i] / *elem;
        }
        else
            kyASSERT(true, "And unknown type operations");
        if (code == kyVecIntCode)
            return ky_variant(retusli);
        return ky_variant(retuslf);
    }
    virtual ky_variant modulo  (const ky_variant &v1, const ky_variant &v2)
    {
        if (!(v2.mem && v2.mem->math))
            return v1;

        const int part1 = v1.mem->math->component(v1);
        const int part2 = v2.mem->math->component(v2);
        const int part_max = part1 > part2 ? part1 : part2;

        fourwd_veci retusli;retusli.resize(part_max);
        fourwd_vecf retuslf;retuslf.resize(part_max);
        ky_memory::copy(retusli.data(), v1.mem->data, v1.mem->byte);

        fourwd_t code = 0;
        if (v2.mem->math->code() == kyIntCode)
        {
            code = kyVecIntCode;
            fourwd_int *elem = (fourwd_int*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                retusli[i] = retusli[i] % *elem;
        }
        else if (v2.mem->math->code() == kyFltCode)
        {
            code = kyVecFltCode;
            fourwd_flt *elem = (fourwd_flt*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                retuslf[i] = ky_fmod(retusli[i], *elem);
        }
        else
            kyASSERT(true, "And unknown type operations");
        if (code == kyVecIntCode)
            return ky_variant(retusli);
        return ky_variant(retuslf);
    }
    virtual ky_variant plus  (const ky_variant &v1, const ky_variant &v2)
    {
        if (!(v2.mem && v2.mem->math))
            return v1;

        const int part1 = v1.mem->math->component(v1);
        const int part2 = v2.mem->math->component(v2);
        const int part_max = part1 > part2 ? part1 : part2;

        fourwd_veci retusli;retusli.resize(part_max);
        fourwd_vecf retuslf;retuslf.resize(part_max);
        ky_memory::copy(retusli.data(), v1.mem->data, v1.mem->byte);

        fourwd_t code = 0;
        if (v2.mem->math->code() == kyIntCode)
        {
            code = kyVecIntCode;
            fourwd_int *elem = (fourwd_int*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                retusli[i] = retusli[i] + *elem;
        }
        else if (v2.mem->math->code() == kyFltCode)
        {
            code = kyVecFltCode;
            fourwd_flt *elem = (fourwd_flt*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                retuslf[i] = retusli[i] + *elem;
        }
        else
            kyASSERT(true, "And unknown type operations");
        if (code == kyVecIntCode)
            return ky_variant(retusli);
        return ky_variant(retuslf);
    }
    virtual ky_variant minus  (const ky_variant &v1, const ky_variant &v2)
    {
        if (!(v2.mem && v2.mem->math))
            return v1;

        const int part1 = v1.mem->math->component(v1);
        const int part2 = v2.mem->math->component(v2);
        const int part_max = part1 > part2 ? part1 : part2;

        fourwd_veci retusli;retusli.resize(part_max);
        fourwd_vecf retuslf;retuslf.resize(part_max);
        ky_memory::copy(retusli.data(), v1.mem->data, v1.mem->byte);

        fourwd_t code = 0;
        if (v2.mem->math->code() == kyIntCode)
        {
            code = kyVecIntCode;
            fourwd_int *elem = (fourwd_int*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                retusli[i] = retusli[i] - *elem;
        }
        else if (v2.mem->math->code() == kyFltCode)
        {
            code = kyVecFltCode;
            fourwd_flt *elem = (fourwd_flt*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                retuslf[i] = retusli[i] - *elem;
        }
        else
            kyASSERT(true, "And unknown type operations");
        if (code == kyVecIntCode)
            return ky_variant(retusli);
        return ky_variant(retuslf);
    }
    virtual ky_variant invert  (const ky_variant &v)
    {
        fourwd_veci veci;veci.resize(v.mem->math->component(v));
        fourwd_int *vd = (fourwd_int *)v.mem->data;
        for (int i = 0; i < v.mem->math->component(v); ++i)
            veci[i] = -vd[i];
        return ky_variant(veci);
    }

    // 关系运算
    virtual ky_variant less  (const ky_variant &v1, const ky_variant &v2)
    {;
        if (!(v2.mem && v2.mem->math))
            return ky_variant(false);

        const int part1 = v1.mem->math->component(v1);
        const int part2 = v2.mem->math->component(v2);
        fourwd_int *tmp = (fourwd_int*)v1.mem->data;
        fourwd_int sutx = 0;
        for (int i = 0; i < part1; ++i, ++tmp)
            sutx += *tmp;

        bool comp = 0;
        if (v2.mem->math->code() == kyIntCode)
        {
            fourwd_int suto = 0;
            fourwd_int *elem = (fourwd_int*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                suto += *elem;

            comp = (sutx - suto) < 0 ? true : false;
        }
        else if (v2.mem->math->code() == kyFltCode)
        {
            fourwd_flt suto = 0;
            fourwd_flt *elem = (fourwd_flt*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                suto += *elem;
            comp = (sutx - suto) < 0 ? true : false;
        }
        else
            kyASSERT(true, "And unknown type operations");

        return ky_variant(comp);
    }
    virtual ky_variant less_equal  (const ky_variant &v1, const ky_variant &v2)
    {
        if (!(v2.mem && v2.mem->math))
            return ky_variant(false);

        const int part1 = v1.mem->math->component(v1);
        const int part2 = v2.mem->math->component(v2);
        fourwd_int *tmp = (fourwd_int*)v1.mem->data;
        fourwd_int sutx = 0;
        for (int i = 0; i < part1; ++i, ++tmp)
            sutx += *tmp;

        bool comp = 0;
        if (v2.mem->math->code() == kyIntCode)
        {
            fourwd_int suto = 0;
            fourwd_int *elem = (fourwd_int*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                suto += *elem;

            comp = (sutx - suto) <= 0 ? true : false;
        }
        else if (v2.mem->math->code() == kyFltCode)
        {
            fourwd_flt suto = 0;
            fourwd_flt *elem = (fourwd_flt*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                suto += *elem;
            comp = (sutx - suto) <= 0 ? true : false;
        }
        else
            kyASSERT(true, "And unknown type operations");

        return ky_variant(comp);
    }
    virtual ky_variant equal  (const ky_variant &v1, const ky_variant &v2)
    {
        if (!(v2.mem && v2.mem->math))
            return ky_variant(false);

        const int part1 = v1.mem->math->component(v1);
        const int part2 = v2.mem->math->component(v2);
        fourwd_int *tmp = (fourwd_int*)v1.mem->data;
        fourwd_int sutx = 0;
        for (int i = 0; i < part1; ++i, ++tmp)
            sutx += *tmp;

        bool comp = 0;
        if (v2.mem->math->code() == kyIntCode)
        {
            fourwd_int suto = 0;
            fourwd_int *elem = (fourwd_int*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                suto += *elem;

            comp = (sutx - suto) == 0 ? true : false;
        }
        else if (v2.mem->math->code() == kyFltCode)
        {
            fourwd_flt suto = 0;
            fourwd_flt *elem = (fourwd_flt*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                suto += *elem;
            comp = (sutx - suto) == 0 ? true : false;
        }
        else
            kyASSERT(true, "And unknown type operations");

        return ky_variant(comp);
    }
    virtual ky_variant not_equal  (const ky_variant &v1, const ky_variant &v2)
    {
        if (!(v2.mem && v2.mem->math))
            return ky_variant(true);

        const int part1 = v1.mem->math->component(v1);
        const int part2 = v2.mem->math->component(v2);
        fourwd_int *tmp = (fourwd_int*)v1.mem->data;
        fourwd_int sutx = 0;
        for (int i = 0; i < part1; ++i, ++tmp)
            sutx += *tmp;

        bool comp = 0;
        if (v2.mem->math->code() == kyIntCode)
        {
            fourwd_int suto = 0;
            fourwd_int *elem = (fourwd_int*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                suto += *elem;

            comp = (sutx - suto) != 0 ? true : false;
        }
        else if (v2.mem->math->code() == kyFltCode)
        {
            fourwd_flt suto = 0;
            fourwd_flt *elem = (fourwd_flt*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                suto += *elem;
            comp = (sutx - suto) != 0 ? true : false;
        }
        else
            kyASSERT(true, "And unknown type operations");

        return ky_variant(comp);
    }
    virtual ky_variant greater_equal  (const ky_variant &v1, const ky_variant &v2)
    {
        if (!(v2.mem && v2.mem->math))
            return ky_variant(false);

        const int part1 = v1.mem->math->component(v1);
        const int part2 = v2.mem->math->component(v2);
        fourwd_int *tmp = (fourwd_int*)v1.mem->data;
        fourwd_int sutx = 0;
        for (int i = 0; i < part1; ++i, ++tmp)
            sutx += *tmp;

        bool comp = 0;
        if (v2.mem->math->code() == kyIntCode)
        {
            fourwd_int suto = 0;
            fourwd_int *elem = (fourwd_int*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                suto += *elem;

            comp = (sutx - suto) >= 0 ? true : false;
        }
        else if (v2.mem->math->code() == kyFltCode)
        {
            fourwd_flt suto = 0;
            fourwd_flt *elem = (fourwd_flt*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                suto += *elem;
            comp = (sutx - suto) >= 0 ? true : false;
        }
        else
            kyASSERT(true, "And unknown type operations");

        return ky_variant(comp);
    }
    virtual ky_variant greater  (const ky_variant &v1, const ky_variant &v2)
    {
        if (!(v2.mem && v2.mem->math))
            return ky_variant(false);

        const int part1 = v1.mem->math->component(v1);
        const int part2 = v2.mem->math->component(v2);
        fourwd_int *tmp = (fourwd_int*)v1.mem->data;
        fourwd_int sutx = 0;
        for (int i = 0; i < part1; ++i, ++tmp)
            sutx += *tmp;

        bool comp = 0;
        if (v2.mem->math->code() == kyIntCode)
        {
            fourwd_int suto = 0;
            fourwd_int *elem = (fourwd_int*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                suto += *elem;

            comp = (sutx - suto) > 0 ? true : false;
        }
        else if (v2.mem->math->code() == kyFltCode)
        {
            fourwd_flt suto = 0;
            fourwd_flt *elem = (fourwd_flt*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                suto += *elem;
            comp = (sutx - suto) > 0 ? true : false;
        }
        else
            kyASSERT(true, "And unknown type operations");

        return ky_variant(comp);
    }

}__s_veci;

static struct math_vecf : ky_variant_math
{
    math_vecf()
    {
        s_vecf = this;
    }
    virtual ky_array<char> dump(const ky_variant &v)
    {
        ky_string pm("vecf[");
        const int count = v.mem->math->component(v);
        fourwd_flt *vd = (fourwd_flt*)v.mem->data;
        for (int i = 0; i < count; ++i)
        {
            pm += ky_string::number(vd[i]);
            if (i+1 != count)
              pm += ", ";
        }

        pm += "]";
        return ky_array<char>(pm.to_latin1(), pm.count());
    }
    virtual bool to_bool(const ky_variant &v)
    {
        const int count = v.mem->math->component(v);
        fourwd_flt *vd = (fourwd_flt*)v.mem->data;
        fourwd_flt retusl = 0;
        for (int i = 0; i < count; ++i)
            retusl += vd[i];
        return retusl;
    }
    virtual int component(const ky_variant &v) {return v.mem->byte / sizeof(fourwd_int);}
    virtual fourwd_t code() {return kyFltCode;}

    // 逻辑运算
    virtual ky_variant logic_not  (const ky_variant &v)
    {
        fourwd_vecf vecf;vecf.resize(v.mem->math->component(v));
        fourwd_int *vd = (fourwd_int*)v.mem->data;
        for (int i = 0; i < v.mem->math->component(v); ++i)
            vecf[i] = !vd[i];
        return ky_variant(vecf);
    }
    virtual ky_variant logic_and  (const ky_variant &v1, const ky_variant &v2)
    {
        if (!(v2.mem && v2.mem->math))
            return v1;

        const int part1 = v1.mem->math->component(v1);
        const int part2 = v2.mem->math->component(v2);
        const int part_max = part1 > part2 ? part1 : part2;

        fourwd_vecf retuslf;retuslf.resize(part_max);
        ky_memory::copy(retuslf.data(), v1.mem->data, v1.mem->byte);

        if (v2.mem->math->code() == kyIntCode)
        {
            fourwd_int *elem = (fourwd_int*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                retuslf[i] = retuslf[i] && *elem;
        }
        else if (v2.mem->math->code() == kyFltCode)
        {
            fourwd_flt *elem = (fourwd_flt*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                retuslf[i] = retuslf[i] && *elem;
        }
        else
            kyASSERT(true, "And unknown type operations");
        return ky_variant(retuslf);
    }
    virtual ky_variant logic_or  (const ky_variant &v1, const ky_variant &v2)
    {
        if (!(v2.mem && v2.mem->math))
            return v1;

        const int part1 = v1.mem->math->component(v1);
        const int part2 = v2.mem->math->component(v2);
        const int part_max = part1 > part2 ? part1 : part2;

        fourwd_vecf retuslf;retuslf.resize(part_max);
        ky_memory::copy(retuslf.data(), v1.mem->data, v1.mem->byte);

        if (v2.mem->math->code() == kyIntCode)
        {
            fourwd_int *elem = (fourwd_int*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                retuslf[i] = retuslf[i] || *elem;
        }
        else if (v2.mem->math->code() == kyFltCode)
        {
            fourwd_flt *elem = (fourwd_flt*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                retuslf[i] = retuslf[i] || *elem;
        }
        else
            kyASSERT(true, "And unknown type operations");
        return ky_variant(retuslf);
    }

    // 位运算
    virtual ky_variant bit_and  (const ky_variant &, const ky_variant &)
    {
        kyASSERT(true, "Float type cannot be bitwise operation");
        return ky_variant();
    }
    virtual ky_variant bit_or  (const ky_variant &, const ky_variant &)
    {
        kyASSERT(true, "Float type cannot be bitwise operation");
        return ky_variant();
    }
    virtual ky_variant bit_xor  (const ky_variant &, const ky_variant &)
    {
        kyASSERT(true, "Float type cannot be bitwise operation");
        return ky_variant();
    }
    virtual ky_variant bit_negate (const ky_variant &)
    {
        kyASSERT(true, "Float type cannot be bitwise operation");
        return ky_variant();
    }
    virtual ky_variant shift_left (const ky_variant &, const ky_variant &)
    {
        kyASSERT(true, "Shift operation does not support multi-component operations");
        return ky_variant();
    }
    virtual ky_variant shift_right (const ky_variant &, const ky_variant &)
    {
        kyASSERT(true, "Shift operation does not support multi-component operations");
        return ky_variant();
    }

    // 算术运算
    virtual ky_variant multiply  (const ky_variant &v1, const ky_variant &v2)
    {
        if (!(v2.mem && v2.mem->math))
            return v1;

        const int part1 = v1.mem->math->component(v1);
        const int part2 = v2.mem->math->component(v2);
        const int part_max = part1 > part2 ? part1 : part2;

        fourwd_vecf retuslf;retuslf.resize(part_max);
        ky_memory::copy(retuslf.data(), v1.mem->data, v1.mem->byte);

        if (v2.mem->math->code() == kyIntCode)
        {
            fourwd_int *elem = (fourwd_int*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                retuslf[i] = retuslf[i] * *elem;
        }
        else if (v2.mem->math->code() == kyFltCode)
        {
            fourwd_flt *elem = (fourwd_flt*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                retuslf[i] = retuslf[i] * *elem;
        }
        else
            kyASSERT(true, "And unknown type operations");
        return ky_variant(retuslf);
    }
    virtual ky_variant division  (const ky_variant &v1, const ky_variant &v2)
    {
        if (!(v2.mem && v2.mem->math))
            return v1;

        const int part1 = v1.mem->math->component(v1);
        const int part2 = v2.mem->math->component(v2);
        const int part_max = part1 > part2 ? part1 : part2;

        fourwd_vecf retuslf;retuslf.resize(part_max);
        ky_memory::copy(retuslf.data(), v1.mem->data, v1.mem->byte);

        if (v2.mem->math->code() == kyIntCode)
        {
            fourwd_int *elem = (fourwd_int*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                retuslf[i] = retuslf[i] / *elem;
        }
        else if (v2.mem->math->code() == kyFltCode)
        {
            fourwd_flt *elem = (fourwd_flt*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                retuslf[i] = retuslf[i] / *elem;
        }
        else
            kyASSERT(true, "And unknown type operations");
        return ky_variant(retuslf);
    }
    virtual ky_variant modulo  (const ky_variant &v1, const ky_variant &v2)
    {
        if (!(v2.mem && v2.mem->math))
            return v1;

        const int part1 = v1.mem->math->component(v1);
        const int part2 = v2.mem->math->component(v2);
        const int part_max = part1 > part2 ? part1 : part2;

        fourwd_vecf retuslf;retuslf.resize(part_max);
        ky_memory::copy(retuslf.data(), v1.mem->data, v1.mem->byte);

        if (v2.mem->math->code() == kyIntCode)
        {
            fourwd_int *elem = (fourwd_int*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                retuslf[i] = ky_fmod(retuslf[i], *elem);
        }
        else if (v2.mem->math->code() == kyFltCode)
        {
            fourwd_flt *elem = (fourwd_flt*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                retuslf[i] = ky_fmod(retuslf[i], *elem);
        }
        else
            kyASSERT(true, "And unknown type operations");
        return ky_variant(retuslf);
    }
    virtual ky_variant plus  (const ky_variant &v1, const ky_variant &v2)
    {
        if (!(v2.mem && v2.mem->math))
            return v1;

        const int part1 = v1.mem->math->component(v1);
        const int part2 = v2.mem->math->component(v2);
        const int part_max = part1 > part2 ? part1 : part2;

        fourwd_vecf retuslf;retuslf.resize(part_max);
        ky_memory::copy(retuslf.data(), v1.mem->data, v1.mem->byte);

        if (v2.mem->math->code() == kyIntCode)
        {
            fourwd_int *elem = (fourwd_int*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                retuslf[i] = retuslf[i] + *elem;
        }
        else if (v2.mem->math->code() == kyFltCode)
        {
            fourwd_flt *elem = (fourwd_flt*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                retuslf[i] = retuslf[i] + *elem;
        }
        else
            kyASSERT(true, "And unknown type operations");
        return ky_variant(retuslf);
    }
    virtual ky_variant minus  (const ky_variant &v1, const ky_variant &v2)
    {
        if (!(v2.mem && v2.mem->math))
            return v1;

        const int part1 = v1.mem->math->component(v1);
        const int part2 = v2.mem->math->component(v2);
        const int part_max = part1 > part2 ? part1 : part2;

        fourwd_vecf retuslf;retuslf.resize(part_max);
        ky_memory::copy(retuslf.data(), v1.mem->data, v1.mem->byte);

        if (v2.mem->math->code() == kyIntCode)
        {
            fourwd_int *elem = (fourwd_int*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                retuslf[i] = retuslf[i] - *elem;
        }
        else if (v2.mem->math->code() == kyFltCode)
        {
            fourwd_flt *elem = (fourwd_flt*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                retuslf[i] = retuslf[i] - *elem;
        }
        else
            kyASSERT(true, "And unknown type operations");
        return ky_variant(retuslf);
    }
    virtual ky_variant invert  (const ky_variant &v)
    {
        fourwd_vecf vecf;vecf.resize(v.mem->math->component(v));
        fourwd_flt *vd = (fourwd_flt*)v.mem->data;
        for (int i = 0; i < v.mem->math->component(v); ++i)
            vecf[i] = -vd[i];
        return ky_variant(vecf);
    }

    // 关系运算
    virtual ky_variant less  (const ky_variant &v1, const ky_variant &v2)
    {;
        if (!(v2.mem && v2.mem->math))
            return ky_variant(false);

        const int part1 = v1.mem->math->component(v1);
        const int part2 = v2.mem->math->component(v2);
        fourwd_flt *tmp = (fourwd_flt*)v1.mem->data;
        fourwd_flt sutx = 0;
        for (int i = 0; i < part1; ++i, ++tmp)
            sutx += *tmp;

        bool comp = 0;
        if (v2.mem->math->code() == kyIntCode)
        {
            fourwd_int suto = 0;
            fourwd_int *elem = (fourwd_int*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                suto += *elem;

            comp = (sutx - suto) < 0 ? true : false;
        }
        else if (v2.mem->math->code() == kyFltCode)
        {
            fourwd_flt suto = 0;
            fourwd_flt *elem = (fourwd_flt*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                suto += *elem;
            comp = (sutx - suto) < 0 ? true : false;
        }
        else
            kyASSERT(true, "And unknown type operations");

        return ky_variant(comp);
    }
    virtual ky_variant less_equal  (const ky_variant &v1, const ky_variant &v2)
    {
        if (!(v2.mem && v2.mem->math))
            return ky_variant(false);

        const int part1 = v1.mem->math->component(v1);
        const int part2 = v2.mem->math->component(v2);
        fourwd_flt *tmp = (fourwd_flt*)v1.mem->data;
        fourwd_flt sutx = 0;
        for (int i = 0; i < part1; ++i, ++tmp)
            sutx += *tmp;

        bool comp = 0;
        if (v2.mem->math->code() == kyIntCode)
        {
            fourwd_int suto = 0;
            fourwd_int *elem = (fourwd_int*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                suto += *elem;

            comp = (sutx - suto) <= 0 ? true : false;
        }
        else if (v2.mem->math->code() == kyFltCode)
        {
            fourwd_flt suto = 0;
            fourwd_flt *elem = (fourwd_flt*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                suto += *elem;
            comp = (sutx - suto) <= 0 ? true : false;
        }
        else
            kyASSERT(true, "And unknown type operations");

        return ky_variant(comp);
    }
    virtual ky_variant equal  (const ky_variant &v1, const ky_variant &v2)
    {
        if (!(v2.mem && v2.mem->math))
            return ky_variant(false);

        const int part1 = v1.mem->math->component(v1);
        const int part2 = v2.mem->math->component(v2);
        fourwd_flt *tmp = (fourwd_flt*)v1.mem->data;
        fourwd_flt sutx = 0;
        for (int i = 0; i < part1; ++i, ++tmp)
            sutx += *tmp;

        bool comp = 0;
        if (v2.mem->math->code() == kyIntCode)
        {
            fourwd_int suto = 0;
            fourwd_int *elem = (fourwd_int*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                suto += *elem;

            comp = (sutx - suto) == 0 ? true : false;
        }
        else if (v2.mem->math->code() == kyFltCode)
        {
            fourwd_flt suto = 0;
            fourwd_flt *elem = (fourwd_flt*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                suto += *elem;
            comp = (sutx - suto) == 0 ? true : false;
        }
        else
            kyASSERT(true, "And unknown type operations");

        return ky_variant(comp);
    }
    virtual ky_variant not_equal  (const ky_variant &v1, const ky_variant &v2)
    {
        if (!(v2.mem && v2.mem->math))
            return ky_variant(true);

        const int part1 = v1.mem->math->component(v1);
        const int part2 = v2.mem->math->component(v2);
        fourwd_flt *tmp = (fourwd_flt*)v1.mem->data;
        fourwd_flt sutx = 0;
        for (int i = 0; i < part1; ++i, ++tmp)
            sutx += *tmp;

        bool comp = 0;
        if (v2.mem->math->code() == kyIntCode)
        {
            fourwd_int suto = 0;
            fourwd_int *elem = (fourwd_int*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                suto += *elem;

            comp = (sutx - suto) != 0 ? true : false;
        }
        else if (v2.mem->math->code() == kyFltCode)
        {
            fourwd_flt suto = 0;
            fourwd_flt *elem = (fourwd_flt*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                suto += *elem;
            comp = (sutx - suto) != 0 ? true : false;
        }
        else
            kyASSERT(true, "And unknown type operations");

        return ky_variant(comp);
    }
    virtual ky_variant greater_equal  (const ky_variant &v1, const ky_variant &v2)
    {
        if (!(v2.mem && v2.mem->math))
            return ky_variant(false);

        const int part1 = v1.mem->math->component(v1);
        const int part2 = v2.mem->math->component(v2);
        fourwd_flt *tmp = (fourwd_flt*)v1.mem->data;
        fourwd_flt sutx = 0;
        for (int i = 0; i < part1; ++i, ++tmp)
            sutx += *tmp;

        bool comp = 0;
        if (v2.mem->math->code() == kyIntCode)
        {
            fourwd_int suto = 0;
            fourwd_int *elem = (fourwd_int*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                suto += *elem;

            comp = (sutx - suto) >= 0 ? true : false;
        }
        else if (v2.mem->math->code() == kyFltCode)
        {
            fourwd_flt suto = 0;
            fourwd_flt *elem = (fourwd_flt*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                suto += *elem;
            comp = (sutx - suto) >= 0 ? true : false;
        }
        else
            kyASSERT(true, "And unknown type operations");

        return ky_variant(comp);
    }
    virtual ky_variant greater  (const ky_variant &v1, const ky_variant &v2)
    {
        if (!(v2.mem && v2.mem->math))
            return ky_variant(false);

        const int part1 = v1.mem->math->component(v1);
        const int part2 = v2.mem->math->component(v2);
        fourwd_flt *tmp = (fourwd_flt*)v1.mem->data;
        fourwd_flt sutx = 0;
        for (int i = 0; i < part1; ++i, ++tmp)
            sutx += *tmp;

        bool comp = 0;
        if (v2.mem->math->code() == kyIntCode)
        {
            fourwd_int suto = 0;
            fourwd_int *elem = (fourwd_int*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                suto += *elem;

            comp = (sutx - suto) > 0 ? true : false;
        }
        else if (v2.mem->math->code() == kyFltCode)
        {
            fourwd_flt suto = 0;
            fourwd_flt *elem = (fourwd_flt*)v2.mem->data;
            for (int i = 0; i < part2; ++i, ++elem)
                suto += *elem;
            comp = (sutx - suto) > 0 ? true : false;
        }
        else
            kyASSERT(true, "And unknown type operations");

        return ky_variant(comp);
    }

}__s_vecf;

static struct math_str : ky_variant_math
{
    math_str()
    {
        s_str = this;
    }
    virtual fourwd_str dump(const ky_variant &v)
    {
        ky_string pm("string[");
        pm += (char*)v.mem->data;
        pm += "]";
        return fourwd_str(pm.to_latin1(), pm.count());
    }
    virtual bool to_bool(const ky_variant &v)
    {
        return v.mem->byte;
    }
    virtual int component(const ky_variant &v) {return 1;}
    virtual fourwd_t code() {return kyStrCode;}

    // 逻辑运算
    virtual ky_variant logic_not  (const ky_variant &v)
    {
        return v;
    }
    virtual ky_variant logic_and  (const ky_variant &, const ky_variant &)
    {
        kyASSERT(true, "String type cannot be logic operation");
        return ky_variant();
    }
    virtual ky_variant logic_or  (const ky_variant &, const ky_variant &)
    {
        kyASSERT(true, "String type cannot be logic operation");
        return ky_variant();
    }

    // 位运算
    virtual ky_variant bit_and  (const ky_variant &, const ky_variant &)
    {
        kyASSERT(true, "String type cannot be bitwise operation");
        return ky_variant();
    }
    virtual ky_variant bit_or  (const ky_variant &v1, const ky_variant &v2)
    {
        kyASSERT(true, "String type cannot be bitwise operation");
        return ky_variant();
    }
    virtual ky_variant bit_xor  (const ky_variant &v1, const ky_variant &v2)
    {
        kyASSERT(true, "String type cannot be bitwise operation");
        return ky_variant();
    }
    virtual ky_variant bit_negate (const ky_variant &v)
    {
        return v;
    }
    virtual ky_variant shift_left (const ky_variant &, const ky_variant &)
    {
        kyASSERT(true, "String type cannot be shift operation");
        return ky_variant();
    }
    virtual ky_variant shift_right (const ky_variant &, const ky_variant &)
    {
        kyASSERT(true, "String type cannot be shift operation");
        return ky_variant();
    }

    // 算术运算
    virtual ky_variant multiply  (const ky_variant &v1, const ky_variant &v2)
    {
        kyASSERT(true, "String type cannot be arithmetic operation");
        return ky_variant();
    }
    virtual ky_variant division  (const ky_variant &v1, const ky_variant &v2)
    {
        kyASSERT(true, "String type cannot be arithmetic operation");
        return ky_variant();
    }
    virtual ky_variant modulo  (const ky_variant &, const ky_variant &)
    {
        kyASSERT(true, "String type cannot be arithmetic operation");
        return ky_variant();
    }
    virtual ky_variant plus  (const ky_variant &, const ky_variant &)
    {
        kyASSERT(true, "String type cannot be arithmetic operation");
        return ky_variant();
    }
    virtual ky_variant minus  (const ky_variant &, const ky_variant &)
    {
        kyASSERT(true, "String type cannot be arithmetic operation");
        return ky_variant();
    }
    virtual ky_variant invert  (const ky_variant &v)
    {
        kyASSERT(true, "String type cannot be arithmetic operation");
        return ky_variant();
    }

    // 关系运算
    virtual ky_variant less  (const ky_variant &v1, const ky_variant &v2)
    {
        if (!(v2.mem && v2.mem->math))
            return ky_variant(false);

        const int len1 = v1.mem->byte;
        const int len2 = v2.mem->byte;
        const int min = len1 - len2;
        const int cmp = ky_memory::compare(v1.mem->data, v2.mem->data, min);
        if (cmp < 0 || (cmp == 0 && min < 0))
            return ky_variant(true);
        return ky_variant(false);
    }
    virtual ky_variant less_equal  (const ky_variant &v1, const ky_variant &v2)
    {
        if (!(v2.mem && v2.mem->math))
            return ky_variant(false);

        const int len1 = v1.mem->byte;
        const int len2 = v2.mem->byte;
        const int min = len1 - len2;
        const int cmp = ky_memory::compare(v1.mem->data, v2.mem->data, min);
        if (cmp <= 0 || (cmp == 0 && min <= 0))
            return ky_variant(true);
        return ky_variant(false);
    }
    virtual ky_variant equal  (const ky_variant &v1, const ky_variant &v2)
    {
        if (!(v2.mem && v2.mem->math))
            return ky_variant(false);

        const int len1 = v1.mem->byte;
        const int len2 = v2.mem->byte;
        const int min = len1 - len2;
        const int cmp = ky_memory::compare(v1.mem->data, v2.mem->data, min);
        if (cmp == 0 && min == 0)
            return ky_variant(true);
        return ky_variant(false);
    }
    virtual ky_variant not_equal  (const ky_variant &v1, const ky_variant &v2)
    {
        if (!(v2.mem && v2.mem->math))
            return ky_variant(true);

        const int len1 = v1.mem->byte;
        const int len2 = v2.mem->byte;
        const int min = len1 - len2;
        const int cmp = ky_memory::compare(v1.mem->data, v2.mem->data, min);
        if (cmp == 0 && min == 0)
            return ky_variant(false);
        return ky_variant(true);
    }
    virtual ky_variant greater_equal  (const ky_variant &v1, const ky_variant &v2)
    {
        if (!(v2.mem && v2.mem->math))
            return ky_variant(false);

        const int len1 = v1.mem->byte;
        const int len2 = v2.mem->byte;
        const int min = len1 - len2;
        const int cmp = ky_memory::compare(v1.mem->data, v2.mem->data, min);
        if (cmp >= 0 || (cmp == 0 && min >= 0))
            return ky_variant(true);
        return ky_variant(false);
    }
    virtual ky_variant greater  (const ky_variant &v1, const ky_variant &v2)
    {
        if (!(v2.mem && v2.mem->math))
            return ky_variant(false);

        const int len1 = v1.mem->byte;
        const int len2 = v2.mem->byte;
        const int min = len1 - len2;
        const int cmp = ky_memory::compare(v1.mem->data, v2.mem->data, min);
        if (cmp > 0 || (cmp == 0 && min > 0))
            return ky_variant(true);
        return ky_variant(false);
    }

}__s_str;
