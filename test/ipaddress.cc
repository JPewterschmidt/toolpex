#include "toolpex/ipaddress.h"
#include "gtest/gtest.h"
#include <sys/socket.h>
#include <string_view>
#include <arpa/inet.h>
#include <cstring>
#include "toolpex/exceptions.h"

using namespace toolpex;
using namespace ip_address_literals;

TEST(ipaddress, v4endians)
{
    auto ip = "127.0.0.1"_ip;
    auto [posixaddr, sz] = ip->to_sockaddr(8889);
    auto [ip2, port] = ip_address::make(
        reinterpret_cast<::sockaddr*>(&posixaddr), 
        sizeof(::sockaddr_in)
    );
    ASSERT_EQ(ip->to_string(), ip2->to_string());
}

TEST(ipaddress, v6endians)
{
    auto ip = "FC00:0000:130F:0000:0000:09C0:876A:130B"_ip;
    auto [posixaddr, sz] = ip->to_sockaddr(8889);
    auto [ip2, port] = ip_address::make(
        reinterpret_cast<::sockaddr*>(&posixaddr), 
        sizeof(::sockaddr_in6)
    );
    ASSERT_EQ(ip->to_string(), ip2->to_string());
}

TEST(ipaddress, v4_with_pton)
{
    auto ip = "127.0.0.1"_ip;
    auto [myposixaddr, sz] = ip->to_sockaddr(8889);
    
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
    auto [myposixaddr, sz] = ip->to_sockaddr(8889);
    
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

TEST(ipaddress, v4_tosockaddr)
{
    auto ip = "127.0.0.1"_ip;
    constexpr ::in_port_t right_port = 8889;
    auto [myposixaddr, sz] = ip->to_sockaddr(right_port);

    auto [newip, port] = ip_address::make((sockaddr*)&myposixaddr, sz);
    ASSERT_EQ(port, right_port);
    ASSERT_EQ(newip->to_string(), ip->to_string());
}

TEST(ipaddress, v6_tosockaddr)
{
    auto ip = "FC00:0000:130F:0000:0000:09C0:876A:130B"_ip;
    constexpr ::in_port_t right_port = 8889;
    auto [myposixaddr, sz] = ip->to_sockaddr(right_port);

    auto [newip, port] = ip_address::make((sockaddr*)&myposixaddr, sz);
    ASSERT_EQ(port, right_port);
    ASSERT_EQ(newip->to_string(), ip->to_string());
}

TEST(ipaddress, family)
{
    auto ip1 = "FC00:0000:130F:0000:0000:09C0:876A:130B"_ip;
    auto ip2 = "127.0.0.1"_ip;
    
    ASSERT_EQ(ip1->family(), AF_INET6);
    ASSERT_EQ(ip2->family(), AF_INET);
}

TEST(ipaddress, useful)
{
    auto a1 = ipv4_address::get_loopback();
    auto a2 = "127.0.0.1"_ip;
    ASSERT_TRUE(a1 != nullptr && a2 != nullptr);
    ASSERT_EQ(a1->to_string(), a2->to_string());

    a1 = ipv4_address::get_broadcast();
    a2 = "255.255.255.255"_ip;
    ASSERT_TRUE(a1 != nullptr && a2 != nullptr);
    ASSERT_EQ(a1->to_string(), a2->to_string());

    a1 = ipv4_address::get_allzero();
    a2 = "0.0.0.0"_ip;
    ASSERT_TRUE(a1 != nullptr && a2 != nullptr);
    ASSERT_EQ(a1->to_string(), a2->to_string());

    a1 = ipv6_address::get_allzero();
    a2 = "::"_ip;
    ASSERT_TRUE(a1 != nullptr && a2 != nullptr);
    ASSERT_EQ(a1->to_string(), a2->to_string());

    a1 = ipv6_address::get_loopback();
    a2 = "::1"_ip;
    ASSERT_TRUE(a1 != nullptr && a2 != nullptr);
    ASSERT_EQ(a1->to_string(), a2->to_string());

    a1 = ipv4_address::get_localhost();
    a2 = ipv4_address::get_allzero(); // TODO
    ASSERT_TRUE(a1 != nullptr && a2 != nullptr);
    ASSERT_EQ(a1->to_string(), a2->to_string());
}

TEST(ipaddress, wrong_usage)
{
    bool flag {};

    ::std::vector<::std::string_view> wrong_ipstrs{ 
        "fuck you", 
        "8890", 
        //"ip = ::1",  // TODO
        "::192.168.0.1", 
        "192.168.0", 
        "192.168.0.1::1", 
        "127.0.0.0.0.0.1",
        "我是你爹",
    };

    for (::std::string_view str : wrong_ipstrs)
    {
        flag = false;
        try { auto _ = ip_address::make(str); }
        catch (const toolpex::ip_address_exception& e)
        {
            flag = true;
        }
        ASSERT_TRUE(flag);
    }
}
