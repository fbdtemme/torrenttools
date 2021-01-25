#pragma once
#include <cmath>
#include <string>
#include <chrono>
#include <fmt/format.h>

#include <dottorrent/general.hpp>

namespace torrenttools {

using namespace std::string_view_literals;

/// Format a std::chrono::duration to SI style human readeable durations.
/// eg 1h30min. No leading zeros or unused units.
template <typename Rep, typename Period>
std::string format_duration(std::chrono::duration<Rep, Period> duration)
{
    using double_seconds = std::chrono::duration<double>;

    auto hours = std::chrono::duration_cast<std::chrono::hours>(duration);
    auto minutes = std::chrono::duration_cast<std::chrono::minutes>(duration - hours);
    auto seconds = std::chrono::duration_cast<double_seconds>(duration - hours - minutes);

    if (hours.count() > 0) {
        return fmt::format("{}{}{}", hours, minutes, seconds);
    } else if (minutes.count() > 0) {
        return fmt::format("{}{}", minutes, seconds);
    } else {
        return fmt::format("{:.2%Q%q}", seconds);
    }
}

std::string format_size(double size, std::size_t precision = 2);

std::string format_tree_size(double size);

std::string format_percentage(double value);

enum class unit_base { decimal, binary };

std::string format_si_prefix(double value, std::string_view unit, unit_base base = unit_base::decimal);


inline auto format_hash_rate(double rate)-> std::string
{
    static constexpr auto unit = "B/s"sv;
    return format_si_prefix(rate, unit, unit_base::binary);
}


std::string format_protocol_version(dottorrent::protocol version);


} // namespace torrenttools
