#ifndef TOOLPEX_BUFFER_H
#define TOOLPEX_BUFFER_H

#include <memory>
#include <span>
#include <memory_resource>
#include <cstddef>
#include <ranges>
#include <variant>
#include "toolpex/assert.h"

namespace toolpex
{

class buffer_block
{
public:
    constexpr static size_t alignment = alignof(::std::max_align_t);

public:
    buffer_block(size_t block_capa = 4096, 
                 ::std::pmr::memory_resource* pmr = nullptr);

    buffer_block(buffer_block&& other) noexcept;
    buffer_block& operator=(buffer_block&& other) noexcept;

    ~buffer_block() noexcept;

    size_t capacity() const noexcept { return m_block_capacity; }
    size_t size() const noexcept { return m_size; }
    size_t left() const noexcept { return capacity() - size(); }

    void release() noexcept;

    operator ::std::span<::std::byte> () noexcept;
    operator ::std::span<const ::std::byte> () const noexcept;

    ::std::span<::std::byte> writable_span() noexcept;
    
    bool commit_write(size_t bytes) noexcept;

    ::std::span<::std::byte> valid_span() noexcept;
    ::std::span<const ::std::byte> valid_span() const noexcept;

private:
    ::std::byte* cursor() noexcept;
    bool fit(size_t nbytes_wanna_write) const noexcept;

private:
    ::std::pmr::memory_resource* m_pmr{};
    size_t m_block_capacity{};
    size_t m_size{};
    ::std::byte* m_storage{};
};

class buffer
{
public:
    constexpr static size_t alignment = alignof(::std::max_align_t);

    static_assert(buffer_block::alignment == alignment, 
        "Alignment of buffer_block has to equvalent to the max_align_t");

public:
    buffer(size_t block_capacity = 4096, 
           ::std::pmr::memory_resource* pmr = nullptr) noexcept;
    
    buffer(buffer&& other) noexcept;
    buffer& operator=(buffer&& other) noexcept;
    ~buffer() noexcept { reset(); m_pmr = nullptr; }

    size_t set_new_block_capacity(size_t newblock_capacity_bytes) noexcept;

    bool append(::std::string_view str);

    template<typename T>
    bool append(::std::span<const T> stuff)
    {
        return this->append_bytes(::std::as_bytes(stuff));
    }

    ::std::span<::std::byte> writable_span(size_t at_least = 0);
    bool commit_write(size_t nbytes_wrote) noexcept;

    ::std::span<const ::std::byte> next_readable_span() const noexcept;
    bool commit_read(size_t nbytes_read) noexcept { return commit_read_impl(nbytes_read, false); }
    bool commit_remove_after_read(size_t nbytes_read) noexcept { return commit_read_impl(nbytes_read, true); }
    void reset_reading_info() noexcept;

    size_t current_block_left() const noexcept; 
    size_t current_block_capacity() const noexcept;
    size_t new_block_capacity() const noexcept { return m_newblock_capa; }
    size_t total_nbytes_allocated() const noexcept;
    size_t total_nbytes_valid() const noexcept;
    bool empty() const noexcept;

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

    ::std::ranges::range auto flattened_view() const noexcept 
    {
        namespace rv = ::std::ranges::views;
        return blocks_valid_span() | rv::join;
    }

    buffer dup(::std::pmr::memory_resource* pmr = nullptr) const;

private:
    void reset() noexcept;
    bool append_bytes(::std::span<const ::std::byte> bytes);
    bool commit_read_impl(size_t nbytes_read, bool remove_after_read = false) noexcept;
    bool has_no_remove_after_read() const noexcept;

private:
    ::std::pmr::memory_resource* m_pmr{};
    ::std::vector<buffer_block> m_blocks;

    size_t m_newblock_capa{};
    buffer_block* m_current_block{};

    size_t m_current_reading_block_idx{};
    size_t m_current_block_readed_nbytes{};
};

/**
 *  @class  buffer_lens
 *  @brief  A utility class that provides a lens or view over a buffer's contents, 
 *          allowing reinterpretation as a different arithmetic type.
 *  @tparam EleT The type of elements to interpret the buffer as. Defaults to char8_t.
 *  @requires   EleT Must be an arithmetic type (std::is_arithmetic_v<EleT>).
 *
 *  @attention  You have to make sure that the your target type is perfected aligned to `::std::max_align_t`.
 *              there's a runtime checker to help you achieve that.
 */
template<typename EleT = char8_t>
requires (::std::is_arithmetic_v<EleT> 
      and buffer::alignment % alignof(EleT) == 0 
      and buffer::alignment == alignof(::std::max_align_t))
class buffer_lens
{
public:
    /**
     * @brief Constructs a `buffer_lens` by refer a buffer.
     * @param buf An lvalue reference to the buffer to be wrapped.
     */
    buffer_lens(buffer& buf) noexcept
        : m_buffer{ &buf }
    {
        toolpex_assert(alignment_check());
    }

