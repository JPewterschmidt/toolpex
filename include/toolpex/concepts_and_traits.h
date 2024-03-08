#ifndef TOOLPEX_CONCETPS_AND_TRAITS_H
#define TOOLPEX_CONCETPS_AND_TRAITS_H

#include <functional>
#include <chrono>
#include <type_traits>
#include <concepts>
#include <span>
#include <string>
#include <string_view>

#include "toolpex/macros.h"
#include "toolpex/is_specialization_of.h"

TOOLPEX_NAMESPACE_BEG

template<typename Functor>
struct number_of_parameters
{
};

template<typename Ret, typename... Args>
struct number_of_parameters<Ret(Args...)>
{
    static constexpr size_t value = sizeof...(Args);
};

template<typename Ret, typename... Args>
struct number_of_parameters<::std::function<Ret(Args...)>>
{
    static constexpr size_t value = sizeof...(Args);
};

template<typename Ret, typename... Args>
struct number_of_parameters<Ret(*)(Args...)>
{
    static constexpr size_t value = sizeof...(Args);
};

template<typename Functor>
constexpr size_t number_of_parameters_v = number_of_parameters<Functor>::value;

template<typename Duration>
concept is_std_chrono_duration = toolpex::is_specialization_of<Duration, ::std::chrono::duration>;

template<typename Timepoint>
concept is_std_chrono_time_point = toolpex::is_specialization_of<Timepoint, ::std::chrono::time_point>;

template<typename DuraOrPoint>
concept is_std_chrono_duration_or_time_point = is_std_chrono_duration<DuraOrPoint> or is_std_chrono_time_point<DuraOrPoint>;

template <typename Callable>
struct get_return_type_helper
{
};

template <typename Ret, typename... Args>
struct get_return_type_helper<Ret (Args...)> 
{
    using type = Ret;
};

template <typename Ret, typename... Args>
struct get_return_type_helper<Ret (*) (Args...)> 
{
    using type = Ret;
};

template <typename Ret, typename... Args>
struct get_return_type_helper<Ret (&) (Args...)> 
{
    using type = Ret;
};

template <typename Ret, typename... Args>
struct get_return_type_helper<Ret (Args...) noexcept> 
{
    using type = Ret;
};

template <typename Ret, typename... Args>
struct get_return_type_helper<Ret (*) (Args...) noexcept> 
{
    using type = Ret;
};

template <typename Ret, typename... Args>
struct get_return_type_helper<Ret (&) (Args...) noexcept> 
{
    using type = Ret;
};

template <typename F>
struct get_return_type_helper<::std::function<F>> 
{
    using type = ::std::function<F>::result_type;
};

template <typename F>
struct get_return_type_helper<::std::move_only_function<F>> 
{
    using type = ::std::function<F>::result_type;
};

template <typename Callable>
struct get_return_type
{
    using type = get_return_type_helper<::std::remove_pointer_t<::std::remove_reference_t<Callable>>>::type;
};

template <typename Callable>
using get_return_type_t = typename get_return_type<Callable>::type;

template <typename TimespecLike>
concept timespec_like_concept = requires(TimespecLike ts)
{
    ts.tv_sec;
    ts.tv_nsec;
};

template <typename TimevalLike>
concept timeval_like_concept = requires(TimevalLike tv)
{
    tv.tv_sec;
    tv.tv_usec;
};

template <typename Span>
concept span_like = requires (Span s)
{
    begin(s);
    end(s);
    s.size();
    s.empty();
    s.subspan(0);
} && ::std::is_trivially_copyable_v<Span>;

template<typename StringLike>
concept string_like = requires (StringLike str)
{
    { ::std::char_traits<decltype(&str[0])>{} } -> is_specialization_of<::std::char_traits>;
} && ::std::ranges::range<StringLike>;

template<typename ToStringAble>
concept to_string_able = requires(ToStringAble o)
{
    { o.to_string() } -> string_like;
};

template<typename ToStringAble>
concept std_to_string_able = requires(ToStringAble o)
{
    ::std::to_string(o);
};

TOOLPEX_NAMESPACE_END

#endif
