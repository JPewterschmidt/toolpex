// This file is part of Toolpex
// https://github.com/JPewterschmidt/toolpex
//
// Copyleft 2023 - 2024, ShiXin Wang. All wrongs reserved.

#ifndef TOOLPEX_TIC_TOC_H
#define TOOLPEX_TIC_TOC_H

#include <chrono>
#include <print>
#include <sstream>
#include <format>

#include "toolpex/macros.h"

TOOLPEX_NAMESPACE_BEG

auto tic()
{
    return ::std::chrono::high_resolution_clock::now();
}

[[nodiscard]] auto toc(const auto& tp)
{
    const auto now = ::std::chrono::high_resolution_clock::now();
    return ::std::format("{}ms", ::std::chrono::duration_cast<::std::chrono::milliseconds>(now - tp).count());
}

TOOLPEX_NAMESPACE_END

#endif
