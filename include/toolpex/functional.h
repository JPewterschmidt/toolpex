#ifndef TOOLPEX_FUNCTIONAL_H
#define TOOLPEX_FUNCTIONAL_H

#include "toolpex/macros.h"
#include <functional>
#include <ranges>
#include <tuple>

TOOLPEX_NAMESPACE_BEG

struct do_nothing
{
    constexpr void operator()([[maybe_unused]] auto&& args...) const noexcept { }
    constexpr void operator()() const noexcept { }
};

template<typename StrTuple = ::std::tuple<::std::string_view>>
class lazy_string_concater
{
public:
    constexpr lazy_string_concater() = default;
    constexpr lazy_string_concater(StrTuple strs) noexcept 
        : m_strs{ ::std::move(strs) }
    {
    }

    constexpr size_t size() const noexcept 
    { 
        return ::std::apply([](auto&&... strs) { 
            return (::std::size(strs) + ...);
        }, m_strs);
    }

    ::std::string get() const noexcept { return *this; }
    
    operator ::std::string() const 
    {
        ::std::string result;
        result.reserve(size());
        ::std::apply([&result](auto&&... str) mutable { 
            (result.append(str), ...);
        }, m_strs);
        return result;
    }

    auto operator + (::std::string_view other) &&
    {
        auto t = ::std::tuple_cat(::std::move(m_strs), ::std::tuple(other));
        return lazy_string_concater<::std::remove_reference_t<decltype(t)>>{ ::std::move(t) };
    }

private:
    StrTuple m_strs{::std::string_view{}};
};

TOOLPEX_NAMESPACE_END

#endif
