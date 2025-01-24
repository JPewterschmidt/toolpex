#include "toolpex/buffer.h"
#include "gtest/gtest.h"
#include "toolpex/functional.h"

#include <string>
#include <span>

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
        ::std::array<::std::byte, 1> isingle{(::std::byte)'8'};
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
    b = { 16 };
    ASSERT_TRUE(feed_one());

    // make sure all writes has been committed
    ASSERT_FALSE(b.last_block().valid_span().empty());
}

TEST_F(buffer_suite, joint)
{
    b = { 16 };
    feed_one();
    feed_large_chunk();

    auto sp1 = b.blocks()[0].valid_span();
    auto sp2 = b.blocks()[1].valid_span();

    auto str = buffer_lens<char>(b).flattened_view() | r::to<::std::string>();
    auto initl = { sp1, sp2 };
    auto a = initl | rv::join | rv::transform([](auto item) { return (char)item; }) | r::to<::std::string>();
    ASSERT_EQ(str, a)
        << "str: " << str
        << "sp1: " << (sp1 | rv::transform(byte_to_char) | r::to<::std::string>())
        << "sp2: " << (sp2 | rv::transform(byte_to_char) | r::to<::std::string>())
        ;
}

TEST_F(buffer_suite, dup)
{
    b = { 16 };
    feed_one();
    feed_large_chunk();
    feed_one();
    feed_large_chunk();
    feed_one();
    feed_large_chunk();
    
    buffer c = b.dup();
    ASSERT_TRUE(::std::ranges::equal(c.flattened_view(), b.flattened_view()));
    ASSERT_LE(c.total_bytes_allocated(), b.total_bytes_allocated())
        << "new total bytes allocated = " << c.total_bytes_allocated()
        << ", old :" << b.total_bytes_allocated()
        ;
}
