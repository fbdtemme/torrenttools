#include <ranges>
#include <bit>
#include <algorithm>
#include <functional>
#include <charconv>
#include <unordered_set>
#include <chrono>
#include <date/date.h>

#include "dottorrent/serialization/path.hpp"

#include <CLI/CLI.hpp>
#include <CLI/Error.hpp>

#include <fmt/format.h>
#include <fmt/chrono.h>

#include "argument_parsers.hpp"

namespace rng = std::ranges;
namespace dt = dottorrent;


using namespace dottorrent::literals;
static std::string err_msg = "Invalid value {} for option {}: {}.";

// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}


std::unordered_set<dottorrent::hash_function> checksum_transformer(const std::vector<std::string>& s)
{
    // use a set to filter out duplicates
    std::unordered_set<dt::hash_function> res {};

    for (const auto& name : s) {
        if (auto f = dt::make_hash_function(name); f) {
            res.insert(f.value());
        }
        else {
            throw std::invalid_argument(
                    fmt::format("Invalid checksum algorithm: {}", name));
        }
    }
    return res;
}


std::optional<std::size_t> piece_size_transformer(const std::vector<std::string>& v)
{
    if (v.size() > 1)
        throw std::invalid_argument("Multiple values not supported.");

    auto res = parse_commandline_size("piece size", v.at(0));
    return res;
}


std::optional<std::size_t> io_block_size_transformer(const std::vector<std::string>& v)
{
    if (v.size() > 1)
        throw std::invalid_argument("Multiple values not supported.");

    auto res = parse_commandline_size("io block size", v.at(0));
    return res;
}

std::vector<std::vector<std::string>> announce_transformer(const std::vector<std::string>& args)
{
    std::vector<std::vector<std::string>> res {};
    bool in_tier = false;

    for (auto& v: args) {
        if (v.starts_with('[')) {
            // nested tiers are invalid
            if (in_tier) {
                throw std::invalid_argument("nested tiers are not allowed");
            }

            in_tier = true;
            auto& r = res.emplace_back();
            r.push_back(v.substr(1));
        }
        else if (v.ends_with(']')) {
            if (!in_tier) {
                throw std::invalid_argument("unmatched tier bracket");
            }
            res.back().push_back(v.substr(0, v.size()-1));
            in_tier = false;
        }
        else {
            if (in_tier) {
                res.back().push_back(v);
            } else {
                res.emplace_back().push_back(v);
            }
        }
    }
    return res;
}


/// Parse a string with dht nodes to dht_node instances.
std::vector<dottorrent::dht_node> dht_node_transformer(const std::vector<std::string>& s)
{
    std::vector<dottorrent::dht_node> res {};

    for (const auto& node : s) {
        auto pos = node.find_last_of(':');
        if (pos == node.size()) {
            throw std::invalid_argument("Invalid dht node: no port number seperator ':'");
        }
        auto port_str = std::string_view(node.begin()+pos+1, node.end());
        std::uint16_t port;

        if (auto [ptr, ecc] = std::from_chars(
                    port_str.data(), port_str.data()+port_str.size(), port);
                ecc != std::errc{})         {
            throw std::invalid_argument("Invalid dht node: invalid port number");
        }

        res.emplace_back(std::string_view(node.data(), pos), port);
    }
    return res;
}

dottorrent::protocol protocol_transformer(const std::vector<std::string>& v, bool allow_hybrid)
{
    if (v.size() > 1) {
        throw std::invalid_argument("multiple options given.");
    }
    const auto& s  = v.at(0);

    auto it = s.begin();
    if (s == "hybrid") {
        if (!allow_hybrid) {
            throw std::invalid_argument("Hybrid protocol is not allowed for this option");
        }
        return dottorrent::protocol::hybrid;
    }

    if (*it == 'v') {
        ++it;
    }
    if (*it == '1')
        return dottorrent::protocol::v1;
    else if (*it == '2')
        return dottorrent::protocol::v2;

    throw std::invalid_argument(fmt::format("Invalid bittorrent protocol: {}", s));
}

