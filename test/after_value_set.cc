// This file is part of Koios
// https://github.com/JPewterschmidt/koios
//
// Copyleft 2023 - 2024, ShiXin Wang. All wrongs reserved.

#include "toolpex/after_value_set.h"
#include "gtest/gtest.h"

using namespace toolpex;

namespace 
{
    bool val{};

} // annoymous namespace

TEST(after_value_set, basic)
{
    after_value_set<bool> p([](bool intr) noexcept { val = intr; });
    p.set_value(true);
    ASSERT_TRUE(val);
    EXPECT_THROW(p.set_value(false), ::std::logic_error);
}
