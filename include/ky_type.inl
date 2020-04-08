#ifndef KY_TYPE_INL
#define KY_TYPE_INL

template <bool Cond, typename T = void> struct enif_t;
template <typename T> struct enif_t<true, T> { typedef T type; };

/// remove_const
template<typename T>struct _del_const_{ typedef T type;};
template<typename T>struct _del_const_<T const>{typedef T type;};
/// remove_volatile
template<typename T>struct _del_volatile_{typedef T type; };
template<typename T>struct _del_volatile_<T volatile>{typedef T type;};
/// remove_const_volatile
template<typename T>struct _del_const_volatile_
{
    typedef typename _del_const_<typename _del_volatile_<T>::type>::type type;
};

/// integral_constant
template<typename T, T __v>
struct const_int
{
    static constexpr T             value = __v;
    typedef T                      v_type;
    typedef const_int<T, __v>   type;
    constexpr operator v_type() const { return value; }
#if kyLanguage > kyLanguage11
    constexpr v_type operator()() const { return value; }
#endif
};
template<typename T, T __v>
constexpr T const_int<T, __v>::value;

/// The type used as a compile-time boolean with true value.
typedef const_int<bool, true>     true_type_t;
/// The type used as a compile-time boolean with false value.
typedef const_int<bool, false>    false_type_t;

template<typename>struct _helper_is_int_: public false_type_t { };
template<>struct _helper_is_int_<bool_t>: public true_type_t { };
template<>struct _helper_is_int_<bool>: public true_type_t { };
template<>struct _helper_is_int_<char>: public true_type_t { };
template<>struct _helper_is_int_<signed char>: public true_type_t { };
template<>struct _helper_is_int_<unsigned char>: public true_type_t { };
template<>struct _helper_is_int_<wchar_t>: public true_type_t { };
template<>struct _helper_is_int_<char16_t>: public true_type_t { };
template<>struct _helper_is_int_<char32_t>: public true_type_t { };
template<>struct _helper_is_int_<short>: public true_type_t { };
template<>struct _helper_is_int_<unsigned short>: public true_type_t { };
template<>struct _helper_is_int_<int>: public true_type_t { };
template<>struct _helper_is_int_<unsigned int>: public true_type_t { };
template<>struct _helper_is_int_<long>: public true_type_t { };
template<>struct _helper_is_int_<unsigned long>: public true_type_t { };
template<>struct _helper_is_int_<long long>: public true_type_t { };
template<>struct _helper_is_int_<unsigned long long>: public true_type_t { };

template<typename>struct _helper_is_flt_: public false_type_t { };
template<>struct _helper_is_flt_<half_float>: public true_type_t { };
template<>struct _helper_is_flt_<float>: public true_type_t { };
template<>struct _helper_is_flt_<double>: public true_type_t { };
template<>struct _helper_is_flt_<long double>: public true_type_t { };

template<typename>struct _helper_is_void_: public false_type_t { };
template<>struct _helper_is_void_<void>: public true_type_t { };

template<typename>struct _helper_is_pointer_: public false_type_t { };
template<typename T>struct _helper_is_pointer_<T*>: public true_type_t { };

