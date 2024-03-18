#include "gtest/gtest.h"
#include "toolpex/skip_list.h"
#include <ranges>
#include <map>

using namespace toolpex;

TEST(skip_list, basic)
{
    skip_list<int, int, 16> s{};

    for (int i : ::std::ranges::iota_view{0, 1000})
        s.insert(i, i + 1);
    ASSERT_EQ(skip_list_debug{s}.actual_size(), s.size());

    auto s2 = ::std::move(s);

    ASSERT_EQ(s.size(), 0);
    ASSERT_EQ(s2.size(), 1000);
    ASSERT_EQ(s2.find(3)->second, 4);
    ASSERT_EQ(s2.find(100)->second, 101);

    ASSERT_TRUE(s2.contains(10));

    s2.clear();
    ASSERT_TRUE(s2.empty());
}

TEST(skip_list, special_member_func)
{
    skip_list<int, int, 8> s{};

    for (int i : ::std::ranges::iota_view{0, 1000})
        s.insert(i, i + 1);

    skip_list<int, int, 8> s2{};
    for (int i : ::std::ranges::iota_view{0, 100})
        s2.insert(i, i + 1);

    ASSERT_EQ(s2.size(), 100);
    s2 = ::std::move(s);
    ASSERT_EQ(s2.size(), 1000);
    ASSERT_EQ(s.size(), 0);
}

TEST(skip_list, iterator)
{
    skip_list<int, int, 4> s{};

    for (int i : ::std::ranges::iota_view{0, 100})
        s.insert(i, i + 1);
    auto iter = s.insert(500, 10);
    iter->second++;
    ASSERT_EQ(s.find(500)->second, 11);

    ::std::map<int, int> imap(s.begin(), s.end());
    ASSERT_EQ(imap.size(), s.size());

    iter = s.last();
    ASSERT_EQ(iter->second, 11);
    iter->second = 12;
    auto val = ::std::as_const(s).last()->second;
    ASSERT_EQ(val, 12);
}

TEST(skip_list, subscriptor_operator)
{
    skip_list<int, int, 4> s{};
    s[1] = 24;
    ASSERT_EQ(s[1], 24);
}

TEST(skip_list, insert_range)
{
    skip_list<int, int, 4> s{};
    auto r = ::std::ranges::iota_view{0, 100} 
        | ::std::ranges::views::transform([](auto&& v) noexcept { 
            return ::std::pair{ v, v + 1 }; 
          });
    s.insert_range(r);
}

TEST(skip_list, front_and_back)
{
    skip_list<int, int, 4> s{};
    auto r = ::std::ranges::iota_view{0, 100} 
        | ::std::ranges::views::transform([](auto&& v) noexcept { 
            return ::std::pair{ v, v + 1 }; 
          });
    s.insert_range(r);

    ASSERT_EQ(s.front().first, 0);
    ASSERT_EQ(s.back().first, 99);
    s.front().second = 2;
    ASSERT_EQ(::std::as_const(s).front().second, 2);
}
