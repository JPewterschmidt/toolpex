#include "gtest/gtest.h"
#include "toolpex/functional.h"

using namespace toolpex;
using namespace ::std::string_literals;
using namespace ::std::string_view_literals;

TEST(lazy_string_concater, basic)
{
    ASSERT_EQ(::std::string(lazy_string_concater{} + "xxx"s + "aaaaaa"sv + "bbb"), "xxxaaaaaabbb"sv);
}
