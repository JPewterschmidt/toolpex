// This file is part of Toolpex
// https://github.com/JPewterschmidt/toolpex
//
// Copyleft 2023 - 2024, ShiXin Wang. All wrongs reserved.

#ifndef TOOLPEX_SPECIFIC_COUNTER_HANDLER_H
#define TOOLPEX_SPECIFIC_COUNTER_HANDLER_H

#include "toolpex/macros.h"

TOOLPEX_NAMESPACE_BEG

/*! \brief  RAII per thread or coroutine counter handler.
 *  Objects of this type will do some clearning up after it's lifetime.
 *  And forward counter operation to the underlying counter object.
 */
template<typename ExecutionIdType, typename Counter>
class specific_counter_handler
{
public:
    specific_counter_handler(ExecutionIdType id, Counter& cnt) noexcept
        : m_parent{ &cnt }, 
          m_tid{ ::std::move(id) }
    {
    }

    specific_counter_handler(specific_counter_handler&& other) noexcept
        : m_parent{ ::std::exchange(other.m_parent, nullptr) }, 
          m_tid{ ::std::move(other.m_tid) }
    {
    }

    specific_counter_handler& 
    operator=(specific_counter_handler&& other) noexcept
    {
        m_parent = ::std::exchange(other.m_parent, nullptr);
        m_tid = other.m_tid;
        return *this;
    }

    ~specific_counter_handler() noexcept
    {
        if (!m_parent) return;
        m_parent->count_unregister_execution_unit(m_tid);
    }

    /*! \return The execution unit indicator 
     *          (a thread id, or pointer to a coroutine frame, 
     *           or other stuff could represent a execution unit)
     */
    auto tid() const noexcept { return m_tid; }

    decltype(auto) add_count(::std::integral auto delta) noexcept
    {
        return m_parent->add_count(*this, delta);
    }

    decltype(auto) sub_count(::std::integral auto delta) noexcept
    {
        return m_parent->sub_count(*this, delta);
    }

    decltype(auto) read_count() noexcept
    {
        return m_parent->read_count(*this);
    }

private:
    Counter* m_parent{};
    ExecutionIdType m_tid;
};

TOOLPEX_NAMESPACE_END

#endif
