#include <string_view>
#include <ranges>

#include "gtest/gtest.h"
#include "toolpex/tic_toc.h"
#include "toolpex/in_there.h"

using namespace toolpex;
using namespace ::std::string_literals;
using namespace ::std::string_view_literals;

TEST(tic_toc, basic)
{
    auto tp = toolpex::tic();
    auto str = toolpex::toc(tp);
}

TEST(in_there, basic)
{
    ASSERT_TRUE(bool(in_there{ "123"sv, "123"s, "345", "456"sv }));
    ASSERT_TRUE(bool(in_there{ 123, 123, 34, 45, 56 }));
    ASSERT_FALSE(bool(in_there{ 123, 34, 45, 56 }));
}
