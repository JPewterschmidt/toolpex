// This file is part of Koios
// https://github.com/JPewterschmidt/koios
//
// Copyleft 2023 - 2024, ShiXin Wang. All wrongs reserved.

#ifndef TOOLPEX_REF_COUNT_H
#define TOOLPEX_REF_COUNT_H

#include "toolpex/macros.h"
#include <atomic>
#include <cstddef>
#include <utility>

TOOLPEX_NAMESPACE_BEG

class ref_count
{
public:
    constexpr ref_count() noexcept = default;

    ref_count(::std::ptrdiff_t original_val) noexcept
        : m_count{ original_val }
    {
    }

    auto fetch_add(::std::ptrdiff_t val = 1) noexcept
    {
        return m_count.fetch_add(val, ::std::memory_order_relaxed);
    }

    auto fetch_sub(::std::ptrdiff_t val = 1) noexcept
    {
        return m_count.fetch_sub(val, ::std::memory_order_acq_rel);
    }

    auto load(::std::memory_order order = ::std::memory_order_seq_cst) const noexcept
    {
        return m_count.load(order);
    }

    auto operator ++(int) noexcept { return fetch_add(); }
    auto operator --(int) noexcept { return fetch_sub(); }
    
private:
    ::std::atomic_ptrdiff_t m_count;
};

class ref_count_guard
{
public:
    constexpr ref_count_guard(ref_count& r) noexcept
        : m_cnt{ &r }
    {
        m_cnt->fetch_add();
    }

    constexpr ~ref_count_guard() noexcept { release(); }

    ref_count_guard(ref_count_guard&& other) noexcept
        : m_cnt{ ::std::exchange(other.m_cnt, nullptr) }
    {
    }

    ref_count_guard& operator=(ref_count_guard&& other) noexcept
    {
        release();
        m_cnt = ::std::exchange(other.m_cnt, nullptr);
        return *this;
    }

    constexpr void release() noexcept { if (m_cnt) ::std::exchange(m_cnt, nullptr)->fetch_sub(); }

private:
    ref_count* m_cnt{};
};

TOOLPEX_NAMESPACE_END

#endif
