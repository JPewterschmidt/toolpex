#include "gtest/gtest.h"
#include "toolpex/errret_thrower.h"
#include <cerrno>
#include <string_view>

using namespace ::std::string_view_literals;

TEST(errret_thrower, negative_1)
{
    toolpex::errret_thrower et;
    ASSERT_EQ(et(1), 1);
    ASSERT_EQ(et(0), 0);
    EXPECT_THROW((et << -1), ::std::runtime_error);
}

TEST(errret_thrower, negative_errno)
{
    toolpex::errret_thrower et;
    EXPECT_THROW((et << -22), ::std::runtime_error);
}
