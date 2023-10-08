#ifndef TOOLPEX_LRU_CACHE_H
#define TOOLPEX_LRU_CACHE_H

#include "toolpex/macros.h"
#include "toolpex/tuple_hash.h"
#include <unordered_map>
#include <concepts>
#include <functional>

TOOLPEX_NAMESAPCE_BEG

template<unsigned Capacity, typename R, typename... Args>
class lru_cache
{
public:
    using result_t = R;

public:
    lru_cache(::std::move_only_function<R(Args...)> func)
        : m_f{ ::std::move(func) }
    {
    }

    auto operator()(Args... args)
    {
        ::std::tuple<Args...> argst{ ::std::move(args)... };
        if (m_cache.contains(argst))
            return m_cache[argst];
        auto ret = ::std::apply(m_f, argst);
        m_cache[argst] = ret;
        return ret;
    }
    
    constexpr size_t capacity() const noexcept { return Capacity; }

private:
    ::std::move_only_function<R(Args...)> m_f;
    ::std::unordered_map<::std::tuple<Args...>, result_t, tuple_hash> m_cache; // TODO dummy
};

TOOLPEX_NAMESAPCE_END

#endif
