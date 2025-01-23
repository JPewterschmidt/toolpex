#include "toolpex/buffer.h"
#include <utility>
#include <limits>
#include <iterator>
#include <functional>
#include <algorithm>

namespace rv = ::std::ranges::views;
namespace r = ::std::ranges;

namespace toolpex
{

// buffer_block -----------------------------------------------------

buffer_block::buffer_block(size_t block_capa, 
                           ::std::pmr::memory_resource* pmr)
    : m_pmr{ pmr ? pmr : ::std::pmr::get_default_resource() }
{
    constexpr size_t alignment = alignof(::std::max_align_t);
    m_block_capacity = (block_capa + alignment - 1) & ~(alignment - 1);
    m_storage = static_cast<char8_t*>(m_pmr->allocate(m_block_capacity));
}

void buffer_block::release() noexcept
{
    if (m_storage)
        m_pmr->deallocate(m_storage, m_block_capacity);

    m_storage = nullptr;
    m_size = m_block_capacity = 0;
}

buffer_block::~buffer_block() noexcept
{
    release();
}

buffer_block::buffer_block(buffer_block&& other) noexcept
    : m_pmr{ other.m_pmr }, 
      m_block_capacity{ ::std::exchange(other.m_block_capacity, 0) }, 
      m_size{ ::std::exchange(other.m_size, 0) }, 
      m_storage{ ::std::exchange(other.m_storage, nullptr) }
{
}

buffer_block& buffer_block::operator=(buffer_block&& other) noexcept
{
    release();   

    m_pmr = other.m_pmr; 
    m_block_capacity = ::std::exchange(other.m_block_capacity, 0); 
    m_size = ::std::exchange(other.m_size, 0); 
    m_storage = ::std::exchange(other.m_storage, nullptr);

    return *this;
}

buffer_block::operator ::std::span<char8_t> () noexcept
{
    return { m_storage, m_block_capacity };
}

buffer_block::operator ::std::span<const char8_t> () const noexcept
{
    return { m_storage, m_block_capacity };
}

char8_t* buffer_block::cursor() noexcept
{
    return m_storage + size();
}

bool buffer_block::fit(size_t nbytes_wanna_write) const noexcept
{
    return nbytes_wanna_write <= left();
}

::std::span<char8_t> buffer_block::writable_span() noexcept
{
    return { cursor(), left() };
}

bool buffer_block::commit_write(size_t bytes) noexcept
{
    if (!fit(bytes)) return false;
    m_size += bytes;
    return true;
}

::std::span<char8_t> buffer_block::valid_span() noexcept
{
    return { m_storage, size() };
}

::std::span<const char8_t> buffer_block::valid_span() const noexcept
{
    return { m_storage, size() };
}

// buffer ------------------------------------------------------------

buffer::buffer(size_t block_capacity, 
               ::std::pmr::memory_resource* pmr) noexcept
    : m_pmr{ pmr ? pmr : ::std::pmr::get_default_resource() }, 
      m_newblock_capa{ block_capacity }
{
}

buffer::buffer(buffer&& other) noexcept
    : m_pmr{ other.m_pmr }, 
      m_blocks{ ::std::move(other.m_blocks) }, 
      m_newblock_capa{ ::std::exchange(other.m_newblock_capa, 0) }, 
      m_current_block{ ::std::exchange(other.m_current_block, nullptr) }
{
}

buffer& buffer::operator=(buffer&& other) noexcept
{
    // release() is not necessary
    m_pmr = other.m_pmr;
    m_blocks = ::std::move(other.m_blocks);
    m_current_block = ::std::exchange(other.m_current_block, nullptr);

    return *this;
}

void buffer::reset() noexcept
{
    m_current_block = nullptr;
    m_blocks.clear();
}

size_t buffer::current_block_left() const noexcept
{
    toolpex_assert(!!m_current_block);
    return m_current_block->left();
}
 
size_t buffer::current_block_capacity() const noexcept
{
    toolpex_assert(!!m_current_block);
    return m_current_block->capacity();
}

::std::span<char8_t> buffer::writable_span(size_t at_least)
{
    ::std::span<char8_t> result = m_current_block ? m_current_block->writable_span() : ::std::span<char8_t>{};
    if (result.size() == 0 || result.size() < (at_least ? at_least : ::std::numeric_limits<size_t>::max()))
    {
        toolpex_assert(m_pmr && m_newblock_capa); // prevent use after destruct.
        
        if (at_least < m_newblock_capa)
        {
            m_current_block = &m_blocks.emplace_back(m_newblock_capa, m_pmr);
        }
        else
        {
            m_current_block = &m_blocks.emplace_back(at_least, m_pmr);
        }
        result = m_current_block->writable_span();
    }

    return result;
}

bool buffer::commit_write(size_t nbytes_wrote) noexcept
{
    toolpex_assert(m_current_block);
    return m_current_block->commit_write(nbytes_wrote);
}

bool buffer::append(::std::string_view str)
{
    return this->append(::std::span<const char8_t>{ 
        reinterpret_cast<const char8_t*>(str.data()), 
        str.size()
    });
}

bool buffer::append(::std::span<const char8_t> bytes)
{
    auto writable = this->writable_span(bytes.size());
    if (writable.size() == 0)
        return false;

    ::std::copy(begin(bytes), end(bytes), begin(writable));
    commit_write(bytes.size());

    return true;
}

size_t buffer::set_new_block_capacity(size_t newblock_capacity_bytes) noexcept
{
    return ::std::exchange(m_newblock_capa, newblock_capacity_bytes);
}

buffer buffer::dup(::std::pmr::memory_resource* pmr) const
{
    const size_t old_block_capa = new_block_capacity();
    buffer result(old_block_capa, pmr ? pmr : m_pmr);

    auto fit_in_a_page = [first = true, sum = 0ul, old_block_capa] (auto&& lhs, auto&& rhs) mutable { 
        if (::std::exchange(first, false))
        {
            if ((sum += lhs.size()) < old_block_capa)
                return true;
            sum = 0ul;
            return false;
        }

        const bool result = (sum += rhs.size() < old_block_capa);
        if (!result)
        {
            sum = 0ul;
        }
        return result;
    };

    for (auto block_sps : blocks_valid_span() | rv::chunk_by(fit_in_a_page))
    {
        const size_t need = r::fold_left(block_sps | rv::transform([](auto&& item) { return item.size(); }), 0, ::std::plus{});
        result.set_new_block_capacity(need);
        for (auto sp : block_sps)
            result.append(sp);
    }

    result.set_new_block_capacity(old_block_capa);
    
    return result;
}

size_t buffer::total_bytes_allocated() const noexcept
{
    return r::fold_left_first(
        blocks() | rv::transform([](auto&& item) { return item.capacity(); }),
        ::std::plus{}
    ).value_or(0);
}

} // namespace toolpex
