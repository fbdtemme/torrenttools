#pragma once

#include <cstdint>
#include <vector>
#include <stack>
#include <algorithm>
#include <compare>
#include <gsl-lite/gsl-lite.hpp>


#include "bencode/detail/bencode_type.hpp"
#include "bencode/detail/symbol.hpp"
#include "bencode/detail/utils.hpp"

#include "bencode/detail/parser/parsing_error.hpp"
#include "bencode/detail/bitmask_operators.hpp"

namespace bencode {
enum class descriptor_type : std::uint8_t;
BENCODE_ENABLE_BITMASK_OPERATORS(descriptor_type);

}
namespace bencode {

/// Enumeration type specifying the data stored in a descriptor.
enum class descriptor_type : std::uint8_t
{
    // core types
    integer = 0x01,     ///< descriptor holds integer data
    string = 0x02,      ///< descriptor holds string data
    list = 0x04,        ///< descriptor holds list data
    dict = 0x08,        ///< descriptor holds dict data
    // modifiers
    none = 0x00,         ///< object has no modifiers
    list_value = 0x10,   ///< value is a list item
    dict_key = 0x20,     ///< value is a dict key
    dict_value = 0x40,   ///< value is a dict value
    end = 0x80,          ///< list or dict descriptor is end descriptor
    stop = 0xF0          ///< placholder type for the end of a sequence of descriptors
};

/// class describing the value and metadata contained in bencode tokens.
class alignas(16) descriptor
{
private:
    /// the decoded bvalue of an bencoded integer
    struct integer_data
    {
        std::int64_t value;
    };

    /// Data to describes a bencoded data types.
    /// The meaning of the fields varies with the `descriptor_type`.
    ///     * string:
    ///         offset: offset to the first character byte of the string.
    ///         size:   length of the string.
    ///     * list
    ///         offset: number of tape_entry objects between the matching list start/end token.
    ///         size:   the number of bencoded values in list
    ///     * dict
    ///         offset: number of tape_entry objects between the matching list start/end token.
    ///         size:   the number of bencoded key bvalue pairs in the list
    ///
    struct structured_data
    {
        std::uint32_t offset;
        std::uint32_t size;
    };

    union descriptor_data
    {
        integer_data integer;
        structured_data structured;
    };

public:
    template <typename... Args>
    constexpr descriptor(descriptor_type type, std::size_t position, Args... args) noexcept
            : type_(type)
            , position_(position)
            , data_()
    {
        if constexpr(sizeof...(Args) == 0) {}
        else if constexpr(sizeof...(Args) == 1) {
            if (is_integer())
                data_.integer = {args...};
        }
        else if constexpr (sizeof...(Args) == 2) {
            if (is_string() || is_list() || is_dict())
                data_.structured = {static_cast<std::uint32_t>(args)...};
        }
        else {
            static_assert((detail::always_false_v<Args> || ...), "invalid number of arguments");
        }
    }


    /// Returns the type of the bencode data.
    /// @returns the type of the bencode data
    [[nodiscard]]
    constexpr bencode_type type() const noexcept
    {
        const std::uint8_t low_nibble = std::uint8_t(type_) & 0x0F;
        switch (low_nibble) {
        case std::uint8_t(descriptor_type::integer): return bencode_type::integer;
        case std::uint8_t(descriptor_type::string):  return bencode_type::string;
        case std::uint8_t(descriptor_type::list):    return bencode_type::list;
        case std::uint8_t(descriptor_type::dict):    return bencode_type::dict;
        default: BENCODE_UNREACHABLE;
        };
    }

    /// Returns true if the descriptor describes an integer, false otherwise.
    constexpr bool is_integer() const noexcept
    { return (type_ & descriptor_type::integer) == descriptor_type::integer; }

    /// Returns true if the descriptor describes a string, false otherwise.
    constexpr bool is_string() const noexcept
    { return (type_ & descriptor_type::string) == descriptor_type::string; }

    /// Returns true if the descriptor describes a list start token.
    constexpr bool is_list_begin() const noexcept
    {
        constexpr auto mask = (descriptor_type::list | descriptor_type::end);
        return (type_ & mask) == descriptor_type::list;
    }

