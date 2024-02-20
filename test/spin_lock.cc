#include "gtest/gtest.h"
#include "toolpex/spin_lock.h"
#include <mutex>
#include <thread>

using namespace toolpex;

namespace
{
    spin_lock g_lock;
    int g_value{};
    
    void increaser()
    {
        for (size_t i{}; i < 100000; ++i)
        {
            ::std::lock_guard lk{ g_lock };
            g_value += 2;
        }
    }

    void decreaser()
    {
        for (size_t i{}; i < 99999; ++i)
        {
            ::std::lock_guard lk{ g_lock };
            g_value -= 2;
        }
    }
}

TEST(spin_lock, basic)
{
    {
        ::std::jthread t1{ increaser };
        ::std::jthread t2{ decreaser };
    }
    ASSERT_EQ(g_value, 2);
}

TEST(spin_lock, observer)
{
    ::std::lock_guard lk{ g_lock };
    ASSERT_EQ(g_lock.is_locked(), true);
}
