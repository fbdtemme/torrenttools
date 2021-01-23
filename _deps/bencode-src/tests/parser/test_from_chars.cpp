#include <catch2/catch.hpp>

#include <sstream>
#include <ranges>


#include <bencode/detail/parser/parsing_error.hpp>
#include <bencode/detail/parser/from_chars.hpp>

//#include "../../benchmark/integer_experiments/avx2.hpp"


using namespace std::string_view_literals;
using namespace std::string_literals;

using namespace bencode::detail;
namespace rng = std::ranges;

template <auto Implementation = implementation::serial, typename T>
auto valid_from_chars_helper(std::string_view src, T result)
{
    T value;
    const auto res = bencode::detail::from_chars(
            rng::data(src), rng::data(src)+ rng::size(src), value, Implementation);
    CHECK(res.ec == bencode::parsing_errc{});
    CHECK(value == result);
}

template <typename T, auto Implementation = implementation::serial>
auto invalid_from_chars_helper(std::string_view src, bencode::parsing_errc ec)
{
    T value;
    const auto res = bencode::detail::from_chars(
            rng::data(src), rng::data(src)+ rng::size(src), value, Implementation);
    CHECK(res.ec == ec);
}

TEMPLATE_TEST_CASE("test from_chars - serial", "[integer]", std::int32_t, std::int64_t)
{
    using namespace bencode;

    constexpr auto positive = "666"sv;
    constexpr auto negative = "-666"sv;
    constexpr auto zero = "0"sv;
    constexpr auto leading_zero = "000912e"sv;
    constexpr auto negative_zero = "-0"sv;
    constexpr auto empty = ""sv;
    constexpr auto minus_only = "-"sv;
    constexpr auto minus_and_invalid = "-a"sv;
    constexpr auto max_value = "9223372036854775807"sv;
    constexpr auto positive_overflow = "92233791812123120312116854775808"sv;
    constexpr auto negative_overflow = "-92233791812123120312116854775808"sv;
    constexpr auto overflow_after_conversion_to_signed = "-9223372036854775809"sv;

    SECTION("empty") {
        invalid_from_chars_helper<TestType>(empty, parsing_errc::unexpected_eof);
    }
    SECTION("positive bvalue") {
        valid_from_chars_helper(positive, TestType(666));
    }
    SECTION("negative bvalue") {
        valid_from_chars_helper(negative, TestType(-666));
    }
    SECTION("zero") {
        valid_from_chars_helper(zero, TestType(0));
    }
    SECTION("negative zero") {
        invalid_from_chars_helper<TestType>(negative_zero, parsing_errc::negative_zero);
    }
    SECTION("minus only") {
        invalid_from_chars_helper<TestType>(minus_only, parsing_errc::unexpected_eof);
    }
    SECTION("minus and invalid") {
        invalid_from_chars_helper<TestType>(minus_and_invalid, parsing_errc::expected_digit);
    }

    SECTION("leading zeros") {
        invalid_from_chars_helper<TestType>(leading_zero, parsing_errc::leading_zero);
    }
    SECTION("positive overflow") {
        invalid_from_chars_helper<TestType>(positive_overflow, parsing_errc::result_out_of_range);
    }
    SECTION("negative overflow") {
        invalid_from_chars_helper<TestType>(negative_overflow, parsing_errc::result_out_of_range);
    }
    SECTION("overflow after sign conversion") {
        invalid_from_chars_helper<TestType>(overflow_after_conversion_to_signed,
                parsing_errc::result_out_of_range);
    }
}


inline std::string operator""_padded(const char* s, std::size_t len)
{
    auto str = std::string(s, len);
    str.resize(32, '\0');
    return str;
}


TEMPLATE_TEST_CASE("test from_chars - swar", "[integer]", std::int32_t, std::int64_t)
{
    using namespace bencode;

    static auto positive = "666"_padded;
    static auto negative = "-666"_padded;
    static auto zero = "0"_padded;
    static auto leading_zero = "000912e"_padded;
    static auto negative_zero = "-0"_padded;
    static auto empty = "";
    static auto minus_only = "-";
    static auto no_digits_and_eof = '-';
    static auto minus_and_invalid = "-a"_padded;
    static auto max_value = "9223372036854775807"_padded;
    static auto positive_overflow = "92233791812123120312116854775808"_padded;
    static auto negative_overflow = "-92233791812123120312116854775808"_padded;
    static auto overflow_after_conversion_to_signed = "-9223372036854775809"_padded;

    static constexpr auto Impl = implementation::swar;

    SECTION("empty") {
        invalid_from_chars_helper<TestType, Impl>(empty, parsing_errc::unexpected_eof);
    }
    SECTION("positive bvalue") {
        valid_from_chars_helper<Impl>(positive, TestType(666));
    }
    SECTION("negative bvalue") {
        valid_from_chars_helper<Impl>(negative, TestType(-666));
    }
    SECTION("zero") {
        valid_from_chars_helper<Impl>(zero, TestType(0));
    }
    SECTION("negative zero") {
        invalid_from_chars_helper<TestType, Impl>(negative_zero, parsing_errc::negative_zero);
    }
    SECTION("minus only") {
        invalid_from_chars_helper<TestType, Impl>(minus_only, parsing_errc::unexpected_eof);
    }
    SECTION("minus and invalid") {
        invalid_from_chars_helper<TestType, Impl>(minus_and_invalid, parsing_errc::expected_digit);
    }
    SECTION("leading zeros") {
        invalid_from_chars_helper<TestType, Impl>(leading_zero, parsing_errc::leading_zero);
    }
    SECTION("positive overflow") {
        invalid_from_chars_helper<TestType, Impl>(positive_overflow, parsing_errc::result_out_of_range);
    }
    SECTION("negative overflow") {
        invalid_from_chars_helper<TestType, Impl>(negative_overflow, parsing_errc::result_out_of_range);
    }
    SECTION("overflow after sign conversion") {
        invalid_from_chars_helper<TestType, Impl>(overflow_after_conversion_to_signed,
                parsing_errc::result_out_of_range);
    }
}

