#include "toolpex/buffer.h"
#include "gtest/gtest.h"
#include "toolpex/functional.h"

#include <string>

using namespace toolpex;
using namespace ::std::string_literals;

namespace r = ::std::ranges;
namespace rv = ::std::ranges::views;

namespace
{
    constinit size_t block_size{ 16 };
}

class buffer_suite : public ::testing::Test
{
protected:
    buffer b{ block_size };

public:
    void feed_large_chunk()
    {
        auto str = rv::iota('a', 'z') | r::to<::std::string>();
        b.append(str);
    }

    bool feed_one()
    {
        ::std::array<char8_t, 1> isingle{'8'};
        auto ws = b.writable_span();
        ws[0] = isingle[0];
        return b.commit_write(1);
    }
};

TEST_F(buffer_suite, big_block)
{
    this->feed_large_chunk();
    ASSERT_GT(b.current_block_capacity(), block_size);

    // make sure all writes has been committed
    ASSERT_FALSE(b.last_block().valid_span().empty());
}

TEST_F(buffer_suite, feed_one)
{
    b.reset();
    ASSERT_TRUE(feed_one());

    // make sure all writes has been committed
    ASSERT_FALSE(b.last_block().valid_span().empty());
}

TEST_F(buffer_suite, joint)
{
    b.reset();
    feed_one();
    feed_large_chunk();

    auto sp1 = b.blocks()[0].valid_span();
    auto sp2 = b.blocks()[1].valid_span();

    auto str = b.joint_valid_view() | r::to<::std::string>();
    auto a = { sp1, sp2 }; // I'm actually wanna use rv::concat, but it's form C++26
    ASSERT_TRUE(::std::ranges::equal(str, a | rv::join)) 
        << "str: " << str
        << "sp1: " << (sp1 | r::to<::std::string>())
        << "sp2: " << (sp2 | r::to<::std::string>())
        ;
}

TEST_F(buffer_suite, dup)
{
    b.reset();
    feed_one();
    feed_large_chunk();
    
    buffer c = b.dup();
    ASSERT_TRUE(::std::ranges::equal(c.joint_valid_view(), b.joint_valid_view()));
    ASSERT_LE(c.total_bytes_allocated(), b.total_bytes_allocated());
}
