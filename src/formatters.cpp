#include "formatters.hpp"

namespace torrenttools {


std::string format_si_prefix(double value, std::string_view unit, unit_base base)
{
    static constexpr std::array binary_prefix_table = {
            ""sv, "Ki"sv, "Mi"sv, "Gi"sv, "Ti"sv, "Pi"sv, "Ei"sv, "Zi"sv};
    static constexpr std::array decimal_prefix_table = {
            ""sv, "k"sv, "M"sv, "G"sv, "T"sv, "P"sv, "E"sv, "Z"sv};

    auto& prefix_table = (base == unit_base::decimal) ?
                         decimal_prefix_table : binary_prefix_table;
    std::size_t prefix_value = (base == unit_base::decimal) ? 1000 : 1024;
    static constexpr auto size_template = "{:{}.{}f} {}{}";
    std::string_view prefix {};
    int width = 3;

    for (std::size_t i = 0; i < prefix_table.size()-1; ++i) {
        prefix = prefix_table[i];

        if (value < prefix_value) {
            width = (value < 100) ? ((value < 10) ? 1 : 2) : 3;
            break;
        }
        value /= prefix_value;
    }
    return fmt::format(size_template, value, width, 3-width, prefix, unit);
}


std::string format_tree_size(double size)
{
    /// use 6 characters
    using namespace std::string_view_literals;
    static constexpr std::array prefix_table = {
            ""sv, "Ki"sv, "Mi"sv, "Gi"sv, "Ti"sv, "Pi"sv, "Ei"sv, "Zi"sv};

    auto it = prefix_table.begin();
    for ( ; it < prefix_table.end()-1; size /= 1024, ++it) {
        if (size < 1024) break;
    }

    std::size_t precision = size > 1000 ? 0 : (size > 100 ? 0 : (size > 10 ? 1 : 2));
    return fmt::format("{:>4.{}f} {:>2}B", size, precision, *it);
}


std::string format_size(double size, std::size_t precision)
{
    using namespace std::string_view_literals;
    static constexpr std::array prefix_table = {
            ""sv, "Ki"sv, "Mi"sv, "Gi"sv, "Ti"sv, "Pi"sv, "Ei"sv, "Zi"sv};
    static constexpr auto size_template = "{:.{}f} {}B";

    for (std::size_t i = 0; i < prefix_table.size()-1; ++i) {
        const auto prefix = prefix_table[i];

        if (size < 1024) {
            double dec;
            double fract = std::modf(size, &dec);

            if (std::abs(fract - 0) < 10e-3) {
                return fmt::format(size_template, size, 0, prefix);
            }
            return fmt::format(size_template, size, precision, prefix);
        }
        size /= 1024;
    }
    return fmt::format(size_template, size, precision, prefix_table.back());
}


std::string format_percentage(double value)
{
    return fmt::format("{:>3.0f}%", value);
}


std::string format_protocol_version(dottorrent::protocol version)
{
    if ((version & dottorrent::protocol::hybrid) == dottorrent::protocol::hybrid) {
        return "v1 + v2 (hybrid)";
    }

    if ((version & dottorrent::protocol::v1) == dottorrent::protocol::v1) {
        return "v1";
    }

    if ((version & dottorrent::protocol::v2) == dottorrent::protocol::v2) {
        return "v2";
    }

    throw std::invalid_argument("invalid bittorrent protocol version");
}

}