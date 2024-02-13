#include "toolpex/bits_manipulator.h"
#include "gtest/gtest.h"
#include <bitset>

using namespace toolpex;

TEST(bits_manipulator, basic)
{
    constexpr ::std::bitset<8> flags[]{
        ::std::bitset<8>{"00000001"}, 
        ::std::bitset<8>{"00000010"}, 
        ::std::bitset<8>{"00000100"}, 
        ::std::bitset<8>{"00001000"}
    };

    bits_manipulator m{ 
        flags[0].to_ulong() |
        flags[1].to_ulong() |
        flags[2].to_ulong() | 
        flags[3].to_ulong()
    };

    ASSERT_TRUE(m.contains(flags[0].to_ulong())) << ::std::endl;
    ASSERT_TRUE(m.contains(flags[1].to_ulong())) << ::std::endl;
    ASSERT_TRUE(m.contains(flags[2].to_ulong())) << ::std::endl;
    ASSERT_TRUE(m.contains(flags[3].to_ulong())) << ::std::endl;

    m.remove(flags[0].to_ulong()).remove(flags[3].to_ulong());
    ASSERT_FALSE(m.contains(flags[0].to_ulong())) << ::std::endl;
    ASSERT_FALSE(m.contains(flags[3].to_ulong())) << ::std::endl;
}
