#ifndef TOOLPEX_OBJECT_STORAGE_H
#define TOOLPEX_OBJECT_STORAGE_H

#include <cstddef>
#include <stdexcept>
#include <memory>
#include <utility>

#include "toolpex/assert.h"
#include "toolpex/move_only.h"

namespace toolpex
{

template<typename T>
class object_storage : public toolpex::move_only
{
public:
    object_storage() = default;

    object_storage(object_storage&& other) noexcept
        : m_storage{ ::std::move(other.m_storage) }, 
          m_has_value{ ::std::exchange(other.m_has_value, false) }
    {
    }

    object_storage& operator=(object_storage&& other) noexcept
    {
        m_storage = ::std::move(other.m_storage);
        m_has_value = ::std::exchange(other.m_has_value, false);
        return *this;
    }

    template<typename... Args>
    void set_value(Args&&... args)
    {
        if (has_value())
        {
            value_ref() = T(::std::forward<Args>(args)...);
        }
        else
        {
            new (storage()) T(::std::forward<Args>(args)...);
        }
        m_has_value = true;
    }

    T get_value()
    {
        if (!has_value())
        {
            throw ::std::out_of_range{ "toolpex::object_storage: there's no value could be got" };
        }
        
        T result{ ::std::move(value_ref()) };
        clear();
        return result;
    }

    bool has_value() const noexcept { return m_has_value; }

    void clear() noexcept 
    {
        storage()->~T();
        m_has_value = false;
    }

    T& value_ref() noexcept
    {
        toolpex_assert(has_value());
        return *storage();
    }

    const T& value_ref() const noexcept
    {
        toolpex_assert(has_value());
        return *storage();
    }

private:
    T* storage() noexcept
    {
        return reinterpret_cast<T*>(m_storage.get());
    }

    const T* storage() const noexcept
    {
        return reinterpret_cast<const T*>(m_storage.get());
    }

private:
    ::std::unique_ptr<::std::byte[]> m_storage{ new ::std::byte[sizeof(T)] };
    bool m_has_value{};
}; 

} // namespace toolpex

#endif
