#ifndef TOOLPEX_BITS_MANIPULATOR_H
#define TOOLPEX_BITS_MANIPULATOR_H

#include <concepts>
#include "toolpex/macros.h"

TOOLPEX_NAMESPACE_BEG

template<::std::integral RepT>
class bits_manipulator
{
public:
    constexpr bits_manipulator(RepT val) noexcept
        : m_rep{ val }
    {
    }

    constexpr bits_manipulator& 
    add(::std::integral auto flag) noexcept
    {
        m_rep |= flag;
        return *this;
    }

    constexpr bits_manipulator&
    remove(::std::integral auto flag) noexcept
    {
        m_rep &= ~flag;      
        return *this;
    }

    constexpr bool contains(::std::integral auto flag) const noexcept
    {
        return (m_rep & flag) == flag;
    }

    template<::std::integral IntT>
    constexpr bits_manipulator& add(const bits_manipulator<IntT>& other) noexcept
    {
        return add(other.value());
    }

    template<::std::integral IntT>
    constexpr bits_manipulator& remove(const bits_manipulator<IntT>& other) noexcept
    {
        return remove(other.value());
    }

    template<::std::integral IntT>
    constexpr bool contains(const bits_manipulator<IntT>& other) const noexcept
    {
        return contains(other.value());
    }

    constexpr operator RepT() const noexcept { return value(); }
    constexpr RepT& value() noexcept { return m_rep; }
    constexpr const RepT& value() const noexcept { return m_rep; }
    constexpr void clear() noexcept { m_rep = RepT{}; }

private: 
    RepT m_rep{};
};

TOOLPEX_NAMESPACE_END

#endif
