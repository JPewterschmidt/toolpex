// This idea coms from **perfbook** Chapter 5, 
// code simply copyed from perfbook example.
// https://github.com/paulmckrcu/perfbook

#ifndef TOOLPEX_COUNTER_H
#define TOOLPEX_COUNTER_H

#include "toolpex/macros.h"
#include "toolpex/spin_lock.h"
#include "toolpex/specific_counter_handler.h"
#include <unordered_map>
#include <ranges>
#include <numeric>

TOOLPEX_NAMESPACE_BEG

template<typename ExecutionIdType = ::std::thread::id, ::std::integral CounterT = ::std::size_t>
class approximate_limit_counter
{
public:
    using execution_unit_handler = specific_counter_handler<ExecutionIdType, approximate_limit_counter>;
    
public:
    constexpr approximate_limit_counter(CounterT global_counter_max) noexcept
        : m_global_counter_max{ global_counter_max }
    {
    }

    execution_unit_handler 
    get_specific_handler(auto execution_specific_initer) noexcept 
    { 
        return { ::std::move(execution_specific_initer), *this }; 
    }

    void reset(CounterT global_counter_max) noexcept
    {
        ::std::lock_guard lk{ m_lock };
        m_global_counter_max = global_counter_max;
        m_global_counter_reserve = {};
        m_global_counter = {};
        m_counter.clear();
    }

    void reset() noexcept
    {
        return reset(m_global_counter_max);
    }

    bool add_count(const execution_unit_handler& h, CounterT delta) noexcept
    {
        auto& [cnt_max, cnt] = local_variable(h);
        const auto cnt_max_v = cnt_max.load(::std::memory_order_relaxed);
        const auto cnt_v = cnt.load(::std::memory_order_relaxed);
        if (cnt_max_v - cnt_v >= delta)
        {
            cnt.store(cnt_v + delta, ::std::memory_order_relaxed);
            return true;   
        }
        
        ::std::lock_guard lk{ m_lock };
        if (m_global_counter_max - m_global_counter - m_global_counter_reserve < delta)
        {
            return false;
        }
        m_global_counter += delta;
        balance_count(h);
        return true;
    }

    bool sub_count(const execution_unit_handler& h, CounterT delta) noexcept
    {
        auto& [cntmax, cnt] = local_variable(h);
        const auto cntval = cnt.load(::std::memory_order_relaxed);
        if (cntval >= delta)
        {
            cnt.store(cntval - delta, ::std::memory_order_relaxed);
            return true;
        }
        ::std::lock_guard lk{ m_lock };
        globalize_count(h);
        if (m_global_counter < delta) return false;
        m_global_counter -= delta;
        balance_count(h);
        return true;
    }
    
    auto read_count([[maybe_unused]] const execution_unit_handler&) noexcept
    {
        ::std::lock_guard lk{ m_lock };
        CounterT sum{ m_global_counter };
        for (const auto& item : m_counter)
        {
            sum += item.second.m_counter.load(::std::memory_order_relaxed);
        }
        return sum;
    }

    void count_unregister_thread(::std::thread::id tid) noexcept
    {
        ::std::lock_guard lk{ m_lock };
        globalize_count(tid);
        m_counter.erase(tid);       
    }

    CounterT limit() const noexcept { return m_global_counter_max; }

private:
    struct local_variable_t
    {
        ::std::atomic<CounterT> m_counter_max{};
        ::std::atomic<CounterT> m_counter{};
    };

private:
    auto& local_variable(const execution_unit_handler& h) noexcept
    {
        return local_variable(h.tid());
    }

    auto& local_variable(::std::thread::id tid) noexcept
    {
        if (!m_counter.contains(tid)) m_num_online.fetch_add(1, ::std::memory_order_relaxed);
        return m_counter[tid];
    }

    void balance_count(const execution_unit_handler& h) noexcept
    {
        auto& [cntmax, cnt] = local_variable(h);
        const CounterT cntmax_val = 
            (m_global_counter_max - m_global_counter - m_global_counter_reserve) / num_online_execution_unit();

        m_global_counter_reserve += cntmax_val;
        const CounterT cnt_val = cntmax_val / 2;
        cnt.store(cnt_val, ::std::memory_order_relaxed);
        cntmax.store(cntmax_val, ::std::memory_order_relaxed);
        if (cnt_val > m_global_counter)
            cnt.store(m_global_counter, ::std::memory_order_relaxed);
        m_global_counter -= cnt_val;
    }

    void globalize_count(::std::thread::id tid) noexcept
    {
        auto& [cntmax, cnt] = local_variable(tid);
        m_global_counter += cnt.load(::std::memory_order_relaxed);
        cnt.store(0, ::std::memory_order_relaxed);
        m_global_counter_reserve -= cntmax.load(::std::memory_order_relaxed);
        cntmax.store(0, ::std::memory_order_relaxed);
    }

    void globalize_count(const execution_unit_handler& h) noexcept
    {
        globalize_count(h.tid());
    }

    size_t num_online_execution_unit() const noexcept { return m_num_online; }

private:
    CounterT m_global_counter_max{};
    CounterT m_global_counter_reserve{};
    CounterT m_global_counter{};
    ::std::unordered_map<::std::thread::id, local_variable_t> m_counter;
    ::std::atomic_size_t m_num_online{};
    mutable spin_lock m_lock;   
};

TOOLPEX_NAMESPACE_END

#endif
