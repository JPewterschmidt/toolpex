#include "gtest/gtest.h"
#include "toolpex/page.h"
#include <ranges>

TEST(page, basic)
{
    page p{ 4096 };
    ASSERT_EQ(p.capacity(), 4096);

    auto pp = p.next_writable();
    for (size_t i{}; i < 4098; ++i)
    {
        pp[i] = (::std::byte)1;
    }
    p.commit_write(4098);
    
    ASSERT_EQ(p.size(), 4098);
}
