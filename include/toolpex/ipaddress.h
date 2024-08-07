// This file is part of Toolpex
// https://github.com/JPewterschmidt/toolpex
//
// Copyleft 2023 - 2024, ShiXin Wang. All wrongs reserved.

#ifndef TOOLPEX_IPADDRESS_H
#define TOOLPEX_IPADDRESS_H

#include "toolpex/macros.h"
#include "toolpex/unique_posix_fd.h"

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
#include <memory>
#include <utility>

TOOLPEX_NAMESPACE_BEG

class ip_address
{
public:
    using ptr = ::std::shared_ptr<ip_address>;

public:
    virtual ::std::string to_string() const = 0;
    virtual ~ip_address() noexcept {};
    virtual ::std::pair<::sockaddr_storage, ::socklen_t> to_sockaddr(::in_port_t port) const = 0;
    virtual int family() const noexcept = 0;
    virtual ptr dup() const = 0;

    static ::std::pair<ip_address::ptr, ::in_port_t> 
    make(const ::sockaddr* addr, ::socklen_t len);

    /*  \brief Build a ipaddress from string
     *
     *  Automaticall detect which family it should be
     *  Never return a nullptr
     *
     *  \return the smart pointer which point to a object 
     *          of the pure virtual base class `ip_address`.
     *          Never be a nullptr.
     *
     *  \attention If there're something wrong, it will throw a exception `toolpex::ip_address_exception`
     *  \see `ip_address_exception`
     */
    static ip_address::ptr make(::std::string_view str);
    static ::std::pair<ip_address::ptr, ::in_port_t> getpeername(const unique_posix_fd& fd);
};

::std::ostream& operator<<(::std::ostream& os, ip_address::ptr addr);

namespace ip_address_literals
{
    ip_address::ptr 
    operator""_ip(const char* ipstr, ::std::size_t);
}

class ipv4_address : public ip_address
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

    explicit ipv4_address(const ::sockaddr_in* sock4);

    ipv4_address(const ipv4_address& other) noexcept;

    ipv4_address& operator+=(int i);
    ipv4_address& operator-=(int i);
    ipv4_address& operator++();
    ipv4_address& operator--();
    ipv4_address  operator++(int);
    ipv4_address  operator--(int);

    virtual ::std::string to_string() const override;
    uint32_t to_uint32() const noexcept;
    virtual ~ipv4_address() noexcept {};
    virtual ::std::pair<::sockaddr_storage, ::socklen_t> to_sockaddr(::in_port_t port) const override;

    friend ::sockaddr_in 
    to_sockaddr(const ipv4_address& v4, ::in_port_t port);

    friend ::std::strong_ordering operator<=>(ipv4_address a, ipv4_address b)
    {
        return a.to_uint32() <=> b.to_uint32();
    }

    int family() const noexcept override { return AF_INET; }
    ip_address::ptr dup() const override;

public:
    static ip_address::ptr get_loopback();
    static ip_address::ptr get_broadcast();
    static ip_address::ptr get_allzero();

private:
    ::std::array<uint8_t, 4> ia_data{};
};

class ipv6_address : public ip_address
{
public:
    constexpr ipv6_address() = default;

    ipv6_address(::std::initializer_list<uint16_t> vals);
    ipv6_address(::std::span<uint16_t> vals);
    explicit ipv6_address(const ::sockaddr_in6* sock6);

    virtual ::std::string to_string() const override;
    ::std::span<const uint16_t> as_uint16s() const noexcept;
    virtual ::std::pair<::sockaddr_storage, ::socklen_t> to_sockaddr(::in_port_t port) const override;

    friend ipv6_address to_v6addr(ipv4_address v4a);
    virtual ~ipv6_address() noexcept {}

    friend ::std::strong_ordering 
    operator<=>(const ipv6_address& a, const ipv6_address& b);

    friend
    ::sockaddr_in6 
    to_sockaddr(const ipv6_address& v6, ::in_port_t port);
    
    int family() const noexcept override { return AF_INET6; }
    ip_address::ptr dup() const override;

public:
    static ip_address::ptr get_loopback();
    static ip_address::ptr get_allzero();

private:
    ::std::array<uint16_t, 8> i6a_data{};
};

::std::strong_ordering operator<=>(const ipv6_address& a, const ipv6_address& b);
ipv6_address to_v6addr(ipv4_address v4a);
::sockaddr_in  to_sockaddr(const ipv4_address& v4, ::in_port_t port);
::sockaddr_in6 to_sockaddr(const ipv6_address& v6, ::in_port_t port);

TOOLPEX_NAMESPACE_END

extern template class ::std::shared_ptr<toolpex::ip_address>;

#endif
