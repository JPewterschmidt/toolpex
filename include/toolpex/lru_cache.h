// This file is part of Koios
// https://github.com/JPewterschmidt/toolpex
//
// Copyleft 2023 - 2024, ShiXin Wang. All wrongs reserved.

#ifndef TOOLPEX_LRU_CACHE_H
#define TOOLPEX_LRU_CACHE_H

#include <unordered_map>
#include <concepts>
#include <functional>
#include <cstddef>
#include <utility>
#include <memory>
#include <optional>
#include <queue>
#include <list>

#include "toolpex/macros.h"
#include "toolpex/assert.h"

TOOLPEX_NAMESPACE_BEG

template<
    typename KeyType, 
    typename ValueType, 
    typename Hash = ::std::hash<KeyType>, 
    typename KeyEq = ::std::equal_to<KeyType>>
class lru_cache
{
public:
    lru_cache(size_t capacity) 
        : m_capacity{ capacity }
    {
        if (capacity == 0)
        {
            throw std::invalid_argument("Capacity must be a positive integer.");
        }
    }

    std::optional<ValueType> get(const KeyType &key) noexcept
    {
        ::std::optional<ValueType> result{};
        auto it = m_cache_map.find(key);
        if (it != m_cache_map.end())
        {
            // Move the accessed item to the front of the list
            m_cache_list.splice(m_cache_list.begin(), m_cache_list, it->second);
            return result.emplace(it->second->second);
        }
        return result; 
    }

    template<std::convertible_to<KeyType>   K, 
             std::convertible_to<ValueType> V>
    void put(K&& key, V&& value)
    {
        auto it = m_cache_map.find(key);

        if (it != m_cache_map.end())
        {
            // Update the value and move to the front
            it->second->second = std::forward<V>(value);
            m_cache_list.splice(m_cache_list.begin(), m_cache_list, it->second);
        }
        else
        {
            evict_if_needed();

            // Insert the new item at the front
            m_cache_list.emplace_front(std::forward<K>(key), std::forward<V>(value));
            m_cache_map[m_cache_list.front().first] = m_cache_list.begin();
        }
    }

    size_t capacity() const noexcept { return m_capacity; }
    size_t size()     const noexcept { return m_cache_map.size(); }

private:
    void evict_if_needed() noexcept
    {
        if (m_cache_map.size() < m_capacity) return;
        toolpex_assert(!m_cache_list.empty());

        KeyType last_key = m_cache_list.back().first;
        m_cache_map.erase(last_key);
        m_cache_list.pop_back();
    }

private:
    using cache_list_type = std::list<std::pair<KeyType, ValueType>>;

    using cache_map_iterator = typename std::unordered_map<
        KeyType, 
        typename cache_list_type::iterator, 
        Hash, 
        KeyEq>::iterator;

    using cache_map_type = std::unordered_map<
        KeyType, 
        typename cache_list_type::iterator, 
        Hash, 
        KeyEq>;

    size_t          m_capacity{};
    cache_map_type  m_cache_map{};
    cache_list_type m_cache_list{};
};

TOOLPEX_NAMESPACE_END

#endif
