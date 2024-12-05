#include <sys/socket.h>

#include "toolpex/socket_util.h"
#include "toolpex/posix_err_thrower.h"

namespace toolpex
{

unique_posix_fd socket_nonblock(int domain, int type, int protocol)
{
    return pet{} << ::socket(domain, type | SOCK_NONBLOCK, protocol);
}

} // namespace toolpex
