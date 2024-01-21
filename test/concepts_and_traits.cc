#include <functional>
#include <string>
#include <typeinfo>

#include "gtest/gtest.h"
#include "toolpex/concepts_and_traits.h"

using namespace toolpex;

namespace 
{
    void dummy1(int, double, int){}
    void dummy2(double, int){}
    void dummy3(){}

    ::std::function<void(int, int, int)> dummy4;

    struct dummy5_t
    {
        constexpr void operator()(int, int) const noexcept {}
    };

    // TODO
    //auto dummy6 = ::std::bind(dummy1, 1);
    
    ::std::function<double()> dummy7;
    double dummy8() { return 1.0; }
}

TEST(number_of_parameters, basic)
{
    ASSERT_EQ(int(toolpex::number_of_parameters_v<decltype(dummy1)>), 3);
    ASSERT_EQ(int(toolpex::number_of_parameters_v<decltype(dummy2)>), 2);
    ASSERT_EQ(int(toolpex::number_of_parameters_v<decltype(dummy3)>), 0);
    ASSERT_EQ(toolpex::number_of_parameters_v<decltype(dummy4)>, 3);
    // TODO
    //ASSERT_EQ(toolpex::number_of_parameters_v<decltype(dummy5_t::operator())>, 2);
}

TEST(get_return_type, basic)
{
    ::std::string name1 = typeid(get_return_type_t<decltype(dummy7)>).name();
    ::std::string name2 = typeid(get_return_type_t<decltype(dummy8)>).name();
    ASSERT_EQ(name1, name2);
}
