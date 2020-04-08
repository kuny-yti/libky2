
/**
 * Basic tool library
 * Copyright (C) 2014 kunyang kunyang.yk@gmail.com
 *
 * @file     ky_variant.h
 * @brief    数据类型的变种类型
 *
 * @author   kunyang
 * @email    kunyang.yk@gmail.com
 * @version  1.1.0.1
 * @date     2015/04/08
 * @license  GNU General Public License (GPL)
 *
 * Change History :
 *    Date    |  Version  |  Author  |   Description
 * 2015/04/08 | 1.0.0.1   | kunyang  | 创建文件
 * 2015/07/23 | 1.0.1.1   | kunyang  | 修改字符串存储
 * 2016/03/08 | 1.0.2.1   | kunyang  | 修改>>、<<符号操作
 * 2018/02/18 | 1.1.0.1   | kunyang  | 重构类并独立
 * 2019/04/28 | 1.2.0.1   | kunyang  | 添加基础类型和四字码标注
 */
#ifndef KY_VARIANT_H
#define KY_VARIANT_H

#include "ky_type.h"
#include "ky_array.h"
#include "ky_typeinfo.h"

// 基础类型四字码
const fourwd_t kyIntCode = kyFourWord(ix64);
const fourwd_t kyFltCode = kyFourWord(fx64);
const fourwd_t kyStrCode = kyFourWord(strx);
const fourwd_t kyVecIntCode = kyFourWord(iv64);
const fourwd_t kyVecFltCode = kyFourWord(fv64);
typedef int64 fourwd_int ;
typedef flt64 fourwd_flt ;
typedef ky_array<fourwd_int> fourwd_veci;
typedef ky_array<fourwd_flt> fourwd_vecf;
typedef ky_array<char> fourwd_str;

/*!
 * @brief The ky_variant struct 解析器获取的数据
 * @struct ky_variant
 */
typedef struct ky_variant
{
    struct layout : ky_ref
    {
        fourwd_t                code;
        struct ky_variant_math* math;
        uint8                 * data;
        int                     byte;
    } *mem;

    virtual ~ky_variant();
    ky_variant();

    ky_variant(const ky_variant &pd);
    ky_variant &operator = (const ky_variant &pd);

    void swap(ky_variant &b){layout *tmp = mem; mem = b.mem; b.mem = tmp;}
    friend void ky_swap(ky_variant &a, ky_variant &b) {a.swap(b);}

    // 基础类型
    friend ky_variant & operator << (ky_variant &va, bool v)
    {
        va = ky_variant(v);
        return va;
    }
    friend ky_variant & operator << (ky_variant & va, fourwd_int v)
    {
        va = ky_variant(v);
        return va;
    }
    friend ky_variant & operator << (ky_variant & va, fourwd_flt v)
    {
        va = ky_variant(v);
        return va;
    }

    friend ky_variant & operator >> (ky_variant &va, bool & v)
    {
        const fourwd_int it = va.get<fourwd_int>(kyIntCode);
        v = it;
        return va;
    }
    friend ky_variant & operator >> (ky_variant &va, fourwd_int & v)
    {
        const fourwd_int it = va.get<fourwd_int>(kyIntCode);
        v = it;
        return va;
    }
    friend ky_variant & operator >> (ky_variant &va, fourwd_flt & v)
    {
        const fourwd_flt it = va.get<fourwd_flt>(kyFltCode);
        v = it;
        return va;
    }

    // 衍生类型
    friend ky_variant & operator << (ky_variant &va, bool_t v)
    {
        bool s = v; va << s; return va;
    }
    friend ky_variant & operator << (ky_variant &va, int32 v)
    {
        fourwd_int s = v;va << s;return va;
    }

    explicit ky_variant(fourwd_int v);
    explicit ky_variant(fourwd_flt v);
    explicit ky_variant(const fourwd_str &v);
    explicit ky_variant(const fourwd_veci &v);
    explicit ky_variant(const fourwd_vecf &v);

    explicit ky_variant(bool_t v):mem(0){*this = ky_variant(fourwd_int(v));}
    explicit ky_variant(bool v):mem(0){*this = ky_variant(fourwd_int(v));}
    explicit ky_variant(int32 v):mem(0){*this = ky_variant(fourwd_int(v));}
    explicit ky_variant(flt16 v):mem(0){*this = ky_variant(fourwd_flt(v));}
    explicit ky_variant(flt32 v):mem(0){*this = ky_variant(fourwd_flt(v));}

    //!
    //! \brief ky_variant 构造变种类型数据
    //! \param code 四字码
    //! \param math 数学运算对象
    //! \param data 数据指针
    //! \param byte 数据长度
    //!
    ky_variant(const fourwd_t code, struct ky_variant_math *math, void *data, int byte);

    //!
    //! \brief set 设置变种类型数据
    //! \param code 四字代码
    //! \param math 处理运算的对象
    //! \param data 数据指针
    //! \param byte 数字字节
    //!
    void set(const fourwd_t code, struct ky_variant_math *math, void *data, int byte);
    //!
    //! \brief is_set 是否被设置为特定类型
    //! \param code
    //! \return
    //!
    bool is_set(const fourwd_t code)const;
    //!
    //! \brief is_valid 是否有效数据
    //! \return
    //!
    bool is_valid() const;

    //!
    //! \brief get 根据四字码和模板类型，转为特定对象
    //! \param code
    //! \return
    //!
    template<typename T>
    T get(fourwd_t code)const
    {
        if (is_set(code) && (kyStrCode != code))
            return T(*((T*)mem->data));
        return T();
    }
    template<typename T> T to(int code)const{return get<T>(code);}

    //!
    //! \brief to_str 转换到字符串
    //! \param v
    //! \return
    //!
    fourwd_str to_str()const;

    //!
    //! \brief to_veci
    //! \return
    //!
    fourwd_veci to_veci()const;
    //!
    //! \brief to_vecf
    //! \return
    //!
    fourwd_vecf to_vecf()const;

    //!
    //! \brief dump
    //! \return
    //!
    fourwd_str dump()const;

    //!
    //! \brief operator bool 布尔测试
    //!
    operator bool() const;

    // 关系运算
    friend ky_variant operator==(const ky_variant &v, const ky_variant &v1) ;
    friend ky_variant operator!=(const ky_variant &v, const ky_variant &v1) ;
    friend ky_variant operator<(const ky_variant &v, const ky_variant &v1) ;
    friend ky_variant operator<=(const ky_variant &v, const ky_variant &v1) ;
    friend ky_variant operator>=(const ky_variant &v, const ky_variant &v1) ;
    friend ky_variant operator>(const ky_variant &v, const ky_variant &v1) ;

    // 算术运算
    friend ky_variant operator+(const ky_variant &v, const ky_variant &v1) ;
    friend ky_variant operator-(const ky_variant &v, const ky_variant &v1) ;
    friend ky_variant operator*(const ky_variant &v, const ky_variant &v1) ;
    friend ky_variant operator/(const ky_variant &v, const ky_variant &v1) ;
    friend ky_variant operator%(const ky_variant &v, const ky_variant &v1) ;
    friend ky_variant operator-(const ky_variant &v) ;

    // 位运算
    friend ky_variant operator&(const ky_variant &v, const ky_variant &v1) ;
    friend ky_variant operator|(const ky_variant &v, const ky_variant &v1) ;
    friend ky_variant operator^(const ky_variant &v, const ky_variant &v1) ;
    friend ky_variant operator~(const ky_variant &v) ;

    friend ky_variant operator<<(const ky_variant &v, const ky_variant &v1) ;
    friend ky_variant operator>>(const ky_variant &v, const ky_variant &v1) ;

    // 逻辑运算
    friend ky_variant operator!(const ky_variant &v) ;
    friend ky_variant operator&&(const ky_variant &v, const ky_variant &v1) ;
    friend ky_variant operator||(const ky_variant &v, const ky_variant &v1) ;

}ky_variant;

