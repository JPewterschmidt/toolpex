#include "toolpex/match.h"
#include "toolpex/exceptions.h"

TOOLPEX_NAMESPACE_BEG namespace match_detials {

long match_ps_e1(::std::span<::std::byte> src, 
                   ::std::span<const ::std::byte> pattern)
{
    for (size_t i{}; i < src.size(); ++i)
    {
        if (src[i] == pattern[0])
            return static_cast<long>(i);
    }
    return -1;
}

long match_ps_word(::std::span<::std::byte> src, 
                      ::std::span<const ::std::byte> pattern)
{
    not_implemented();   
    return -1;
}

long match_ps_giant(::std::span<::std::byte> src, 
                       ::std::span<const ::std::byte> pattern)
{
    not_implemented();   
    return -1;
}

}TOOLPEX_NAMESPACE_END
