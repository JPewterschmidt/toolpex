#ifndef TOOLPEX_REF_COUNT_H
#define TOOLPEX_REF_COUNT_H

#include "toolpex/macros.h"
#include <atomic>
#include <cstddef>

TOOLPEX_NAMESPACE_BEG

class ref_count
{
public:
    constexpr ref_count() noexcept = default;

    ref_count(::std::ptrdiff_t original_val) noexcept
        : m_count{ original_val }
    {
    }

    auto fetch_add(::std::ptrdiff_t val = 1) noexcept
    {
        return m_count.fetch_add(val, ::std::memory_order_relaxed);
    }

    auto fetch_sub(::std::ptrdiff_t val = 1) noexcept
    {
        return m_count.fetch_sub(val, ::std::memory_order_acq_rel);
    }

    auto load(::std::memory_order order = ::std::memory_order_seq_cst) const noexcept
    {
        return m_count.load(order);
    }

    auto operator ++(int) noexcept { return fetch_add(); }
    auto operator --(int) noexcept { return fetch_sub(); }
    
private:
    ::std::atomic_ptrdiff_t m_count;
};

TOOLPEX_NAMESPACE_END

#endif
