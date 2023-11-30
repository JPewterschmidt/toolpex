#include <iomanip>
#include <iostream>
#include <ranges>
#include <string_view>
#include "toolpex/exceptions.h"
#include "toolpex/concepts_and_traits.h"

void func()
{
    toolpex::not_implemented("shit gold");
}
 
int main()
{
    ::std::cout << toolpex::is_std_chrono_duration<::std::chrono::nanoseconds>;
}
