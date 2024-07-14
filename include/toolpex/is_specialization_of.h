// This file is part of Toolpex
// https://github.com/JPewterschmidt/toolpex
//
// Copyleft 2023 - 2024, ShiXin Wang. All wrongs reserved.

#ifndef TOOLPEX_IS_SPECIALIZATION_OF_H
#define TOOLPEX_IS_SPECIALIZATION_OF_H

#include <type_traits>

#include "toolpex/macros.h"

TOOLPEX_NAMESPACE_BEG

template<typename T, template<typename...> typename Primary>
struct is_specialization_of_helper : ::std::false_type
{
};

template<template<typename...> typename Primary, typename... Args>
struct is_specialization_of_helper<Primary<Args...>, Primary> : ::std::true_type
{
};

template<typename T, template<typename...> typename Primary>
concept is_specialization_of = !!is_specialization_of_helper<T, Primary>{};

TOOLPEX_NAMESPACE_END

#endif
