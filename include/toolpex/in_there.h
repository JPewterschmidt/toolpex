#ifndef TOOLPEX_IN_THERE_H
#define TOOLPEX_IN_THERE_H

namespace toolpex
{

class in_there
{
public:
    constexpr in_there() noexcept = default;
    constexpr in_there(const auto&&) noexcept {}

    template<typename Arg1, typename... Args>
    in_there(const Arg1&& arg1, const Args&&... rests)
    {
        ((m_result |= (::std::forward<const Arg1>(arg1) == ::std::forward<const Args>(rests))), ...);
    }

    operator bool () const noexcept { return m_result; }

private:
    bool m_result{};
};


} // namespace toolpex

#endif
