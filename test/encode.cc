#include "toolpex/encode.h"
#include "gtest/gtest.h"

#include <array>

using namespace toolpex;

TEST(encode, little_endian)
{
    ::std::array<uint8_t, 8> buffer{};
    encode_little_endian_to(270u, buffer);
    ASSERT_EQ(buffer[0], 0xe);
    ASSERT_EQ(buffer[1], 0x1);
}

TEST(encode, big_endian)
{
    ::std::array<uint8_t, 8> buffer{};
    encode_big_endian_to(270u, buffer);
    ASSERT_EQ(buffer[3], 0xe);
    ASSERT_EQ(buffer[2], 0x1);
}
