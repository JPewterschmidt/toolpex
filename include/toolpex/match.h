#ifndef TOOLPEX_MATCH_H
#define TOOLPEX_MATCH_H

#include "toolpex/macros.h"
#include <span>
#include <concepts>
#include <cstddef>
#include <cstdint>

TOOLPEX_NAMESPACE_BEG

namespace match_detials
{
    long match_ps_e1(::std::span<::std::byte> src, 
                        ::std::span<const ::std::byte> pattern);
    long match_ps_word(::std::span<::std::byte> src, 
                          ::std::span<const ::std::byte> pattern);
    long match_ps_giant(::std::span<::std::byte> src, 
                           ::std::span<const ::std::byte> pattern);
}

template<typename T>
requires (::std::is_fundamental_v<T>) // could compare the bytes directly
long
match(::std::span<T> src, ::std::span<const T> pattern)
{
    auto src_bytes = as_writable_bytes(src);
    auto pattern_bytes = as_bytes(pattern);

    if (pattern.size() == 0) return src;
    if (pattern.size() == 1) return match_detials::match_ps_e1(src_bytes, pattern_bytes);

    if (pattern.size() >= 2 && pattern_bytes.size() <= 4)
        return match_detials::match_ps_word(src_bytes, pattern_bytes);
    
    switch (pattern.size())
    {
        case 0: return src;
        case 1: return match_detials::match_ps_e1(src_bytes, pattern_bytes);
        case 2: case 3: 
        case 4: return match_detials::match_ps_word(src_bytes, pattern_bytes);
        default: return match_detials::match_ps_giant(src_bytes, pattern_bytes);
    }

    return {}; // prevent warning
}

TOOLPEX_NAMESPACE_END

#endif
