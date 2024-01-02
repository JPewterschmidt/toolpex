#include <iomanip>
#include <iostream>
#include <ranges>
#include <string_view>
#include "toolpex/exceptions.h"
#include "toolpex/concepts_and_traits.h"
#include <sys/socket.h>
#include <string_view>
#include <arpa/inet.h>
#include <cstring>
#include "toolpex/ipaddress.h"

using namespace toolpex;

void func()
{
    toolpex::not_implemented("shit gold");
}
 
int main()
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

}
