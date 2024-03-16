#ifndef TOOLPEX_SKIP_LIST_H
#define TOOLPEX_SKIP_LIST_H

#include <cstddef>
#include <memory>
#include <utility>
#include <iterator>
#include <random>

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

        const key_type* key_ptr() const noexcept 
        { 
            if (m_storage == nullptr) return {};
            return &(value().first);
        }

        reference value() noexcept { return *m_storage; }
        const_reference value() const noexcept { return *m_storage; }

    private:
        ::std::array<node*, max_level()> m_forward_ptrs{};
        ::std::unique_ptr<value_type, decltype(demake_value)> m_storage{};
    };

    template<typename KVPair>
    node* make_node(KVPair&& kvp)
    {
        // TODO: Should I receive a level argument and do soemthing init stuff about the level ? 
        node* result = ::std::allocator_traits<allocator_traits>::rebind_alloc<node>::allocate(m_alloc, 1);
        new (result) node{};
        result->m_storage = make_value(::std::forward<KVPair>(kvp));
        return result;
    }

    template<typename KKey, typename VValue>
    node* make_node(KKey k, VValue v)
    {
        return make_node(::std::pair<key_type, value_type>{
            ::std::forward<KKey>(k), 
            ::std::forward<VValue>(v)
        });
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
        return make_value(::std::pair<key_type, value_type>{
            ::std::forward<KeyT>(t), 
            value_type(::std::forward<Args>(args_for_mapped)...)
        });
    }

    template<typename KVPair>
    ::std::unique_ptr<value_type, decltype(demake_value)> 
    make_value(KVPair&& kvp)
    {
        pointer result = ::std::allocator_traits<allocator_type>::allocate(m_alloc, 1);
        new (result) value_type(::std::forward<KVPair>(kvp));
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
        // TODO: Rewrite this part. Read the paper.
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
        auto* x = next(left_nearest(k));
        if (x == end_node_ptr() || *(x->key_ptr()) != k) return { end_node_ptr() };
        return { x };
    }

    template<template... Args>
    iterator emplace(value_type kv)
    {
        ::std::array<node*, max_level()> update{};
        node* x = next(left_nearest(kv.first, update));
        if (const auto* kp = x->key_ptr(); kp && *kp == k.first) 
        {
            x->value() = ::std::move(kv.second);
            return {x};
        }
        const size_t new_level = random_level();
        if (new_level > level())
        {
            for (size_t i = level(); i < new_level; ++i)
                update[i] = head_node_ptr();
            m_level = new_level;
        }
        node* newnode = make_node(::std::move(kv));
        for (size_t i{}; i < new_level; ++i)
        {
            (*newnode)[i] = ::std::exchange((*update[i])[i], newnode);
        }
        ++ m_size;
        return { newnode };
    }

    void erase(const key_type& key)
    {
        ::std::array<node*, max_level()> update{};
        node* x = next(left_nearest(key, update));
        if (const auto* keyp = x->key_ptr(); keyp && *keyp == key)
        {
            for (size_t i{}; i < level(); ++i)
            {
                if ((*update[i])[i] != x) break;
                (*update[i])[i] = (*x)[i];
            }
            demake_node(x);
            while (level() >= 1 && (*head_node_ptr())[level()-1] == end_node_ptr())
                -- m_level;
            -- m_size;
        }
    }

private:
    node* left_nearest(const key_type& k)
    {
        node* x = m_head.get();
        for (size_t l = max_level() - 1; l >= 0; --l)
        {
            auto& xref = *x;
            node* x_forward = xref[l];
            while (auto* valp = x_forward->key(); valp && *valp < k)
                x = x_forward;
        }
        return x;
    }

    node* left_nearest(const key_type& k, ::std::array<node*, max_level()>& update)
    {
        node* x = m_head.get();
        for (size_t l = max_level() - 1; l >= 0; --l)
        {
            auto& xref = *x;
            node* x_forward = xref[l];
            while (auto* valp = x_forward->key(); valp && *valp < k)
                x = x_forward;
            update[l] = x;
        }
        return x;
    }

    static node* next(node* n) noexcept
    {
        return (*n)[0];
    }

    const node* end_node_ptr() const noexcept { return m_end_sentinel.get(); }
    const node* head_node_ptr() const noexcept { return m_head.get(); }
    node* head_node_ptr() noexcept { return m_head.get(); }

    size_t random_level() const noexcept
    {
        ::std::uniform_int_distrbution<size_t> dist(1, max_level());
        return dist(m_rng);
    }
    
private:
    ::std::unique_ptr<node> m_end_sentinel{};
    ::std::unique_ptr<node> m_head{};
    allocator m_alloc;
    size_t m_size{};
    size_t m_level{}
    mutable ::std::mt19937 m_rng{::std::random_device{}};
};

} // namespace toolpex

#endif
