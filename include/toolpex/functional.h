// This file is part of Toolpex
// https://github.com/JPewterschmidt/toolpex
//
// Copyleft 2023 - 2024, ShiXin Wang. All wrongs reserved.

#ifndef TOOLPEX_FUNCTIONAL_H
#define TOOLPEX_FUNCTIONAL_H

#include "toolpex/macros.h"
#include "toolpex/concepts_and_traits.h"
#include "toolpex/functional.h" // for lazy_string_concater

#include <functional>
#include <ranges>
#include <tuple>
#include <cstddef>

TOOLPEX_NAMESPACE_BEG

struct do_nothing
{
    constexpr void operator()([[maybe_unused]] auto&& args...) const noexcept { }
    constexpr void operator()() const noexcept { }
};

inline char byte_to_char(::std::byte b)
{
    return static_cast<char>(b);
}

TOOLPEX_NAMESPACE_END

#endif
