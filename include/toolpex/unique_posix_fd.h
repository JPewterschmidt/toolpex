#ifndef TOOLPEX_UNIQUE_POSIX_FD_H
#define TOOLPEX_UNIQUE_POSIX_FD_H

#include "toolpex/macros.h"
#include "toolpex/unique_resource.h"

extern "C"
{
    int close(int);
}

TOOLPEX_NAMESAPCE_BEG

struct posix_fd_closer
{
    void operator()(int fd) const noexcept
    {
        ::close(fd);
    }
};

class unique_posix_fd : public unique_resource<int, posix_fd_closer>
{
public:
    unique_posix_fd(int fd)
        : unique_resource<int, posix_fd_closer>(fd, posix_fd_closer{})
    {
    }

    operator int() const noexcept 
    {
        return m_resource;
    }
};

TOOLPEX_NAMESAPCE_END

#endif
