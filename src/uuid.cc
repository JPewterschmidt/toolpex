// This file is part of Koios
// https://github.com/JPewterschmidt/koios
//
// Copyleft 2023 - 2024, ShiXin Wang. All wrongs reserved.

#include "toolpex/uuid.h"
#include <stdexcept>

namespace toolpex 
{

static constexpr size_t uuid_length = 36;

uuid::uuid() noexcept
{
    ::uuid_generate(m_uuid);
}

uuid::uuid(::std::string str)
    : m_str{ ::std::move(str) }
{
    const int ret = ::uuid_parse(m_str.c_str(), m_uuid);
    if (ret) [[unlikely]] throw ::std::logic_error{ "uuid parse failed." };
}

uuid::uuid(uuid&& other) noexcept
    : m_str{ ::std::move(other.m_str) }
{
    ::uuid_copy(m_uuid, other.m_uuid);
    other.m_str = {};
}

uuid& uuid::operator=(uuid&& other) noexcept
{
    ::uuid_copy(m_uuid, other.m_uuid);
    m_str = ::std::move(other.m_str);
    other.m_str = {};
    return *this;
}

uuid::uuid(const uuid& other)
    : m_str{ other.m_str }
{
    ::uuid_copy(m_uuid, other.m_uuid);
}

uuid& uuid::operator=(const uuid& other)
{
    ::uuid_copy(m_uuid, other.m_uuid);
    m_str = other.m_str;
    return *this;
}

::std::string_view uuid::to_string() const 
{ 
    fill_string(); 
    return ::std::string_view{ m_str.data(), uuid_length };
}

void uuid::fill_string() const
{
    if (!m_str.empty()) return;
    m_str = ::std::string(uuid_length + 1, 0);
    ::uuid_unparse(m_uuid, m_str.data());
}

::std::strong_ordering operator<=>(const uuid& lhs, const uuid& rhs) noexcept
{
    const int val = ::uuid_compare(lhs.m_uuid, rhs.m_uuid);
    if (val == 0) return ::std::strong_ordering::equal;
    if (val <  0) return ::std::strong_ordering::less;
    return ::std::strong_ordering::greater;
}

} // namespace toolpex 
