// This file is part of Koios
// https://github.com/JPewterschmidt/koios
//
// Copyleft 2023 - 2024, ShiXin Wang. All wrongs reserved.

#ifndef TOOLPEX_PAGE_H
#define TOOLPEX_PAGE_H

#include <memory>
#include <span>
#include <cstddef>
#include <cstdint>

template<typename Alloc = ::std::allocator<char>>
class page
{
public:
    using allocator = Alloc;

public:
    page(size_t page_size, const allocator& alloc = allocator())
        : m_capacity{ page_size }, m_alloc{ alloc }, m_storage{ m_alloc.allocate(page_size) }
    {
        if (m_storage == 0) throw ::std::logic_error{"bad alloc, or bad page_size"};
    }

    page(page&& other) noexcept
        : m_capacity{ ::std::exchange(other.m_capacity, 0) }, 
          m_size{ ::std::exchange(other.m_size, 0) }, 
          m_alloc{ ::std::move(other.m_alloc) },
          m_storage{ ::std::exchange(other.m_storage, nullptr) }
    {
    }

    page& operator=(page&& other) noexcept
    {
        release();
        
        m_capacity = ::std::exchange(other.m_capacity, 0); 
        m_size = ::std::exchange(other.m_size, 0); 
        m_alloc = ::std::move(other.m_alloc);
        m_storage = ::std::exchange(other.m_storage, nullptr);

        return *this;
    }

    bool full() const noexcept { return capacity() - size() == 0; }
    bool empty() const noexcept { return size() == 0; }

    size_t capacity() const noexcept { return m_capacity; }
    size_t size() const noexcept { return m_size; }

    void release() noexcept
    {
        if (m_storage)
            m_alloc.deallocate(::std::exchange(m_storage, nullptr), capacity());
    }

    ~page() noexcept { release(); }

    ::std::span<::std::byte> next_writable() noexcept { return { reinterpret_cast<::std::byte*>(m_storage) + size(), capacity() - size() }; }
    operator ::std::span<const ::std::byte>() const noexcept { return { reinterpret_cast<::std::byte*>(m_storage), size() }; }
    auto readable() const noexcept { return ::std::span<const ::std::byte>(*this); }
    void commit_write(size_t wrote_sz) { m_size += wrote_sz; }

private:
    size_t m_capacity;
    size_t m_size{};
    allocator m_alloc;
    char* m_storage{};
};

#endif
