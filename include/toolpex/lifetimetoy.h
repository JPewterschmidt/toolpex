#ifndef TOOLPEX_LIFETIMETOY_H
#define TOOLPEX_LIFETIMETOY_H

#include <cstddef>
#include <print>
#include "toolpex/assert.h"

namespace toolpex
{

class lifetimetoy
{
public:
    explicit lifetimetoy(int i, bool enable_print = false) 
        : m_marker{ i }, 
          m_enable_print{ enable_print }
    {
    }

    ~lifetimetoy() noexcept { m_destruction_canary = 123456; }

    lifetimetoy(lifetimetoy&& other) noexcept
        : m_marker{ other.visit() }
    {
        if (m_enable_print) ::std::println("lifetimetoy move ctor");
    }

    lifetimetoy(const lifetimetoy& other) noexcept
        : m_marker{ other.visit() }
    {
        if (m_enable_print) ::std::println("lifetimetoy copy ctor");
    }

    lifetimetoy& operator=(lifetimetoy&& other) noexcept
    {
        m_marker = other.visit();
        if (m_enable_print) ::std::println("lifetimetoy move operator=");
        return *this;
    }

    lifetimetoy& operator=(const lifetimetoy& other) noexcept
    {
        m_marker = other.visit();
        if (m_enable_print) ::std::println("lifetimetoy copy operator=");
        return *this;
    }

    int visit() const noexcept
    {
        toolpex_assert(m_destruction_canary == 0);
        return m_marker;
    }

    bool operator==(const lifetimetoy& other) const noexcept
    {
        return visit() == other.visit();
    }

    bool operator!=(const lifetimetoy& other) const noexcept
    {
        return !((*this) == other);
    }

    bool operator<(const lifetimetoy& other) const noexcept
    {
        return visit() < other.visit();
    }

    bool operator>(const lifetimetoy& other) const noexcept
    {
        return visit() > other.visit();
    }

private:
    int m_marker{};
    bool m_enable_print{};
    size_t m_destruction_canary{};
};

} // namespace toolpex

#endif
