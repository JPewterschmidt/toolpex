#ifndef TOOLPEX_UNIQUE_RESOURCE_H
#define TOOLPEX_UNIQUE_RESOURCE_H

#include <concepts>
#include <utility>

#include "toolpex/macros.h"

TOOLPEX_NAMESAPCE_BEG

template<typename R, ::std::invocable<R&> Dtor>
class unique_resource
{
public:
    unique_resource(R r, Dtor d)
        : m_resource{ ::std::forward<decltype(r)>(r) }, 
          m_dtor{ ::std::forward<decltype(d)>(d) }
    {
    }

    ~unique_resource() noexcept(noexcept(Dtor{}(::std::declval<R&>())))
    {
        if (m_valid)
        {
            m_dtor(m_resource);
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

protected:
    R m_resource;
    Dtor m_dtor;
    bool m_valid{ true };
};

TOOLPEX_NAMESAPCE_END

#endif
