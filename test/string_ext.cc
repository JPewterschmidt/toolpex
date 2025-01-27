#include "gtest/gtest.h"
#include "toolpex/string_ext.h"

#include <string>
#include <string_view>

using namespace toolpex;
using namespace ::std::string_literals;
using namespace ::std::string_view_literals;

TEST(string_ext, trim)
{
    ::std::string str = "     abc   ";
    ASSERT_EQ(trim(str), "abc"s);
}

TEST(string_ext, remove_quotation_mark)
{
    ::std::string_view str = "     \"  abc  \"    ";
    ASSERT_EQ(trim(remove_quotation_mark(trim(str))), "abc"sv);
}

TEST(string_ext, case_compare)
{
    auto str1 = "AbC"sv, str2 = "aBC"sv;
    ASSERT_TRUE(str_case_equal(str1, str2));
    ASSERT_TRUE(str_case_leq(str1, str2));
    ASSERT_TRUE(str_case_geq(str1, str2));
    
    auto str3 = "123"sv, str4 = "456"sv;
    ASSERT_FALSE(str_case_equal(str3, str4));
    ASSERT_TRUE(str_case_less(str3, str4));
    ASSERT_TRUE(str_case_leq(str3, str4));
    ASSERT_FALSE(str_case_geq(str3, str4));
    ASSERT_FALSE(str_case_greater(str3, str4));
}
