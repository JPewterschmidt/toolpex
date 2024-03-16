#ifndef TOOLPEX_AFTER_VALUE_SET_H
#define TOOLPEX_AFTER_VALUE_SET_H

#include <memory>
#include <functional>

namespace toolpex
{
    
template<typename T>
class after_value_set
{
public:
    using value_type = T;
    using pointer = value_type*;
    using reference = T&;

public:
    after_value_set(::std::move_only_function<void(reference)> callback)
        : m_storage{ new char[sizeof(value_type)] }, 
          m_cb{ ::std::move(callback) }
    {
    }

    template<typename... Args>
    set_value(Args&&... args)
    {
        new(storage()) value_type(::std::forward<Args>(args)...);
        m_cb(object());
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
    ::std::move_only_function<void(reference)> m_cb;
};


} // namespace toolpex

#endif
