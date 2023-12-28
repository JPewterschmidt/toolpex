#ifndef TOOLPEX_EXCEPTIONS_H
#define TOOLPEX_EXCEPTIONS_H

#include <source_location>
#include <string_view>
#include <cstdlib>
#include <cstdio>
#include <exception>
#include <stdexcept>
#include <system_error>

#include "fmt/core.h"
#include "toolpex/macros.h"

TOOLPEX_NAMESAPCE_BEG

[[noreturn]] inline void not_implemented(::std::string_view msg = {}, ::std::source_location sl = ::std::source_location::current())
{
    fmt::print("This functionality is not implemented! "
               "msg: {}, location: {} :{}, function signature: {}.\n",
               msg, sl.file_name(), sl.line(), sl.function_name());

    ::fflush(nullptr);
    ::quick_exit(1);
}

class posix_exception : public ::std::system_error
{
public:
    explicit posix_exception(int err) noexcept
        : ::std::system_error(err, ::std::system_error())
    {
    }

    explicit posix_exception(::std::error_code ec) noexcept
        : ::std::system_error(ec)
    {
    }
};

TOOLPEX_NAMESAPCE_END

#endif
