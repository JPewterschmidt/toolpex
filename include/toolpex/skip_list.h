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
requires (::std::is_nothrow_move_constructible_v<Key> 
       && ::std::is_nothrow_move_constructible_v<Mapped>)
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
        bool is_end_sentinel() const noexcept { return m_forward_ptrs[0] == nullptr; }
        
    private:
        ::std::array<node*, max_level()> m_forward_ptrs{};
        ::std::unique_ptr<value_type, value_deleter> m_valp{};
    };

    template<typename KKey, typename VValue>
    node* make_node(KKey&& k, VValue&& v)
    {
        typename ::std::allocator_traits<allocator_type>::template rebind_alloc<node> alloc;
        node* result = alloc.allocate(1);
        try
        {
            new (result) node{make_value(::std::forward<KKey>(k), ::std::forward<VValue>(v))};
        }
        catch (...)
        {
            alloc.deallocate(result, 1);
            throw;
        }
        return result;
    }

    void demake_node(node* n)
    {
        n->~node();
        typename ::std::allocator_traits<allocator_type>::template rebind_alloc<node> alloc;
        alloc.deallocate(n, 1);
    }

    template<typename KeyT, typename... Args>
    ::std::unique_ptr<value_type, value_deleter> 
    make_value(KeyT&& k, Args&&... vargs)
    {
        pointer result = ::std::allocator_traits<allocator_type>::allocate(m_alloc, 1);
        try
        {
            new (result) value_type(::std::forward<KeyT>(k), ::std::forward<Args>(vargs)...);
        }
        catch (...)
        {
            ::std::allocator_traits<allocator_type>::deallocate(allocator(), result, 1);
            throw;
        }
        return { result, value_deleter{this} };
    }

    void demake_value(pointer p)
    {
        p->~value_type();
        ::std::allocator_traits<allocator_type>::deallocate(m_alloc, p, 1);
    }

public:
    template<typename NodeT = node>
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
        constexpr iterator() noexcept = default;
        iterator(NodeT* n) noexcept : m_ptr{ n } {}

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

        bool operator == (const iterator& other) const noexcept 
        { 
            if (m_ptr) [[likely]] return (m_ptr == other.m_ptr);
            else return other.m_ptr->is_end_sentinel();               
        }

    private:
        NodeT* m_ptr{};
    };

private:
    void demake_node(iterator<node> iter)
    {
        demake_node(iter.m_ptr);
    }

public:
    iterator<node>          begin() noexcept { return { next(head_node_ptr()) }; }
    iterator<node>          end() noexcept { return { end_node_ptr() }; }
    iterator<const node>    begin() const noexcept { return { next(head_node_ptr()) }; }
    iterator<const node>    end() const noexcept { return { end_node_ptr() }; }
    iterator<const node>    cbegin() const noexcept { return begin(); }
    iterator<const node>    cend() const noexcept { return end(); }

    ~skip_list() noexcept { clear(); }

    void clear() noexcept
    {
        node* cur = head_node_ptr();
        if (cur == nullptr) return;
        cur = next(cur);
        while (cur && cur != end_node_ptr())
        {
            node* n = next(cur);
            demake_node(cur);
            cur = n;
        }
        m_size = 0;
        m_level = 1;
    }

    skip_list() 
        : m_head{ ::std::make_unique<node>() }, 
          m_end_sentinel{ ::std::make_unique<node>() }
    {
        auto& h = *head_node_ptr();
        for (size_t i{}; i < max_level(); ++i)
            h[i] = end_node_ptr();
    }

    skip_list(skip_list&& other) noexcept
        : m_head{ ::std::move(other.m_head) }, 
          m_end_sentinel{ ::std::move(other.m_end_sentinel) }, 
          m_alloc{ ::std::move(other.m_alloc) }, 
          m_size{ ::std::exchange(other.m_size, 0) }, 
          m_level{ ::std::exchange(other.m_level, 0) }
    {
    }

    skip_list& operator=(skip_list&& other) noexcept
    {
        clear();
        m_head          = ::std::move(other.m_head); 
        m_end_sentinel  = ::std::move(other.m_end_sentinel); 
        m_alloc         = ::std::move(other.m_alloc); 
        m_size          = ::std::exchange(other.m_size, 0); 
        m_level         = ::std::exchange(other.m_level, 0); 
        m_rd            = ::std::random_device{};
        m_rng           = ::std::mt19937{m_rd};
        return *this;
    }

    size_t  size()  const noexcept { return m_size; }
    size_t  level() const noexcept { return m_level; }
    bool    empty() const noexcept { return size() == 0; }
    auto&   allocator() noexcept { return m_alloc; }

    iterator<node> find(const key_type& k) noexcept
    {
        auto* x = next(left_nearest(k));
        if (x == end_node_ptr() || *(x->key_ptr()) != k) return { end_node_ptr() };
        return { x };
    }

    iterator<const node> find(const key_type& k) const noexcept
    {
        const auto* x = next(left_nearest(k));
        if (x == end_node_ptr() || *(x->key_ptr()) != k) return { end_node_ptr() };
        return { x };
    }

    bool contains(const key_type& k) const noexcept
    {
        return find(k) != end();
    }

    template<typename KK, typename VV>
    iterator<node> insert(KK&& k, VV&& v)
    {
        const size_t old_size = size(), old_level = level();

        ::std::array<node*, max_level()> update{};
        node* x = next(left_nearest(k, update));
        if (const auto* kp = x->key_ptr(); kp && *kp == k) 
        {
            // Exception free. There's a constraint about nothrow_move_constructible
            x->value().second = ::std::forward<VV>(v);
            return {x};
        }
        const size_t new_level = random_level();
        if (new_level > level())
        {
            for (size_t i = level(); i < new_level; ++i)
                update[i] = head_node_ptr();
            m_level = new_level;
        }

        // Strong exception-safty.
        try
        {
            node* newnode = make_node(::std::forward<KK>(k), ::std::forward<VV>(v));
            for (size_t i{}; i < new_level; ++i)
            {
                forward(newnode, i) = ::std::exchange(forward(update[i], i), newnode);
            }
            ++ m_size;
            return { newnode };
        }
        catch (...)
        {
            // Reset the context to provide strong exception safty.
            m_size = old_size;
            m_level = old_level;
            throw;   
        }
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
    static decltype(auto) forward(auto* n, size_t l) noexcept
    {
        return ((*n)[l]);
    }

    const node* left_nearest(const key_type& k) const noexcept
    {
        const node* x = head_node_ptr();
        for (int l = level() - 1; l >= 0; --l)
        {
            while (forward(x, l)->key_ptr() && *(forward(x, l)->key_ptr()) < k)
                x = forward(x, l);
        }
        return x;
    }

    node* left_nearest(const key_type& k) noexcept
    {
        return const_cast<node*>(::std::as_const(*this).left_nearest(k));
    }

    node* left_nearest(const key_type& k, ::std::array<node*, max_level()>& update) noexcept
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

    static auto* next(auto* n) noexcept
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
    allocator_type          m_alloc;
    size_t                  m_size{};
    size_t                  m_level{1};
    mutable ::std::random_device m_rd;
    mutable ::std::mt19937 m_rng{m_rd()};
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
