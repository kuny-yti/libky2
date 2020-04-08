

// 模板信号槽实现
#define ParamCount 0

template<typename X, typename Y , typename Ret>
void connect(Signal<Ret ()> *sig, Y* x, Ret (X::*func)() const)
{
    if (slot_map.contains ((intptr)(sig)))
        return ;
    sig->connect(x, func);
    slot_map.append ((intptr)(sig), ParamCount);
}
template< typename X, typename Y , typename Ret>
void connect(Signal<Ret ()> *sig, Y* x, Ret (X::*func)())
{
    if (slot_map.contains ((intptr)(sig)))
        return ;
    sig->connect(x, func);
    slot_map.append ((intptr)(sig), ParamCount);
}
template<typename Ret>
void connect(Signal<Ret ()> *sig, Ret (*func)())
{
    if (slot_map.contains ((intptr)(sig)))
        return ;
    sig->connect(func);
    slot_map.append ((intptr)(sig), ParamCount);
}

template< typename X, typename Y , typename Ret>
void disconnect(Signal<Ret ()> *sig, Y* x, Ret (X::*func)() const)
{
    if (!slot_map.contains ((intptr)(sig)))
        return ;

    sig->disconnect(x, func);
    slot_map.remove ((intptr)sig);
}
template< typename X, typename Y , typename Ret>
void disconnect(Signal<Ret ()> *sig, Y* x, Ret (X::*func)())
{
    if (!slot_map.contains ((intptr)(sig)))
        return ;

    sig->disconnect(x, func);
    slot_map.remove ((intptr)sig);
}
template<typename Ret>
void disconnect(Signal<Ret ()> *sig, Ret (*func)())
{
    if (!slot_map.contains ((intptr)(sig)))
        return ;

    sig->disconnect(func);
    slot_map.remove ((intptr)sig);
}

template<typename Ret = void>
void emit (Signal<Ret ()> *sig)
{
    if (!slot_map.contains ((intptr)(sig)))
        return ;
    sig->emit();
}
#undef ParamCount
#define ParamCount 1

template<typename X, typename Y , typename Ret, typename Param0>
void connect(Signal<Ret (Param0)> *sig, Y* x,
                    Ret (X::*func)(Param0) const)
{
    if (slot_map.contains ((intptr)(sig)))
        return ;
    sig->connect(x, func);
    slot_map.append ((intptr)(sig), ParamCount);
}
template< typename X, typename Y , typename Ret, typename Param0>
void connect(Signal<Ret (Param0)> *sig, Y* x,
                    Ret (X::*func)(Param0))
{
    if (slot_map.contains ((intptr)(sig)))
        return ;
    sig->connect(x, func);
    slot_map.append ((intptr)(sig), ParamCount);
}
template<typename Ret, typename Param0>
void connect(Signal<Ret (Param0)> *sig,
                    Ret (*func)(Param0))
{
    if (slot_map.contains ((intptr)(sig)))
        return ;
    sig->connect(func);
    slot_map.append ((intptr)(sig), ParamCount);
}
template< typename X, typename Y , typename Ret, typename Param0>
void disconnect(Signal<Ret (Param0)> *sig, Y* x,
                       Ret (X::*func)(Param0) const)
{
    if (!slot_map.contains ((intptr)(sig)))
        return ;

    sig->disconnect(x, func);
    slot_map.remove ((intptr)sig);
}
template< typename X, typename Y , typename Ret, typename Param0>
void disconnect(Signal<Ret (Param0)> *sig, Y* x,
                       Ret (X::*func)(Param0))
{
    if (!slot_map.contains ((intptr)(sig)))
        return ;

    sig->disconnect(x, func);
    slot_map.remove ((intptr)sig);
}
template<typename Ret, typename Param0>
void disconnect(Signal<Ret (Param0)> *sig,
                       Ret (*func)(Param0))
{
    if (!slot_map.contains ((intptr)(sig)))
        return ;

    sig->disconnect(func);
    slot_map.remove ((intptr)sig);
}

