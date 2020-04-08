#ifndef SIGNAL_INL_H
#define SIGNAL_INL_H

#include "ky_list.h"

template <typename OutObject, typename InObject>
inline OutObject implicit_cast(InObject in){return in;}

template <typename OutObject, typename InObject>
union horrible_union{OutObject out;InObject in;};

template <typename OutObject, typename InObject>
inline OutObject horrible_cast(const InObject input)
{
    horrible_union<OutObject, InObject> us;
    typedef int ERROR_horrible_cast[
            sizeof(InObject) == sizeof(us) &&
            sizeof(InObject) == sizeof(OutObject)
            ? 1 : -1];
    us.in = input;
    return us.out;
}

class GenericObject;
const int GenericObjectMethodSize = sizeof(void (GenericObject::*)());

template <int N>
struct SimplifyMethod
{
    template <typename Object, typename ObjectMethodType, typename GenericMethodType>
    inline static GenericObject *convert(Object *, ObjectMethodType, GenericMethodType &)
    {
        typedef char ERROR_Method[N - 64];
        return 0;
    }
};
template <>
struct SimplifyMethod<GenericObjectMethodSize>
{
    template <typename Object, typename ObjectMethodType , typename GenericMethodType>
    inline static GenericObject *convert(Object *p,
                                         ObjectMethodType function_to_bind,
                                         GenericMethodType &bound_func)
    {

        bound_func = reinterpret_cast<GenericMethodType>(function_to_bind);
        return reinterpret_cast<GenericObject *>(p);
    }
};

class DelegateData
{
protected:
    typedef void (GenericObject::*GenericMethodType)();
    GenericObject *_object;
    GenericMethodType _method;

public:
    DelegateData() : _object(0), _method(0) {}
    explicit DelegateData (const DelegateData &right):
        _object(right._object),
        _method(right._method){}
    inline DelegateData & operator = (const DelegateData &right)
    {
        _method = right._method;
        _object = right._object;
        return *this;
    }

    inline GenericObject *object() const { return _object; }
    inline GenericMethodType method() const { return (_method); }

public:
    inline bool operator == (const DelegateData &x) const
    {
        return _object == x._object && _method == x._method;
    }
    inline bool operator < (const DelegateData &right) const
    {
        if (_object != right._object)
            return _object < right._object;

        return ::memcmp(&_method, &right._method, sizeof(_method)) < 0;

    }
    inline bool operator > (const DelegateData &right)const{return right < (*this);}
    inline bool is_empty() const{return _object == 0 && _method==0;}
    inline void clear() {_object = 0; _method = 0;}
};

template < typename GenericMethod, typename StaticMethod, typename UnvoidStaticMethod>
class DelegateMethod : public DelegateData
{
public:
    template < typename Object, typename ObjectMethod >
    inline void bind_method(Object *p, ObjectMethod function_to_bind )
    {
        _object = SimplifyMethod< sizeof(function_to_bind) >
                ::convert(p, function_to_bind, _method);
    }

    template < typename Object, typename ObjectMethod>
    inline void bind_method_const(const Object *p, ObjectMethod function_to_bind)
    {
        _object= SimplifyMethod< sizeof(function_to_bind) >
            ::convert(const_cast<Object*>(p), function_to_bind, _method);

    }

    template < typename Object, typename ObjectMethod>
    inline void bind_method(const Object *p, ObjectMethod function_to_bind)
    {
        bind_method_const(p, function_to_bind);
    }

    template <	typename DerivedObject, typename ParentInvokerSig>
    inline void bind_method_static(DerivedObject *parent, ParentInvokerSig static_function_invoker,
                                   StaticMethod function_to_bind)
    {
        if (function_to_bind == 0)
            _method = 0;
        else
            bind_method(parent, static_function_invoker);

        typedef int ERROR_Method[sizeof(GenericObject *) == sizeof(function_to_bind) ? 1 : -1];
        _object = horrible_cast<GenericObject *>(function_to_bind);
     }

    template< typename DerivedObject >
    inline void copy (DerivedObject *parent, const DelegateData &right)
    {
        (void)parent;
        _method = right.method();
        _object = right.object();
    }

    inline GenericMethod method() const { return reinterpret_cast<GenericMethod>(DelegateData::method ()); }

    inline UnvoidStaticMethod method_static() const
    {
        typedef int ERROR_Method[sizeof(UnvoidStaticMethod) == sizeof(this) ? 1 : -1];
        return horrible_cast<UnvoidStaticMethod>(this);
    }

    inline bool IsEqualStaticMethod(StaticMethod funcptr)
    {
        if (funcptr == 0)
            return is_empty();
        else
            return funcptr == reinterpret_cast<StaticMethod> (method_static ());
    }
};

#define defParamARG(...) __VA_ARGS__
#define defDelegate(oNAME, PA, PB) \
    class oNAME \
    { \
    private:\
        typedef RetType (*StaticMethod)(PA);\
        typedef RetType (*UnvoidStaticMethod)(PA);\
        typedef RetType (GenericObject::*GenericMethod)(PA);\
        typedef DelegateMethod<GenericMethod, StaticMethod, UnvoidStaticMethod> DelegateMethodType;\
        DelegateMethodType data;\
    public:\
        typedef oNAME type;\
        oNAME() {clear();} \
        explicit oNAME(const oNAME &x) {data.copy(this, x.data);} \
        inline void operator = (const oNAME &x)  {data.copy(this, x.data); } \
        inline bool operator ==(const oNAME &x) const {return data ==(x.data);	} \
        inline bool operator !=(const oNAME &x) const {return data !=(x.data); } \
        inline bool operator <(const oNAME &x) const {return data < (x.data);	} \
        inline bool operator >(const oNAME &x) const {return data > (x.data);	} \
        template < typename X, typename Y > \
        oNAME(Y *p, RetType (X::* function_to_bind)(PA) ) \
        { \
            data.bind_method(implicit_cast<X*>(p), function_to_bind); \
        }\
        template < typename X, typename Y > \
        inline void bind(Y *p, RetType (X::* function_to_bind)(PA)) \
        {\
            data.bind_method(implicit_cast<X*>(p), function_to_bind);\
        }\
        template < typename X, typename Y >\
        oNAME(const Y *p, RetType (X::* function_to_bind)(PA) const) \
        { \
            data.bind_method_const(implicit_cast<const X*>(p), function_to_bind);\
        }\
        template < typename X, typename Y > \
        inline void bind(const Y *p, RetType (X::* function_to_bind)(PA) const) \
        { \
            data.bind_method_const(implicit_cast<const X *>(p), function_to_bind); \
        } \
        explicit oNAME(RetType (*function_to_bind)(PA) ) {bind(function_to_bind);} \
        inline void operator = (RetType (*function_to_bind)(PA)) \
        {bind(function_to_bind);} \
        inline void bind(RetType (*function_to_bind)(PA)) \
        { \
            data.bind_method_static(this, &oNAME::InvokeMethodStatic, \
                                    function_to_bind); \
        } \
    private:\
        typedef struct SafeBoolStruct {int data_pointer;StaticMethod m_nonzero;} UselessTypedef; \
        typedef StaticMethod SafeBoolStruct::*unspecified_bool_type; \
    public: \
        inline operator unspecified_bool_type() const \
        { \
            return is_empty()? 0: &SafeBoolStruct::m_nonzero; \
        } \
        inline bool operator == (StaticMethod funcptr) \
        { \
            return data == StaticMethod(funcptr); \
        } \
        inline bool operator != (StaticMethod funcptr) \
        { \
            return data != StaticMethod(funcptr); \
        } \
        inline bool is_empty() const{return !data; }\
        inline void clear() { data.clear();} \
    private:	\
        inline RetType InvokeMethodStatic(PA) const\
        { \
            return (*(data.method_static()))(PB); \
        } \
    public: \
        inline RetType operator() (PA) const \
        {\
            return (data.object()->*(data.method()))(PB); \
        }\
    };