inline auto valid_bdecode_integer_helper(std::string_view src, std::int64_t result)
{
    std::int64_t value;
    const auto res = bencode::detail::binteger_from_chars(
            rng::data(src), rng::data(src)+ rng::size(src), value,
            implementation::serial);
    CHECK(res.ec == bencode::parsing_errc{});
    CHECK(value == result);
}


inline auto invalid_bdecode_integer_helper(std::string_view src, bencode::parsing_errc ec)
{
    std::int64_t value;
    const auto res = bencode::detail::binteger_from_chars(
            rng::data(src), rng::data(src)+ rng::size(src), value,
            implementation::serial);
    CHECK(res.ec == ec);
}


TEST_CASE("test bdecode_int", "[integer]") {
    using namespace bencode;

    auto positive = "i666e";
    auto negative = "i-666e";
    auto missing_end_token = "i666";
    auto invalid_end_token= "i666k";
    auto invalid_integer = "i-0e";
    auto empty = "";
    auto not_integer = "li1ee";

    SECTION("positive values") {
        valid_bdecode_integer_helper(positive, 666);
    }
    SECTION("negative values") {
        valid_bdecode_integer_helper(negative, -666);
    }
    SECTION("error - missing end token") {
        invalid_bdecode_integer_helper(missing_end_token, parsing_errc::unexpected_eof);
    }
    SECTION("error - expected end token") {
        invalid_bdecode_integer_helper(invalid_end_token, parsing_errc::expected_end);
    }
    SECTION("error - negative zero") {
        invalid_bdecode_integer_helper(invalid_integer, parsing_errc::negative_zero);
    }
    SECTION("error - empty string") {
        invalid_bdecode_integer_helper(empty, parsing_errc::unexpected_eof);
    }
    SECTION("error - not an integer token") {
        invalid_bdecode_integer_helper(not_integer, parsing_errc::expected_integer_start_token);
    }
}



template <typename T>
inline auto valid_bdecode_string_helper(std::string_view src, std::string_view result)
{
    T value;
    const auto res = bencode::detail::bstring_from_chars(
            rng::data(src), rng::data(src)+ rng::size(src), value,
            implementation::serial);

    CHECK(res.ec == bencode::parsing_errc{});
    CHECK(value == result);
}


template <typename T>
inline auto invalid_bdecode_string_helper(std::string_view src, bencode::parsing_errc ec)
{
    T value;
    const auto res = bencode::detail::bstring_from_chars(
            rng::data(src), rng::data(src)+ rng::size(src), value,
            implementation::serial);

    CHECK(res.ec == ec);
}

TEMPLATE_TEST_CASE("test bdecode_string", "[string]", std::string_view)
{
    using namespace bencode;
    constexpr auto valid = "3:foo";
    constexpr auto missing_colon = "3a"sv;
    constexpr auto missing_seperator = "3f"sv;
    constexpr auto unexpected_eof0 = ""sv;
    constexpr auto unexpected_eof1 = "3:fo"sv;
    constexpr auto unexpected_eof2 = "3"sv;
    constexpr auto empty = "";
    constexpr auto not_string = "i2e";
    constexpr auto negative_string_length = "-3:foo";

    SECTION("valid") {
        valid_bdecode_string_helper<TestType>(valid, "foo");
    }
    SECTION("missing colon") {
        invalid_bdecode_string_helper<TestType>(missing_colon, parsing_errc::expected_colon);
    }
    SECTION("invalid seperator colon") {
        invalid_bdecode_string_helper<TestType>(missing_seperator, parsing_errc::expected_colon);
    }
    SECTION("unexpected eof") {
        invalid_bdecode_string_helper<TestType>(unexpected_eof0, parsing_errc::unexpected_eof);
        invalid_bdecode_string_helper<TestType>(unexpected_eof1, parsing_errc::unexpected_eof);
        invalid_bdecode_string_helper<TestType>(unexpected_eof2, parsing_errc::unexpected_eof);
    }
    SECTION("error - empty string") {
        invalid_bdecode_string_helper<TestType>(empty, parsing_errc::unexpected_eof);
    }
    SECTION("error - not a string token") {
        invalid_bdecode_string_helper<TestType>(not_string, parsing_errc::expected_digit);
    }
    SECTION("error - negative string length") {
        invalid_bdecode_string_helper<TestType>(negative_string_length, parsing_errc::negative_string_length);
    }
}
