#ifndef TOOLPEX_ENCODE_H
#define TOOLPEX_ENCODE_H

#include <bit>
#include <span>
#include <string>
#include <cassert>
#include <concepts>
#include <cstring>
#include <cstddef>
#include <cstdint>

namespace toolpex
{

template<::std::integral Int>
Int decode_big_endian_from(::std::span<const ::std::byte> buffer)
{
    assert(buffer.size() >= sizeof(Int));
    Int result{};
    ::std::memcpy(&result, buffer.data(), sizeof(Int));
    
    if constexpr (::std::endian::native == ::std::endian::little)
    {
        return ::std::byteswap(result);
    }
    else if constexpr (::std::endian::native == ::std::endian::big)
    {
        return result;
    }
    else // Not support mixed endian
    {
        assert(false);
        return {};
    }
}

void encode_big_endian_to(::std::integral auto i, ::std::span<::std::byte> buffer)
{
    assert(buffer.size() >= sizeof(i));
    if constexpr (::std::endian::native == ::std::endian::little)
    {
        i = ::std::byteswap(i);
    }
    ::std::memcpy(buffer.data(), &i, sizeof(i));
}

template<::std::integral Int>
Int decode_little_endian_from(::std::span<const ::std::byte> buffer)
{
    assert(buffer.size() >= sizeof(Int));
    Int result{};
    ::std::memcpy(&result, buffer.data(), sizeof(Int));
    
    if constexpr (::std::endian::native == ::std::endian::big)
    {
        return ::std::byteswap(result);
    }
    else if constexpr (::std::endian::native == ::std::endian::little)
    {
        return result;
    }
    else // Not support mixed endian
    {
        assert(false);
        return {};
    }
}

void encode_little_endian_to(::std::integral auto i, ::std::span<::std::byte> buffer)
{
    assert(buffer.size() >= sizeof(i));
    if constexpr (::std::endian::native == ::std::endian::big)
    {
        i = ::std::byteswap(i);
    }
    ::std::memcpy(buffer.data(), &i, sizeof(i));
}

template<::std::integral Int>
Int decode_little_endian_from(::std::span<const char> buffer)
{
    return decode_little_endian_from<Int>(::std::as_bytes(buffer));
}

void encode_little_endian_to(::std::integral auto i, ::std::span<char> buffer)
{
    encode_little_endian_to(i, ::std::as_writable_bytes(buffer));
}

template<::std::integral Int>
Int decode_big_endian_from(::std::span<const char> buffer)
{
    return decode_big_endian_from<Int>(::std::as_bytes(buffer));
}

void encode_big_endian_to(::std::integral auto i, ::std::span<char> buffer)
{
    encode_big_endian_to(i, ::std::as_writable_bytes(buffer));
}

template<::std::integral Int>
Int decode_little_endian_from(::std::span<const uint8_t> buffer)
{
    return decode_little_endian_from<Int>(::std::as_bytes(buffer));
}

void encode_little_endian_to(::std::integral auto i, ::std::span<uint8_t> buffer)
{
    encode_little_endian_to(i, ::std::as_writable_bytes(buffer));
}

template<::std::integral Int>
Int decode_big_endian_from(::std::span<const uint8_t> buffer)
{
    return decode_big_endian_from<Int>(::std::as_bytes(buffer));
}

void encode_big_endian_to(::std::integral auto i, ::std::span<uint8_t> buffer)
{
    encode_big_endian_to(i, ::std::as_writable_bytes(buffer));
}

} // namespace toolpex

#endif
