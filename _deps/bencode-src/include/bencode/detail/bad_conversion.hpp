#pragma once
#include <string>
#include <exception>
#include <stdexcept>
#include <string_view>
#include "exception.hpp"

namespace bencode {

using namespace std::string_literals;

/// Error code enumeration for type conversion errors.
enum class conversion_errc : std::uint8_t
{
    /// The active alternative is not an integral type.
    not_integer_type,
    /// The active alternative is not a string type.
    not_string_type,
    /// The active alternative is not a list type.
    not_list_type,
    /// The active alternative is not a dict type.
    not_dict_type,
    /// Conversion to a fixed size type and the size of the active alternative does not match.
    size_mismatch,
    /// Unspecified exception was thrown during construction of the requested type.
    construction_error,
    /// The mapped type of a dict cannot be converted to the requested type.
    dict_mapped_type_construction_error,
    /// The bvalue type of a list cannot be converted to the requested type.
    list_value_type_construction_error,
    /// The conversion to the given type is not defined.
    undefined_conversion
};

/// Returns a description of the `ec` bvalue.
constexpr std::string_view to_string(const conversion_errc& ec)
{
    switch (ec) {
    case conversion_errc::not_integer_type:
        return "bvalue not of integer type";
    case conversion_errc::not_string_type:
        return "bvalue not of string type";
    case conversion_errc::not_list_type:
        return "bvalue not of list type";
    case conversion_errc::not_dict_type:
        return "bvalue not of dict type";
    case conversion_errc::size_mismatch:
        return "size mismatch between bvalue and destination type";
    case conversion_errc::list_value_type_construction_error:
        return "exception thrown during construction of list bvalue type.";
    case conversion_errc::dict_mapped_type_construction_error:
        return "exception thrown during construction of dict mapped type.";
    case conversion_errc::construction_error:
        return "exception thrown during construction of destination type.";
    default:
        return "unknown error";
    }
}


struct conversion_category : std::error_category
{
    const char* name() const noexcept override
    {
        return "conversion error";
    }

    std::string message(int ev) const override
    {
        return std::string(to_string(static_cast<bencode::conversion_errc>(ev)));
    }
};

inline std::error_code make_error_code(conversion_errc e)
{
    return {static_cast<int>(e), bencode::conversion_category()};
}

} // namespace bencode

namespace std {
template <> struct is_error_code_enum<bencode::conversion_errc> : std::true_type{};
}

namespace bencode {

/// Error thrown when trying to convert a bvalue or bview to a type that does
/// not match the type of the current alternative.
class bad_conversion : public exception
{
public:
    explicit bad_conversion(const conversion_errc& ec)
        : errc_(ec)
    {};

    /// Returns the explanatory string.
    const char * what() const noexcept override
    { return to_string(errc_).data(); }

    /// Return the error code enumeration matching this exception.
    conversion_errc errc() const noexcept
    { return errc_; }

private:
    conversion_errc errc_;
};


} // namespace bencode