template<typename Param0, typename Ret = void>
void emit (Signal<Ret (Param0)> *sig, Param0 p0)
{
    if (!slot_map.contains ((intptr)(sig)))
        return ;
    sig->emit(p0);
}
#undef ParamCount
#define ParamCount 2

template<typename X, typename Y , typename Ret, typename Param0, typename Param1>
void connect(Signal<Ret (Param0, Param1)> *sig, Y* x,
                    Ret (X::*func)(Param0, Param1) const)
{
    if (slot_map.contains ((intptr)(sig)))
        return ;
    sig->connect(x, func);
    slot_map.append ((intptr)(sig), ParamCount);
}
template< typename X, typename Y , typename Ret, typename Param0, typename Param1>
void connect(Signal<Ret (Param0, Param1)> *sig, Y* x,
                    Ret (X::*func)(Param0, Param1))
{
    if (slot_map.contains ((intptr)(sig)))
        return ;
    sig->connect(x, func);
    slot_map.append ((intptr)(sig), ParamCount);
}
template<typename Ret, typename Param0, typename Param1>
void connect(Signal<Ret (Param0, Param1)> *sig,
                    Ret (*func)(Param0, Param1))
{
    if (slot_map.contains ((intptr)(sig)))
        return ;
    sig->connect(func);
    slot_map.append ((intptr)(sig), ParamCount);
}
template< typename X, typename Y , typename Ret, typename Param0, typename Param1>
void disconnect(Signal<Ret (Param0,Param1)> *sig, Y* x,
                       Ret (X::*func)(Param0,Param1) const)
{
    if (!slot_map.contains ((intptr)(sig)))
        return ;

    sig->disconnect(x, func);
    slot_map.remove ((intptr)sig);
}
template< typename X, typename Y , typename Ret, typename Param0, typename Param1>
void disconnect(Signal<Ret (Param0,Param1)> *sig, Y* x,
                       Ret (X::*func)(Param0,Param1))
{
    if (!slot_map.contains ((intptr)(sig)))
        return ;

    sig->disconnect(x, func);
    slot_map.remove ((intptr)sig);
}
template<typename Ret, typename Param0, typename Param1>
void disconnect(Signal<Ret (Param0, Param1)> *sig,
                       Ret (*func)(Param0, Param1))
{
    if (!slot_map.contains ((intptr)(sig)))
        return ;

    sig->disconnect(func);
    slot_map.remove ((intptr)sig);
}

template<typename Param0, typename Param1, typename Ret = void>
void emit (Signal<Ret (Param0, Param1)> *sig, Param0 p0, Param1 p1)
{
    if (!slot_map.contains ((intptr)(sig)))
        return ;
    sig->emit(p0, p1);
}
#undef ParamCount
#define ParamCount 3

template<typename X, typename Y , typename Ret, typename Param0, typename Param1,
         typename Param2>
void connect(Signal<Ret (Param0, Param1, Param2)> *sig, Y* x,
                    Ret (X::*func)(Param0, Param1, Param2) const)
{
    if (slot_map.contains ((intptr)(sig)))
        return ;
    sig->connect(x, func);
    slot_map.append ((intptr)(sig), ParamCount);
}
template< typename X, typename Y , typename Ret, typename Param0, typename Param1,
          typename Param2>
void connect(Signal<Ret (Param0, Param1, Param2)> *sig, Y* x,
                    Ret (X::*func)(Param0, Param1,Param2))
{
    if (slot_map.contains ((intptr)(sig)))
        return ;
    sig->connect(x, func);
    slot_map.append ((intptr)(sig), ParamCount);
}
template<typename Ret, typename Param0, typename Param1, typename Param2>
void connect(Signal<Ret (Param0, Param1, Param2)> *sig,
                    Ret (*func)(Param0, Param1, Param2))
{
    if (slot_map.contains ((intptr)(sig)))
        return ;
    sig->connect(func);
    slot_map.append ((intptr)(sig), ParamCount);
}
template< typename X, typename Y , typename Ret, typename Param0, typename Param1,
          typename Param2>