template <typename RetType = void>
class Delegate0
{
private:
    typedef RetType (*StaticMethod)();
    typedef RetType (*UnvoidStaticMethod)();
    typedef RetType (GenericObject::*GenericMethod)();
    typedef DelegateMethod<GenericMethod, StaticMethod, UnvoidStaticMethod> DelegateMethodType;
    DelegateMethodType data;

public:
    typedef Delegate0 type;

    Delegate0() { clear(); }
    explicit Delegate0(const Delegate0 &x) {data.copy(this, x.data);}
    inline void operator = (const Delegate0 &x)  {data.copy(this, x.data);}
    inline bool operator == (const Delegate0 &x) const {return data == x.data;}
    inline bool operator != (const Delegate0 &x) const {return data != x.data;}
    inline bool operator < (const Delegate0 &x) const {return data < x.data;}
    inline bool operator > (const Delegate0 &x) const {return data > x.data;}

    template < typename X, typename Y >
    Delegate0(Y *p, RetType (X::* function_to_bind)())
    {
        data.bind_method(implicit_cast<X*>(p), function_to_bind);
    }

    template < typename X, typename Y >
    inline void bind(Y *p, RetType (X::* function_to_bind)())
    {
        data.bind_method(implicit_cast<X*>(p), function_to_bind);
    }

    template < typename X, typename Y >
    Delegate0(const Y *p, RetType (X::* function_to_bind)() const)
    {
        data.bind_method_const(implicit_cast<const X*>(p), function_to_bind);
    }

    template < typename X, typename Y >
    inline void bind(const Y *p, RetType (X::* function_to_bind)() const)
    {
        data.bind_method_const(implicit_cast<const X *>(p), function_to_bind);
    }

    Delegate0(RetType (*function_to_bind)() )
    {
        bind(function_to_bind);
    }

    inline void operator = (RetType (*function_to_bind)() )
    {
        bind(function_to_bind);
    }
    inline void bind(RetType (*function_to_bind)())
    {
        data.bind_method_static(this, &Delegate0::InvokeMethodStatic,
                                   function_to_bind);
    }

    inline RetType operator() () const {return (data.object ()->*(data.method ()))();}

private:
    typedef struct SafeBoolStruct {
        int a_data_pointer_to_this_is_0_on_buggy_compilers;
        StaticMethod m_nonzero;
    } UselessTypedef;
    typedef StaticMethod SafeBoolStruct::*unspecified_bool_type;

public:
    operator unspecified_bool_type() const
    {
        return is_empty() ? 0: &SafeBoolStruct::m_nonzero;
    }

    inline bool operator == (StaticMethod funcptr)
    {
        return data ==StaticMethod (funcptr);
    }
    inline bool operator != (StaticMethod funcptr)
    {
        return data != StaticMethod(funcptr);
    }

    inline bool is_empty() const	{return data.is_empty (); }
    inline void clear() { data.clear();}

private:
    inline RetType InvokeMethodStatic() const{return (*(data.method_static ()))();}
};

template<typename Param0, typename RetType = void>
defDelegate(Delegate1,
            defParamARG(Param0 p0),
            defParamARG(p0))

template<typename Param0, typename Param1, typename RetType = void>
defDelegate(Delegate2,
            defParamARG(Param0 p0, Param1 p1),
            defParamARG(p0, p1))

template<typename Param0, typename Param1, typename Param2, typename RetType = void>
defDelegate(Delegate3,
            defParamARG(Param0 p0, Param1 p1, Param2 p2),
            defParamARG(p0, p1, p2))

template<typename Param0, typename Param1, typename Param2, typename Param3, typename RetType = void>
defDelegate(Delegate4,
            defParamARG(Param0 p0,Param1 p1, Param2 p2, Param3 p3),
            defParamARG(p0, p1, p2, p3))

template<typename Param0, typename Param1, typename Param2, typename Param3, typename Param4,
         typename RetType = void>
defDelegate(Delegate5,
            defParamARG(Param0 p0,Param1 p1, Param2 p2, Param3 p3, Param4 p4),
            defParamARG(p0, p1, p2, p3, p4))

template<typename Param0, typename Param1, typename Param2, typename Param3, typename Param4,
         typename Param5, typename RetType = void>
defDelegate(Delegate6,
            defParamARG(Param0 p0, Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5),
            defParamARG(p0, p1, p2, p3, p4, p5))

template<typename Param0, typename Param1, typename Param2, typename Param3, typename Param4,
         typename Param5, typename Param6, typename RetType = void>
defDelegate(Delegate7,
            defParamARG(Param0 p0, Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6),
            defParamARG(p0, p1, p2, p3, p4, p5, p6))

template<typename Param0, typename Param1, typename Param2, typename Param3, typename Param4,
         typename Param5, typename Param6, typename Param7, typename RetType = void>
defDelegate(Delegate8,
            defParamARG(Param0 p0, Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5,
                        Param6 p6, Param7 p7),
            defParamARG(p0, p1, p2, p3, p4, p5, p6, p7))
