#include "toolpex/assert.h"
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
        assert(false);
    }
} // namespace toolpex
