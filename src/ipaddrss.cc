#include "toolpex/ipaddress.h"
#include "toolpex/exceptions.h"
#include "fmt/core.h"
#include <sstream>
#include <arpa/inet.h>
#include <bit>
#include <ranges>
#include <vector>
#include <sys/socket.h>
#include "toolpex/errret_thrower.h"

TOOLPEX_NAMESPACE_BEG

namespace ip_address_literals
{
    ip_address::ptr 
    operator""_ip(const char* ipstr, ::std::size_t len)
    {
        return ip_address::make({ ipstr, len });
    }
}

::std::pair<ip_address::ptr, ::in_port_t>
ip_address::
getpeername(const unique_posix_fd& fd)
{
    errret_thrower et;
    ::sockaddr_storage ss{};
    ::socklen_t len;
    et << ::getpeername(fd, (sockaddr*)&ss, &len);
    return ip_address::make((sockaddr*)&ss, len);
}

::std::pair<ip_address::ptr, ::in_port_t>
ip_address::make(const ::sockaddr* addr, ::socklen_t len)
{
    if (addr == nullptr) 
    {
        throw ip_address_exception{ "the ::sockaddr parameter is nullptr!" };
    }
    if (len >= sizeof(::sockaddr_in) && addr->sa_family == AF_INET)
    {
        const sockaddr_in* saddr = reinterpret_cast<const sockaddr_in*>(addr);
        return {::std::make_shared<ipv4_address>(saddr), ::ntohs(saddr->sin_port) };
    }
    else if (len >= sizeof(::sockaddr_in6) && addr->sa_family == AF_INET6)
    {
        const sockaddr_in6* saddr = reinterpret_cast<const sockaddr_in6*>(addr);
        return { ::std::make_shared<ipv6_address>(saddr), ::ntohs(saddr->sin6_port) };
    }
    throw ip_address_exception("only support ipv4 and v6");
    return {};
}

static 
ip_address::ptr
v4_localhost()
{
    char buf[sizeof(::in_addr)]{};
    errno = 0;
    if (int s = ::inet_pton(AF_INET, "localhost", &buf); s <= -1)
        throw ip_address_exception{ errno };

    ::sockaddr_in temp{
        .sin_family = AF_INET, 
    };
    ::std::memcpy(&(temp.sin_addr), buf, sizeof(::in_addr));
    return ip_address::make(reinterpret_cast<::sockaddr*>(&temp), sizeof(::sockaddr_in)).first;
}

static 
ip_address::ptr
make_v4(::std::string_view str)
{
    namespace sv = ::std::ranges::views;
    if (str == "localhost")
    {
        return v4_localhost();
    }

    ::std::vector<uint8_t> buf;
    buf.reserve(4);
    for (const auto i : str 
        | sv::split('.')
        | sv::transform([](auto const&& seg) noexcept { 
              return ::atoi(::std::data(seg));
          }))
    {
        buf.push_back(i);
    }
    if (buf.size() != 4) 
    {
        throw ip_address_exception{ 
            "there should be 4 number saperated by dot "
            "to represent a ipv4 address!" 
        };
    }
    return ::std::make_shared<ipv4_address>(
        buf[0], buf[1], buf[2], buf[3]
    );
}

static 
ip_address::ptr
make_v6(::std::string_view str)
{
    char buf[sizeof(::in6_addr)]{};
    errno = 0;
    if (int s = ::inet_pton(AF_INET6, str.data(), &buf); s <= -1)
        throw ipv4_address{ errno };

    ::sockaddr_in6 temp{
        .sin6_family = AF_INET6, 
    };
    ::std::memcpy(&(temp.sin6_addr), buf, sizeof(::in6_addr));
    return ip_address::make(reinterpret_cast<::sockaddr*>(&temp), sizeof(::sockaddr_in6)).first;
}

static inline
ip_address::ptr
v6_localhost()
{
    return make_v6("localhost");
}

ip_address::ptr 
ip_address::make(::std::string_view str)
{
    using namespace ::std::string_view_literals;

    if (str.contains('.') && !str.contains(":"))
        return make_v4(str);
    else if (str.contains(':') && !str.contains("."))
        return make_v6(str);
    else if (str == "localhost"sv)
        return ::std::make_shared<ipv4_address>(0);
    throw ip_address_exception{ "bad str to convert to a ipaddress." };
    return {};
}

ipv4_address::ipv4_address(const ::sockaddr_in* sock4)
{
    if (sock4->sin_family != AF_INET) [[unlikely]]
        throw ::std::logic_error{"the ::sockaddr_in* parameter is not for ipv4!!"};
    uint32_t temp{ sock4->sin_addr.s_addr };
    ::memcpy(ia_data.data(), &temp, sizeof(temp));
}

ipv4_address::ipv4_address(const ipv4_address& other) noexcept
    : ia_data{ other.ia_data }
{
}

