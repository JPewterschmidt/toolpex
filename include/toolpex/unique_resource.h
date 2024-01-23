#ifndef TOOLPEX_UNIQUE_RESOURCE_H
#define TOOLPEX_UNIQUE_RESOURCE_H

#include <concepts>
#include <utility>

#include "toolpex/macros.h"

TOOLPEX_NAMESPACE_BEG

template<typename R, ::std::invocable<R*> Dtor>
class unique_resource
{
public:
    using value_type = R;
    using deleter_type = Dtor;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;

public:
    unique_resource(R r, Dtor d)
        : m_resource{ ::std::forward<decltype(r)>(r) }, 
          m_dtor{ ::std::forward<decltype(d)>(d) }
    {
    }

    ~unique_resource() noexcept
    {
        if (m_valid)
        {
            m_dtor(&m_resource);
            m_valid = false;
        }
    }

    unique_resource(unique_resource&& other) noexcept
        : m_resource{ ::std::move(other.m_resource) }, 
          m_dtor{ ::std::move(other.m_dtor) }
    {
        other.m_valid = false;
    }

    unique_resource(const unique_resource&) = delete;
    unique_resource& operator=(const unique_resource&) = delete;

    unique_resource& operator=(unique_resource&& other) noexcept
    {
        m_resource = ::std::move(other.m_resource); 
        m_dtor     = ::std::move(other.m_dtor);
        m_valid    = true;
        other.m_valid = false;

        return *this;
    }

    reference       operator  *()       noexcept { return m_resource;  }
    const_reference operator  *() const noexcept { return m_resource;  }
    pointer         operator ->()       noexcept { return &m_resource; }
    const_pointer   operator ->() const noexcept { return &m_resource; }

protected:
    R m_resource;
    Dtor m_dtor;
    bool m_valid{ true };
};

TOOLPEX_NAMESPACE_END

#endif
