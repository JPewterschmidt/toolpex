#include "gtest/gtest.h"
#include "toolpex/lru_cache.h"

namespace
{
    size_t evaluate_cnt{};
}

int func(int i, unsigned b, double d)
{
    ++evaluate_cnt;
    return i + b + d;
}

TEST(lru_cache, basic)
{
    toolpex::lru_cache<100, int, int, unsigned, double> f{ func };

    ASSERT_EQ(f(1, 2, 1.0), 4);
    ASSERT_EQ(f(1, 2, 1.0), 4);
    ASSERT_EQ(f(2, 2, 1.0), 5);
    ASSERT_EQ(evaluate_cnt, 2);
}

