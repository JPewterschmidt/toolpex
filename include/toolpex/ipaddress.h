#ifndef TOOLPEX_IPADDRESS_H
#define TOOLPEX_IPADDRESS_H

#include "toolpex/macros.h"

#include <string_view>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <concepts>
#include <initializer_list>
#include <compare>
#include <array>
#include <span>
#include <cstdint>
#include <sys/socket.h>
#include <netinet/in.h>

TOOLPEX_NAMESAPCE_BEG

class ipv4_address
{
public:
    constexpr ipv4_address() = default;
    constexpr ipv4_address(const uint8_t a, const uint8_t b, 
                           const uint8_t c, const uint8_t d) noexcept
        : ia_data{ {a,b,c,d} }
    {
    }

    explicit constexpr ipv4_address(uint32_t a) noexcept
        : ia_data{ { static_cast<uint8_t>((a >> 24) & 0xFF), 
                     static_cast<uint8_t>((a >> 16) & 0xFF), 
                     static_cast<uint8_t>((a >>  8) & 0xFF), 
                     static_cast<uint8_t>((a      ) & 0xFF) } }
    {
    }

    ipv4_address(const ipv4_address& other) noexcept;

    ipv4_address& operator+=(int i);
    ipv4_address& operator-=(int i);
    ipv4_address& operator++();
    ipv4_address& operator--();
    ipv4_address  operator++(int);
    ipv4_address  operator--(int);

    ::std::string to_string() const;
    uint32_t      to_uint32() const noexcept;
    ::std::span<const uint8_t> as_uint8s() const noexcept;
    
private:
    ::std::array<uint8_t, 4> ia_data{};
};

::std::strong_ordering operator<=>(ipv4_address a, ipv4_address b);   

class ipv6_address
{
public:
    constexpr ipv6_address() = default;

    ipv6_address(::std::initializer_list<uint16_t> vals);
    ipv6_address(::std::span<uint16_t> vals);
    ipv6_address(::std::span<uint32_t> vals);
    ipv6_address(::std::span<uint64_t> vals);

    ::std::string to_string() const;
    const uint8_t* data() const noexcept;
    ::std::span<const uint64_t> as_uint64s() const noexcept;
    ::std::span<const uint16_t> as_uint16s() const noexcept;

    // Unicast embedded ipv6 address translator
    friend ipv6_address to_v6addr(ipv4_address v4a)
    {
        // embedded ipv6 address
        // well-known prefix 64:FF9B with ipv4 32-bits address

        // reference: https://www.rfc-editor.org/rfc/rfc6052

        // 0               16              32 
        // 12345678123456781234567812345678
        // |       |       |       |       |
        // 00000000011001001111111110011011     => 64:FF9B

        // 00000000000000000000000000000000
        // 00000000000000000000000000000000     => ::

        uint16_t prefix[8]{ 0x64, 0xFF9B };
        *reinterpret_cast<uint32_t*>(&prefix[6]) = v4a.to_uint32();
        ipv6_address ret{};
        ::memcpy(ret.i6a_data.data(), prefix, sizeof(prefix));
        return ret;
    }

    
private:
    ::std::array<uint16_t, 8> i6a_data{};
};

::std::strong_ordering 
operator<=>(const ipv6_address& a, const ipv6_address& b);   

::sockaddr_in  to_sockaddr(const ipv4_address& v4, ::in_port_t port);
::sockaddr_in6 to_sockaddr(const ipv6_address& v6, ::in_port_t port);

TOOLPEX_NAMESAPCE_END

#endif
