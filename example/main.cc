#include <iostream>
#include "toolpex/unique_resource.h"
#include "toolpex/unique_posix_fd.h"

namespace
{
    int success{};
}

int main()
{
    {
        toolpex::unique_resource handle1{ 1, [](int i){ success = 1; } };
        //auto handle2 = ::std::move(handle1);
    }
    ::std::cout << success << ::std::endl;
}
