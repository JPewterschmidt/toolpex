#ifndef TOOLPEX_UNIQUE_RESOURCE_H
#define TOOLPEX_UNIQUE_RESOURCE_H

#include <concepts>

#include "toolpex/macros.h"

TOOLPEX_NAMESAPCE_BEG

template<typename R, ::std::invocable<R&> Dtor>
class unique_resource
{
public:
    unique_resource(R&& r, Dtor&& d)
        : m_resource{ ::std::move(r) }, 
          m_dtor{ ::std::move(d) }
    {
    }

    ~unique_resource() noexcept(noexcept(Dtor{}(::std::declval<R&>())))
    {
        m_dtor(m_resource);
    }

    unique_resource(unique_resource&& other) noexcept
        : m_resource{ ::std::move(other.m_resource) }, 
          m_dtor{ ::std::move(other.m_dtor) }
    {
    }

    unique_resource(const unique_resource&) = delete;

protected:
    R m_resource;
    Dtor m_dtor;
};



TOOLPEX_NAMESAPCE_END

#endif
