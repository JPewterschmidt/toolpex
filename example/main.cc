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
#include "toolpex/lru_cache.h"
#include "toolpex/functional.h"
#include "toolpex/in_there.h"
#include "toolpex/skip_list.h"

using namespace toolpex;
using namespace ip_address_literals;
using namespace ::std::string_literals;
using namespace ::std::string_view_literals;

void access_vec(const ::std::vector<int>& vec)
{
    for (const auto item : vec)
    {
        ::std::cout << item << " ";
    }
    ::std::cout << ::std::endl;
}

void access_vec2(::std::vector<int>& vec)
{
    for (const auto item : vec)
    {
        ::std::cout << item << " ";
    }
    ::std::cout << ::std::endl;
}

int main()
{
    ::std::cout << toolpex::is_power_of_2(skip_list_suggested_max_level(199));

    return 0;
}
