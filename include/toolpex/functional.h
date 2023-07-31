#ifndef TOOLPEX_FUNCTIONAL_H
#define TOOLPEX_FUNCTIONAL_H

#include "toolpex/macros.h"

TOOLPEX_NAMESAPCE_BEG

struct do_nothing
{
    constexpr void operator()([[maybe_unused]] auto&&) const noexcept { }
};

TOOLPEX_NAMESAPCE_END

#endif