std::filesystem::path target_transformer(const std::vector<std::string>& v, bool check_exists, bool keep_trailing)
{
    if (v.size() != 1) {
        throw std::invalid_argument("Multiple targets given.");
    }
    if (v.front() == "-") {
        return "-";
    }

    auto f = std::filesystem::path(v.front());
    if (check_exists && !std::filesystem::exists(f)) {
        throw std::invalid_argument(fmt::format("Path does not exist: {}", f.string()));
    }

    if (check_exists) {
        return std::filesystem::canonical(f);
    }
    else {
        bool has_trailing_dir_seperator = f.filename().empty();
        bool is_directory = std::filesystem::is_directory(f);
        auto canonical_f = std::filesystem::weakly_canonical(f);

        if (keep_trailing && (has_trailing_dir_seperator || is_directory)) {
            return canonical_f / "";
        }
        return canonical_f;
    }
}


/// Parse the name of a target metafile.
std::filesystem::path metafile_transformer(const std::vector<std::string>& v)
{
    if (v.size() != 1) {
        throw std::invalid_argument("Multiple targets given.");
    }

    if (v.front() == "-") {
        return "-";
    }

    auto metafile = std::filesystem::path(v.front());

    if (!std::filesystem::exists(metafile))
        throw std::invalid_argument(
                fmt::format("Metafile not found: {}", metafile.string()));

    if (std::filesystem::is_directory(metafile))
        throw std::invalid_argument(
                fmt::format("Target is a directory, not a metafile: {}", metafile.string()));

    return std::filesystem::canonical(metafile);
}

std::optional<std::size_t> parse_commandline_size(std::string_view option, const std::string& v)
{
    std::string s {};
    rng::transform(v, std::back_inserter(s), [](const char c) { return std::tolower(c); });

    if (s == "auto")
        return std::nullopt;

    std::size_t value;
    auto [ptr, ec] = std::from_chars(s.data(), s.data()+s.size(), value);

    if (ec != std::errc{}) {
        throw CLI::ConversionError(fmt::format(err_msg, value, option, "expected integer"));
    }
    auto suffix = std::string(ptr, (s.data()+s.size()-ptr));
    trim(suffix);

    if (suffix == "k" || suffix == "ki" || suffix == "kib") {
        value *= 1024;
    }
    else if (suffix == "m" || suffix == "mi" || suffix == "mib") {
        value *= 1024 * 1024;
    }

    if (suffix.empty()) {
        // allow the specify the size as the exponent of a power of two
        if (value < 16384) {
            value = pow(2, value);
        }
    }
    if (!std::has_single_bit(value))
        throw CLI::ConversionError(
                fmt::format(err_msg, value, option, "not a power of 2"));
    if (value < 16_KiB) {
        throw std::invalid_argument(
                fmt::format(err_msg, value, option, "must be larger or equal to 16 KiB"));
    }
    return value;
}

std::vector<std::optional<bool>> parse_commandline_booleans(std::string_view option, const std::vector<std::string>& v)
{
    std::vector<std::optional<bool>> result;

    if (v.empty())
        return {std::nullopt};

    for (const auto& value : v) {
        std::string lower_case_value;
        rng::transform(value, std::back_inserter(lower_case_value), [](const char c) { return std::tolower(c); });
        trim(lower_case_value);

        if (value == "on" || value == "true" || value == "1") {
            result.emplace_back(true);
        }
        else if (value == "off" || value == "false" || value == "0") {
            result.emplace_back(false);
        }
        else {
            result.emplace_back(std::nullopt);
        }
    }
    return result;
}

std::optional<bool> parse_commandline_bool(std::string_view option, const std::vector<std::string>& v)
{
    auto flags = parse_commandline_booleans(option, v);
    if (v.size() > 1) {
        throw std::invalid_argument(fmt::format("error parsing {}: multiple values given", option));
    }
    if (v.empty()) {
        throw std::invalid_argument(fmt::format("error parsing {}: no value given", option));
    }
    return flags.at(0);
}

