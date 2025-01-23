#ifndef TOOLPEX_BUFFER_H
#define TOOLPEX_BUFFER_H

#include <memory>
#include <span>
#include <memory_resource>
#include <cstddef>
#include <ranges>
#include "toolpex/assert.h"

namespace toolpex
{

class buffer_block
{
public:
    buffer_block(size_t block_capa = 4096, 
                 ::std::pmr::memory_resource* pmr = nullptr);

    buffer_block(buffer_block&& other) noexcept;
    buffer_block& operator=(buffer_block&& other) noexcept;

    ~buffer_block() noexcept;

    size_t capacity() const noexcept { return m_block_capacity; }
    size_t size() const noexcept { return m_size; }
    size_t left() const noexcept { return capacity() - size(); }

    operator ::std::span<char8_t> () noexcept;
    operator ::std::span<const char8_t> () const noexcept;

    ::std::span<char8_t> writable_span() noexcept;
    
    bool commit_write(size_t bytes) noexcept;

    ::std::span<char8_t> valid_span() noexcept;
    ::std::span<const char8_t> valid_span() const noexcept;

private:
    char8_t* cursor() noexcept;
    bool fit(size_t nbytes_wanna_write) const noexcept;
    void release() noexcept;

private:
    ::std::pmr::memory_resource* m_pmr{};
    size_t m_block_capacity{};
    size_t m_size{};
    char8_t* m_storage{};
};

class buffer
{
public:
    buffer(size_t block_capacity = 4096, 
           ::std::pmr::memory_resource* pmr = nullptr) noexcept;
    
    buffer(buffer&& other) noexcept;
    buffer& operator=(buffer&& other) noexcept;
    ~buffer() noexcept { reset(); m_pmr = nullptr; }

    size_t set_new_block_capacity(size_t newblock_capacity_bytes) noexcept;

    bool append(::std::string_view str);
    bool append(::std::span<const char8_t> bytes);

    ::std::span<char8_t> writable_span(size_t at_least = 0);
    bool commit_write(size_t nbytes_wrote) noexcept;

    size_t current_block_left() const noexcept; 
    size_t current_block_capacity() const noexcept;
    size_t new_block_capacity() const noexcept { return m_newblock_capa; }

    const auto& blocks() const noexcept { return m_blocks; }
    const auto& last_block() const noexcept
    {
        toolpex_assert(!m_blocks.empty());
        return m_blocks.back();
    }

    ::std::ranges::range auto blocks_valid_span() const noexcept
    {
        namespace rv = ::std::ranges::views;
        return blocks() | rv::transform([](auto&& item) { return item.valid_span(); });
    }

    ::std::ranges::range auto joint_valid_view() const noexcept 
    {
        namespace rv = ::std::ranges::views;
        return blocks_valid_span() | rv::join;
    }

    buffer dup(::std::pmr::memory_resource* pmr = nullptr) const;

    void reset() noexcept;

private:
    ::std::pmr::memory_resource* m_pmr{};
    ::std::vector<buffer_block> m_blocks;

    size_t m_newblock_capa{};
    buffer_block* m_current_block{};
};

} // namespace toolpex

#endif
