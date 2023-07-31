#ifndef TOOLPEX_ERRRET_THROWER_H
#define TOOLPEX_ERRRET_THROWER_H

#include <cstring>
#include <concepts>
#include <functional>
#include <stdexcept>
#include <cerrno>

#include "toolpex/macros.h"
#include "toolpex/exceptions.h"
#include "toolpex/functional.h"

TOOLPEX_NAMESAPCE_BEG

template<typename ExceptType = ::std::runtime_error, ::std::default_initializable Logger = do_nothing>
class errret_thrower
{
public:
    int operator()(int err)
    {
        if (err >= 0) return err;

        const char* const msg = (err == -1) ? ::strerror(errno) : ::strerror(-err);
        Logger{}(msg);
        throw ExceptType{ msg };
    }

    void operator()(::std::error_code ec)
    {
        if (ec.value() == 0) return;

        Logger{}(ec.message());
        throw ExceptType{ ec.message() };
    }

    int operator <<(int err) { return operator()(err); }
    void operator << (::std::error_code ec) { operator()(::std::move(ec)); }
};

TOOLPEX_NAMESAPCE_END

#endif