//!
//! \brief The ky_variant_math struct
//! \note 设计用于变种类型进行数学运算
//!
struct ky_variant_math
{
    virtual ~ky_variant_math(){}

    //!
    //! \brief dump 形成字符串序列
    //! \param v
    //! \return
    //!
    virtual fourwd_str dump(const ky_variant &v) = 0;

    //!
    //! \brief to_bool 转换到bool
    //! \param v
    //! \return
    //!
    virtual bool to_bool(const ky_variant &v) =0;

    //!
    //! \brief component 返回用于运算的元素数
    //! \return
    //!
    virtual int component(const ky_variant &v) = 0;

    //!
    //! \brief code 基础四字码
    //! \return
    //!
    virtual fourwd_t code() = 0;

    // 逻辑运算
    virtual ky_variant logic_not  (const ky_variant &v) =0;
    virtual ky_variant logic_and  (const ky_variant &v1, const ky_variant &v2) =0;
    virtual ky_variant logic_or  (const ky_variant &v1, const ky_variant &v2) =0;

    // 位运算
    virtual ky_variant bit_and  (const ky_variant &v1, const ky_variant &v2) =0;
    virtual ky_variant bit_or  (const ky_variant &v1, const ky_variant &v2) =0;
    virtual ky_variant bit_xor  (const ky_variant &v1, const ky_variant &v2) =0;
    virtual ky_variant bit_negate (const ky_variant &v) =0;
    virtual ky_variant shift_left (const ky_variant &v1, const ky_variant &v2) =0;
    virtual ky_variant shift_right (const ky_variant &v1, const ky_variant &v2) =0;

    // 算术运算
    virtual ky_variant multiply  (const ky_variant &v1, const ky_variant &v2) =0;
    virtual ky_variant division  (const ky_variant &v1, const ky_variant &v2) =0;
    virtual ky_variant modulo  (const ky_variant &v1, const ky_variant &v2) =0;
    virtual ky_variant plus  (const ky_variant &v1, const ky_variant &v2) =0;
    virtual ky_variant minus  (const ky_variant &v1, const ky_variant &v2) =0;
    virtual ky_variant invert  (const ky_variant &v) =0;

    // 关系运算
    virtual ky_variant less  (const ky_variant &v1, const ky_variant &v2) =0;
    virtual ky_variant less_equal  (const ky_variant &v1, const ky_variant &v2) =0;
    virtual ky_variant equal  (const ky_variant &v1, const ky_variant &v2) =0;
    virtual ky_variant not_equal  (const ky_variant &v1, const ky_variant &v2) =0;
    virtual ky_variant greater_equal  (const ky_variant &v1, const ky_variant &v2) =0;
    virtual ky_variant greater  (const ky_variant &v1, const ky_variant &v2) =0;
} ;

#endif
