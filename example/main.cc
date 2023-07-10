#include <iostream>
#include <functional>

#include "toolpex/unique_resource.h"
#include "toolpex/unique_posix_fd.h"

#include "toolpex/concepts_and_traits.h"

int func(int, int, double) { return 1; }

int main()
{
    ::std::cout << toolpex::number_of_parameters_v<decltype(func)> << ::std::endl;
}
