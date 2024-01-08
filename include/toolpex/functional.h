#ifndef TOOLPEX_FUNCTIONAL_H
#define TOOLPEX_FUNCTIONAL_H

#include "toolpex/macros.h"
#include <functional>

TOOLPEX_NAMESPACE_BEG

struct do_nothing
{
    constexpr void operator()([[maybe_unused]] auto&& args...) const noexcept { }
    constexpr void operator()() const noexcept { }
};

TOOLPEX_NAMESPACE_END

#endif
