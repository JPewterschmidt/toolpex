// This file is part of Koios
// https://github.com/JPewterschmidt/koios
//
// Copyleft 2023 - 2024, ShiXin Wang. All wrongs reserved.

#include "gtest/gtest.h"
#include "toolpex/functional.h"

TEST(functional, do_nothing)
{
    toolpex::do_nothing{}();
    toolpex::do_nothing{}(1);
    toolpex::do_nothing{}(toolpex::do_nothing{}, 2, 3);
}
