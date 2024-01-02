#include "toolpex/ipaddress.h"
#include "gtest/gtest.h"
#include <sys/socket.h>
#include <string_view>
#include <arpa/inet.h>
#include <cstring>

using namespace toolpex;
using namespace ip_address_literals;

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

TEST(ipaddress, v4_with_pton)
{
    auto ip = "127.0.0.1"_ip;
    auto myposixaddr = ip->to_sockaddr(8889);
    
    ::std::string_view sv{ "127.0.0.1" };
    char buf[sizeof(::in_addr)]{};
    ::inet_pton(AF_INET, sv.data(), buf);
    ::sockaddr_in sock{ 
        .sin_family = AF_INET, 
        .sin_port = ::htons(8889), 
    };
    ::std::memcpy(&sock.sin_addr.s_addr, &buf, sizeof(buf));
    ::sockaddr_in mysock{};
    ::std::memcpy(&mysock, &myposixaddr, sizeof(mysock));

    ASSERT_EQ(::std::memcmp(&mysock, &sock, sizeof(mysock)), 0);
}

TEST(ipaddress, v6_with_pton)
{
    auto ip = "FC00:0000:130F:0000:0000:09C0:876A:130B"_ip;
    auto myposixaddr = ip->to_sockaddr(8889);
    
    ::std::string_view sv{ "FC00:0000:130F:0000:0000:09C0:876A:130B" };
    char buf[sizeof(::in6_addr)]{};
    ::inet_pton(AF_INET6, sv.data(), buf);
    ::sockaddr_in6 sock{ 
        .sin6_family = AF_INET6, 
        .sin6_port = ::htons(8889), 
    };
    ::std::memcpy(&sock.sin6_addr.s6_addr, &buf, sizeof(buf));
    ::sockaddr_in6 mysock{};
    ::std::memcpy(&mysock, &myposixaddr, sizeof(mysock));

    ASSERT_EQ(::std::memcmp(&mysock, &sock, sizeof(mysock)), 0);
}
