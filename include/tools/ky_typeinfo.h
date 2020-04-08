
/**
 * Basic tool library
 * Copyright (C) 2014 kunyang kunyang.yk@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * You should have received a copy of the GNU General Public License.
 * If not, see <http://www.gnu.org/licenses/>.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file     ky_typeinfo.h
 * @brief    C++类信息类实现
 *
 * @author   kunyang
 * @email    kunyang.yk@gmail.com
 * @version  1.0.0.1
 * @date     2015/04/19
 * @license  GNU General Public License (GPL)
 *
 * Change History :
 *    Date    |  Version  |  Author  |   Description
 * 2015/04/19 | 1.0.0.1   | kunyang  | 创建文件
 */
#ifndef CHECK_TYPE_H___
#define CHECK_TYPE_H___

#include "ky_define.h"
#include <typeinfo>     // typeid
#include <sstream>      // std::ostringstream, std::string
#include <type_traits>  // std::is_array
#include <utility>      // std::move

#if kyCompilerIsGNUC
#include <memory>       // std::unique_ptr
#include <cxxabi.h>     // abi::__cxa_demangle
#endif
#if kyLanguage >= kyLanguage11
namespace base_type {

template <typename T, bool IsBase = false>
struct ky_typeinfo;

/*
    Output state management
*/

class ky_output
{
    bool is_compact_ = true;

    template <typename T>
    bool check_empty(const T&) { return false; }
    bool check_empty(const char* val)
    {
        return (!val) || (val[0] == 0);
    }

    template <typename T>
    void out(const T& val)
    {
        if (check_empty(val)) return;
        if (!is_compact_) sr_ += " ";
        using ss_t = std::ostringstream;
        sr_ += static_cast<ss_t&>(ss_t() << val).str();
        is_compact_ = false;
    }

    std::string& sr_;

public:
    ky_output(std::string& sr) : sr_(sr) {}

    ky_output& operator()(void) { return (*this); }

    template <typename T1, typename... T>
    ky_output& operator()(const T1& val, const T&... args)
    {
        out(val);
        return operator()(args...);
    }

    ky_output& compact(void)
    {
        is_compact_ = true;
        return (*this);
    }
};

// ()

template <bool>
struct ky_bracket
{
    ky_output& out_;

    ky_bracket(ky_output& out, const char* = 0) : out_(out)
    { out_("(").compact(); }

    ~ky_bracket(void)
    { out_.compact()(")"); }
};

template <>
struct ky_bracket<false>
{
    ky_bracket(ky_output& out, const char* str = 0)
    { out(str); }
};

// [N]

template <int64 N = 0>
struct ky_bound
{
    ky_output& out_;

    ky_bound(ky_output& out) : out_(out) {}
    ~ky_bound(void)
    {
        if (N == 0) out_("[]");
        else        out_("[").compact()
                        ( N ).compact()
                        ("]");
    }
};

// (P1, P2, ...)

template <bool, typename... P>
struct ky_parameter;

template <bool IsStart>
struct ky_parameter<IsStart>
{
    ky_output& out_;

    ky_parameter(ky_output& out) : out_(out) {}
    ~ky_parameter(void)
    { ky_bracket<IsStart> { out_ }; }
};

template <bool IsStart, typename P1, typename... P>
struct ky_parameter<IsStart, P1, P...>
{
    ky_output& out_;

    ky_parameter(ky_output& out) : out_(out) {}
    ~ky_parameter(void)
    {
        [this](ky_bracket<IsStart>&&)
        {
            ky_typeinfo<P1> { out_ };
            ky_parameter<false, P...> { out_.compact() };
        } (ky_bracket<IsStart> { out_, "," });
    }
};

// Do output at destruct

struct at_destruct
{
    ky_output&     out_;
    const char* str_;

    at_destruct(ky_output& out, const char* str = nullptr)
        : out_(out)
        , str_(str)
    {}
    ~at_destruct(void)
    { out_(str_); }