void disconnect(Signal<Ret (Param0,Param1,Param2)> *sig, Y* x,
                       Ret (X::*func)(Param0,Param1,Param2) const)
{
    if (!slot_map.contains ((intptr)(sig)))
        return ;

    sig->disconnect(x, func);
    slot_map.remove ((intptr)sig);
}
template< typename X, typename Y , typename Ret, typename Param0, typename Param1,
          typename Param2>
void disconnect(Signal<Ret (Param0,Param1,Param2)> *sig, Y* x,
                       Ret (X::*func)(Param0,Param1,Param2))
{
    if (!slot_map.contains ((intptr)(sig)))
        return ;

    sig->disconnect(x, func);
    slot_map.remove ((intptr)sig);
}
template<typename Ret, typename Param0, typename Param1, typename Param2>
void disconnect(Signal<Ret (Param0, Param1, Param2)> *sig,
                       Ret (*func)(Param0, Param1, Param2))
{
    if (!slot_map.contains ((intptr)(sig)))
        return ;

    sig->disconnect(func);
    slot_map.remove ((intptr)sig);
}

template<typename Param0, typename Param1, typename Param2, typename Ret = void>
void emit (Signal<Ret (Param0, Param1, Param2)> *sig, Param0 p0, Param1 p1, Param2 p2)
{
    if (!slot_map.contains ((intptr)(sig)))
        return ;
    sig->emit(p0, p1, p2);
}
#undef ParamCount
#define ParamCount 4

template<typename X, typename Y , typename Ret, typename Param0, typename Param1,
         typename Param2, typename Param3>
void connect(Signal<Ret (Param0, Param1, Param2, Param3)> *sig, Y* x,
                    Ret (X::*func)(Param0, Param1, Param2, Param3) const)
{
    if (slot_map.contains ((intptr)(sig)))
        return ;
    sig->connect(x, func);
    slot_map.append ((intptr)(sig), ParamCount);
}
template< typename X, typename Y , typename Ret, typename Param0, typename Param1,
          typename Param2, typename Param3>
void connect(Signal<Ret (Param0, Param1, Param2,Param3)> *sig, Y* x,
                    Ret (X::*func)(Param0, Param1,Param2,Param3))
{
    if (slot_map.contains ((intptr)(sig)))
        return ;
    sig->connect(x, func);
    slot_map.append ((intptr)(sig), ParamCount);
}
template<typename Ret, typename Param0, typename Param1, typename Param2, typename Param3>
void connect(Signal<Ret (Param0, Param1, Param2,Param3)> *sig,
                    Ret (*func)(Param0, Param1, Param2,Param3))
{
    if (slot_map.contains ((intptr)(sig)))
        return ;
    sig->connect(func);
    slot_map.append ((intptr)(sig), ParamCount);
}
template< typename X, typename Y , typename Ret, typename Param0, typename Param1,
          typename Param2, typename Param3>
void disconnect(Signal<Ret (Param0,Param1,Param2,Param3)> *sig, Y* x,
                       Ret (X::*func)(Param0,Param1,Param2,Param3) const)
{
    if (!slot_map.contains ((intptr)(sig)))
        return ;

    sig->disconnect(x, func);
    slot_map.remove ((intptr)sig);
}
template< typename X, typename Y , typename Ret, typename Param0, typename Param1,
          typename Param2, typename Param3>
void disconnect(Signal<Ret (Param0,Param1,Param2,Param3)> *sig, Y* x,
                       Ret (X::*func)(Param0,Param1,Param2,Param3))
{
    if (!slot_map.contains ((intptr)(sig)))
        return ;

    sig->disconnect(x, func);
    slot_map.remove ((intptr)sig);
}
template<typename Ret, typename Param0, typename Param1, typename Param2, typename Param3>
void disconnect(Signal<Ret (Param0, Param1, Param2,Param3)> *sig,
                       Ret (*func)(Param0, Param1, Param2,Param3))
{
    if (!slot_map.contains ((intptr)(sig)))
        return ;

    sig->disconnect(func);
    slot_map.remove ((intptr)sig);
}

