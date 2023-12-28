#ifndef TOOLPEX_POSIX_ERR_THROWER_H
#define TOOLPEX_POSIX_ERR_THROWER_H

#include <system_error>

#include "toolpex/macros.h"
#include "toolpex/move_only.h"

TOOLPEX_NAMESAPCE_BEG

class posix_err_thrower : toolpex::move_only
{
public:
    posix_err_thrower() noexcept = default;

    explicit posix_err_thrower(int err)
    {
        if (err) [[unlikely]]
        {
            throw toolpex::posix_exception{ err };
        }
    }

    explicit posix_err_thrower(::std::error_code ec)
    {
        if (ec) [[unlikely]]
        {
            throw toolpex::posix_err_thrower{ ::std::move(ec) };
        }
    }
};

TOOLPEX_NAMESAPCE_END

#endif