template<typename Param0, typename Param1, typename Param2, typename Param3, typename Param4,
         typename Param5, typename Param6, typename Param7, typename Param8, typename RetType = void>
defDelegate(Delegate9,
            defParamARG(Param0 p0, Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5,
                        Param6 p6, Param7 p7, Param8 p8),
            defParamARG(p0, p1, p2, p3, p4, p5, p6, p7, p8))

template <typename Signature>
class Signal;

template<typename Ret >
class Signal <Ret()>
{
public:
    typedef Delegate0<Ret> SlotType;

private:
    typedef ky_list<SlotType> SlotList;
    typedef typename SlotList::const_iterator SlotIterator;
    SlotList list_slot;

public:
    inline void connect( const SlotType &slot )
    {
        list_slot.push_back (slot);
    }

    inline void connect (Ret (*method)())
    {
        SlotType slt(method);
        connect(slt);
    }
    template< typename X, typename Y >
    inline void connect( Y * obj, Ret (X::*method)() )
    {
        SlotType slt(obj, method);
        connect(slt);
    }

    template< typename X, typename Y >
    inline void connect( Y * obj, Ret (X::*method)() const )
    {
        SlotType slt(obj, method);
        connect(slt);
    }

    inline void disconnect(const SlotType &slot )
    {
        list_slot.remove(list_slot.find(slot));
    }

    inline void disconnect (Ret (*method)())
    {
        SlotType slt(method);
        connect(slt);
    }
    template< typename X, typename Y >
    inline void disconnect( Y * obj, Ret (X::*method)() )
    {
        SlotType slt(obj, method);
        disconnect(slt);
    }

    template< typename X, typename Y >
    inline void disconnect( Y * obj, Ret (X::*method)() const )
    {
        SlotType slt(obj, method);
        disconnect(slt);
    }

public:
    inline void clear(){list_slot.clear();}

    inline void emit() const
    {
        for (SlotIterator ite = list_slot.begin(); ite != list_slot.end(); ++ite)
            (*ite)();
    }

    inline void operator() () const{emit();}

    inline bool is_empty() const
    {
        return list_slot.empty();
    }

    template< typename X, typename Y >
    friend void connect(Signal<Ret ()> *sig, Y* x, Ret (X::*func)() const)
    {
        sig->connect(x, func);
    }
    template< typename X, typename Y >
    friend void connect(Signal<Ret ()> *sig, Y* x, Ret (X::*func)())
    {
        sig->connect(x, func);
    }
    friend void connect(Signal<Ret ()> *sig, Ret (*func)())
    {
        sig->connect(func);
    }
    template< typename X, typename Y >
    friend void disconnect(Signal<Ret ()> *sig, Y* x, Ret (X::*func)() const)
    {
        sig->disconnect(x, func);
    }
    template< typename X, typename Y >
    friend void disconnect(Signal<Ret ()> *sig, Y* x, Ret (X::*func)())
    {
        sig->disconnect(x, func);
    }
    friend void disconnect(Signal<Ret ()> *sig, Ret (*func)())
    {
        sig->disconnect(func);
    }
};

template<typename Ret,typename Param0 >
class Signal <Ret(Param0)>
{
public:
    typedef Delegate1<Param0, Ret> SlotType;

private:
    typedef ky_list<SlotType> SlotList;
    typedef typename SlotList::const_iterator SlotIterator;
    SlotList list_slot;

public:
    inline void connect(const SlotType &slot )
    {
        list_slot.push_back (slot);
    }

    inline void connect (Ret (*method)( Param0))
    {
        SlotType slt(method);
        connect(slt);
    }
    template< typename X, typename Y >
    inline void connect( Y * obj, Ret (X::*method)(Param0) )
    {
        SlotType slt(obj, method);
        connect(slt);
    }

    template< typename X, typename Y >
    inline void connect( Y * obj, Ret (X::*method)(Param0) const )
    {
        SlotType slt(obj, method);
        connect(slt);
    }

    inline void disconnect(const SlotType &slot )
    {
        list_slot.remove(list_slot.find(slot));
    }

    inline void disconnect (Ret (*method)( Param0))
    {
        SlotType slt(method);
        connect(slt);
    }
    template< typename X, typename Y >
    inline void disconnect( Y * obj, Ret (X::*method)(Param0) )
    {
        SlotType slt(obj, method);
        disconnect(slt);
    }

    template< typename X, typename Y >
    inline void disconnect( Y * obj, Ret (X::*method)(Param0) const )
    {
        SlotType slt(obj, method);
        disconnect(slt);
    }

public:
    inline void clear(){list_slot.clear();}

    inline void emit(Param0 p0) const
    {
        for (SlotIterator ite = list_slot.begin(); ite != list_slot.end(); ++ite)
            (*ite)(p0);
    }

    inline void operator() (Param0 p0) const{emit(p0);}

    inline bool is_empty() const
    {
        return list_slot.empty();
    }

    template< typename X, typename Y >
    friend void connect(Signal<Ret (Param0)> *sig, Y* x, Ret (X::*func)(Param0) const)
    {
        sig->connect(x, func);
    }
    template< typename X, typename Y >
    friend void connect(Signal<Ret (Param0)> *sig, Y* x, Ret (X::*func)(Param0))
    {
        sig->connect(x, func);
    }
    friend void connect(Signal<Ret (Param0)> *sig, Ret (*func)(Param0))
    {
        sig->connect(func);
    }
    template< typename X, typename Y >
    friend void disconnect(Signal<Ret (Param0)> *sig, Y* x, Ret (X::*func)(Param0) const)
    {
        sig->disconnect(x, func);
    }
    template< typename X, typename Y >
    friend void disconnect(Signal<Ret (Param0)> *sig, Y* x, Ret (X::*func)(Param0))
    {
        sig->disconnect(x, func);
    }
    friend void disconnect(Signal<Ret (Param0)> *sig, Ret (*func)(Param0))
    {
        sig->disconnect(func);
    }

};

template<typename Ret,typename Param0, typename Param1 >
class Signal <Ret(Param0, Param1)>
{
public:
    typedef Delegate2<Param0, Param1, Ret> SlotType;

private:
    typedef ky_list<SlotType> SlotList;
    typedef typename SlotList::const_iterator SlotIterator;
    SlotList list_slot;

public:
    inline void connect(const SlotType &slot )
    {
        list_slot.push_back (slot);
    }

    inline void connect (Ret (*method)( Param0, Param1 ))
    {
        SlotType slt(method);
        connect(slt);
    }
    template< typename X, typename Y >
    inline void connect( Y * obj, Ret (X::*method)(Param0, Param1) )
    {
        SlotType slt(obj, method);
        connect(slt);
    }

