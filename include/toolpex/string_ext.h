#ifndef TOOLPEX_STRING_EXT_H
#define TOOLPEX_STRING_EXT_H

#include <compare>
#include <utility>
#include <cstring>
#include <algorithm>
#include <type_traits>
#include <string>
#include <string_view>

namespace toolpex
{

template<typename StringLike>
StringLike trim(const StringLike& str, typename StringLike::value_type delim = {' '})
{
    using size_type = typename StringLike::size_type;

    auto space_char = delim;
    size_type i;
    for (i = 0; i < str.size(); ++i)
    {
        if (str[i] != space_char) break;
    }
    size_type newstr_beg = i;
    for (i = str.size() - 1; i > newstr_beg - 1; --i)
    {
        if (str[i] != space_char) break;
    }
    size_type newstr_end = i + 1;
    
    return str.substr(newstr_beg, newstr_end - newstr_beg);
}

template<typename StringLike>
StringLike remove_quotation_mark(const StringLike& str)
{
    return trim(str, '"');
}

template<typename StringViewLike1, typename StringViewLike2>
::std::strong_ordering str_case_cmp(StringViewLike1&& s1, StringViewLike2&& s2) noexcept
{
    using size_type = typename ::std::remove_reference_t<StringViewLike1>::size_type;
    size_type len1 = s1.size(), len2 = s2.size();
    auto result = ::strncasecmp(s1.data(), s2.data(), ::std::min(len1, len2));
    if (result == 0)
    {
        if (len1 == len2) return ::std::strong_ordering::equal;
        else if (len1 < len2) return ::std::strong_ordering::less;
        else return ::std::strong_ordering::greater;
    }
    else if (result < 0) return ::std::strong_ordering::less;
    return ::std::strong_ordering::greater;
}

template<typename StringViewLike1, typename StringViewLike2>
bool str_case_equal(StringViewLike1&& s1, StringViewLike2&& s2) noexcept
{
    return str_case_cmp(
        ::std::string_view{::std::forward<StringViewLike1>(s1)}, 
        ::std::string_view{::std::forward<StringViewLike2>(s2)}
    ) == ::std::strong_ordering::equal;
}

template<typename StringViewLike1, typename StringViewLike2>
bool str_case_less(StringViewLike1&& s1, StringViewLike2&& s2) noexcept
{
    return str_case_cmp(
        ::std::string_view{::std::forward<StringViewLike1>(s1)}, 
        ::std::string_view{::std::forward<StringViewLike2>(s2)}
    ) == ::std::strong_ordering::less;
}

template<typename StringViewLike1, typename StringViewLike2>
bool str_case_greater(StringViewLike1&& s1, StringViewLike2&& s2) noexcept
{
    return str_case_cmp(
        ::std::string_view{::std::forward<StringViewLike1>(s1)}, 
        ::std::string_view{::std::forward<StringViewLike2>(s2)}
    ) == ::std::strong_ordering::greater;
}

template<typename StringViewLike1, typename StringViewLike2>
bool str_case_leq(StringViewLike1&& s1, StringViewLike2&& s2) noexcept
{
    auto od = str_case_cmp(::std::string_view{::std::forward<StringViewLike1>(s1)}, 
                            ::std::string_view{::std::forward<StringViewLike2>(s2)}
                           );
    return od == ::std::strong_ordering::less || od == ::std::strong_ordering::equal;
}

template<typename StringViewLike1, typename StringViewLike2>
bool str_case_geq(StringViewLike1&& s1, StringViewLike2&& s2) noexcept
{
    auto od = str_case_cmp(::std::string_view{::std::forward<StringViewLike1>(s1)}, 
                            ::std::string_view{::std::forward<StringViewLike2>(s2)}
                           );
    return od == ::std::strong_ordering::greater || od == ::std::strong_ordering::equal;
}

} // namespace toolpex

#endif
