#include "gtest/gtest.h"
#include "toolpex/unique_resource.h"
#include "toolpex/unique_posix_fd.h"

namespace
{
    int success{};
}

TEST(unique_resource, basic)
{
    success = 0;
    {
        toolpex::unique_resource handle1{ 1, [](int i){ success = 1; } };
    }
    ASSERT_EQ(success, 1);
}

TEST(unique_resource, special_member_functions)
{
    success = 0;
    {
        toolpex::unique_resource handle1{ 1, [](int i){ ++success; } };
        auto handle2 = ::std::move(handle1);
        toolpex::unique_resource handle3{ ::std::move(handle2) };
    }
    ASSERT_EQ(success, 1);
}

TEST(unique_resource, posix_fd)
{
    toolpex::unique_posix_fd handle{ -2 };
    int val = handle;
    ASSERT_EQ(val, -2);
}