    template< typename X, typename Y >
    inline void connect( Y * obj, Ret (X::*method)(Param0, Param1) const )
    {
        SlotType slt(obj, method);
        connect(slt);
    }

    inline void disconnect(const SlotType &slot )
    {
        list_slot.remove(list_slot.find(slot));
    }

    inline void disconnect (Ret (*method)( Param0, Param1 ))
    {
        SlotType slt(method);
        disconnect(slt);
    }
    template< typename X, typename Y >
    inline void disconnect( Y * obj, Ret (X::*method)(Param0, Param1) )
    {
        SlotType slt(obj, method);
        disconnect(slt);
    }

    template< typename X, typename Y >
    inline void disconnect( Y * obj, Ret (X::*method)(Param0, Param1) const )
    {
        SlotType slt(obj, method);
        disconnect(slt);
    }

public:
    inline void clear(){list_slot.clear();}

    inline void emit(Param0 p0, Param1 p1) const
    {
        for (SlotIterator ite = list_slot.begin(); ite != list_slot.end(); ++ite)
            (*ite)(p0, p1);
    }

    inline void operator() (Param0 p0, Param1 p1) const{emit(p0, p1);}

    inline bool is_empty() const
    {
        return list_slot.empty();
    }

    template< typename X, typename Y >
    friend void connect(Signal<Ret (Param0, Param1)> *sig, Y* x, Ret (X::*func)(Param0, Param1) const)
    {
        sig->connect(x, func);
    }
    template< typename X, typename Y >
    friend void connect(Signal<Ret (Param0, Param1)> *sig, Y* x, Ret (X::*func)(Param0, Param1))
    {
        sig->connect(x, func);
    }
    friend void connect(Signal<Ret (Param0, Param1)> *sig, Ret (*func)(Param0, Param1))
    {
        sig->connect(func);
    }
    template< typename X, typename Y >
    friend void disconnect(Signal<Ret (Param0, Param1)> *sig, Y* x, Ret (X::*func)(Param0, Param1) const)
    {
        sig->disconnect(x, func);
    }
    template< typename X, typename Y >
    friend void disconnect(Signal<Ret (Param0, Param1)> *sig, Y* x, Ret (X::*func)(Param0, Param1))
    {
        sig->disconnect(x, func);
    }
    friend void disconnect(Signal<Ret (Param0, Param1)> *sig, Ret (*func)(Param0, Param1))
    {
        sig->disconnect(func);
    }

};

template<typename Ret,typename Param0, typename Param1, typename Param2 >
class Signal <Ret(Param0, Param1, Param2)>
{
public:
    typedef Delegate3<Param0, Param1,Param2, Ret> SlotType;

private:
    typedef ky_list<SlotType> SlotList;
    typedef typename SlotList::const_iterator SlotIterator;
    SlotList list_slot;

public:
    inline void connect(const SlotType &slot )
    {
        list_slot.push_back (slot);
    }

    inline void connect (Ret (*method)( Param0, Param1, Param2 ))
    {
        SlotType slt(method);
        connect(slt);
    }
    template< typename X, typename Y >
    inline void connect( Y * obj, Ret (X::*method)(Param0, Param1, Param2) )
    {
        SlotType slt(obj, method);
        connect(slt);
    }

    template< typename X, typename Y >
    inline void connect( Y * obj, Ret (X::*method)(Param0, Param1, Param2) const )
    {
        SlotType slt(obj, method);
        connect(slt);
    }

    inline void disconnect(const SlotType &slot )
    {
        list_slot.remove(list_slot.find(slot));
    }

    inline void disconnect (Ret (*method)( Param0 , Param1 , Param2 ))
    {
        SlotType slt(method);
        disconnect(slt);
    }
    template< typename X, typename Y >
    inline void disconnect( Y * obj, Ret (X::*method)(Param0, Param1, Param2 ) )
    {
        SlotType slt(obj, method);
        disconnect(slt);
    }

    template< typename X, typename Y >
    inline void disconnect( Y * obj, Ret (X::*method)(Param0, Param1, Param2) const )
    {
        SlotType slt(obj, method);
        disconnect(slt);
    }
public:
    inline void clear(){list_slot.clear();}

    inline void emit(Param0 p0, Param1 p1, Param2 p2) const
    {
        for (SlotIterator ite = list_slot.begin(); ite != list_slot.end(); ++ite)
            (*ite)(p0, p1, p2);
    }

    inline void operator() (Param0 p0, Param1 p1, Param2 p2) const{emit(p0, p1, p2);}

    inline bool is_empty() const
    {
        return list_slot.empty();
    }

    template< typename X, typename Y >
    friend void connect(Signal<Ret (Param0, Param1, Param2)> *sig, Y* x,
                        Ret (X::*func)(Param0, Param1, Param2) const)
    {
        sig->connect(x, func);
    }
    template< typename X, typename Y >
    friend void connect(Signal<Ret (Param0, Param1, Param2)> *sig, Y* x,
                        Ret (X::*func)(Param0, Param1, Param2))
    {
        sig->connect(x, func);
    }
    friend void connect(Signal<Ret (Param0, Param1, Param2)> *sig,
                        Ret (*func)(Param0, Param1, Param2))
    {
        sig->connect(func);
    }
    template< typename X, typename Y >
    friend void disconnect(Signal<Ret (Param0, Param1, Param2)> *sig, Y* x,
                           Ret (X::*func)(Param0, Param1, Param2) const)
    {
        sig->disconnect(x, func);
    }
    template< typename X, typename Y >
    friend void disconnect(Signal<Ret (Param0, Param1, Param2)> *sig, Y* x,
                           Ret (X::*func)(Param0, Param1, Param2))
    {
        sig->disconnect(x, func);
    }
    friend void disconnect(Signal<Ret (Param0, Param1, Param2)> *sig,
                           Ret (*func)(Param0, Param1, Param2))
    {
        sig->disconnect(func);
    }

};

template<typename Ret,typename Param0, typename Param1, typename Param2, typename Param3 >
class Signal <Ret(Param0, Param1, Param2, Param3)>
{
public:
    typedef Delegate4<Param0, Param1,Param2, Param3, Ret> SlotType;

private:
    typedef ky_list<SlotType> SlotList;
    typedef typename SlotList::const_iterator SlotIterator;
    SlotList list_slot;

public:
    inline void connect(const SlotType &slot )
    {
        list_slot.push_back (slot);
    }

