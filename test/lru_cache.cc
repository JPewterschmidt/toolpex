#include "gtest/gtest.h"
#include "toolpex/lru_cache.h"

using namespace toolpex;

TEST(lru_cache_test, basic_functionality)
{
    lru_cache<int, int> cache(2);

    cache.put(1, 10);
    cache.put(2, 20);

    EXPECT_EQ(cache.get(1).value(), 10);
    EXPECT_EQ(cache.get(2).value(), 20);

    cache.put(3, 30);

    EXPECT_FALSE(cache.get(1).has_value());

    EXPECT_EQ(cache.get(2).value(), 20);
    EXPECT_EQ(cache.get(3).value(), 30);
}
