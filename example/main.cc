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
#include <bitset>
#include "toolpex/bit_mask.h"

using namespace toolpex;
using namespace ip_address_literals;

int main()
{
    constexpr ::std::bitset<8> flags[]{
        ::std::bitset<8>{"00000001"}, 
        ::std::bitset<8>{"00000010"}, 
        ::std::bitset<8>{"00000100"}, 
        ::std::bitset<8>{"00001000"}
    };

    bit_mask m{ 
        flags[0].to_ulong() |
        flags[1].to_ulong() |
        flags[2].to_ulong() | 
        flags[3].to_ulong()
    };

    ::std::cout << (m.contains(flags[0].to_ulong())) << ::std::endl;
    ::std::cout << (m.contains(flags[1].to_ulong())) << ::std::endl;
    ::std::cout << (m.contains(flags[2].to_ulong())) << ::std::endl;
    ::std::cout << (m.contains(flags[3].to_ulong())) << ::std::endl;

    m.remove(flags[0].to_ulong()).remove(flags[3].to_ulong());
    ::std::cout << (m.contains(flags[0].to_ulong())) << ::std::endl;
    ::std::cout << (m.contains(flags[3].to_ulong())) << ::std::endl;

    return 0;
}
