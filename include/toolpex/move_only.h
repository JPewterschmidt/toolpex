#ifndef TOOLPEX_MOVE_ONLY_H
#define TOOLPEX_MOVE_ONLY_H

#include "toolpex/macros.h"

TOOLPEX_NAMESAPCE_BEG

class move_only
{
public:
    move_only(move_only&&) noexcept = default;
    move_only& operator=(move_only&&) noexcept = default;
    ~move_only() noexcept = default;

    move_only(const move_only&) = delete;
    move_only(move_only&&) = delete;
};

TOOLPEX_NAMESAPCE_END

#endif