    /// Returns true if the descriptor describes a list end token, false otherwise.
    constexpr bool is_list_end() const noexcept
    {
        constexpr auto mask = (descriptor_type::list | descriptor_type::end);
        return (type_ & mask) == mask;
    }

    /// Returns true if the descriptor describes a dict start token, false otherwise.
    constexpr bool is_dict_begin() const noexcept
    {
        constexpr auto mask = (descriptor_type::dict | descriptor_type::end);
        return (type_ & mask) == descriptor_type::dict;
    }

    /// Returns true if the descriptor describes a dict end token, false otherwise.
    constexpr bool is_dict_end() const noexcept
    {
        constexpr auto mask = (descriptor_type::dict | descriptor_type::end);
        return (type_ & mask) == mask;
    }

    /// Returns true if the descriptor describes a list start or end token, false otherwise.
    constexpr bool is_list() const noexcept
    { return (type_ & descriptor_type::list) == descriptor_type::list; }

    /// Returns true if the descriptor describes a dict start or end token, false otherwise.
    constexpr bool is_dict() const noexcept
    { return (type_ & descriptor_type::dict) == descriptor_type::dict; }

    /// Returns true if the descriptor describes a value of a list, false otherwise.
    constexpr bool is_list_value() const noexcept
    { return (type_ & descriptor_type::list_value) == descriptor_type::list_value; }

    /// Returns true if the descriptor describes a dict key, false otherwise.
    constexpr bool is_dict_key() const noexcept
    { return (type_ & descriptor_type::dict_key) == descriptor_type::dict_key; }

    /// Returns true if the descriptor describes a dict value, false otherwise.
    constexpr bool is_dict_value() const noexcept
    { return (type_ & descriptor_type::dict_value) == descriptor_type::dict_value; }

    /// Returns the position in the bencoded buffer of the start of the token.
    constexpr auto position() const noexcept -> std::uint32_t
    { return position_; }

    /// Returns the value of an integer token,
    /// Behavior is undefined if the data type is not an integer.
    constexpr auto value() const noexcept -> std::int64_t
    {
        Expects(is_integer());
        return data_.integer.value;
    }

    /// Sets the value of an integer token,
    /// Behavior is undefined if the data type is not an integer.
    /// @param v the value to set
    constexpr void set_value(std::int64_t v) noexcept
    {
        Expects(is_integer());
        data_.integer.value = v;
    };


    /// Returns the size of a string, list or dict data type.
    /// Behavior is undefined if the data type is an integer.
    constexpr auto size() const noexcept -> std::uint32_t
    {
        Expects(is_string() || is_list_begin() || is_dict_begin());
        return data_.structured.size;
    }

    /// Sets the size of a string, list or dict data type.
    /// Behavior is undefined if the data type is an integer.
    /// @param v the size to set
    constexpr void set_size(std::uint32_t v) noexcept
    {
        Expects(is_string() || is_list() || is_dict());
        data_.structured.size = v;
    }

    /// Returns the offset to matching begin/end token for list/dict,
    /// or the offset to the string data for strings.
    /// Behavior is undefined if the data type is an integer.
    constexpr auto offset() const noexcept -> std::uint32_t
    {
        Expects(is_string() || is_list() || is_dict());
        return data_.structured.offset;
    }

    /// Sets the offset to matching begin/end token for list/dict,
    /// or the offset to the string data for strings.
    /// Behavior is undefined if the data type is an integer.
    /// @param v the offset to set
    constexpr void set_offset(std::uint32_t v) noexcept
    {
        Expects(is_string() || is_list() || is_dict());
        data_.structured.offset = v;
    }

    /// Set this descriptor to be the last one in a sequence of descriptors.
    /// @param flag whether to enable or disable the stop flag
    constexpr void set_stop_flag(bool flag = true) noexcept
    { type_ |= descriptor_type::stop; }

    /// Compare equality
    /// @param that the descriptor to compare with
    constexpr bool operator==(const descriptor& that) const noexcept
    {
        if (type_ != that.type_) return false;
        if (position_ != that.position_) return false;
        if (is_integer()) {
            return value() == that.value();
        } else {
            return (offset() == that.offset()) && (size() == that.size());
        }
    }

protected:
    descriptor_type type_;
    std::uint32_t position_;
    descriptor_data data_;
};
}