template<typename Param0, typename Param1, typename Param2, typename Param3, typename Ret = void>
void emit (Signal<Ret (Param0, Param1, Param2,Param3)> *sig,
                  Param0 p0, Param1 p1, Param2 p2, Param3 p3)
{
    if (!slot_map.contains ((intptr)(sig)))
        return ;
    sig->emit(p0, p1, p2, p3);
}
#undef ParamCount
#define ParamCount 5

template<typename X, typename Y , typename Ret, typename Param0, typename Param1,
         typename Param2, typename Param3, typename Param4>
void connect(Signal<Ret (Param0, Param1, Param2, Param3,Param4)> *sig, Y* x,
                    Ret (X::*func)(Param0, Param1, Param2, Param3,Param4) const)
{
    if (slot_map.contains ((intptr)(sig)))
        return ;
    sig->connect(x, func);
    slot_map.append ((intptr)(sig), ParamCount);
}
template< typename X, typename Y , typename Ret, typename Param0, typename Param1,
          typename Param2, typename Param3, typename Param4>
void connect(Signal<Ret (Param0, Param1, Param2,Param3,Param4)> *sig, Y* x,
                    Ret (X::*func)(Param0, Param1,Param2,Param3,Param4))
{
    if (slot_map.contains ((intptr)(sig)))
        return ;
    sig->connect(x, func);
    slot_map.append ((intptr)(sig), ParamCount);
}
template<typename Ret, typename Param0, typename Param1, typename Param2, typename Param3,
         typename Param4>
void connect(Signal<Ret (Param0, Param1, Param2,Param3,Param4)> *sig,
                    Ret (*func)(Param0, Param1, Param2,Param3,Param4))
{
    if (slot_map.contains ((intptr)(sig)))
        return ;
    sig->connect(func);
    slot_map.append ((intptr)(sig), ParamCount);
}
template< typename X, typename Y , typename Ret, typename Param0, typename Param1,
          typename Param2, typename Param3, typename Param4>
void disconnect(Signal<Ret (Param0,Param1,Param2,Param3,Param4)> *sig, Y* x,
                       Ret (X::*func)(Param0,Param1,Param2,Param3,Param4) const)
{
    if (!slot_map.contains ((intptr)(sig)))
        return ;

    sig->disconnect(x, func);
    slot_map.remove ((intptr)sig);
}
template< typename X, typename Y , typename Ret, typename Param0, typename Param1,
          typename Param2, typename Param3, typename Param4>
void disconnect(Signal<Ret (Param0,Param1,Param2,Param3,Param4)> *sig, Y* x,
                       Ret (X::*func)(Param0,Param1,Param2,Param3,Param4))
{
    if (!slot_map.contains ((intptr)(sig)))
        return ;

    sig->disconnect(x, func);
    slot_map.remove ((intptr)sig);
}
template<typename Ret, typename Param0, typename Param1, typename Param2, typename Param3,
         typename Param4>
void disconnect(Signal<Ret (Param0, Param1, Param2,Param3,Param4)> *sig,
                       Ret (*func)(Param0, Param1, Param2,Param3,Param4))
{
    if (!slot_map.contains ((intptr)(sig)))
        return ;

    sig->disconnect(func);
    slot_map.remove ((intptr)sig);
}

template<typename Param0, typename Param1, typename Param2, typename Param3,
         typename Param4, typename Ret = void>
void emit (Signal<Ret (Param0, Param1, Param2,Param3,Param4)> *sig,
                  Param0 p0, Param1 p1, Param2 p2, Param3 p3, Param4 p4)
{
    if (!slot_map.contains ((intptr)(sig)))
        return ;
    sig->emit(p0, p1, p2, p3, p4);
}
#undef ParamCount
#define ParamCount 6

