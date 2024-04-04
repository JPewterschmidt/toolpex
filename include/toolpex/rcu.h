#ifndef TOOLPEX_RCU_H
#define TOOLPEX_RCU_H

#include <atomic>
#include <memory>

namespace toolpex
{

namespace rcu_detials
{



} // namespace rcu_detials

/*! \brief An user-space RCU implementation.
 *  \tparam T The value type (also `value_type`)
 */
template<typename T>
requires (::std::atomic<T>::is_always_lock_free())
class rcu
{
public:
    using value_type        = T;
    using reference         = value_type&;
    using const_reference   = const value_type&;
    using pointer           = value_type*;
    using const_pointer     = const value_type*;

public:
    rcu();
    rcu(rcu&& other) noexcept = default;
    rcu& operator=(rcu&& other) noexcept = default;

private:
    
};

} // namespace toolpex

#endif