    inline void connect (Ret (*method)( Param0, Param1, Param2 ,Param3))
    {
        SlotType slt(method);
        connect(slt);
    }
    template< typename X, typename Y >
    inline void connect( Y * obj, Ret (X::*method)(Param0, Param1, Param2, Param3) )
    {
        SlotType slt(obj, method);
        connect(slt);
    }

    template< typename X, typename Y >
    inline void connect( Y * obj, Ret (X::*method)(Param0, Param1, Param2, Param3) const )
    {
        SlotType slt(obj, method);
        connect(slt);
    }

    inline void disconnect(const SlotType &slot )
    {
        list_slot.remove(list_slot.find(slot));
    }

    inline void disconnect (Ret (*method)( Param0 , Param1 , Param2,Param3 ))
    {
        SlotType slt(method);
        disconnect(slt);
    }
    template< typename X, typename Y >
    inline void disconnect( Y * obj, Ret (X::*method)(Param0, Param1, Param2,Param3 ) )
    {
        SlotType slt(obj, method);
        disconnect(slt);
    }

    template< typename X, typename Y >
    inline void disconnect( Y * obj, Ret (X::*method)(Param0, Param1, Param2,Param3) const )
    {
        SlotType slt(obj, method);
        disconnect(slt);
    }
public:
    inline void clear(){list_slot.clear();}

    inline void emit(Param0 p0, Param1 p1, Param2 p2, Param3 p3) const
    {
        for (SlotIterator ite = list_slot.begin(); ite != list_slot.end(); ++ite)
            (*ite)(p0, p1, p2, p3);
    }

    inline void operator() (Param0 p0, Param1 p1, Param2 p2, Param3 p3) const{emit(p0, p1, p2, p3);}

    inline bool is_empty() const
    {
        return list_slot.empty();
    }

    template< typename X, typename Y >
    friend void connect(Signal<Ret (Param0, Param1, Param2, Param3)> *sig, Y* x,
                        Ret (X::*func)(Param0, Param1, Param2, Param3) const)
    {
        sig->connect(x, func);
    }
    template< typename X, typename Y >
    friend void connect(Signal<Ret (Param0, Param1, Param2, Param3)> *sig, Y* x,
                        Ret (X::*func)(Param0, Param1, Param2, Param3))
    {
        sig->connect(x, func);
    }
    friend void connect(Signal<Ret (Param0, Param1, Param2, Param3)> *sig,
                        Ret (*func)(Param0, Param1, Param2, Param3))
    {
        sig->connect(func);
    }
    template< typename X, typename Y >
    friend void disconnect(Signal<Ret (Param0, Param1, Param2, Param3)> *sig, Y* x,
                           Ret (X::*func)(Param0, Param1, Param2, Param3) const)
    {
        sig->disconnect(x, func);
    }
    template< typename X, typename Y >
    friend void disconnect(Signal<Ret (Param0, Param1, Param2, Param3)> *sig, Y* x,
                           Ret (X::*func)(Param0, Param1, Param2, Param3))
    {
        sig->disconnect(x, func);
    }
    friend void disconnect(Signal<Ret (Param0, Param1, Param2, Param3)> *sig,
                           Ret (*func)(Param0, Param1, Param2, Param3))
    {
        sig->disconnect(func);
    }
};

template<typename Ret,typename Param0, typename Param1, typename Param2, typename Param3, typename Param4 >
class Signal <Ret(Param0, Param1, Param2, Param3, Param4)>
{
public:
    typedef Delegate5<Param0, Param1,Param2, Param3, Param4, Ret> SlotType;

private:
    typedef ky_list<SlotType> SlotList;
    typedef typename SlotList::const_iterator SlotIterator;
    SlotList list_slot;

public:
    inline void connect(const SlotType &slot )
    {
        list_slot.push_back (slot);
    }

    inline void connect (Ret (*method)( Param0, Param1, Param2 ,Param3, Param4))
    {
        SlotType slt(method);
        connect(slt);
    }
    template< typename X, typename Y >
    inline void connect( Y * obj, Ret (X::*method)(Param0, Param1, Param2, Param3, Param4) )
    {
        SlotType slt(obj, method);
        connect(slt);
    }

    template< typename X, typename Y >
    inline void connect( Y * obj, Ret (X::*method)(Param0, Param1, Param2, Param3, Param4) const )
    {
        SlotType slt(obj, method);
        connect(slt);
    }

    inline void disconnect(const SlotType &slot )
    {
        list_slot.remove(list_slot.find(slot));
    }

    inline void disconnect (Ret (*method)( Param0 , Param1 , Param2,Param3 ,Param4))
    {
        SlotType slt(method);
        disconnect(slt);
    }
    template< typename X, typename Y >
    inline void disconnect( Y * obj, Ret (X::*method)(Param0, Param1, Param2,Param3,Param4 ) )
    {
        SlotType slt(obj, method);
        disconnect(slt);
    }

    template< typename X, typename Y >
    inline void disconnect( Y * obj, Ret (X::*method)(Param0, Param1, Param2,Param3,Param4) const )
    {
        SlotType slt(obj, method);
        disconnect(slt);
    }
public:
    inline void clear(){list_slot.clear();}

    inline void emit(Param0 p0, Param1 p1, Param2 p2, Param3 p3, Param4 p4) const
    {
        for (SlotIterator ite = list_slot.begin(); ite != list_slot.end(); ++ite)
            (*ite)(p0, p1, p2, p3, p4);
    }

    inline void operator() (Param0 p0, Param1 p1, Param2 p2, Param3 p3,Param4 p4) const{emit(p0, p1, p2, p3, p4);}

    inline bool is_empty() const
    {
        return list_slot.empty();
    }


    template< typename X, typename Y >
    friend void connect(Signal<Ret (Param0, Param1, Param2, Param3, Param4)> *sig, Y* x,
                        Ret (X::*func)(Param0, Param1, Param2, Param3, Param4) const)
    {
        sig->connect(x, func);
    }
    template< typename X, typename Y >
    friend void connect(Signal<Ret (Param0, Param1, Param2, Param3, Param4)> *sig, Y* x,
                        Ret (X::*func)(Param0, Param1, Param2, Param3, Param4))
    {
        sig->connect(x, func);
    }
    friend void connect(Signal<Ret (Param0, Param1, Param2, Param3, Param4)> *sig,
                        Ret (*func)(Param0, Param1, Param2, Param3, Param4))
    {
        sig->connect(func);
    }
    template< typename X, typename Y >
    friend void disconnect(Signal<Ret (Param0, Param1, Param2, Param3, Param4)> *sig, Y* x,
                           Ret (X::*func)(Param0, Param1, Param2, Param3, Param4) const)
    {
        sig->disconnect(x, func);
    }
    template< typename X, typename Y >
    friend void disconnect(Signal<Ret (Param0, Param1, Param2, Param3, Param4)> *sig, Y* x,
                           Ret (X::*func)(Param0, Param1, Param2, Param3, Param4))
    {
        sig->disconnect(x, func);
    }
    friend void disconnect(Signal<Ret (Param0, Param1, Param2, Param3, Param4)> *sig,
                           Ret (*func)(Param0, Param1, Param2, Param3, Param4))
    {
        sig->disconnect(func);
    }
};