template<typename X, typename Y , typename Ret, typename Param0, typename Param1,
         typename Param2, typename Param3, typename Param4, typename Param5>
void connect(Signal<Ret (Param0, Param1, Param2, Param3,Param4,Param5)> *sig, Y* x,
                    Ret (X::*func)(Param0, Param1, Param2, Param3,Param4,Param5) const)
{
    if (slot_map.contains ((intptr)(sig)))
        return ;
    sig->connect(x, func);
    slot_map.append ((intptr)(sig), ParamCount);
}
template< typename X, typename Y , typename Ret, typename Param0, typename Param1,
          typename Param2, typename Param3, typename Param4, typename Param5>
void connect(Signal<Ret (Param0, Param1, Param2,Param3,Param4,Param5)> *sig, Y* x,
                    Ret (X::*func)(Param0, Param1,Param2,Param3,Param4,Param5))
{
    if (slot_map.contains ((intptr)(sig)))
        return ;
    sig->connect(x, func);
    slot_map.append ((intptr)(sig), ParamCount);
}
template<typename Ret, typename Param0, typename Param1, typename Param2, typename Param3,
         typename Param4, typename Param5>
void connect(Signal<Ret (Param0, Param1, Param2,Param3,Param4,Param5)> *sig,
                    Ret (*func)(Param0, Param1, Param2,Param3,Param4,Param5))
{
    if (slot_map.contains ((intptr)(sig)))
        return ;
    sig->connect(func);
    slot_map.append ((intptr)(sig), ParamCount);
}
template< typename X, typename Y , typename Ret, typename Param0, typename Param1,
          typename Param2, typename Param3, typename Param4, typename Param5>
void disconnect(Signal<Ret (Param0,Param1,Param2,Param3,Param4,Param5)> *sig, Y* x,
                       Ret (X::*func)(Param0,Param1,Param2,Param3,Param4,Param5) const)
{
    if (!slot_map.contains ((intptr)(sig)))
        return ;

    sig->disconnect(x, func);
    slot_map.remove ((intptr)sig);
}
template< typename X, typename Y , typename Ret, typename Param0, typename Param1,
          typename Param2, typename Param3, typename Param4, typename Param5>
void disconnect(Signal<Ret (Param0,Param1,Param2,Param3,Param4,Param5)> *sig, Y* x,
                       Ret (X::*func)(Param0,Param1,Param2,Param3,Param4,Param5))
{
    if (!slot_map.contains ((intptr)(sig)))
        return ;

    sig->disconnect(x, func);
    slot_map.remove ((intptr)sig);
}
template<typename Ret, typename Param0, typename Param1, typename Param2, typename Param3,
         typename Param4, typename Param5>
void disconnect(Signal<Ret (Param0, Param1, Param2,Param3,Param4,Param5)> *sig,
                       Ret (*func)(Param0, Param1, Param2,Param3,Param4,Param5))
{
    if (!slot_map.contains ((intptr)(sig)))
        return ;

    sig->disconnect(func);
    slot_map.remove ((intptr)sig);
}

template<typename Param0, typename Param1, typename Param2, typename Param3,
         typename Param4, typename Param5, typename Ret = void>
void emit (Signal<Ret (Param0, Param1, Param2,Param3,Param4,Param5)> *sig,
                  Param0 p0, Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5)
{
    if (!slot_map.contains ((intptr)(sig)))
        return ;
    sig->emit(p0, p1, p2, p3, p4, p5);
}
#undef ParamCount
#define ParamCount 7

template<typename X, typename Y , typename Ret, typename Param0, typename Param1,
         typename Param2, typename Param3, typename Param4, typename Param5, typename Param6>
