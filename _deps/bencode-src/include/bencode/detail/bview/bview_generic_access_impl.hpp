#pragma once
#include <cstdint>

#include "bencode/detail/bview/bview.hpp"
#include "bencode/detail/bview/bad_bview_access.hpp"

namespace bencode
{

// forward declaration
namespace detail {
bview evaluate(const bpointer& pointer, const bview& bv);
bool contains(const bpointer& pointer, const bview& bv);
}


constexpr bview bview::operator[](std::size_t pos) const
{
    if (!is_list()) [[unlikely]]
        throw bad_bview_access("bview alternative type is not list");
    return static_cast<const list_bview*>(this)->operator[](pos);
}


constexpr bview bview::at(std::size_t pos) const
{
    if (!is_list()) [[unlikely]]
        throw bad_bview_access("bview alternative type is not list");
    return static_cast<const list_bview*>(this)->at(pos);
}

constexpr bview bview::at(std::string_view key) const
{
    if (!is_dict()) [[unlikely]]
        throw bad_bview_access("bview alternative type is not dict");
    return static_cast<const dict_bview*>(this)->at(key);
}

inline  bview bview::at(const bpointer& pointer) const
{
    return detail::evaluate(pointer, *this);
}

constexpr bview bview::front() const
{
    if (!is_list()) [[unlikely]]
        throw bad_bview_access("bview alternative type is not list");
    return static_cast<const list_bview*>(this)->front();
}

constexpr bview bview::back() const
{
    if (!is_list()) [[unlikely]]
        throw bad_bview_access("bview alternative type is not list");
    return static_cast<const list_bview*>(this)->back();
}

constexpr bool bview::contains(std::string_view key) const
{
    if (!is_dict())
        throw bad_bview_access("bview alternative type is not dict");
    const auto* bdict = static_cast<const dict_bview*>(this);
    return bdict->find(key) != bdict->end();
}

inline bool bview::contains(const bpointer& pointer) const
{
    return detail::contains(pointer, *this);
}


}