    /**
     * @brief Constructs a `buffer_lens` by taking ownership of a buffer.
     * @param buf An rvalue reference to the buffer to be wrapped.
     */
    buffer_lens(buffer&& buf)
        : m_buffer{ ::std::make_unique<buffer>(::std::move(buf)) }
    {
        toolpex_assert(alignment_check());
    }

    /**
     * @brief Returns a reference to the underlying buffer.
     * @return A constant reference to the buffer.
     */
    const buffer& buffer_ref() const noexcept
    {
        return *buffer_ptr();
    }

    /**
     * @brief Returns the total number of bytes allocated in the buffer.
     * @return The total byte count.
     */
    size_t total_bytes_allocated() const noexcept
    {
        return buffer_ptr()->total_bytes_allocated();
    }

    /**
     * @brief Provides a range of spans, each reinterpreted as `EleT`.
     * @return A range of `std::span<const EleT>` corresponding to valid buffer blocks.
     */
    ::std::ranges::range auto blocks_valid_span() const noexcept
    {
        namespace rv = ::std::ranges::views;
        return buffer_ptr()->blocks_valid_span()
            | rv::transform([](auto sp) { 
                return ::std::span<const EleT>{ 
                    reinterpret_cast<const EleT*>(sp.data()), 
                    sp.size_bytes() / sizeof(EleT) };
            });
    }

    /**
     * @brief Provides a single joint view of all valid spans in the buffer.
     * @return A joined range of `EleT` values.
     */
    ::std::ranges::range auto flattened_view() const noexcept
    {
        namespace rv = ::std::ranges::views;
        return blocks_valid_span() | rv::join;
    }

    /**
     * @brief Retrieves the next readable span of data.
     *
     * This function returns a span representing the next available readable
     * portion of the buffer. The data is cast to `const char*` to ensure
     * correct interpretation.
     *
     * @tparam EleT The element type of the buffer.
     * @return ::std::span<const EleT> The span of readable data.
     * @note This function does not modify the buffer.
     * @warning Ensure the buffer remains valid while using the returned span.
     */
    ::std::span<const EleT> next_readable_span() const noexcept
    {
        auto sp = buffer_ptr()->next_readable_span();
        return { reinterpret_cast<const EleT*>(sp.data()), sp.size_bytes() / sizeof(EleT) };
    }

    /**
     * @brief Marks a certain number of elements as read in the buffer.
     *
     * This function commits `nele_read` elements as read, advancing the read pointer.
     * The number of bytes marked as read is computed as `nele_read * sizeof(EleT)`.
     *
     * @param nele_read The number of elements to mark as read.
     * @return `true` if the operation was successful, `false` otherwise.
     * @note This function does not remove data from the buffer, only marks it as read.
     */
    bool commit_read(size_t nele_read) noexcept
    {
        return buffer_ptr()->commit_read(nele_read * sizeof(EleT));
    }

    /**
     * @brief Commits and removes data after reading.
     *
     * This function commits `nele_read` elements as read and removes them
     * from the buffer, freeing up space for future writes. The number of bytes
     * removed is calculated as `nele_read * sizeof(EleT)`.
     *
     * @param nele_read The number of elements to commit and remove.
     * @return `true` if the operation was successful, `false` otherwise.
     * @note This function modifies the buffer by removing data.
     */
    bool commit_remove_after_read(size_t nele_read) noexcept
    {
        return buffer_ptr()->commit_remove_after_read(nele_read * sizeof(EleT));
    }

private:
    // Check if the data aligned to specific type.
    bool alignment_check()
    {
        for (const auto sp : buffer_ptr()->blocks_valid_span())
        {
            if (sp.size_bytes() % sizeof(EleT) != 0)
                return false;
        }
        return true;
    }

    const buffer* buffer_ptr() const noexcept
    {
        if (::std::holds_alternative<buffer*>(m_buffer))
        {
            return ::std::get<buffer*>(m_buffer);
        }
        else
        {
            return ::std::get<::std::unique_ptr<buffer>>(m_buffer).get();
        }
    }

    buffer* buffer_ptr() noexcept
    {
        if (::std::holds_alternative<buffer*>(m_buffer))
        {
            return ::std::get<buffer*>(m_buffer);
        }
        else
        {
            return ::std::get<::std::unique_ptr<buffer>>(m_buffer).get();
        }
    }

private:
    ::std::variant<::std::unique_ptr<buffer>, buffer*> m_buffer;
};

} // namespace toolpex

#endif