template<typename Ret,typename Param0, typename Param1, typename Param2, typename Param3, typename Param4,
         typename Param5>
class Signal <Ret(Param0, Param1, Param2, Param3, Param4, Param5)>
{
public:
    typedef Delegate6<Param0, Param1,Param2, Param3, Param4, Param5, Ret> SlotType;

private:
    typedef ky_list<SlotType> SlotList;
    typedef typename SlotList::const_iterator SlotIterator;
    SlotList list_slot;

public:
    inline void connect(const SlotType &slot )
    {
        list_slot.push_back (slot);
    }

    inline void connect (Ret (*method)( Param0, Param1, Param2 ,Param3, Param4, Param5))
    {
        SlotType slt(method);
        connect(slt);
    }
    template< typename X, typename Y >
    inline void connect( Y * obj, Ret (X::*method)(Param0, Param1, Param2, Param3, Param4, Param5) )
    {
        SlotType slt(obj, method);
        connect(slt);
    }

    template< typename X, typename Y >
    inline void connect( Y * obj, Ret (X::*method)(Param0, Param1, Param2, Param3, Param4, Param5) const )
    {
        SlotType slt(obj, method);
        connect(slt);
    }

    inline void disconnect(const SlotType &slot )
    {
        list_slot.remove(list_slot.find(slot));
    }

    inline void disconnect (Ret (*method)( Param0 , Param1 , Param2,Param3 ,Param4, Param5))
    {
        SlotType slt(method);
        disconnect(slt);
    }
    template< typename X, typename Y >
    inline void disconnect( Y * obj, Ret (X::*method)(Param0, Param1, Param2,Param3,Param4, Param5 ) )
    {
        SlotType slt(obj, method);
        disconnect(slt);
    }

    template< typename X, typename Y >
    inline void disconnect( Y * obj, Ret (X::*method)(Param0, Param1, Param2,Param3,Param4, Param5) const )
    {
        SlotType slt(obj, method);
        disconnect(slt);
    }
public:
    inline void clear(){list_slot.clear();}

    inline void emit(Param0 p0, Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5) const
    {
        for (SlotIterator ite = list_slot.begin(); ite != list_slot.end(); ++ite)
            (*ite)(p0, p1, p2, p3, p4, p5);
    }

    inline void operator() (Param0 p0, Param1 p1, Param2 p2, Param3 p3,Param4 p4, Param5 p5) const
    {emit(p0, p1, p2, p3, p4, p5);}

    inline bool is_empty() const
    {
        return list_slot.empty();
    }


    template< typename X, typename Y >
    friend void connect(Signal<Ret (Param0, Param1, Param2, Param3, Param4, Param5)> *sig, Y* x,
                        Ret (X::*func)(Param0, Param1, Param2, Param3, Param4, Param5) const)
    {
        sig->connect(x, func);
    }
    template< typename X, typename Y >
    friend void connect(Signal<Ret (Param0, Param1, Param2, Param3, Param4, Param5)> *sig, Y* x,
                        Ret (X::*func)(Param0, Param1, Param2, Param3, Param4, Param5))
    {
        sig->connect(x, func);
    }
    friend void connect(Signal<Ret (Param0, Param1, Param2, Param3, Param4, Param5)> *sig,
                        Ret (*func)(Param0, Param1, Param2, Param3, Param4, Param5))
    {
        sig->connect(func);
    }
    template< typename X, typename Y >
    friend void disconnect(Signal<Ret (Param0, Param1, Param2, Param3, Param4, Param5)> *sig, Y* x,
                           Ret (X::*func)(Param0, Param1, Param2, Param3, Param4, Param5) const)
    {
        sig->disconnect(x, func);
    }
    template< typename X, typename Y >
    friend void disconnect(Signal<Ret (Param0, Param1, Param2, Param3, Param4, Param5)> *sig, Y* x,
                           Ret (X::*func)(Param0, Param1, Param2, Param3, Param4, Param5))
    {
        sig->disconnect(x, func);
    }
    friend void disconnect(Signal<Ret (Param0, Param1, Param2, Param3, Param4, Param5)> *sig,
                           Ret (*func)(Param0, Param1, Param2, Param3, Param4, Param5))
    {
        sig->disconnect(func);
    }
};

template<typename Ret,typename Param0, typename Param1, typename Param2, typename Param3, typename Param4,
         typename Param5, typename Param6>
class Signal <Ret(Param0, Param1, Param2, Param3, Param4, Param5, Param6)>
{
public:
    typedef Delegate7<Param0, Param1,Param2, Param3, Param4, Param5, Param6, Ret> SlotType;

private:
    typedef ky_list<SlotType> SlotList;
    typedef typename SlotList::const_iterator SlotIterator;
    SlotList list_slot;

public:
    inline void connect(const SlotType &slot )
    {
        list_slot.push_back (slot);
    }

    inline void connect (Ret (*method)( Param0, Param1, Param2 ,Param3, Param4, Param5, Param6))
    {
        SlotType slt(method);
        connect(slt);
    }
    template< typename X, typename Y >
    inline void connect( Y * obj, Ret (X::*method)(Param0, Param1, Param2, Param3, Param4, Param5, Param6) )
    {
        SlotType slt(obj, method);
        connect(slt);
    }

    template< typename X, typename Y >
    inline void connect( Y * obj, Ret (X::*method)(Param0, Param1, Param2, Param3, Param4, Param5, Param6) const )
    {
        SlotType slt(obj, method);
        connect(slt);
    }

    inline void disconnect(const SlotType &slot )
    {
        list_slot.remove(list_slot.find(slot));
    }

