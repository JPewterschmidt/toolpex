#ifndef TOOLPEX_SOCKET_UTIL_H
#define TOOLPEX_SOCKET_UTIL_H

#include "toolpex/unique_posix_fd.h"

namespace toolpex
{

unique_posix_fd socket_nonblock(int domain, int type, int protocol);

} // namespace toolpex

#endif
