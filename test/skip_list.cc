#include "gtest/gtest.h"
#include "toolpex/skip_list.h"

using namespace toolpex;

TEST(skip_list, basic)
{
    skip_list<int, int, 5> s{};
    (void)s;
}
