#ifndef TOOLPEX_EXCEPTIONS_H
#define TOOLPEX_EXCEPTIONS_H

#include <source_location>
#include <string_view>
#include <cstdlib>

#include "fmt/core.h"
#include "toolpex/macros.h"

TOOLPEX_NAMESAPCE_BEG

[[noreturn]] inline void not_implemented(::std::string_view msg = {}, ::std::source_location sl = ::std::source_location::current())
{
    fmt::print("This functionality is not implemented! "
               "msg: {}, location: {} :{}, function signature: {}.\n",
               msg, sl.file_name(), sl.line(), sl.function_name());

    ::std::exit(EXIT_FAILURE);   
}

TOOLPEX_NAMESAPCE_END

#endif
