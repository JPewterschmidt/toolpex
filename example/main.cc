#include <iostream>
#include "toolpex/unique_resource.h"

class foo
{
public:
    foo() noexcept { ::std::cout << "construct" << ::std::endl; }
    foo(foo&& f) noexcept { ::std::cout << "move construct" << ::std::endl; }
    ~foo() noexcept { ::std::cout << "foo dtor" << ::std::endl; }
};

int main()
{
    toolpex::unique_resource handle{ foo{}, [](foo& f) noexcept {} };
    
    return 0;
}