void connect(Signal<Ret (Param0, Param1, Param2, Param3,Param4,Param5,Param6)> *sig, Y* x,
                    Ret (X::*func)(Param0, Param1, Param2, Param3,Param4,Param5,Param6) const)
{
    if (slot_map.contains ((intptr)(sig)))
        return ;
    sig->connect(x, func);
    slot_map.append ((intptr)(sig), ParamCount);
}
template< typename X, typename Y , typename Ret, typename Param0, typename Param1,
          typename Param2, typename Param3, typename Param4, typename Param5, typename Param6>
void connect(Signal<Ret (Param0, Param1, Param2,Param3,Param4,Param5,Param6)> *sig, Y* x,
                    Ret (X::*func)(Param0, Param1,Param2,Param3,Param4,Param5,Param6))
{
    if (slot_map.contains ((intptr)(sig)))
        return ;
    sig->connect(x, func);
    slot_map.append ((intptr)(sig), ParamCount);
}
template<typename Ret, typename Param0, typename Param1, typename Param2, typename Param3,
         typename Param4, typename Param5, typename Param6>
void connect(Signal<Ret (Param0, Param1, Param2,Param3,Param4,Param5,Param6)> *sig,
                    Ret (*func)(Param0, Param1, Param2,Param3,Param4,Param5,Param6))
{
    if (slot_map.contains ((intptr)(sig)))
        return ;
    sig->connect(func);
    slot_map.append ((intptr)(sig), ParamCount);
}
template< typename X, typename Y , typename Ret, typename Param0, typename Param1,
          typename Param2, typename Param3, typename Param4, typename Param5, typename Param6>
void disconnect(Signal<Ret (Param0,Param1,Param2,Param3,Param4,Param5,Param6)> *sig, Y* x,
                       Ret (X::*func)(Param0,Param1,Param2,Param3,Param4,Param5,Param6) const)
{
    if (!slot_map.contains ((intptr)(sig)))
        return ;

    sig->disconnect(x, func);
    slot_map.remove ((intptr)sig);
}
template< typename X, typename Y , typename Ret, typename Param0, typename Param1,
          typename Param2, typename Param3, typename Param4, typename Param5, typename Param6>
void disconnect(Signal<Ret (Param0,Param1,Param2,Param3,Param4,Param5,Param6)> *sig, Y* x,
                       Ret (X::*func)(Param0,Param1,Param2,Param3,Param4,Param5,Param6))
{
    if (!slot_map.contains ((intptr)(sig)))
        return ;

    sig->disconnect(x, func);
    slot_map.remove ((intptr)sig);
}
template<typename Ret, typename Param0, typename Param1, typename Param2, typename Param3,
         typename Param4, typename Param5, typename Param6>
void disconnect(Signal<Ret (Param0, Param1, Param2,Param3,Param4,Param5,Param6)> *sig,
                       Ret (*func)(Param0, Param1, Param2,Param3,Param4,Param5,Param6))
{
    if (!slot_map.contains ((intptr)(sig)))
        return ;

    sig->disconnect(func);
    slot_map.remove ((intptr)sig);
}

template<typename Param0, typename Param1, typename Param2, typename Param3,
         typename Param4, typename Param5, typename Param6, typename Ret = void>
void emit (Signal<Ret (Param0, Param1, Param2,Param3,Param4,Param5,Param6)> *sig,
                  Param0 p0, Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6)
{
    if (!slot_map.contains ((intptr)(sig)))
        return ;
    sig->emit(p0, p1, p2, p3, p4, p5, p6);
}

#undef ParamCount
#define ParamCount 8

template<typename X, typename Y , typename Ret, typename Param0, typename Param1,
         typename Param2, typename Param3, typename Param4, typename Param5,
         typename Param6, typename Param7>
void connect(Signal<Ret (Param0, Param1, Param2, Param3,Param4,Param5,Param6,Param7)> *sig, Y* x,
                    Ret (X::*func)(Param0, Param1, Param2, Param3,Param4,Param5,Param6,Param7) const)
{
    if (slot_map.contains ((intptr)(sig)))
        return ;
    sig->connect(x, func);
    slot_map.append ((intptr)(sig), ParamCount);
}
template< typename X, typename Y , typename Ret, typename Param0, typename Param1,
          typename Param2, typename Param3, typename Param4, typename Param5,
          typename Param6, typename Param7>
