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

void print(::std::ranges::range auto& r)
{
    for (const auto& item : r)
    {
        ::std::cout << item.second << ::std::endl;
    }
}

int main()
{
    skip_list<int, int, 5> s{};

    for (int i : ::std::ranges::iota_view{0, 1000})
        s.insert(i, i + 1);

    auto s2 = ::std::move(s);


    return 0;
}
