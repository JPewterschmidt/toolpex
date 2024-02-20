#ifndef TOOLPEX_SPIN_LOCK_H
#define TOOLPEX_SPIN_LOCK_H

#include "toolpex/macros.h"
#include <atomic>

TOOLPEX_NAMESPACE_BEG

class spin_lock
{
public:
    inline void lock() noexcept
    {
        while (try_lock()) { }
    }

    inline bool try_lock() noexcept
    {
        return m_flag.test_and_set(::std::memory_order_acquire);
    }

    inline void unlock() noexcept
    {
        return m_flag.clear(::std::memory_order_release);
    }

private:
    ::std::atomic_flag m_flag{ ATOMIC_FLAG_INIT };
};

TOOLPEX_NAMESPACE_END

#endif