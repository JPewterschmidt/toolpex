// This file is part of Koios
// https://github.com/JPewterschmidt/koios
//
// Copyleft 2023 - 2024, ShiXin Wang. All wrongs reserved.

#include "gtest/gtest.h"

#include "toolpex/skip_list.h"
#include "toolpex/assert.h"

#include <algorithm>
#include <ranges>
#include <map>

using namespace toolpex;
namespace rv = ::std::ranges::views;

namespace 
{

class skip_list_test : public ::testing::Test
{
public:
    skip_list_test()
        : s(old_max_level)
    {
        prepare_data();
    }

    void reset(size_t max_level, size_t max_value)
    {
        s = skip_list<long long, long long>(max_level);
        prepare_data(max_value);
    }

    void reset()
    {
        s = skip_list<long long, long long>(old_max_level);
        prepare_data();
    }

    auto& list() { return s; }

    void prepare_data(long long max = 1000)
    {
        toolpex_assert(max > 50);
        for (long long i : ::std::ranges::iota_view{50, max})
            list().insert(i, i + 1);

        for (int i = 49; i >= 0; --i)
        {
            list().insert((long long)i, (long long)(i + 1));
        }
    }

    auto make_vec_as_pd(long long max = 1000)
    {
        ::std::vector<::std::pair<long long, long long>> result;
        toolpex_assert(max > 50);
        for (long long i : ::std::ranges::iota_view{50, max})
            list().insert(i, i + 1);

        for (int i = 49; i >= 0; --i)
        {
            result.emplace_back((long long)i, (long long)(i + 1));
        }

        ::std::sort(result.begin(), result.end());
        return result;
    }

private:
    long long old_max_level{16};
    skip_list<long long, long long> s;
};

} // annoymous namespace

TEST_F(skip_list_test, basic)
{
    ASSERT_EQ(skip_list_debug{list()}.actual_size(), list().size());

    auto s2 = ::std::move(list());

    ASSERT_EQ(list().size(), 0);
    ASSERT_EQ(s2.size(), 1000);
    ASSERT_EQ(s2.find(3)->second, 4);
    ASSERT_EQ(s2.find(100)->second, 101);

    ASSERT_TRUE(s2.contains(10));

    s2.clear();
    ASSERT_TRUE(s2.empty());
    reset();
}

TEST_F(skip_list_test, special_member_func)
{
    reset(8, 1000);

    skip_list<long long, long long> s2{8};
    for (int i : ::std::ranges::iota_view{0, 100})
        s2.insert(i, i + 1);

    ASSERT_EQ(s2.size(), 100);
    s2 = ::std::move(list());
    ASSERT_EQ(s2.size(), 1000);
    ASSERT_EQ(list().size(), 0);
}

TEST_F(skip_list_test, iterator)
{
    reset(4, 100);
    auto iter = list().insert(500, 10);
    iter->second++;
    ASSERT_EQ(list().find(500)->second, 11);

    ::std::map<long long, long long> imap(list().begin(), list().end());
    ASSERT_EQ(imap.size(), list().size());

    iter = list().last();
    ASSERT_EQ(iter->second, 11);
    iter->second = 12;
    auto val = ::std::as_const(list()).last()->second;
    ASSERT_EQ(val, 12);

    ASSERT_TRUE((::std::ranges::range<decltype(list())>));
}

TEST_F(skip_list_test, subscriptor_operator)
{
    reset(4, 60);
    list()[1] = 24;
    ASSERT_EQ(list()[1], 24);
}

TEST_F(skip_list_test, insert_range)
{
    auto r = ::std::ranges::iota_view{0, 100} 
        | ::std::ranges::views::transform([](auto&& v) noexcept { 
            return ::std::pair{ v, v + 1 }; 
          });
    list().insert_range(r);
}

TEST_F(skip_list_test, front_and_back)
{
    reset(4, 100);

    ASSERT_EQ(list().front().first, 0);
    ASSERT_EQ(list().back().first, 99);
    list().front().second = 2;
    ASSERT_EQ(::std::as_const(list()).front().second, 2);
}

TEST_F(skip_list_test, lower_bound)
{
    reset(4, 100);

    auto iter = list().lower_bound(99);
    ASSERT_EQ(iter->first, 99);
    ASSERT_EQ(::std::as_const(list()).lower_bound(99)->first, iter->first);
}

TEST_F(skip_list_test, upper_bound)
{
    reset(4, 100);

    auto iter = list().upper_bound(99);
    ASSERT_EQ(iter->first, 98);
    ASSERT_EQ(::std::as_const(list()).upper_bound(99)->first, iter->first);
}

TEST_F(skip_list_test, find_first_bigger)
{
    reset(4, 100);

    auto iter = list().find_first_bigger(99);
    ASSERT_EQ(iter, list().end());
    iter = list().find_first_bigger(1);
    ASSERT_EQ(iter->first, 2);
    ASSERT_EQ(::std::as_const(list()).find_first_bigger(1)->first, iter->first);
}

TEST_F(skip_list_test, find_last_less)
{
    reset(4, 100);

    auto iter = list().find_last_less(0);
    ASSERT_EQ(iter, list().end());
    iter = list().find_last_less(3);
    ASSERT_EQ(iter->first, 2);
    ASSERT_EQ(::std::as_const(list()).find_last_less(3)->first, iter->first);
}

TEST_F(skip_list_test, find_first_bigger_equal)
{
    reset(4, 100);

    auto iter = list().find_first_bigger_equal(99);
    ASSERT_EQ(iter->first, 99);
    ASSERT_EQ(::std::as_const(list()).find_first_bigger_equal(99)->first, iter->first);

    iter = list().find_first_bigger_equal(1000);
    ASSERT_EQ(iter, list().end());
}

TEST_F(skip_list_test, find_last_less_equal)
{
    reset(4, 100);

    auto iter = list().find_last_less_equal(0);
    ASSERT_EQ(iter->first, 0);
    ASSERT_EQ(::std::as_const(list()).find_last_less_equal(0)->first, iter->first);

    iter = list().find_last_less_equal(-1);
    ASSERT_EQ(iter, list().end());
    
    iter = list().find_last_less_equal(20);
    ASSERT_EQ(iter->first, 20);

    iter = list().find_last_less_equal(100);
    ASSERT_EQ(iter->first, 99) << "iter->first == " << iter->first;
}

TEST_F(skip_list_test, erase_with_value)
{
    reset();
    ::std::pair<long long, long long> kv;
    bool success = list().erase(99, &kv);
    ASSERT_TRUE(success);
    ASSERT_EQ(kv.first, 99);
    ASSERT_EQ(kv.second, 100);
    ASSERT_FALSE(list().erase(99));
}

TEST_F(skip_list_test, erase_without_value)
{
    reset();
    bool success = list().erase(99);
    ASSERT_TRUE(success);
    ASSERT_FALSE(list().erase(99));
}

TEST_F(skip_list_test, ordered_test)
{
    reset();
    ASSERT_TRUE(::std::is_sorted(
        list().begin(), list().end(), 
        [](const auto& lhs, const auto& rhs) { 
            return lhs.first < rhs.first; 
        }
    ));
}

TEST_F(skip_list_test, correctness)
{
    reset();
    prepare_data();
    auto vec = make_vec_as_pd();
    auto& l = list();
    ASSERT_TRUE(::std::ranges::is_sorted(l));
    ASSERT_TRUE(::std::ranges::is_sorted(vec));

    auto vi = begin(vec);
    auto li = begin(l);
    for (; vi != end(vec) && li != end(l); ++vi, ++li)
    {
        ASSERT_EQ(*vi, *li);
    }
}
