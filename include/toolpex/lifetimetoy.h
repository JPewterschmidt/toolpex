#ifndef TOOLPEX_LIFETIMETOY_H
#define TOOLPEX_LIFETIMETOY_H

#include "toolpex/assert.h"

namespace toolpex
{

class lifetimetoy
{
public:
    explicit lifetimetoy(int i) : m_marker{ i } {};
    ~lifetimetoy() noexcept { m_destruction_canary = 123456; }

    lifetimetoy(lifetimetoy&& other) noexcept
        : m_marker{ other.visit() }
    {
        ::std::cerr << "lifetimetoy move ctor" << ::std::endl;
    }

    lifetimetoy(const lifetimetoy& other) noexcept
        : m_marker{ other.visit() }
    {
        ::std::cerr << "lifetimetoy copy ctor" << ::std::endl;
    }

    lifetimetoy& operator=(lifetimetoy&& other) noexcept
    {
        m_marker = other.visit();
        ::std::cerr << "lifetimetoy move operator=" << ::std::endl;
        return *this;
    }

    lifetimetoy& operator=(const lifetimetoy& other) noexcept
    {
        m_marker = other.visit();
        ::std::cerr << "lifetimetoy copy operator=" << ::std::endl;
        return *this;
    }

    int visit() const noexcept
    {
        toolpex_assert(m_destruction_canary == 0);
        return m_destruction_canary;
    }

private:
    int m_marker{};
    size_t m_destruction_canary{};
};

} // namespace toolpex

#endif
