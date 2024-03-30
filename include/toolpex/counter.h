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

/*! \brief  Perfbook's Approximate Limit Counter implementation (lock free)
 *  \tparam ExecutionIdType A type which could represent a execution unit, like thread id or coroutine address, etc.
 *  \tparam The underlying counter type.
 *
 *  \see Perfbook Chatper 5.3
 */
template<
    ::std::integral CounterT = ::std::size_t, 
    typename ExecutionIdType = ::std::thread::id>
class approximate_limit_counter
{
public:
    using execution_unit_handler = specific_counter_handler<ExecutionIdType, approximate_limit_counter>;
    
public:
    /*! \brief Ctor
     *  \param  global_counter_max The maximum value of the global counter.
     */
    constexpr approximate_limit_counter(long long global_counter_max) noexcept
        : m_global_counter_max{ static_cast<CounterT>(global_counter_max) }
    {
    }

    /*! \brief Make a `specific_counter_handler`
     *  \return A `specific_counter_handler`, 
     *          which could forward all the useful counter operations to the relative object of this type, 
     *          contains execution unit specific information.
     */
    execution_unit_handler 
    get_specific_handler(auto execution_specific_initer) noexcept 
    { 
        return { ::std::move(execution_specific_initer), *this }; 
    }

    /*! \brief Reset the counter, make it back to initial state.
     *  \param global_counter_max a new maximum limit of the global counter. 
     */
    void reset(CounterT global_counter_max) noexcept
    {
        ::std::lock_guard lk{ m_lock };
        m_global_counter_max = global_counter_max;
        m_global_counter_reserve = {};
        m_global_counter = {};
        m_counter.clear();
    }

    /*! \brief Reset the counter, make it back to initial state. */
    void reset() noexcept
    {
        return reset(m_global_counter_max);
    }

    /*! \brief Increase the value of counter.
     *  \param h the execution specific information object.
     *  \param delta the value you want to add.
     */
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

    /*! \brief Decrease the value of counter.
     *  \param h the execution specific information object.
     *  \param delta the value you want to subtract.
     */
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
    
    /*! \brief  Read the counter value.
     *  \attention  This call will access all the execution specific counter, 
     *              which caused cache miss potentially.
     */
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

    /*! \brief Unregister the current execution unit.
     *  
     *  Globalize the counter.
     *  Clean the resource the current execution unit occuiped.
     *
     *  \param tid Current execution unit id.
     *  
     *  \attention This function should be called after the use of 
     *             this counter in each execution unit (typically thread).
     *             Then you'd better to use a `execution_unit_handler` 
     *             returned by `get_specific_handler()`, 
     *             which utilze RAII to make sure this function will be called.
     */
    void count_unregister_execution_unit(ExecutionIdType tid) noexcept
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

    auto& local_variable(ExecutionIdType tid) noexcept
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

    void globalize_count(ExecutionIdType tid) noexcept
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
    ::std::unordered_map<ExecutionIdType, local_variable_t> m_counter;
    ::std::atomic_size_t m_num_online{};
    mutable spin_lock m_lock;   
};

TOOLPEX_NAMESPACE_END

#endif
