// This file is part of Koios
// https://github.com/JPewterschmidt/koios
//
// Copyleft 2023 - 2024, ShiXin Wang. All wrongs reserved.

#include "gtest/gtest.h"
#include "toolpex/counter.h"

using namespace toolpex;

TEST(counter, approximate_limit_counter)
{
    approximate_limit_counter c{1000};
    auto h = c.get_specific_handler(::std::this_thread::get_id());

    ASSERT_TRUE(h.add_count(8));
    ASSERT_TRUE(h.add_count(1));
    ASSERT_EQ(h.read_count(), 9);

    ASSERT_FALSE(h.add_count(10000));
    ASSERT_EQ(h.read_count(), 9);

    c.reset();
    ASSERT_EQ(h.read_count(), 0);
    ASSERT_EQ(c.limit(), 1000);

    c.reset(100);
    ASSERT_EQ(c.limit(), 100);
}