/// is_integral
template<typename T>struct is_int: public _helper_is_int_<typename _del_const_volatile_<T>::type>::type{};
/// is_float
template<typename T>struct is_flt: public _helper_is_flt_<typename _del_const_volatile_<T>::type>::type{};
/// is_void
template<typename T>struct is_void: public _helper_is_void_<typename _del_const_volatile_<T>::type>::type{};
/// is_array
template<typename>struct is_array: public false_type_t { };
template<typename T, int64 Sz>struct is_array <T[Sz]>: public true_type_t { };
template<typename T>struct is_array<T[]>: public true_type_t { };
/// is_pointer
template<typename T>struct is_pointer: public _helper_is_pointer_<typename _del_const_volatile_<T>::type>::type{};
/// is_const
template<typename>struct is_const: public false_type_t { };
template<typename T>struct is_const<T const>: public true_type_t { };
/// is_volatile
template<typename>struct is_volatile: public false_type_t { };
template<typename T>struct is_volatile<T volatile>: public true_type_t { };
/// is_enum
template<typename T> struct is_enum: public const_int<bool, __is_enum(T)>{ };
/// is_union
template<typename T>struct is_union: public const_int<bool, __is_union(T)>{ };
/// is_class
template<typename T>struct is_class: public const_int<bool, __is_class(T)>{ };
/// is_function
template<typename>struct is_function: public false_type_t { };
template<typename Ret, typename... ArgsT>
struct is_function<Ret(ArgsT...)>: public true_type_t {};
template<typename Ret, typename... ArgsT>
struct is_function<Ret(ArgsT...) &>:public true_type_t {};
template<typename Ret, typename... ArgsT>
struct is_function<Ret(ArgsT...) &&>:public true_type_t {};
template<typename Ret, typename... ArgsT>
struct is_function<Ret(ArgsT......)>:public true_type_t {};
template<typename Ret, typename... ArgsT>
struct is_function<Ret(ArgsT......) &>:public true_type_t {};
template<typename Ret, typename... ArgsT>
struct is_function<Ret(ArgsT......) &&>:public true_type_t {};
template<typename Ret, typename... ArgsT>
struct is_function<Ret(ArgsT...) const>:public true_type_t {};
template<typename Ret, typename... ArgsT>
struct is_function<Ret(ArgsT...) const &>:public true_type_t {};
template<typename Ret, typename... ArgsT>
struct is_function<Ret(ArgsT...) const &&>:public true_type_t {};
template<typename Ret, typename... ArgsT>
struct is_function<Ret(ArgsT......) const>:public true_type_t {};
template<typename Ret, typename... ArgsT>
struct is_function<Ret(ArgsT......) const &>:public true_type_t {};
template<typename Ret, typename... ArgsT>
struct is_function<Ret(ArgsT......) const &&>:public true_type_t {};
template<typename Ret, typename... ArgsT>
struct is_function<Ret(ArgsT...) volatile>:public true_type_t {};
template<typename Ret, typename... ArgsT>
struct is_function<Ret(ArgsT...) volatile &>:public true_type_t {};
template<typename Ret, typename... ArgsT>
struct is_function<Ret(ArgsT...) volatile &&>:public true_type_t {};
template<typename Ret, typename... ArgsT>
struct is_function<Ret(ArgsT......) volatile>:public true_type_t {};
template<typename Ret, typename... ArgsT>
struct is_function<Ret(ArgsT......) volatile &>:public true_type_t {};
template<typename Ret, typename... ArgsT>
struct is_function<Ret(ArgsT......) volatile &&>:public true_type_t {};
template<typename Ret, typename... ArgsT>
struct is_function<Ret(ArgsT...) const volatile>:public true_type_t {};
template<typename Ret, typename... ArgsT>
struct is_function<Ret(ArgsT...) const volatile &>:public true_type_t {};
template<typename Ret, typename... ArgsT>
struct is_function<Ret(ArgsT...) const volatile &&>:public true_type_t {};
template<typename Ret, typename... ArgsT>
struct is_function<Ret(ArgsT......) const volatile>:public true_type_t {};
template<typename Ret, typename... ArgsT>
struct is_function<Ret(ArgsT......) const volatile &>:
    public true_type_t {};
template<typename Ret, typename... ArgsT>
struct is_function<Ret(ArgsT......) const volatile &&>:
    public true_type_t {};

/// alignment_of
template<typename T> struct align_of : public const_int<int64, __alignof__(T)> { };
/// is_complex
template <typename T> struct is_complex:
        public const_int<bool, !is_enum<T>::value && !is_int<T>::value && !is_flt<T>::value> {};

/// complex_destroy
template <typename T>
static typename enif_t<is_complex<T>::value>::type
complex_destruct(T &o) { kyUnused2(o); o.~T(); }
template <typename T>
static typename enif_t<!is_complex<T>::value>::type
complex_destruct(T &) {}
template <typename T>
static typename enif_t<is_complex<T>::value>::type
complex_construct(T &x, const T &o) {new (&x) T(o);}
template <typename T>
static typename enif_t<!is_complex<T>::value>::type
complex_construct(T &x, const T &o) {x = o;}
#endif
