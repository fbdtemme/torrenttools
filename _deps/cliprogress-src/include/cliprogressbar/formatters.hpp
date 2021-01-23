#pragma once
#include <array>
#include <ranges>
#include <string_view>
#include <fmt/format.h>

namespace cliprogress {

namespace rng = std::ranges;
using namespace std::string_view_literals;

static constexpr std::array<const char*, 16> decimal_prefix_table = {
        "m", "µ", "n", "p", "f", "a", "z", "y",
        "",
        "k", "M", "G", "T", "P", "E", "Z"
};

static constexpr std::array<const char*, 8> binary_prefix_table = {
        "", "Ki", "Mi", "Gi", "Ti", "Pi", "Ei", "Zi"
};

///// Format a unit with SI metrix prefixes
//template <typename OutputIt>
//inline OutputIt format_metric_unit_to(
//        OutputIt out, double value, std::string_view unit, std::size_t precision = 2)
//{
//    auto& prefix_table = decimal_prefix_table;
//    const std::size_t prefix_value = 1000;
//    std::size_t table_base = 8;
//    static constexpr std::string_view size_template = "{:.{}f} ";
//    std::string_view prefix = {};
//    std::size_t value_width = 0;
//
//    if (value > 0) {
//        for (std::size_t i = 0; i < 7; ++i) {
//            prefix = prefix_table[table_base+i];
//            if (value < prefix_value) {
//                value_width = (value < 100) ? ((value < 10) ? 1 : 2) : 3;
//                break;
//            }
//            value /= prefix_value;
//        }
//    }
//    else {
//        for (std::size_t i = 0; i < 7; ++i) {
//            prefix = prefix_table[table_base-i];
//            if (value > 0) {
//                value_width = (value < 100) ? ((value < 10) ? 1 : 2) : 3;
//                break;
//            }
//            value *= prefix_value;
//        }
//    }
//
//    double dec;
//    double fract = std::modf(value, &dec);
//
//    if (std::abs(fract - 0) < 10e-3) {
//        fmt::format_to(out, size_template, value, 0);
//    } else {
//        fmt::format_to(out, size_template, value, precision);
//    }
//    rng::copy(prefix, out);
//    rng::copy(unit, out);
//    return out;
//}
//
//inline auto format_metric_unit(
//        double value,
//        std::string_view unit,
//        std::size_t width = 2) -> std::string
//{
//    std::string s {};
//    format_metric_unit_to(std::back_inserter(s), value, unit, width);
//    return s;
//};



template <typename OutputIt>
inline OutputIt format_binary_unit_to(
        OutputIt out, double value, std::string_view unit, std::size_t width = 4)
{
    auto& prefix_table = binary_prefix_table;
    const std::size_t prefix_value = 1024;
    std::size_t table_base = 0;

    static constexpr std::string_view size_template = "{:>{}.{}f} ";
    std::string_view prefix = {};

    double dec;
    double fract = std::modf(value, &dec);

    if (dec > 0) {
        for (std::size_t i = 0; i < 7; ++i) {
            prefix = prefix_table[table_base+i];
            if (value < prefix_value) break;
            value /= prefix_value;
        }
    }
    else {
        for (std::size_t i = 0; i < 7; ++i) {
            prefix = prefix_table[table_base+i];
            if (value < prefix_value) break;
            value *= prefix_value;
        }
    }

    fract = std::modf(value, &dec);

    int digits_width = value < 1000 ? (value < 100 ? (value < 10 ? 1 : 2) : 3) : 4;

    fmt::format_to(out, size_template, value, width, std::max(0, int(width)-digits_width-1));
    if (!prefix.empty()) {
        std::copy(prefix.begin(), prefix.end(), out);
    } else {
        // two blank spaces to align with value with prefixes in tables
        *out++ = ' ';
        *out++ = ' ';
    }
    std::copy(unit.begin(), unit.end(), out);
    return out;
}

inline std::string format_binary_unit(double value, std::string_view unit, std::size_t width = 4)
{
    std::string s {};
    format_binary_unit_to(std::back_inserter(s), value, unit, width);
    return s;
};

}