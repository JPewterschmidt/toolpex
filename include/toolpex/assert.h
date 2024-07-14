// This file is part of Koios
// https://github.com/JPewterschmidt/toolpex
//
// Copyleft 2023 - 2024, ShiXin Wang. All wrongs reserved.

#ifndef TOOLPEX_ASSERT_H
#define TOOLPEX_ASSERT_H

namespace toolpex
{
    [[noreturn]] void assert_fail(const char* expr);
};

#ifdef	NDEBUG

# define toolpex_assert(expr)		(static_cast<void>(0))

#else

# define toolpex_assert(expr)                   \
    (static_cast<bool>(expr)                    \
     ? void (0)                                 \
     : toolpex::assert_fail(#expr))

#endif

#endif  // TOOLPEX_ASSERT_H
