#ifndef TOOLPEX_UNIQUE_POSIX_FD_H
#define TOOLPEX_UNIQUE_POSIX_FD_H

#include <utility>

#include "toolpex/macros.h"
#include "toolpex/unique_resource.h"

extern "C"
{
    int close(int);
}

TOOLPEX_NAMESPACE_BEG

class unique_posix_fd
{
public:
    constexpr unique_posix_fd() = default;

    unique_posix_fd(int fd) noexcept
        : m_fd{ fd }
    {
    }

    ~unique_posix_fd() noexcept
    {
        close();
    }

    unique_posix_fd(unique_posix_fd&& other) noexcept
        : m_fd{ other.release() }
    {
    }

    unique_posix_fd& operator=(unique_posix_fd&& other) noexcept
    {
        close();
        m_fd = other.release();

        return *this;
    }

    unique_posix_fd(const unique_posix_fd&) = delete;
    unique_posix_fd& operator=(const unique_posix_fd&) = delete;

    operator int() const noexcept { return m_fd; }

    void close() noexcept
    {
        if (valid())
            ::close(release());
    }

private:
    bool valid() const noexcept { return m_fd != -1; }
    int release() noexcept { return ::std::exchange(m_fd, -1); }

private:
    int m_fd{ -1 };
};

TOOLPEX_NAMESPACE_END

#endif
