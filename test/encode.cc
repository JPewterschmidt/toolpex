// This file is part of Koios
// https://github.com/JPewterschmidt/koios
//
// Copyleft 2023 - 2024, ShiXin Wang. All wrongs reserved.

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

TEST(encode, append)
{
    ::std::string dst;
    append_encode_big_endian_to(270u, dst);
    ASSERT_EQ(dst.size(), sizeof(unsigned));
    ASSERT_EQ(dst[3], 0xe);
    ASSERT_EQ(dst[2], 0x1);
}