void connect(Signal<Ret (Param0, Param1, Param2,Param3,Param4,Param5,Param6,Param7)> *sig, Y* x,
                    Ret (X::*func)(Param0, Param1,Param2,Param3,Param4,Param5,Param6,Param7))
{
    if (slot_map.contains ((intptr)(sig)))
        return ;
    sig->connect(x, func);
    slot_map.append ((intptr)(sig), ParamCount);
}
template<typename Ret, typename Param0, typename Param1, typename Param2, typename Param3,
         typename Param4, typename Param5, typename Param6, typename Param7>
void connect(Signal<Ret (Param0, Param1, Param2,Param3,Param4,Param5,Param6,Param7)> *sig,
                    Ret (*func)(Param0, Param1, Param2,Param3,Param4,Param5,Param6,Param7))
{
    if (slot_map.contains ((intptr)(sig)))
        return ;
    sig->connect(func);
    slot_map.append ((intptr)(sig), ParamCount);
}
template< typename X, typename Y , typename Ret, typename Param0, typename Param1,
          typename Param2, typename Param3, typename Param4, typename Param5,
          typename Param6, typename Param7>
void disconnect(Signal<Ret (Param0,Param1,Param2,Param3,Param4,Param5,Param6,Param7)> *sig, Y* x,
                       Ret (X::*func)(Param0,Param1,Param2,Param3,Param4,Param5,Param6,Param7) const)
{
    if (!slot_map.contains ((intptr)(sig)))
        return ;

    sig->disconnect(x, func);
    slot_map.remove ((intptr)sig);
}
template< typename X, typename Y , typename Ret, typename Param0, typename Param1,
          typename Param2, typename Param3, typename Param4, typename Param5,
          typename Param6, typename Param7>
void disconnect(Signal<Ret (Param0,Param1,Param2,Param3,Param4,Param5,Param6,Param7)> *sig, Y* x,
                       Ret (X::*func)(Param0,Param1,Param2,Param3,Param4,Param5,Param6,Param7))
{
    if (!slot_map.contains ((intptr)(sig)))
        return ;

    sig->disconnect(x, func);
    slot_map.remove ((intptr)sig);
}
template<typename Ret, typename Param0, typename Param1, typename Param2, typename Param3,
         typename Param4, typename Param5, typename Param6, typename Param7>
void disconnect(Signal<Ret (Param0, Param1, Param2,Param3,Param4,Param5,Param6,Param7)> *sig,
                       Ret (*func)(Param0, Param1, Param2,Param3,Param4,Param5,Param6,Param7))
{
    if (!slot_map.contains ((intptr)(sig)))
        return ;

    sig->disconnect(func);
    slot_map.remove ((intptr)sig);
}

template<typename Param0, typename Param1, typename Param2, typename Param3,
         typename Param4, typename Param5, typename Param6, typename Param7,
         typename Ret = void>
void emit (Signal<Ret (Param0, Param1, Param2,Param3,Param4,Param5,Param6,Param7)> *sig,
                  Param0 p0, Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7)
{
    if (!slot_map.contains ((intptr)(sig)))
        return ;
    sig->emit(p0, p1, p2, p3, p4, p5, p6, p7);
}
#undef ParamCount
#define ParamCount 9

template<typename X, typename Y , typename Ret, typename Param0, typename Param1,
         typename Param2, typename Param3, typename Param4, typename Param5,
         typename Param6, typename Param7, typename Param8>
