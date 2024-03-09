#include "gtest/gtest.h"
#include "toolpex/functional.h"

using namespace toolpex;
using namespace ::std::string_literals;
using namespace ::std::string_view_literals;

namespace
{
    class foo1 { public: ::std::string to_string() const { return "foo1"; } };
}

TEST(lazy_string_concater, basic)
{
    ASSERT_EQ(::std::string(lazy_string_concater{} + "xxx"s + "aaaaaa"sv + "bbb"), "xxxaaaaaabbb"sv);
    ASSERT_EQ(::std::string(lazy_string_concater{} + foo1{} + "abc"), "foo1abc"sv);
    ASSERT_EQ(::std::string(lazy_string_concater{} + size_t{12} + "ab"), "12ab"sv);
    ASSERT_EQ(::std::string(lazy_string_concater{} + int{12} + "ab"), "12ab"sv);
}
