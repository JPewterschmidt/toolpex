#ifndef TOOLPEX_REF_COUNT_H
#define TOOLPEX_REF_COUNT_H

#include "toolpex/macros.h"
#include <atomic>

TOOLPEX_NAMESPACE_BEG

class ref_count
{
public:
    constexpr ref_count() noexcept = default;

    auto fetch_add(size_t val = 1) noexcept
    {
        return m_count.fetch_add(val, ::std::memory_order_relaxed);
    }

    auto fetch_sub(size_t val = 1) noexcept
    {
        return m_count.fetch_sub(val, ::std::memory_order_acq_rel);
    }
    
private:
    ::std::atomic_size_t m_count;
};

TOOLPEX_NAMESPACE_END

#endif
