#include "toolpex/after_value_set.h"
#include "gtest/gtest.h"

using namespace toolpex;

namespace 
{
    bool val{};

} // annoymous namespace

TEST(after_value_set, basic)
{
    after_value_set<bool> p([](bool intr) noexcept { val = intr; });
    p.set_value(true);
    p.set_value(false);
    ASSERT_TRUE(val);
}
