#ifndef TOOLPEX_CALLBACK_PROMISE_H
#define TOOLPEX_CALLBACK_PROMISE_H

#include <exception>
#include <functional>
#include <memory>
#include <cstddef>

#include "toolpex/assert.h"
#include "toolpex/move_only.h"

namespace toolpex
{

struct future_frame_exception
{
protected:
    ::std::exception_ptr m_ex;

public:
    bool safely_done() const noexcept { return !m_ex; }
    void set_exception(::std::exception_ptr ex)
    {
        m_ex = ::std::move(ex);
    }

    auto& exception() noexcept { return m_ex; }
};

template<typename T>
class future_frame_value_or_void_common : public future_frame_exception
{
public:
    template<typename... Args>
    void set_value(Args&&... args)
    {
        m_value = ::std::make_unique<T>(::std::forward<Args>(args)...);
    }

    T& value() noexcept
    {
        toolpex_assert(!!m_value);
        return *m_value;
    }

protected:
    ::std::unique_ptr<T> m_value;
};

template<>
class future_frame_value_or_void_common<void> : public future_frame_exception
{
public:
    constexpr void set_value() { }
    constexpr void value() noexcept { }
};

template<typename T>
class future_frame : public future_frame_value_or_void_common<T>, move_only
{
};

template<typename T>
class future_frame<T&> : public future_frame_value_or_void_common<T*>, move_only
{
public:
    void set_value(T& t)
    {
        this->future_frame_value_or_void_common<T*>::set_value(::std::addressof(t));
    }

    decltype(auto) value() noexcept
    {
        return *(this->future_frame_value_or_void_common<T*>::value());
    }
};

template<typename T>
class callback_promise
{
public:
    using callback_t = ::std::move_only_function<void(future_frame<T>)>;

    callback_promise(callback_t cb) noexcept
        : m_cb{ ::std::move(cb) }
    {
        toolpex_assert(!!m_cb);
    }

    template<typename... Args>
    void set_value(Args&&... args)
    {
        if constexpr (::std::same_as<T, void>)
        {
            m_cb({});
        }
        else
        {
            future_frame<T> ff;
            ff.set_value(::std::forward<Args>(args)...);
            m_cb(::std::move(ff));
        }
    }

    void set_exception(::std::exception_ptr ex) noexcept
    {
        future_frame<T> ff;
        ff.set_exception(::std::move(ex));
        m_cb(::std::move(ff));
    }

private:
    callback_t m_cb;
};

template<typename T>
class callback_promise<T&>
{
public:
    using callback_t = ::std::move_only_function<void(future_frame<T&>)>;

    callback_promise(callback_t cb) noexcept
        : m_cb{ ::std::move(cb) }
    {
        toolpex_assert(!!m_cb);
    }

    void set_value(T& t)
    {
        future_frame<T&> ff;
        ff.set_value(t);
        m_cb(::std::move(ff));
    }

    void set_exception(::std::exception_ptr ex) noexcept
    {
        future_frame<T&> ff;
        ff.set_exception(::std::move(ex));
        m_cb(::std::move(ff));
    }

private:
    callback_t m_cb;
};

} // namespace toolpex

#endif
