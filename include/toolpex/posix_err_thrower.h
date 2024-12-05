// This file is part of Toolpex
// https://github.com/JPewterschmidt/toolpex
//
// Copyleft 2023 - 2024, ShiXin Wang. All wrongs reserved.

#ifndef TOOLPEX_POSIX_ERR_THROWER_H
#define TOOLPEX_POSIX_ERR_THROWER_H

#include <system_error>

#include "toolpex/macros.h"
#include "toolpex/move_only.h"
#include "toolpex/exceptions.h"

TOOLPEX_NAMESPACE_BEG

class posix_err_thrower : toolpex::move_only
{
public:
    posix_err_thrower() noexcept = default;

    explicit posix_err_thrower(int err)
    {
        this -> operator << (err);
    }

    explicit posix_err_thrower(::std::error_code ec)
    {
        this -> operator << (ec);
    }

    int operator << (int err)
    {
        if (err) [[unlikely]]
        {
            throw toolpex::posix_exception{ err };
        }
        return err;
    }

    void operator << (::std::error_code ec)
    {
        if (ec) [[unlikely]]
        {
            throw toolpex::posix_err_thrower{ ::std::move(ec) };
        }
    }
};

using pet = posix_err_thrower;

TOOLPEX_NAMESPACE_END

#endif

