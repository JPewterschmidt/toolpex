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

int main()
{
    skip_list<int, int, 4> il{};
    il.insert(1, 1);
    il.insert(1, 2);
    il.insert(1, 3);
    il.insert(1, 4);
    il.insert(1, 5);
    il.insert(1, 6);

    return 0;
}