    void set_str(const char* str = nullptr)
    { str_ = str; }
};

/*
    CV-qualifiers, references, pointers
*/

template <typename T, bool IsBase>
struct ky_typeinfo
{
    ky_output out_;
    ky_typeinfo(const ky_output& out) : out_(out)
    {
#   if kyCompiler == kyCompiler_GNUC
        const char* typeid_name = typeid(T).name();
        auto deleter = [](char* p)
        {
            if (p) free(p);
        };
        std::unique_ptr<char, decltype(deleter)> real_name
        {
            abi::__cxa_demangle(typeid_name, nullptr, nullptr, nullptr), deleter
        };
        out_(real_name ? real_name.get() : typeid_name);
#   else
        out_(typeid(T).name());
#   endif
    }
};

#define ky_TYPEINFO__(OPT) \
    template <typename T, bool IsBase> \
    struct ky_typeinfo<T OPT, IsBase> : ky_typeinfo<T, true> \
    { \
        using base_t = ky_typeinfo<T, true>; \
        using base_t::out_; \
        ky_typeinfo(const ky_output& out) : base_t(out) { out_(#OPT); } \
    };

ky_TYPEINFO__(const)
ky_TYPEINFO__(volatile)
ky_TYPEINFO__(const volatile)
ky_TYPEINFO__(&)
ky_TYPEINFO__(&&)
ky_TYPEINFO__(*)

#undef ky_TYPEINFO__

/*
    Arrays
*/

#define ky_TYPEINFO_ARRAY__(CV_OPT, BOUND_OPT, ...) \
    template <typename T, bool IsBase __VA_ARGS__> \
    struct ky_typeinfo<T CV_OPT [BOUND_OPT], IsBase> : ky_typeinfo<T CV_OPT, !std::is_array<T>::value> \
    { \
        using base_t = ky_typeinfo<T CV_OPT, !std::is_array<T>::value>; \
        using base_t::out_; \
    \
        ky_bound<BOUND_OPT> bound_   = out_; \
        ky_bracket<IsBase>  bracket_ = out_; \
    \
        ky_typeinfo(const ky_output& out) : base_t(out) {} \
    };

#define ky_TYPEINFO_ARRAY_CV__(BOUND_OPT, ...) \
    ky_TYPEINFO_ARRAY__(, BOUND_OPT, ,##__VA_ARGS__) \
    ky_TYPEINFO_ARRAY__(const, BOUND_OPT, ,##__VA_ARGS__) \
    ky_TYPEINFO_ARRAY__(volatile, BOUND_OPT, ,##__VA_ARGS__) \
    ky_TYPEINFO_ARRAY__(const volatile, BOUND_OPT, ,##__VA_ARGS__)

#define ky_TYPEINFO_PLACEHOLDER__
ky_TYPEINFO_ARRAY_CV__(ky_TYPEINFO_PLACEHOLDER__)
#if kyCompiler == kyCompiler_GNUC
ky_TYPEINFO_ARRAY_CV__(0)
#endif
ky_TYPEINFO_ARRAY_CV__(N, int64 N)

#undef ky_TYPEINFO_PLACEHOLDER__
#undef ky_TYPEINFO_ARRAY_CV__
#undef ky_TYPEINFO_ARRAY__

/*
    Functions
*/

template <typename T, bool IsBase, typename... P>
struct ky_typeinfo<T(P...), IsBase> : ky_typeinfo<T, true>
{
    using base_t = ky_typeinfo<T, true>;
    using base_t::out_;

    ky_parameter<true, P...> parameter_ = out_;
    ky_bracket<IsBase>       bracket_   = out_;

    ky_typeinfo(const ky_output& out) : base_t(out) {}
};

/*
    Pointers to members
*/

template <typename T, bool IsBase, typename C>
struct ky_typeinfo<T C::*, IsBase> : ky_typeinfo<T, true>
{
    using base_t = ky_typeinfo<T, true>;
    using base_t::out_;

    ky_typeinfo(const ky_output& out) : base_t(out)
    {
        ky_typeinfo<C> { out_ };
        out_.compact()("::*");
    }
};

/*
    Pointers to member functions
*/

#define ky_TYPEINFO_MEM_FUNC__(...) \
    template <typename T, bool IsBase, typename C, typename... P> \
    struct ky_typeinfo<T(C::*)(P...) __VA_ARGS__, IsBase> \
    { \
        at_destruct cv_ = base_.out_; \
        ky_typeinfo<T(P...), true> base_; \
        ky_output& out_ = base_.out_; \
    \
        ky_typeinfo(const ky_output& out) : base_(out) \
        { \
            cv_.set_str(#__VA_ARGS__); \
            ky_typeinfo<C> { out_ }; \
            out_.compact()("::*"); \
        } \
    };

ky_TYPEINFO_MEM_FUNC__()
ky_TYPEINFO_MEM_FUNC__(const)
ky_TYPEINFO_MEM_FUNC__(volatile)
ky_TYPEINFO_MEM_FUNC__(const volatile)

#undef ky_TYPEINFO_MEM_FUNC__

}
#endif
/*
    Get the name of the given type

    check_type<const volatile void *>()
    -->
    void const volatile *
*/

template <typename T>
inline std::string ky_typeinfo()
{
#if kyLanguage >= kyLanguage11
    std::string str;
    base_type::ky_typeinfo<T> { str };
    return std::move(str);
#else
    return typeid(T).name();
#endif
}

#endif
