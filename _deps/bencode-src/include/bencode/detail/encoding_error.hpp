#pragma once
#include <stdexcept>
#include <system_error>

#include "bencode/detail/exception.hpp"

namespace bencode {

enum class encoding_errc : std::uint8_t
{
    no_error = 0,
    invalid_dict_key,            //< dict key is not a string type.
    unexpected_end_dict,         //< received dict_end before completing a key-bvalue pair.
    unexpected_end_list,         //< list end before a list value is completed.
};

/// Returns a description of the `ec` bvalue.
constexpr std::string_view to_string(const encoding_errc& ec)
{
    switch (ec) {
    case encoding_errc::invalid_dict_key:
        return "invalid dict key";
    case encoding_errc::unexpected_end_dict:
        return "invalid dict end";
    case encoding_errc::unexpected_end_list:
        return "invalid list end";
    default:
        return "unknown error";
    }
}

struct encoding_category : std::error_category
{
    const char* name() const noexcept override
    {
        return "encoding error";
    }

    std::string message(int ev) const override
    {
        return std::string(to_string(static_cast<bencode::encoding_errc>(ev)));
    }
};


inline std::error_code make_error_code(encoding_errc e)
{
    return {static_cast<int>(e), bencode::encoding_category()};
}

} // namespace bencode

namespace std {
template <> struct is_error_code_enum<bencode::encoding_category> : std::true_type{};
}


namespace bencode {

/// Error thrown when encoding invalid data with encoding_ostream.
class encoding_error : exception {
public:
    using exception::exception;

    explicit encoding_error(encoding_errc ec)
            :exception(std::string(to_string(ec)))
            , errc_(ec) {};

    encoding_errc errc() const noexcept { return errc_; }

private:
    encoding_errc errc_;
};

}