ipv6_address::ipv6_address(const ::sockaddr_in6* s) 
{
    if (s->sin6_family != AF_INET6) [[unlikely]]
        throw ::std::logic_error{"the ::sockaddr_in6* parameter is not for ipv6!!"};

    ::std::array<uint16_t, 8> temp_data{};
    ::memcpy(temp_data.data(), &(s->sin6_addr.s6_addr), sizeof(temp_data));
    for (auto& each_seg : temp_data)
    {
        each_seg = ::ntohs(each_seg);
    }
    ::memcpy(i6a_data.data(), temp_data.data(), sizeof(i6a_data));   
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
    uint32_t result{};
    ::std::memcpy(&result, ia_data.data(), sizeof(result));
    return result;
}

::std::span<const uint16_t> ipv6_address::as_uint16s() const noexcept
{
    return { i6a_data.data(), 8 };
}

ipv4_address& ipv4_address::operator+=(int i)
{
    uint32_t temp{};
    ::std::memcpy(&temp, ia_data.data(), sizeof(temp));
    ++temp;
    ::std::memcpy(ia_data.data(), &temp, sizeof(temp));
    return *this;
}

ipv4_address& ipv4_address::operator-=(int i)
{
    uint32_t temp{};
    ::std::memcpy(&temp, ia_data.data(), sizeof(temp));
    --temp;
    ::std::memcpy(ia_data.data(), &temp, sizeof(temp));
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
operator<=>(const ipv6_address& a, const ipv6_address& b)
{
    uint64_t a_span[2]{};
    uint64_t b_span[2]{};

    ::std::memcpy(a_span, a.i6a_data.data(), sizeof(a.i6a_data));
    ::std::memcpy(b_span, b.i6a_data.data(), sizeof(b.i6a_data));

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

// Unicast embedded ipv6 address translator
ipv6_address to_v6addr(ipv4_address v4a)
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
    const uint32_t temp = v4a.to_uint32();
    ::std::memcpy(&(prefix[6]), &temp, sizeof(temp));
    ipv6_address ret{};
    ::memcpy(ret.i6a_data.data(), prefix, sizeof(prefix));
    return ret;
}

::std::pair<::sockaddr_storage, ::socklen_t>
ipv4_address::
to_sockaddr(::in_port_t port) const
{
    ::sockaddr_storage result{};
    auto buf = toolpex::to_sockaddr(*this, port);
    ::std::memcpy(&result, &buf, sizeof(buf));
    return { result, sizeof(buf) };
}

::std::pair<::sockaddr_storage, ::socklen_t>
ipv6_address::
to_sockaddr(::in_port_t port) const
{
    ::sockaddr_storage result{};
    auto buf = toolpex::to_sockaddr(*this, port);
    ::std::memcpy(&result, &buf, sizeof(buf));
    return { result, sizeof(buf) };   
}

::sockaddr_in6 
to_sockaddr(const ipv6_address& v6, ::in_port_t port)
{
    ::sockaddr_in6 result{
        .sin6_family = AF_INET6,
        .sin6_port   = ::htons(port), 
    };
    ::std::array<uint16_t, 8> temp_data{};
    ::memcpy(temp_data.data(), v6.i6a_data.data(), sizeof(temp_data));
    for (auto& each_seg : temp_data)
    {
        each_seg = ::htons(each_seg);
    }
    ::memcpy(result.sin6_addr.s6_addr, temp_data.data(), sizeof(temp_data));

    return result;
}

ip_address::ptr
ipv4_address::
dup() const
{
    return ::std::make_shared<ipv4_address>(*this);
}

ip_address::ptr
ipv6_address::
dup() const
{
    return ::std::make_shared<ipv6_address>(*this);
}

ip_address::ptr
ipv4_address::get_loopback()
{
    static ip_address::ptr result = ::std::make_shared<ipv4_address>(127, 0, 0, 1);
    return result;
}

ip_address::ptr
ipv4_address::get_broadcast()
{
    static ip_address::ptr result = ::std::make_shared<ipv4_address>(255, 255, 255, 255);
    return result;
}

ip_address::ptr
ipv4_address::get_allzero()
{
    static ip_address::ptr result = ::std::make_shared<ipv4_address>(0);
    return result;
}

ip_address::ptr
ipv4_address::get_localhost()
{
    static ip_address::ptr result = v4_localhost();
    return result;
}

ip_address::ptr
ipv6_address::get_loopback()
{
    static ip_address::ptr result = ip_address::make("::1");
    return result;
}

ip_address::ptr
ipv6_address::get_allzero()
{
    static ip_address::ptr result = ip_address::make("::");
    return result;
}

ip_address::ptr
ipv6_address::get_localhost()
{
    static ip_address::ptr result = v6_localhost();
    return result;
}

TOOLPEX_NAMESPACE_END
