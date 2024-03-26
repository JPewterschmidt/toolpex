#include "toolpex/ref_count.h"
#include "gtest/gtest.h"
#include <thread>
#include <future>

using namespace toolpex;

namespace
{

ref_count g_cnt;
constexpr size_t test_scale{ 500 };

}

TEST(ref_count, basic)
{
    ::std::vector<::std::future<void>> fvec;
    for (size_t i{}; i < test_scale / 10; ++i)   
        for (size_t j{}; j < 10; ++j)
            fvec.emplace_back(::std::async(::std::launch::async, [&]{ g_cnt++; }));

    for (auto& f : fvec) f.wait();
    ASSERT_EQ(g_cnt.load(), test_scale);

    for (size_t i{}; i < test_scale / 10; ++i)   
        for (size_t j{}; j < 10; ++j)
            fvec.emplace_back(::std::async(::std::launch::async, [&]{ g_cnt--; }));

    for (auto& f : fvec) f.wait();
    ASSERT_EQ(g_cnt.load(), 0);
}

TEST(ref_count, guard)
{
    ::std::vector<::std::future<void>> fvec;
    for (size_t i{}; i < test_scale / 10; ++i)   
        for (size_t j{}; j < 10; ++j)
            fvec.emplace_back(::std::async(::std::launch::async, [&]{ 
                [[maybe_unused]] ref_count_guard g{ g_cnt }; 
            }));

    for (auto& f : fvec) f.wait();
    ASSERT_EQ(g_cnt.load(), 0);
}
