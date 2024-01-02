#include "toolpex/ipaddress.h"
#include "gtest/gtest.h"
#include <sys/socket.h>

using namespace toolpex;

TEST(ipaddress, v4endians)
{
    auto ip = "127.0.0.1"_ip;
    auto posixaddr = ip->to_sockaddr(8889);
    auto ip2 = ip_address::make(
        reinterpret_cast<::sockaddr*>(&posixaddr), 
        sizeof(::sockaddr_in)
    );
    ASSERT_EQ(ip->to_string(), ip2->to_string());
}

TEST(ipaddress, v6endians)
{
    auto ip = "FC00:0000:130F:0000:0000:09C0:876A:130B"_ip;
    auto posixaddr = ip->to_sockaddr(8889);
    auto ip2 = ip_address::make(
        reinterpret_cast<::sockaddr*>(&posixaddr), 
        sizeof(::sockaddr_in6)
    );
    ASSERT_EQ(ip->to_string(), ip2->to_string());
}
