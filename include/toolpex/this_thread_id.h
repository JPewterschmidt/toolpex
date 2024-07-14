// This file is part of Toolpex
// https://github.com/JPewterschmidt/toolpex
//
// Copyleft 2023 - 2024, ShiXin Wang. All wrongs reserved.

#ifndef TOOLPEX_THIS_THREAD_ID
#define TOOLPEX_THIS_THREAD_ID

#include "toolpex/macros.h"
#include <thread>

TOOLPEX_NAMESPACE_BEG

class this_thread_id
{
public:
    this_thread_id() noexcept = default;
    this_thread_id(::std::thread::id id) noexcept : m_id{ id } {}
    this_thread_id(const this_thread_id&) noexcept = default;
    this_thread_id(this_thread_id&&) noexcept = default;
    this_thread_id& operator=(const this_thread_id&) noexcept = default;
    this_thread_id& operator=(this_thread_id&&) noexcept = default;

    this_thread_id& operator=(::std::thread::id id) noexcept { m_id = id; return *this; }
    operator ::std::thread::id() const noexcept { return m_id; }

private:
    ::std::thread::id m_id{ ::std::this_thread::get_id() };
};

TOOLPEX_NAMESPACE_END

#endif
