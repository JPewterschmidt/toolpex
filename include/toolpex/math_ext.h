// This file is part of Koios
// https://github.com/JPewterschmidt/toolpex
//
// Copyleft 2023 - 2024, ShiXin Wang. All wrongs reserved.

#ifndef TOOLPEX_MATH_EXT_H
#define TOOLPEX_MATH_EXT_H

#include <concepts>
#include <cstddef>

namespace toolpex
{

inline constexpr bool is_even(::std::integral auto i) noexcept
{
    return i % 2 == 0;
}

inline constexpr bool is_odd(::std::integral auto i) noexcept
{
    return !is_even(i);
}

inline constexpr bool is_power_of_2(::std::integral auto i) noexcept
{
    if (i <= 0) return false;
    return (i & (i - 1)) == 0;
}

inline constexpr 
size_t log2(size_t n, size_t p = 0ul)
{
    return (n <= 1) ? p : toolpex::log2(n >> 1, p + 1);
}

} // namespace toolpex

#endif
