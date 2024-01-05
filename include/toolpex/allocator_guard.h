#ifndef TOOLPEX_ALLOCATOR_GUARD_H
#define TOOLPEX_ALLOCATOR_GUARD_H

#include "toolpex/macros.h"
#include "toolpex/move_only.h"

#include <memory>
#include <utility>

TOOLPEX_NAMESPACE_BEG

template<typename Alloc>
class allocator_ptr : public toolpex::move_only
{
public:
    using allocator_type = Alloc;
    using value_type = typename ::std::allocator_traits<Alloc>::value_type;
    using pointer = typename ::std::allocator_traits<Alloc>::pointer;

public:
    constexpr allocator_ptr() noexcept = default;

    allocator_ptr(allocator_type& alloc, pointer p) noexcept
        : m_alloc{ ::std::addressof(alloc) }, m_p{ p }
    {
    }

    allocator_ptr(allocator_ptr&& other) noexcept
        : m_alloc{ ::std::exchange(other.m_alloc, nullptr) }, 
          m_p{ ::std::exchange(other.m_p, nullptr) }
    {
    }

    allocator_ptr& operator=(::std::nullptr_t) noexcept
    {
        m_p = nullptr;
    }

    ~allocator_ptr() noexcept
    {
        if (m_p && m_alloc)
        {
            ::std::allocator_traits<Alloc>::deallocate(*m_alloc, m_p, 1);
            m_p;
        }
    }

private:
    allocator_type* m_alloc{};
    pointer m_p{};
};

template<typename Alloc>
auto allocate_guarded(Alloc& alloc)
{
    return allocator_ptr{ 
        alloc, ::std::allocator_traits<Alloc>::allocate(alloc, 1) 
    };
}

TOOLPEX_NAMESPACE_END

#endif
