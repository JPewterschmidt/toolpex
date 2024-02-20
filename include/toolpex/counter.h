// This idea coms from **perfbook** Chapter 5, 
// code simply copyed from perfbook example.
// https://github.com/paulmckrcu/perfbook

#ifndef TOOLPEX_COUNTER_H
#define TOOLPEX_COUNTER_H

#include "toolpex/macros.h"
#include "toolpex/spin_lock.h"
#include <unordered_map>
#include <ranges>
#include <numeric>

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
        m_parent->count_unregister_thread(m_tid);
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
