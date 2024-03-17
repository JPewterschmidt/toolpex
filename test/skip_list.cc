#include "gtest/gtest.h"
#include "toolpex/skip_list.h"
#include <ranges>

using namespace toolpex;

TEST(skip_list, basic)
{
    skip_list<int, int, 5> s{};

    for (int i : ::std::ranges::iota_view{0, 1000})
        s.insert(i, i + 1);
    ASSERT_EQ(skip_list_debug{s}.actual_size(), s.size());

    auto s2 = ::std::move(s);

    ASSERT_EQ(s.size(), 0);
    ASSERT_EQ(s2.size(), 1000);
    ASSERT_EQ(s2.find(3)->second, 4);
    ASSERT_EQ(s2.find(100)->second, 101);

    ASSERT_TRUE(s2.contains(10));
}
