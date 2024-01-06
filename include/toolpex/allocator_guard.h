#ifndef TOOLPEX_ALLOCATOR_GUARD_H
#define TOOLPEX_ALLOCATOR_GUARD_H

#include "toolpex/macros.h"
#include "toolpex/move_only.h"

#include <memory>
#include <utility>

TOOLPEX_NAMESPACE_BEG

/*! \brief A RAII wrapper for allocated space by any allocator similar to `::std::unique_ptr`.
 *  \tparam Alloc The allocator type.
 */
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

    allocator_ptr& operator=(allocator_ptr&& other) noexcept
    {
        deallocate();
        m_alloc = ::std::exchange(other.m_alloc);
        m_p = ::std::exchange(other.m_p);
        return *this;
    }

    /*! \brief Give up the ownership of guarded space, but won't deallocate the space.
     *  \see `give_up_ownership()`
     */
    allocator_ptr& operator=(::std::nullptr_t) noexcept
    {
        give_up_ownership();
    }

    /*! \brief Give up the ownership of guarded space, but won't deallocate the space
     *  
     *  When a `allocator_ptr` object hold the ownership of a range of space, 
     *  it will be deallocate when the object destructs.
     */
    void give_up_ownership() noexcept
    {
        m_p = nullptr;
    }

    operator pointer() noexcept { return m_p; }
    pointer operator ->() noexcept { return m_p; }
    operator bool() const noexcept { return m_p != nullptr; }

    ~allocator_ptr() noexcept
    {
        deallocate();
    }

private:
    void deallocate() noexcept
    {
        if (m_p && m_alloc)
        {
            ::std::allocator_traits<Alloc>::deallocate(*m_alloc, m_p, 1);
            m_p = nullptr;
        }
    }

private:
    allocator_type* m_alloc{};
    pointer m_p{};
};

/*! \brief  
 *  \param  alloc   The reference of a allocator.
 *  \param  size    The numebr of object(uninitialized) of `::std::allocator_traits<Alloc>::value_type`.
 *  \return A RAII wrapper just like unique_ptr, but for allocated space from a allocator.
 *          This wrapper contains sizeof(::std::allocator_traits<Alloc>::value_type) bytes of space.
 */
template<typename Alloc>
auto allocate_guarded(Alloc& alloc, size_t number_of_obj = 1)
{
    return allocator_ptr{ 
        alloc, ::std::allocator_traits<Alloc>::allocate(alloc, number_of_obj) 
    };
}

TOOLPEX_NAMESPACE_END

#endif
