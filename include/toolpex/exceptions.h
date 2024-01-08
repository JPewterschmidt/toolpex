#ifndef TOOLPEX_EXCEPTIONS_H
#define TOOLPEX_EXCEPTIONS_H

#include <source_location>
#include <string_view>
#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include <exception>
#include <stdexcept>
#include <system_error>
#include <string>

#include "fmt/core.h"
#include "toolpex/macros.h"

TOOLPEX_NAMESPACE_BEG

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
        : ::std::system_error(err, ::std::system_category())
    {
    }

    explicit posix_exception(::std::error_code ec) noexcept
        : ::std::system_error(ec)
    {
    }
};

class ip_address_exception : public ::std::system_error
{
public:
    explicit ip_address_exception(int err) noexcept
        : ::std::system_error(err, ::std::system_category())
    {
        m_is_errno = true;
    }

    explicit ip_address_exception(::std::error_code ec) noexcept
        : ::std::system_error(ec)
    {
        m_is_errno = true;
    }

    ip_address_exception(::std::string_view sv) noexcept
        : m_msg{ sv }
    {
        m_is_errno = false;
    }

    const char* what() const noexcept
    {
        if (m_is_errno) 
            return ::std::system_error::what();
        return m_msg.c_str();
    }

private:
    bool m_is_errno{ false };
    ::std::string m_msg{};
};

TOOLPEX_NAMESPACE_END

#endif
