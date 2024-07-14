// This file is part of Koios
// https://github.com/JPewterschmidt/koios
//
// Copyleft 2023 - 2024, ShiXin Wang. All wrongs reserved.

#include "toolpex/assert.h"
#include <signal.h>
#include <cstdio>
#include <print>
#include <stacktrace>
#include <cassert>

namespace toolpex
{
    [[noreturn]] void assert_fail(const char* expr)
    {
        ::std::print(stderr, "Assertion failed: {}\n{}", expr, ::std::stacktrace::current());
        // We still want gdb could stop here, since sort of ::std::break_point are not supported yet.
        ::raise(SIGINT);
        ::quick_exit(1);
    }
} // namespace toolpex
