#include "gtest/gtest.h"
#include "toolpex/errret_thrower.h"
#include <cerrno>
#include <string_view>

using namespace ::std::string_view_literals;

TEST(errret_thrower, negative_1)
{
    errno = 22;
    toolpex::errret_thrower et;
    ASSERT_EQ(et(1), 1);
    ASSERT_EQ(et(0), 0);
    bool success_flag{};
    
    try
    {
        et(-1);
    }
    catch (::std::runtime_error& e)
    {
        ASSERT_EQ(e.what(), "Invalid argument"sv);
        success_flag = true;
    }

    ASSERT_EQ(success_flag, true);
}

TEST(errret_thrower, negative_errno)
{
    toolpex::errret_thrower et;
    bool success_flag{};
    
    try
    {
        et(-22);
    }
    catch (::std::runtime_error& e)
    {
        ASSERT_EQ(e.what(), "Invalid argument"sv);
        success_flag = true;
    }

    ASSERT_EQ(success_flag, true);
}
