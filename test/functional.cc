#include "gtest/gtest.h"
#include "toolpex/functional.h"

TEST(functional, do_nothing)
{
    toolpex::do_nothing{}();
    toolpex::do_nothing{}(1);
    toolpex::do_nothing{}(toolpex::do_nothing{}, 2, 3);
}
