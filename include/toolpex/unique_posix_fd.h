#ifndef TOOLPEX_UNIQUE_POSIX_FD_H
#define TOOLPEX_UNIQUE_POSIX_FD_H

#include "toolpex/macros.h"
#include "toolpex/unique_resource.h"

TOOLPEX_NAMESAPCE_BEG

struct posix_fd_closer
{
    void operator()(int fd) const noexcept;
};

class unique_posix_fd : public unique_resource<int, posix_fd_closer>
{
public:
    unique_posix_fd(int fd);
    operator int() const noexcept;
};

TOOLPEX_NAMESAPCE_END

#endif