void connect(Signal<Ret (Param0, Param1, Param2, Param3,Param4,Param5,Param6,Param7,Param8)> *sig,
                    Y* x,
                    Ret (X::*func)(Param0, Param1, Param2, Param3,Param4,Param5,Param6,Param7,Param8)
                    const)
{
    if (slot_map.contains ((intptr)(sig)))
        return ;
    sig->connect(x, func);
    slot_map.append ((intptr)(sig), ParamCount);
}
template< typename X, typename Y , typename Ret, typename Param0, typename Param1,
          typename Param2, typename Param3, typename Param4, typename Param5,
          typename Param6, typename Param7, typename Param8>
void connect(Signal<Ret (Param0, Param1, Param2,Param3,Param4,Param5,Param6,Param7,Param8)> *sig,
                    Y* x,
                    Ret (X::*func)(Param0, Param1,Param2,Param3,Param4,Param5,Param6,Param7,Param8))
{
    if (slot_map.contains ((intptr)(sig)))
        return ;
    sig->connect(x, func);
    slot_map.append ((intptr)(sig), ParamCount);
}
template<typename Ret, typename Param0, typename Param1, typename Param2, typename Param3,
         typename Param4, typename Param5, typename Param6, typename Param7, typename Param8>
void connect(Signal<Ret (Param0, Param1, Param2,Param3,Param4,Param5,Param6,Param7,Param8)> *sig,
                    Ret (*func)(Param0, Param1, Param2,Param3,Param4,Param5,Param6,Param7,Param8))
{
    if (slot_map.contains ((intptr)(sig)))
        return ;
    sig->connect(func);
    slot_map.append ((intptr)(sig), ParamCount);
}
template< typename X, typename Y , typename Ret, typename Param0, typename Param1,
          typename Param2, typename Param3, typename Param4, typename Param5,
          typename Param6, typename Param7, typename Param8>
void disconnect(Signal<Ret (Param0,Param1,Param2,Param3,Param4,Param5,Param6,Param7,Param8)> *sig,
                       Y* x,
                       Ret (X::*func)(Param0,Param1,Param2,Param3,Param4,Param5,Param6,Param7,Param8)
                       const)
{
    if (!slot_map.contains ((intptr)(sig)))
        return ;

    sig->disconnect(x, func);
    slot_map.remove ((intptr)sig);
}
template< typename X, typename Y , typename Ret, typename Param0, typename Param1,
          typename Param2, typename Param3, typename Param4, typename Param5,
          typename Param6, typename Param7, typename Param8>
void disconnect(Signal<Ret (Param0,Param1,Param2,Param3,Param4,Param5,Param6,Param7,Param8)> *sig,
                       Y* x,
                       Ret (X::*func)(Param0,Param1,Param2,Param3,Param4,Param5,Param6,Param7,Param8))
{
    if (!slot_map.contains ((intptr)(sig)))
        return ;

    sig->disconnect(x, func);
    slot_map.remove ((intptr)sig);
}
template<typename Ret, typename Param0, typename Param1, typename Param2, typename Param3,
         typename Param4, typename Param5, typename Param6, typename Param7, typename Param8>
void disconnect(Signal<Ret (Param0, Param1, Param2,Param3,Param4,Param5,Param6,Param7,Param8)> *sig,
                       Ret (*func)(Param0, Param1, Param2,Param3,Param4,Param5,Param6,Param7,Param8))
{
    if (!slot_map.contains ((intptr)(sig)))
        return ;

    sig->disconnect(func);
    slot_map.remove ((intptr)sig);
}

template<typename Param0, typename Param1, typename Param2, typename Param3,
         typename Param4, typename Param5, typename Param6, typename Param7,
         typename Param8, typename Ret = void>
void emit (Signal<Ret (Param0, Param1, Param2,Param3,Param4,Param5,Param6,Param7,Param8)> *sig,
                  Param0 p0, Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5,
                  Param6 p6, Param7 p7, Param8 p8)
{
    if (!slot_map.contains ((intptr)(sig)))
        return ;
    sig->emit(p0, p1, p2, p3, p4, p5, p6, p7, p8);
}

#undef ParamCount
