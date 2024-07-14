// This file is part of Toolpex
// https://github.com/JPewterschmidt/toolpex
//
// Copyleft 2023 - 2024, ShiXin Wang. All wrongs reserved.

#ifndef TOOLPEX_TUPLE_HASH_H
#define TOOLPEX_TUPLE_HASH_H

#include "toolpex/macros.h"
#include <functional>
#include <tuple>

TOOLPEX_NAMESPACE_BEG

class tuple_hash
{
public:
    template<typename... Args>
    size_t operator()(const ::std::tuple<Args...>& t) const noexcept 
    {
        size_t hash_val = 0;
        size_t seed = 0;
        std::apply([&](const auto&... args) {
            ((hash_val ^= (std::hash<::std::decay_t<decltype(args)>>()(args) + seed++)), ...);
        }, t);
        return hash_val;
    }
};

TOOLPEX_NAMESPACE_END

#endif
