// This file is part of Toolpex
// https://github.com/JPewterschmidt/toolpex
//
// Copyleft 2023 - 2024, ShiXin Wang. All wrongs reserved.

#ifndef TOOLPEX_UUID_H
#define TOOLPEX_UUID_H

#include <cstring>
#include <string>
#include <string_view>
#include <compare>

#include "uuid/uuid.h"

namespace toolpex 
{

class uuid
{
public:
    uuid() noexcept;
    uuid(::std::string str);
    uuid(uuid&& other) noexcept;
    uuid& operator=(uuid&& other) noexcept;
    uuid(const uuid& other);
    uuid& operator=(const uuid& other);

    operator ::std::string_view() const { return to_string(); }
    ::std::string_view to_string() const;

    // Lexicaographic order
    friend ::std::strong_ordering operator<=>(const uuid& lhs, const uuid& rhs) noexcept;

    bool operator != (const uuid& other) const noexcept
    {
        return ((*this) <=> other) != ::std::strong_ordering::equal;
    }

    bool operator == (const uuid& other) const noexcept
    {
        return ((*this) <=> other) == ::std::strong_ordering::equal;
    }

private:
    void fill_string() const;

private:
    ::uuid_t m_uuid{};
    mutable ::std::string m_str{};
};

} // namespace toolpex 

#endif
