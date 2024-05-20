#include "toolpex/assert.h"
#include <cstdio>
#include <print>
#include <stacktrace>

namespace toolpex
{
    [[noreturn]] void assert_fail(const char* expr)
    {
        ::std::print(stderr, "Assertion failed: {}\n{}", expr, ::std::stacktrace::current());
        ::quick_exit(1);
    }
} // namespace toolpex