    inline void disconnect (Ret (*method)( Param0 , Param1 , Param2,Param3 ,Param4, Param5, Param6))
    {
        SlotType slt(method);
        disconnect(slt);
    }
    template< typename X, typename Y >
    inline void disconnect( Y * obj, Ret (X::*method)(Param0, Param1, Param2,Param3,Param4, Param5,Param6 ) )
    {
        SlotType slt(obj, method);
        disconnect(slt);
    }

    template< typename X, typename Y >
    inline void disconnect( Y * obj, Ret (X::*method)(Param0, Param1, Param2,Param3,Param4, Param5, Param6) const )
    {
        SlotType slt(obj, method);
        disconnect(slt);
    }
public:
    inline void clear(){list_slot.clear();}

    inline void emit(Param0 p0, Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6) const
    {
        for (SlotIterator ite = list_slot.begin(); ite != list_slot.end(); ++ite)
            (*ite)(p0, p1, p2, p3, p4, p5, p6);
    }

    inline void operator() (Param0 p0, Param1 p1, Param2 p2, Param3 p3,Param4 p4, Param5 p5, Param6 p6) const
    {emit(p0, p1, p2, p3, p4, p5, p6);}

    inline  bool is_empty() const
    {
        return list_slot.empty();
    }

    template< typename X, typename Y >
    friend void connect(Signal<Ret (Param0, Param1, Param2, Param3, Param4, Param5, Param6)> *sig, Y* x,
                        Ret (X::*func)(Param0, Param1, Param2, Param3, Param4, Param5, Param6) const)
    {
        sig->connect(x, func);
    }
    template< typename X, typename Y >
    friend void connect(Signal<Ret (Param0, Param1, Param2, Param3, Param4, Param5, Param6)> *sig, Y* x,
                        Ret (X::*func)(Param0, Param1, Param2, Param3, Param4, Param5, Param6))
    {
        sig->connect(x, func);
    }
    friend void connect(Signal<Ret (Param0, Param1, Param2, Param3, Param4, Param5, Param6)> *sig,
                        Ret (*func)(Param0, Param1, Param2, Param3, Param4, Param5, Param6))
    {
        sig->connect(func);
    }
    template< typename X, typename Y >
    friend void disconnect(Signal<Ret (Param0, Param1, Param2, Param3, Param4, Param5, Param6)> *sig, Y* x,
                           Ret (X::*func)(Param0, Param1, Param2, Param3, Param4, Param5, Param6) const)
    {
        sig->disconnect(x, func);
    }
    template< typename X, typename Y >
    friend void disconnect(Signal<Ret (Param0, Param1, Param2, Param3, Param4, Param5, Param6)> *sig, Y* x,
                           Ret (X::*func)(Param0, Param1, Param2, Param3, Param4, Param5, Param6))
    {
        sig->disconnect(x, func);
    }
    friend void disconnect(Signal<Ret (Param0, Param1, Param2, Param3, Param4, Param5, Param6)> *sig,
                           Ret (*func)(Param0, Param1, Param2, Param3, Param4, Param5, Param6))
    {
        sig->disconnect(func);
    }
};

template<typename Ret,typename Param0, typename Param1, typename Param2, typename Param3, typename Param4,
         typename Param5, typename Param6, typename Param7>
class Signal <Ret(Param0, Param1, Param2, Param3, Param4, Param5, Param6, Param7)>
{
public:
    typedef Delegate8<Param0, Param1,Param2, Param3, Param4, Param5, Param6, Param7, Ret> SlotType;

private:
    typedef ky_list<SlotType> SlotList;
    typedef typename SlotList::const_iterator SlotIterator;
    SlotList list_slot;

public:
    inline void connect(const SlotType &slot )
    {
        list_slot.push_back (slot);
    }

    inline void connect (Ret (*method)( Param0, Param1, Param2 ,Param3, Param4, Param5, Param6, Param7))
    {
        SlotType slt(method);
        connect(slt);
    }
    template< typename X, typename Y >
    inline void connect( Y * obj, Ret (X::*method)(Param0, Param1, Param2, Param3, Param4, Param5, Param6, Param7) )
    {
        SlotType slt(obj, method);
        connect(slt);
    }

    template< typename X, typename Y >
    inline void connect( Y * obj, Ret (X::*method)(Param0, Param1, Param2, Param3, Param4, Param5, Param6, Param7) const )
    {
        SlotType slt(obj, method);
        connect(slt);
    }

    inline void disconnect(const SlotType &slot )
    {
        list_slot.remove(list_slot.find(slot));
    }

    inline void disconnect (Ret (*method)( Param0 , Param1 , Param2,Param3 ,Param4, Param5, Param6, Param7))
    {
        SlotType slt(method);
        disconnect(slt);
    }
    template< typename X, typename Y >
    inline void disconnect( Y * obj, Ret (X::*method)(Param0, Param1, Param2,Param3,Param4, Param5,Param6, Param7 ) )
    {
        SlotType slt(obj, method);
        disconnect(slt);
    }

    template< typename X, typename Y >
    inline void disconnect( Y * obj, Ret (X::*method)(Param0, Param1, Param2,Param3,Param4, Param5, Param6, Param7) const )
    {
        SlotType slt(obj, method);
        disconnect(slt);
    }
public:
    inline void clear(){list_slot.clear();}

    inline void emit(Param0 p0, Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7) const
    {
        for (SlotIterator ite = list_slot.begin(); ite != list_slot.end(); ++ite)
            (*ite)(p0, p1, p2, p3, p4, p5, p6, p7);
    }

    inline void operator() (Param0 p0, Param1 p1, Param2 p2, Param3 p3,Param4 p4, Param5 p5, Param6 p6, Param7 p7) const
    {emit(p0, p1, p2, p3, p4, p5, p6, p7);}

    inline bool is_empty() const
    {
        return list_slot.empty();
    }

