#ifndef TOOLPEX_CONCETPS_AND_TRAITS_H
#define TOOLPEX_CONCETPS_AND_TRAITS_H

#include <functional>
#include <chrono>

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

template <typename Callable>
struct get_return_type
{
};

template <typename Ret, typename... Args>
struct get_return_type<Ret (Args...)> 
{
    using type = Ret;
};

template <typename Ret, typename... Args>
struct get_return_type<Ret (*) (Args...)> 
{
    using type = Ret;
};

template <typename Ret, typename... Args>
struct get_return_type<Ret (&) (Args...)> 
{
    using type = Ret;
};

template <typename Ret, typename... Args>
struct get_return_type<::std::function<Ret(Args...)>> 
{
    using type = Ret;
};

template <typename Ret, typename... Args>
struct get_return_type<::std::move_only_function<Ret(Args...)>> 
{
    using type = Ret;
};

template <typename Callable>
using get_return_type_t = typename get_return_type<Callable>::type;

TOOLPEX_NAMESPACE_END

#endif
