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
         typename Alloc = ::std::allocator<::std::pair<Key, Mapped>>>
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
    using const_pointer             = const value_type*;
    using allocator_type            = Alloc;

    constexpr static ::std::size_t max_level() noexcept { return MaxLevel; }
    static_assert(max_level() > 0, "The constant MaxLevel must be greater than 0!");

private:
    class value_deleter
    {
    public:
        value_deleter() = default;
        value_deleter(skip_list* list) noexcept
            : m_list{ list }
        {
        }

        void operator()(pointer n) 
        {
            if (m_list) 
            {
                m_list->demake_value(n);
                m_list = nullptr;
            }
        }

    private:
        skip_list* m_list{};
    };

    class node
    {
    public:
        node() noexcept = default;
        node(::std::unique_ptr<value_type, value_deleter> s) noexcept : m_valp{ ::std::move(s) } {}

        node*& operator[](::std::size_t idx) noexcept { return m_forward_ptrs[idx]; }
        const node* operator[](::std::size_t idx) const noexcept { return m_forward_ptrs[idx]; }

        const key_type* key_ptr() const noexcept 
        { 
            if (m_valp == nullptr) return {};
            return &(value().first);
        }

        reference value() noexcept { return *m_valp; }
        const_reference value() const noexcept { return *m_valp; }

    private:
        ::std::array<node*, max_level()> m_forward_ptrs{};
        ::std::unique_ptr<value_type, value_deleter> m_valp{};
    };

    template<typename KVPair>
    node* make_node(KVPair&& kvp)
    {
        // TODO: Should I receive a level argument and do soemthing init stuff about the level ? 
        typename ::std::allocator_traits<allocator_type>::template rebind_alloc<node> alloc;
        node* result = alloc.allocate(1);
        new (result) node{make_value(::std::forward<KVPair>(kvp))};
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
        typename ::std::allocator_traits<allocator_type>::template rebind_alloc<node> alloc;
        alloc.deallocate(n, 1);
    }

    template<typename KeyT, typename... Args>
    ::std::unique_ptr<value_type, value_deleter> 
    make_value(KeyT&& k, Args&&... args_for_mapped)
    {
        return make_value(::std::pair<key_type, value_type>{
            ::std::forward<KeyT>(k), 
            value_type(::std::forward<Args>(args_for_mapped)...)
        });
    }

    template<typename KVPair>
    ::std::unique_ptr<value_type, value_deleter> 
    make_value(KVPair&& kvp)
    {
        pointer result = ::std::allocator_traits<allocator_type>::allocate(m_alloc, 1);
        new (result) value_type(::std::forward<KVPair>(kvp));
        return { result, value_deleter{this} };
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
        using difference_type = ::std::ptrdiff_t;
        using iterator_category = ::std::forward_iterator_tag;
        using value_type = typename skip_list::value_type;
        using reference = typename skip_list::reference;
        using const_reference = typename skip_list::const_reference;
        using pointer = typename skip_list::pointer;
        using const_pointer = typename skip_list::const_pointer;

        friend class skip_list;

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

        const_reference operator*() const noexcept { return m_ptr->value(); }
        reference       operator*()       noexcept { return m_ptr->value(); }

        const_pointer    operator ->() const noexcept { return &operator*(); }
        pointer          operator ->()       noexcept { return &operator*(); }

        bool operator == (const iterator& other) const noexcept { return m_ptr == other.m_ptr; }

    private:
        node* m_ptr{};
    };

private:
    void demake_node(iterator iter)
    {
        demake_node(iter.m_ptr);
    }

public:
    iterator begin() noexcept
    {
        return { next(head_node_ptr()) };
    }

    iterator end() noexcept
    {
        return { end_node_ptr() };
    }

    ~skip_list() noexcept
    {
        node* cur = next(head_node_ptr());
        while (cur && cur != end_node_ptr())
        {
            node* n = next(cur);
            demake_node(cur);
            cur = n;
        }
    }

    skip_list() 
        : m_head{ ::std::make_unique<node>() }, 
          m_end_sentinel{ ::std::make_unique<node>() }
    {
        // TODO: Rewrite this part. Read the paper.
        auto& h = *head_node_ptr();
        for (size_t i{}; i < max_level(); ++i)
            h[i] = end_node_ptr();
    }

    skip_list(skip_list&& other) noexcept = default;
    skip_list& operator=(skip_list&& other) noexcept = default;
    size_t size() const noexcept { return m_size; }
    size_t level() const noexcept { return m_level; }
    bool empty() const noexcept { return size() == 0; }
    auto& allocator() noexcept { return m_alloc; }

    iterator find(const key_type& k)
    {
        auto* x = next(left_nearest(k));
        if (x == end_node_ptr() || *(x->key_ptr()) != k) return { end_node_ptr() };
        return { x };
    }

    iterator emplace(value_type kv)
    {
        ::std::array<node*, max_level()> update{};
        node* x = next(left_nearest(kv.first, update));
        if (const auto* kp = x->key_ptr(); kp && *kp == kv.first) 
        {
            x->value().second = ::std::move(kv.second);
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
            forward(newnode, i) = forward(update[i], i);
            forward(update[i], i) = newnode;
        }
        ++ m_size;
        return { newnode };
    }

    iterator insert(value_type kv) 
    {
        return emplace(::std::move(kv));
    }

    template<typename Arg1, typename Arg2>
    iterator insert(Arg1&& a1, Arg2&& a2)
    {
        return emplace(value_type{ ::std::forward<Arg1>(a1), ::std::forward<Arg2>(a2) });
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
    static node*& forward(node* n, size_t l) noexcept
    {
        return (*n)[l];
    }

    node* left_nearest(const key_type& k)
    {
        node* x = head_node_ptr();
        for (int l = level() - 1; l >= 0; --l)
        {
            while (forward(x, l)->key_ptr() && *(forward(x, l)->key_ptr()) < k)
                x = forward(x, l);
        }
        return x;
    }

    node* left_nearest(const key_type& k, ::std::array<node*, max_level()>& update)
    {
        node* x = head_node_ptr();
        for (int l = level() - 1; l >= 0; --l)
        {
            while (forward(x, l)->key_ptr() && *(forward(x, l)->key_ptr()) < k)
                x = forward(x, l);
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
    node* end_node_ptr() noexcept { return m_end_sentinel.get(); }

    size_t random_level() const noexcept
    {
        size_t result{};
        ::std::uniform_real_distribution dist(0.0, 1.0);
        while (dist(m_rng) < 0.5) 
            ++result;
        return (result % (max_level() - 1)) + 1;
    }

    template<typename>
    friend class skip_list_debug;
    
private:
    ::std::unique_ptr<node> m_head{};
    ::std::unique_ptr<node> m_end_sentinel{};
    allocator_type m_alloc;
    size_t m_size{};
    size_t m_level{1};
    mutable ::std::random_device rd;
    mutable ::std::mt19937 m_rng{rd()};
};

template<typename L>
class skip_list_debug
{
public:
    skip_list_debug(L& l)
        : m_list{ &l }
    {
    }

    size_t actual_size() const noexcept
    {
        auto* cur = m_list->head_node_ptr();
        size_t result{};
        while (cur && cur != m_list->end_node_ptr())
        {
            cur = (*cur)[0];
            ++result;
        }
        return result - 1;
    }

private:
    L* m_list;
};

} // namespace toolpex

#endif
