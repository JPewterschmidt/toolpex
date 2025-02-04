// This file is part of Toolpex
// https://github.com/JPewterschmidt/toolpex
//
// Copyleft 2023 - 2024, ShiXin Wang. All wrongs reserved.

#ifndef TOOLPEX_SPIN_LOCK_H
#define TOOLPEX_SPIN_LOCK_H

#include "toolpex/macros.h"
#include <atomic>

TOOLPEX_NAMESPACE_BEG

/**
 * @class spin_lock
 * 
 * @brief A simple spinlock implementation for mutual exclusion.
 * 
 * This class provides a lightweight spinlock that can be used for synchronizing access to shared resources.
 * The lock is implemented using `std::atomic_flag`, and operations are performed in a busy-wait (spin) loop. 
 * 
 * @note **Important**: Users of this `spin_lock` should be aware of potential deadlocks caused by interruptions. 
 * Since the lock implementation is based on busy-waiting, it may lead to high CPU utilization and system inefficiency in certain cases, 
 * especially when used in environments where interruption or cancellation of threads is frequent.
 * 
 * A deadlock may occur if an interrupt happens after acquiring the lock but before releasing it, especially in multithreaded applications
 * where a thread may attempt to acquire the lock while it is held by another, potentially causing a circular wait condition. 
 * To prevent such deadlocks, users should be cautious when using this lock in high-interrupt environments, or consider using more 
 * advanced locking mechanisms that support interrupt safety or prevent deadlocks.
 */
class spin_lock
{
public:
    /**
     * @brief Default constructor for the spinlock.
     * 
     * Initializes the spinlock with a default state (unlocked).
     */
    constexpr spin_lock() noexcept = default;

    /**
     * @brief Acquires the lock.
     * 
     * This method will continuously attempt to acquire the lock until successful.
     * It uses a busy-wait loop, which means it will repeatedly check the lock's status until it is acquired.
     * 
     * @note The method should be used with caution to avoid interruption-related deadlocks.
     */
    inline void lock() noexcept
    {
        while (try_lock()) 
        { 
            while (is_locked())
                ;
        }
    }

    /**
     * @brief Attempts to acquire the lock.
     * 
     * This method tries to acquire the lock and returns `true` if the lock was successfully acquired, 
     * or `false` if it was already held by another thread.
     * 
     * @return `true` if the lock was acquired, `false` if it is already locked.
     * 
     * @note This function may cause deadlocks if used incorrectly in environments with interruptions.
     */
    inline bool try_lock() noexcept
    {
        return m_flag.test_and_set(::std::memory_order_acquire);
    }

    /**
     * @brief Releases the lock.
     * 
     * This method releases the spinlock, making it available for other threads to acquire.
     * 
     * @note Be careful with using this in multi-threaded systems where interruptions can happen,
     *       as it may lead to a deadlock condition if interrupts happen during critical sections.
     */
    inline void unlock() noexcept
    {
        return m_flag.clear(::std::memory_order_release);
    }

    /**
     * @brief Checks whether the lock is currently held.
     * 
     * This method returns whether the lock is currently in the locked state.
     * 
     * @return `true` if the lock is held, `false` if it is unlocked.
     */
    bool is_locked() const noexcept
    {
        return m_flag.test(::std::memory_order_relaxed);
    }

private:
    ::std::atomic_flag m_flag{};
};

TOOLPEX_NAMESPACE_END

#endif
