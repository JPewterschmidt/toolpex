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
using namespace ip_address_literals;

void func()
{
    toolpex::not_implemented("shit gold");
}
 
int main()
{
    auto ip1 = "::192.168.0.1"_ip;
    ::std::cout << ip1;
}
