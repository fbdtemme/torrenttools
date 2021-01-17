#include <ranges>
#include <bit>
#include <algorithm>
#include <functional>
#include <charconv>
#include <unordered_set>

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


std::unordered_set<dottorrent::hash_function> checksum_transformer(const std::vector<std::string>& s)
{
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
        auto it = rng::find(node, ':');
        if (it == node.end()) {
            throw std::invalid_argument("Invalid dht node: invalid port number");
        }
        auto port_str = std::string_view(std::next(it), node.end());
        std::uint16_t port;

        if (auto [ptr, ecc] = std::from_chars(
                    port_str.data(), port_str.data()+port_str.size(), port);
                ecc != std::errc{})         {
            throw std::invalid_argument("Invalid dht node: invalid port number");
        }

        res.emplace_back(std::string_view(node.begin(), it), port);
    }
    return res;
}

dottorrent::protocol protocol_transformer(const std::vector<std::string>& v)
{
    if (v.size() > 1) {
        throw std::invalid_argument("multiple options given.");
    }
    const auto& s  = v.at(0);

    auto it = s.begin();
    if (s == "hybrid") {
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

std::filesystem::path target_transformer(const std::vector<std::string>& v)
{
    if (v.size() != 1) {
        throw std::invalid_argument("Multiple targets given.");
    }
    auto f = std::filesystem::path(v.front());
    if (!std::filesystem::exists(f)) {
        throw std::invalid_argument("Path does not exist.");
    }
    return std::filesystem::canonical(f);
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
    auto suffix = std::string_view(ptr, (s.data()+s.size()-ptr));

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
