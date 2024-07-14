// This file is part of Koios
// https://github.com/JPewterschmidt/toolpex
//
// Copyleft 2023 - 2024, ShiXin Wang. All wrongs reserved.

#ifndef TOOLPEX_AFTER_VALUE_SET_H
#define TOOLPEX_AFTER_VALUE_SET_H

#include <memory>
#include <functional>
#include <type_traits>
#include <utility>
#include <stdexcept>

#include "toolpex/assert.h"

namespace toolpex
{
    
template<typename T>
requires (::std::is_nothrow_move_constructible_v<T>)
class after_value_set
{
public:
    using value_type = T;
    using pointer = value_type*;
    using reference = T&;
    using callback_function = ::std::move_only_function<void(value_type)>;

public:
    after_value_set(callback_function callback = nullptr)
        : m_storage{ new char[sizeof(value_type)] }, 
          m_cb{ ::std::move(callback) }
    {
    }

    template<typename... Args>
    void set_value(Args&&... args)
    {
        new(storage()) value_type(::std::forward<Args>(args)...);
        if (m_cb && !m_hitted) [[likely]]
        {
            m_cb(::std::move(object()));
            m_hitted = true;
            return;
        }
        else if (m_hitted)
        {
            throw ::std::logic_error{ "You have already call set_value before. This class was designed for one-shot only." };
        }
        else throw ::std::logic_error{ "You have not set the callback function yet." };
    }

    void set_callback(callback_function cb) noexcept
    {
        toolpex_assert(!m_cb);
        m_cb = ::std::move(cb);
    }

    after_value_set(after_value_set&& other) noexcept = default;
    after_value_set& operator=(after_value_set&& other) noexcept = default;

    ~after_value_set() noexcept
    {
        if (!m_storage) return;
        storage()->~value_type();
    }

private:
    pointer storage() noexcept { return reinterpret_cast<pointer>(m_storage.get()); }
    reference object() noexcept { return *storage(); }

private:
    ::std::unique_ptr<char[]> m_storage;
    callback_function m_cb;
    bool m_hitted{};
};

} // namespace toolpex

#endif