// SOURCE: https://stackoverflow.com/questions/33421450/c-c-time-zone-correct-time-conversion-to-seconds-since-epoch/33438989#33438989
std::chrono::minutes parse_offset(std::istream& in)
{
    using namespace std::chrono;
    char c;
    in >> c;
    minutes result = 10*hours{c - '0'};
    in >> c;
    result += hours{c - '0'};
    in >> c;
    result += 10*minutes{c - '0'};
    in >> c;
    result += minutes{c - '0'};
    return result;
}


std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>
parse_utc_datetime(const std::string& v)
{
    using second_point = std::chrono::time_point<std::chrono::system_clock,
            std::chrono::seconds>;

    std::istringstream in(v);
    in.exceptions(std::ios::failbit | std::ios::badbit);
    int yi, mi, di;
    char dash;
    // check dash if you're picky
    in >> yi >> dash >> mi >> dash >> di;
    using namespace date;
    auto ymd = year{yi}/mi/di;

    // check ymd.ok() if you're picky
    if (!ymd.ok()) {
        throw std::invalid_argument("invalid year-month-date");
    }

    char T;
    in >> T;

    if (T != T) {
        throw std::invalid_argument("expected 'T' after year-month-day");
    }
    // check T if you're picky
    int hi, si;
    char colon;
    in >> hi >> colon >> mi >> colon >> si;

    // check colon if you're picky
    if (colon != ':') {
        throw std::invalid_argument("expected colon after hours-minutes-seconds");
    }
    using namespace std::chrono;
    auto h = hours{hi};
    auto m = minutes{mi};
    auto s = seconds{si};
    second_point result = date::sys_days{ymd} + h + m + s;
    char f;
    in >> f;
    if (f == '+')
        result -= parse_offset(in);
    else if (f == '-')
        result += parse_offset(in);
    else {
        if (f != 'Z') {
            throw std::invalid_argument("expected Z at the end of the input");
        }
    }
    return result;
}

std::chrono::system_clock::time_point
creation_date_transformer(std::string_view option, const std::vector<std::string>& v)
{
    using second_point = std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>;

    if (v.empty())
        throw std::invalid_argument("expected argument");

    if (v.size() != 1)
        throw std::invalid_argument("multiple options given.");

    auto value = v.at(0);
    std::uint64_t time_since_epoch;

    // first try to parse as an integer
    auto res = std::from_chars(value.data(), value.data()+value.size(), time_since_epoch);
    if (res.ec != std::errc{}) {
        auto msg = std::make_error_code(res.ec).message();
        throw std::invalid_argument(fmt::format("error parsing {}: {}", option, msg));
    }

    if (res.ptr == value.data()+value.size())
        return std::chrono::time_point<std::chrono::system_clock>(
                std::chrono::seconds(time_since_epoch));

    auto utc_time = parse_utc_datetime(v.at(0));
    // cast to normal system_clock duration
    return std::chrono::system_clock::time_point(utc_time);
}

torrenttools::list_edit_mode parse_list_edit_mode(std::string_view options, const std::vector<std::string>& v)
{
    using namespace torrenttools;

    if (v.empty())
        throw std::invalid_argument("expected argument");

    if (v.size() != 1)
        throw std::invalid_argument("multiple options given.");

    auto value = v.at(0);
    rng::transform(value, std::back_inserter(value), [](const char c) { return std::tolower(c); });

    if (value == "a" | value == "append") {
        return list_edit_mode::append;
    }
    else if (value == "p" | value == "prepend") {
        return list_edit_mode::prepend;
    }
    else if (value == "r" | value == "replace") {
        return list_edit_mode::replace;
    }
    else {
        throw std::invalid_argument("Invalid list edit mode.");
    }
}


bool parse_explicit_flag(std::string_view option, const std::vector<std::string>& v)
{
    auto flag = parse_commandline_bool(option, v);
    if (flag.has_value()) {
        return *flag;
    }
    else {
        // set implicit private for flag like behavior.
        return true;
    }
}