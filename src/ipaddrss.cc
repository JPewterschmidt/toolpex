#include "toolpex/ipaddress.h"
#include "fmt/core.h"
#include <sstream>
#include <arpa/inet.h>

using namespace toolpex;

ipv4_address::ipv4_address(const ipv4_address& other) noexcept
    : ia_data{ other.ia_data }
{
}

ipv6_address::ipv6_address(::std::initializer_list<uint16_t> vals)
{
    if (vals.size() != 8) [[unlikely]]
    {
        throw ::std::logic_error{ 
            "You need pass a array which type is uint16_t "
            "and it's size is 8"
        };
    }

    for (size_t i = 0; i < 8; ++i)
    {
        i6a_data[i] = ::std::data(vals)[i];
    }
}

ipv6_address::ipv6_address(::std::span<uint16_t> vals)
{
    if (vals.size() != 8) [[unlikely]]
    {
        throw ::std::logic_error{ 
            "You need pass a array which type is uint16_t "
            "and it's size is 8"
        };
    }
    ::memcpy(i6a_data.data(), vals.data(), vals.size_bytes());
}

ipv6_address::ipv6_address(::std::span<uint32_t> vals)
{
    if (vals.size() != 4) [[unlikely]]
    {
        throw ::std::logic_error{ 
            "You need pass a array which type is uint32_t "
            "and it's size is 4"
        };
    }
    ::memcpy(i6a_data.data(), vals.data(), vals.size_bytes());
}

ipv6_address::ipv6_address(::std::span<uint64_t> vals)
{
    if (vals.size() != 2) [[unlikely]]
    {
        throw ::std::logic_error{ 
            "You need pass a array which type is uint64_t "
            "and it's size is 2"
        };
    }
    ::memcpy(i6a_data.data(), vals.data(), vals.size_bytes());
}

::std::string ipv6_address::to_string() const
{
    ::std::stringstream ss;
    ss << "[";
    ::std::span<const uint16_t> addr = as_uint16s();
    bool used_zero{};
    for (size_t i = 0; i < addr.size(); ++i)
    {
        if (addr[i] == 0 && !used_zero) continue;
        if (i && addr[i - 1] == 0 && !used_zero)
        {
            ss << ":";
            used_zero = true;
        }
        if (i) ss << ":";
        ss << ::std::hex << static_cast<int>(addr[i]);
    }
    ss << "]";
    return ss.str();
}

::std::string ipv4_address::to_string() const
{
    return fmt::format("{}.{}.{}.{}", 
            ia_data[0], ia_data[1], ia_data[2], ia_data[3]);
}

uint32_t ipv4_address::to_uint32() const noexcept
{
    return *reinterpret_cast<const uint32_t*>(ia_data.data());
}

::std::span<const uint8_t> ipv4_address::as_uint8s() const noexcept
{
    return { ia_data.data(), ia_data.size() };
}

const uint8_t* ipv6_address::data() const noexcept
{
    return reinterpret_cast<const uint8_t*>(i6a_data.data());
}

::std::span<const uint64_t> ipv6_address::as_uint64s() const noexcept
{
    return { reinterpret_cast<const uint64_t*>(data()), 2 };
}

::std::span<const uint16_t> ipv6_address::as_uint16s() const noexcept
{
    return { reinterpret_cast<const uint16_t*>(data()), 8 };
}

ipv4_address& ipv4_address::operator+=(int i)
{
    *reinterpret_cast<uint32_t*>(ia_data.data()) += i;
    return *this;
}

ipv4_address& ipv4_address::operator-=(int i)
{
    *reinterpret_cast<uint32_t*>(ia_data.data()) -= i;
    return *this;
}

ipv4_address& ipv4_address::operator++()
{
    return (*this) += 1;
}

ipv4_address& ipv4_address::operator--()
{
    return (*this) -= 1;
}

ipv4_address  ipv4_address::operator++(int)
{
    ipv4_address ret{ *this };
    this->operator++();
    return ret;
}

ipv4_address  ipv4_address::operator--(int)
{
    ipv4_address ret{ *this };
    this->operator--();
    return ret;
}

::std::strong_ordering 
operator<=>(ipv4_address a, ipv4_address b)
{
    return a.to_uint32() <=> b.to_uint32();
}

::std::strong_ordering 
operator<=>(const ipv6_address& a, const ipv6_address& b)
{
    auto a_span = a.as_uint64s();
    auto b_span = b.as_uint64s();
    auto ret = a_span[0] <=> b_span[0];
    if (ret == ::std::strong_ordering::equal)
        return a_span[1] <=> b_span[1];
    return ret;
}

::sockaddr_in  
to_sockaddr(const ipv4_address& v4, ::in_port_t port)
{
    ::sockaddr_in result{
        .sin_family = AF_INET, 
        .sin_port = ::htons(port)
    };
    result.sin_addr.s_addr = v4.to_uint32();

    return result;
}

::sockaddr_in6 
to_sockaddr(const ipv6_address& v6, ::in_port_t port)
{
    ::sockaddr_in6 result{
        .sin6_family = AF_INET6,
        .sin6_port   = ::htons(port), 
    };
    ::std::array<uint32_t, 4> temp_data{};
    ::memcpy(temp_data.data(), v6.data(), sizeof(temp_data));
    for (auto& each_seg : temp_data)
    {
        each_seg = ::htonl(each_seg);
    }
    ::memcpy(result.sin6_addr.s6_addr, temp_data.data(), sizeof(temp_data));

    return result;
}
