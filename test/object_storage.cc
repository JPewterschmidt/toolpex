#include "toolpex/object_storage.h"
#include "toolpex/lifetimetoy.h"
#include "gtest/gtest.h"

using namespace toolpex;

TEST(object_storage, basic)
{
    object_storage<lifetimetoy> s1;
    s1.set_value(666);
    ASSERT_TRUE(s1.has_value());

    object_storage<lifetimetoy> s2{ ::std::move(s1) };

    ASSERT_FALSE(s1.has_value());

    auto ltt = s2.get_value();
    ASSERT_EQ(ltt.visit(), 666);
    ASSERT_FALSE(s2.has_value());
}
