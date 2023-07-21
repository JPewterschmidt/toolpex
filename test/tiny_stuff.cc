#include <string_view>
#include <ranges>

#include "gtest/gtest.h"
#include "toolpex/tic_toc.h"

TEST(tic_toc, basic)
{
    auto tp = toolpex::tic();
    auto str = toolpex::toc(tp);
}
