#include <iostream>
#include "toolpex/unique_resource.h"
#include "toolpex/unique_posix_fd.h"

class foo
{
public:
    foo() noexcept { ::std::cout << "construct" << ::std::endl; }
    foo(foo&& f) noexcept { ::std::cout << "move construct" << ::std::endl; }
    ~foo() noexcept { ::std::cout << "foo dtor" << ::std::endl; }
};

int main()
{
    toolpex::unique_posix_fd handle{ 1 };
    int val = handle;
    
    return 0;
}
