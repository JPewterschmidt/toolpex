#include <iomanip>
#include <thread>
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
#include "toolpex/counter.h"

using namespace toolpex;
using namespace ip_address_literals;
using namespace ::std::string_literals;
using namespace ::std::string_view_literals;

int main()
{
    approximate_limit_counter c{1000};
    auto h = c.get_specific_handler(::std::this_thread::get_id());
    h.add_count(1);   
    ::std::cout << h.read_count() << ::std::endl;

    return 0;
}
