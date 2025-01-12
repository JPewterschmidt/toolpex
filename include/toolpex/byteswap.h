#ifndef TOOLPEX_BITSWAP_H
#define TOOLPEX_BITSWAP_H

#include <concepts>
#include <type_traits>
#include <bit>
#include <array>
#include <cstddef>
#include <ranges>
#include <algorithm>

namespace toolpex
{

// Copied directly from cppreference
template<std::integral T>
constexpr T byteswap(T value) noexcept
{
    static_assert(std::has_unique_object_representations_v<T>, 
                  "T may not have padding bits");
    auto value_representation = std::bit_cast<std::array<std::byte, sizeof(T)>>(value);
    std::reverse(begin(value_representation), end(value_representation));
    return std::bit_cast<T>(value_representation);
}

} // namespace toolpex

#endif
