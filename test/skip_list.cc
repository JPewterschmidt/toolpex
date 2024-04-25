#include "gtest/gtest.h"
#include "toolpex/skip_list.h"
#include <ranges>
#include <map>

using namespace toolpex;

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
        s = skip_list<int, int>(max_level);
        prepare_data(max_value);
    }

    void reset()
    {
        s = skip_list<int, int>(old_max_level);
        prepare_data();
    }

    auto& list() { return s; }

    void prepare_data(size_t max = 1000)
    {
        assert(max > 50);
        if (max == 0) return;
        for (size_t i : ::std::ranges::iota_view{50ull, max})
            list().insert(i, i + 1);

        for (int i = 49; i >= 0; --i)
        {
            list().insert((size_t)i, (size_t)(i + 1));
        }
    }

private:
    size_t old_max_level{16};
    skip_list<int, int> s;
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

    skip_list<int, int> s2{8};
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

    ::std::map<int, int> imap(list().begin(), list().end());
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

TEST_F(skip_list_test, find_bigger)
{
    reset(4, 100);

    auto iter = list().find_bigger(99);
    ASSERT_EQ(iter, list().end());
    iter = list().find_bigger(1);
    ASSERT_EQ(iter->first, 2);
    ASSERT_EQ(::std::as_const(list()).find_bigger(1)->first, iter->first);
}

TEST_F(skip_list_test, find_less)
{
    reset(4, 100);

    auto iter = list().find_less(0);
    ASSERT_EQ(iter, list().end());
    iter = list().find_less(2);
    ASSERT_EQ(iter->first, 1);
    ASSERT_EQ(::std::as_const(list()).find_less(2)->first, iter->first);
}

TEST_F(skip_list_test, find_bigger_equal)
{
    reset(4, 100);

    auto iter = list().find_bigger_equal(99);
    ASSERT_EQ(iter->first, 99);
    ASSERT_EQ(::std::as_const(list()).find_bigger_equal(99)->first, iter->first);

    iter = list().find_bigger_equal(1000);
    ASSERT_EQ(iter, list().end());
}

TEST_F(skip_list_test, find_less_equal)
{
    reset(4, 100);

    auto iter = list().find_less_equal(0);
    ASSERT_EQ(iter->first, 0);
    ASSERT_EQ(::std::as_const(list()).find_less_equal(0)->first, iter->first);

    iter = list().find_less_equal(-1);
    ASSERT_EQ(iter, list().end());
}

TEST_F(skip_list_test, erase_with_value)
{
    reset();
    ::std::pair<int, int> kv;
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
