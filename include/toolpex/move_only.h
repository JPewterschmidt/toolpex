// This file is part of Koios
// https://github.com/JPewterschmidt/koios
//
// Copyleft 2023 - 2024, ShiXin Wang. All wrongs reserved.

#ifndef TOOLPEX_MOVE_ONLY_H
#define TOOLPEX_MOVE_ONLY_H

#include "toolpex/macros.h"

TOOLPEX_NAMESPACE_BEG

class move_only
{
public:
    constexpr move_only() noexcept = default;
    ~move_only() noexcept = default;

    move_only(move_only&&) noexcept = default;
    move_only& operator=(move_only&&) noexcept = default;

    move_only(const move_only&) = delete;
    move_only& operator=(move_only&) = delete;
};

TOOLPEX_NAMESPACE_END

#endif
