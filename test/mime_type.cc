#include <string_view>

#include "gtest/gtest.h"
#include "toolpex/mime_type.h"

using namespace toolpex;
using namespace ::std::string_view_literals;

TEST(mime_type, basic)
{
    ASSERT_EQ(toolpex::get_mime_type("xxx.mp4"), "video/mpeg4"sv);
}
