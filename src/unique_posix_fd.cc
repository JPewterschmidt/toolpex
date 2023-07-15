#include "toolpex/unique_posix_fd.h"

extern "C"
{
    int close(int);
}

TOOLPEX_NAMESAPCE_BEG

void posix_fd_closer::operator()(int fd) const noexcept
{
    if (fd < 0) return;
    ::close(fd);
    fd = -1;
}

unique_posix_fd::unique_posix_fd(int fd)
    : unique_resource<int, posix_fd_closer>(fd, posix_fd_closer{})
{
}

unique_posix_fd::operator int() const noexcept
{
    return m_resource;
}

TOOLPEX_NAMESAPCE_END
