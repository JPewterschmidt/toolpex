#ifndef TOOLPEX_BIT_MASK_H
#define TOOLPEX_BIT_MASK_H

#include <concepts>
#include "toolpex/macros.h"

TOOLPEX_NAMESPACE_BEG

template<::std::integral RepT>
class bit_mask
{
public:
    constexpr bit_mask(RepT val) noexcept
        : m_rep{ val }
    {
    }

    constexpr bit_mask& 
    add(::std::integral auto flag) noexcept
    {
        m_rep |= flag;
        return *this;
    }

    constexpr bit_mask& 
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
    constexpr bit_mask& add(const bit_mask<IntT>& other) noexcept
    {
        return add(other.value());
    }

    template<::std::integral IntT>
    constexpr bit_mask& remove(const bit_mask<IntT>& other) noexcept
    {
        return remove(other.value());
    }

    template<::std::integral IntT>
    constexpr bool contains(const bit_mask<IntT>& other) const noexcept
    {
        return contains(other.value());
    }

    template<::std::integral IntT>
    constexpr bool in(const bit_mask<IntT>& other) const noexcept
    {
        return other.contains(*this);
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