    template< typename X, typename Y >
    friend void connect(Signal<Ret (Param0, Param1, Param2, Param3, Param4, Param5, Param6, Param7)> *sig, Y* x,
                        Ret (X::*func)(Param0, Param1, Param2, Param3, Param4, Param5, Param6, Param7) const)
    {
        sig->connect(x, func);
    }
    template< typename X, typename Y >
    friend void connect(Signal<Ret (Param0, Param1, Param2, Param3, Param4, Param5, Param6, Param7)> *sig, Y* x,
                        Ret (X::*func)(Param0, Param1, Param2, Param3, Param4, Param5, Param6, Param7))
    {
        sig->connect(x, func);
    }
    friend void connect(Signal<Ret (Param0, Param1, Param2, Param3, Param4, Param5, Param6, Param7)> *sig,
                        Ret (*func)(Param0, Param1, Param2, Param3, Param4, Param5, Param6, Param7))
    {
        sig->connect(func);
    }
    template< typename X, typename Y >
    friend void disconnect(Signal<Ret (Param0, Param1, Param2, Param3, Param4, Param5, Param6, Param7)> *sig, Y* x,
                           Ret (X::*func)(Param0, Param1, Param2, Param3, Param4, Param5, Param6, Param7) const)
    {
        sig->disconnect(x, func);
    }
    template< typename X, typename Y >
    friend void disconnect(Signal<Ret (Param0, Param1, Param2, Param3, Param4, Param5, Param6, Param7)> *sig, Y* x,
                           Ret (X::*func)(Param0, Param1, Param2, Param3, Param4, Param5, Param6, Param7))
    {
        sig->disconnect(x, func);
    }
    friend void disconnect(Signal<Ret (Param0, Param1, Param2, Param3, Param4, Param5, Param6, Param7)> *sig,
                           Ret (*func)(Param0, Param1, Param2, Param3, Param4, Param5, Param6, Param7))
    {
        sig->disconnect(func);
    }
};

template<typename Ret,typename Param0, typename Param1, typename Param2, typename Param3, typename Param4,
         typename Param5, typename Param6, typename Param7, typename Param8>
class Signal <Ret(Param0, Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8)>
{
public:
    typedef Delegate9<Param0, Param1,Param2, Param3, Param4, Param5, Param6, Param7, Param8, Ret> SlotType;

private:
    typedef ky_list<SlotType> SlotList;
    typedef typename SlotList::const_iterator SlotIterator;
    SlotList list_slot;

public:
    inline void connect(const SlotType &slot)
    {
        list_slot.push_back (slot);
    }

    inline void connect (Ret (*method)( Param0, Param1, Param2 ,Param3, Param4, Param5, Param6, Param7, Param8))
    {
        SlotType slt(method);
        connect(slt);
    }
    template< typename X, typename Y >
    inline void connect( Y * obj, Ret (X::*method)(Param0, Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8) )
    {
        SlotType slt(obj, method);
        connect(slt);
    }

    template< typename X, typename Y >
    inline void connect( Y * obj, Ret (X::*method)(Param0, Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8) const )
    {
        SlotType slt(obj, method);
        connect(slt);
    }

    inline void disconnect(const SlotType &slot )
    {
        list_slot.remove(list_slot.find(slot));
    }

    inline void disconnect (Ret (*method)( Param0 , Param1 , Param2,Param3 ,Param4, Param5, Param6, Param7, Param8))
    {
        SlotType slt(method);
        disconnect(slt);
    }
    template< typename X, typename Y >
    inline void disconnect( Y * obj, Ret (X::*method)(Param0, Param1, Param2,Param3,Param4, Param5,Param6, Param7, Param8) )
    {
        SlotType slt(obj, method);
        disconnect(slt);
    }

    template< typename X, typename Y >
    inline void disconnect( Y * obj, Ret (X::*method)(Param0, Param1, Param2,Param3,Param4, Param5, Param6, Param7, Param8) const )
    {
        SlotType slt(obj, method);
        disconnect(slt);
    }
public:
    inline void clear(){list_slot.clear();}

    inline void emit(Param0 p0, Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8) const
    {
        for (SlotIterator ite = list_slot.begin(); ite != list_slot.end(); ++ite)
            (*ite)(p0, p1, p2, p3, p4, p5, p6, p7, p8);
    }

    inline void operator() (Param0 p0, Param1 p1, Param2 p2, Param3 p3,Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8) const
    {emit(p0, p1, p2, p3, p4, p5, p6, p7, p8);}

    inline bool is_empty() const
    {
        return list_slot.empty();
    }

    template< typename X, typename Y >
    friend void connect(Signal<Ret (Param0, Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8)> *sig, Y* x,
                        Ret (X::*func)(Param0, Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8) const)
    {
        sig->connect(x, func);
    }
    template< typename X, typename Y >
    friend void connect(Signal<Ret (Param0, Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8)> *sig, Y* x,
                        Ret (X::*func)(Param0, Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8))
    {
        sig->connect(x, func);
    }
    friend void connect(Signal<Ret (Param0, Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8)> *sig,
                        Ret (*func)(Param0, Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8))
    {
        sig->connect(func);
    }
    template< typename X, typename Y >
    friend void disconnect(Signal<Ret (Param0, Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8)> *sig, Y* x,
                           Ret (X::*func)(Param0, Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8) const)
    {
        sig->disconnect(x, func);
    }
    template< typename X, typename Y >
    friend void disconnect(Signal<Ret (Param0, Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8)> *sig, Y* x,
                           Ret (X::*func)(Param0, Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8))
    {
        sig->disconnect(x, func);
    }
    friend void disconnect(Signal<Ret (Param0, Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8)> *sig,
                           Ret (*func)(Param0, Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8))
    {
        sig->disconnect(func);
    }
};

//!
//! \brief The sigslot class
//! 使用方法：
//! 1.首先继承sigslot类，定义信号名称，此时需要填充参数。例如：Signal<void (int, int)> toogle;
//! 2.定义好后信号名称，在类内发射信号可以直接调用emit(参数)进行调用。例如:toogle.emit(4,5)或toogle(4,5);
//! 3.外部需要连接信号时，使用connect(信号名称，类对象，类成员方法)或connect(信号名称，静态方法)。
//! 4.外部需要发射信号时，在连接信号时会返回一个句柄slots 在使用emit函数时需要第一个参数加入此句柄。
//! 5.使用例子:
//!   a.定义信号对象
//!   class needsignal : public sigslot
//!   {
//!   public:
//!       Signal<void (int, int)> toogle;
//!       void event()
//!       {
//!          toogle(6, 7); // or toogle.emit(6, 7);
//!       }
//!   };
//!
//!   b.定义槽方法对象
//!   class needslot
//!   {
//!   public:
//!       void callbak(int a, int b)
//!       {
//!
//!       }
//!   };
//!
//!   b.定义静态方法
//!   static void slot_method(int a, int b)
//!   {
//!
//!   }
//!
//!   c.实例化信号槽对象
//!   needsignal sigobj;
//!   needslot   sltobj;
//!
//!   b.进行信号槽连接
//!   sigslot::slots slthd = sigobj.connect(&sigobj.toogle, &sltobj, &needslot::callbak);
//!   sigobj.connect(&sigobj.toogle, &sltobj, &slot_method);
//!
//!   e.外部发射信号
//!   sigobj.emit(slthd, 6, 7);
//!

#endif // SIGNAL_INL
