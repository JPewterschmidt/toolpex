#include "gtest/gtest.h"
#include "toolpex/counter.h"

using namespace toolpex;

TEST(counter, approximate_limit_counter)
{
    approximate_limit_counter c{10};
    auto h = c.get_specific_handler(::std::this_thread::get_id());
    ASSERT_TRUE(h.add_count(8));
    ASSERT_TRUE(h.add_count(1));
    ASSERT_EQ(h.read_count(), 9);
    ASSERT_FALSE(h.add_count(10));
    ASSERT_EQ(h.read_count(), 9);
}
