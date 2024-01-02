#include <iomanip>
#include <iostream>
#include <ranges>
#include <string_view>
#include "toolpex/exceptions.h"
#include "toolpex/concepts_and_traits.h"
#include "toolpex/ipaddress.h"

using namespace toolpex;

void func()
{
    toolpex::not_implemented("shit gold");
}
 
int main()
{
    auto p = ip_address::make("192.168.0.1");
}
