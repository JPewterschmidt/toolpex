#ifndef TOOLPEX_SKIP_LIST_H
#define TOOLPEX_SKIP_LIST_H

#include <cstddef>
#include <memory>
#include <utility>
#include <iterator>

namespace toolpex
{

template<typename Key, typename Mapped, 
         ::std::size_t MaxLevel, 
         typename Alloc = ::std::allocator<::std::pair<Key, Napped>>>
class skip_list
{
public:
    using key_type                  = Key;
    using mapped_type               = Mapped;
    using value_type                = ::std::pair<key_type, mapped_type>;
    using reference                 = value_type&;
    using const_reference           = const value_type&;
    using reference_mapped          = mapped_type&;
    using const_reference_mapped    = const mapped_type&;
    using pointer_mapped            = mapped_type*;
    using const_pointer_mapped      = const mapped_type*;
    using pointer                   = value_type*;
    using allocator_type            = Alloc;

    static_assert(max_level() > 0, "The constant MaxLevel must be greater than 0!");

private:
    class node
    {
    public:
        node(::std::unique_ptr<value_type, decltype(demake_value)> s) noexcept : m_storage{ ::std::move(s) } {}

        node* operator[](::std::size_t idx) noexcept { return m_forward_ptrs[idx]; }
        const node* operator[](::std::size_t idx) const noexcept { return m_forward_ptrs[idx]; }
        const key_type& key() const noexcept { return value().first; }
        reference value() noexcept { return *m_storage; }
        const_reference value() const noexcept { return *m_storage; }

    private:
        ::std::array<node*, max_level()> m_forward_ptrs{};
        ::std::unique_ptr<value_type, decltype(demake_value)> m_storage{};
    };

    node* make_node()
    {
        node* result = ::std::allocator_traits<allocator_traits>::rebind_alloc<node>::allocate(m_alloc, 1);
        new (result) node{};
        result->m_storage = kv_stuff;
    }

    void demake_node(node* n)
    {
        n->~node();
        ::std::allocator_traits<allocator_traits>::rebind_alloc<node>::deallocate(m_alloc, n, 1);
    }

    void demake_node(iterator iter)
    {
        demake_node(iter.operator ->());
    }

    template<typename KeyT, typename... Args>
    ::std::unique_ptr<value_type, decltype(demake_value)> 
    make_value(KeyT&& k, Args&&... args_for_mapped)
    {
        pointer result = ::std::allocator_traits<allocator_type>::allocate(m_alloc, 1);
        new (result) value_type(::std::forward<KeyT>(k), ::std::forward<Args>(args_for_mapped)...);
        return result;
    }

    void demake_value(pointer p)
    {
        p->~value_type();
        ::std::allocator_traits<allocator_type>::deallocate(m_alloc, p, 1);
    }

public:
    class iterator
    {
    public:
        iterator(node* n) noexcept : m_ptr{ n } {}

        iterator& operator++() noexcept 
        { 
            m_ptr = (*m_ptr)[0];
            return *this;
        }

        iterator operator++(int) noexcept
        {
            iterator result{ *this };
            operator++();
            return result;
        }

        const_reference_mapped  operator*() const noexcept { return m_ptr->value().second; }
        reference_mapped        operator*()       noexcept { return m_ptr->value().second; }

        const_pointer_mapped    operator ->() const noexcept { return &operator*(); }
        pointer_mapped          operator ->()       noexcept { return &operator*(); }

        bool operator == (const iterator& other) const noexcept { return m_ptr == other.m_ptr; }

    private:
        node* m_ptr{};
    };

public:
    constexpr static ::std::size_t max_level() noexcept { return MaxLevel; }

    ~skip_list() noexcept
    {
        iterator cur = begin();
        while (cur != end())
        {
            iterator next = ::std::next(cur);
            demake_node(cur);
            cur = next;
        }
    }

    skip_list() noexcept
    {
        auto& h = *m_head;
        for (size_t i{}; i < max_level(); ++i)
            h[i] = m_end_sentinel.get();
    }

    skip_list(skip_list&& other) noexcept = default;
    skip_list& operator=(skip_list&& other) noexcept = default;
    size_t size() const noexcept { return m_size; }
    bool empty() const noexcept { return size() == 0; }

    iterator find(const key_type& k)
    {
        auto* x = left_nearest(k);
        if (x->key() == k) return { x };
        return {m_end_sentinel.get()};
    }

    template<template... Args>
    iterator emplace(iterator iter, value_type kv)
    {
        ::std::array<node*, max_level()> updates{};
        // TODO
    }

private:
    node* left_nearest(const key_type& k)
    {
        node* x = m_head.get();
        for (size_t l = max_level() - 1; l >= 0; --l)
        {
            auto& xref = *x;
            node* x_forward = xref[l];
            while (x_forward->key() < k)
                x = x_forward;
        }
        return x;
    }
    
private:
    ::std::unique_ptr<node> m_end_sentinel{};
    ::std::unique_ptr<node> m_head{};
    allocator m_alloc;
    size_t m_size{};
};

} // namespace toolpex

#endif
