#include <span>
#include <ranges>
#include <vector>
#include <stdexcept>

#include <nlohmann/json.hpp>
#include <fmt/format.h>
#include <fmt/args.h>

#include "app_data.hpp"
#include "tracker_database.hpp"

namespace torrenttools {

using namespace std::string_literals;
namespace rng = std::ranges;

tracker_database::tracker_database()
    : announces_regex_(RE2::Options{}, RE2::Anchor::ANCHOR_START)
{}


std::string tracker::substitute_parameters(const config& config) const
{
    auto first = rng::begin(announce_url);
    auto last = rng::end(announce_url);
    auto open = rng::find(announce_url, '{');

    fmt::dynamic_format_arg_store<fmt::format_context> arg_store {};

    while (open != last) {
        auto close = rng::find(open, last, '}');
        auto key = std::string(std::next(open), close);
        arg_store.push_back(
                fmt::arg(key.c_str(), config.get_announce_parameter(name, key)));
        open = rng::find(close, last, '{');
    }
    return fmt::vformat(announce_url, arg_store);
}

tracker_database::tracker_database(const std::filesystem::path& path)
        : announces_regex_(RE2::Options{}, RE2::Anchor::ANCHOR_START)
{
    namespace nm = nlohmann;

    if (!fs::exists(path)) {
        throw std::invalid_argument("path does not exist");
    }
    if (!fs::is_regular_file(path)) {
        throw std::invalid_argument("path is not a file");
    }
    std::ifstream is(path);

    nm::json j = nm::json::parse(is);
    if (!j.is_array()) {
        throw std::invalid_argument("bad trackers file");
    }

    for (auto it : j) {
        trackers_.emplace_back( tracker {
                .name =          it.at("name").get<std::string>(),
                .abbreviation =  it.at("abbreviation").get<std::string>(),
                .announce_url =  it.at("announce_url").get<std::string>(),
                .is_private   =  it.at("private").get<bool>()
        });
    }

    for (const auto& e : trackers_) {
        std::string lowercase_name = to_lower(e.name);
        std::string lowercase_abbreviation = to_lower(e.abbreviation);

        name_index_[lowercase_name] = &e;
        abbreviation_index_[lowercase_abbreviation] = &e;

        std::string err;
        auto pattern = build_announce_regex(e.announce_url);
        announces_regex_.Add(pattern, &err);

        if (!err.empty()) {
            throw std::invalid_argument("Error while creating a regex from announce_url:"s + err);
        }
    }
    announces_regex_.Compile();
}

bool tracker_database::contains(std::string_view key) const noexcept
{
    auto lkey = to_lower(key);

    if (key.size() <= 4) {
        if (auto it = find_by_abbreviation(lkey); it != end()) {
            return true;
        }
        else if (auto it = find_by_name(lkey); it != end()) {
            return true;
        }
    }
    else {
        if (auto it = find_by_name(lkey); it != end()) {
            return true;
        }
        else if (auto it = find_by_abbreviation(lkey); it != end()) {
            return true;
        }
    }
    return false;
}

tracker_database::const_iterator tracker_database::find_by_name(std::string_view name) const
{
    auto key = to_lower(name);
    if (auto it = name_index_.find(key); it != name_index_.end()) {
        auto idx = std::distance(trackers_.data(), it->second);
        return std::next(begin(), idx);
    }
    return end();
}

tracker_database::const_iterator tracker_database::find_by_abbreviation(std::string_view abbreviation) const
{
    auto key = to_lower(abbreviation);

    if (auto it = abbreviation_index_.find(key); it != abbreviation_index_.end()) {
        auto idx = std::distance(trackers_.data(), it->second);
        return std::next(begin(), idx);
    }
    return end();
}

tracker_database::const_iterator tracker_database::find_by_url(std::string_view url) const
{
    std::vector<int> mask {};
    announces_regex_.Match(url, &mask);
    if (mask.empty()) {
        return end();
    }
    return std::next(trackers_.begin(), mask.front());
}

const tracker& tracker_database::at(std::string_view key) const
{
    std::string lkey = to_lower(key);

    if (key.size() <= 4) {
        if (auto it = find_by_abbreviation(lkey); it != end()) {
            return *it;
        }
        else if (auto it = find_by_name(lkey); it != end()) {
            return *it;
        }
    }
    else {
        if (auto it = find_by_name(lkey); it != end()) {
             return *it;
        }
        else if (auto it = find_by_abbreviation(lkey); it != end()) {
            return *it;
        }
    }
    throw std::out_of_range("no such key");
}

std::string tracker_database::to_lower(std::string_view key)
{
    std::string out;
    out.reserve(key.size());
    auto to_lower = [](char c) { return static_cast<char>(std::tolower(c)); };
    rng::transform(key, std::back_inserter(out), to_lower);
    return out;
}

std::string tracker_database::build_announce_regex(std::string_view announce_url)
{
    std::string pattern {};
    for (auto it = announce_url.begin(); it != announce_url.end(); ) {
        auto bracket_open = rng::find(it, announce_url.end(), '{');

        if (bracket_open != announce_url.end()) {
            auto bracket_close = rng::find(bracket_open, announce_url.end(), '}');

            rng::copy(it, bracket_open, std::back_inserter(pattern));
            pattern.append("(.*)");
            it = bracket_close+1;
        }
        else {
            rng::copy(it, announce_url.end(), std::back_inserter(pattern));
            it = announce_url.end();
        }
    }
    return pattern;
}

tracker_database* load_tracker_database()
{

    static std::vector<fs::path> data_dirs {
        get_user_data_dir(),
        GLOBAL_DATA_DIR,
        BUILD_DATA_DIR,
    };

    if (tracker_db_ptr) {
        return tracker_db_ptr.get();
    }

    fs::path database_location;
    for (const auto& path : data_dirs) {
        std::error_code ec {};
        auto exists = fs::exists(path / tracker_db_name, ec);
        if (ec) continue;
        if (exists) {
            database_location = path / tracker_db_name;
            break;
        }
    }
    if (database_location.empty()) {
        return nullptr;
    }

    tracker_db_ptr = std::make_unique<tracker_database>(database_location);
    return tracker_db_ptr.get();
}

tracker_database* load_tracker_database(const fs::path& custom_path)
{
    fs::path database_location;

    if (!fs::exists(custom_path)) {
        throw std::runtime_error(
                fmt::format("Custom tracker database path [{}] does not exists", custom_path.string()));
    }

    if (fs::is_directory(custom_path)) {
        database_location = custom_path / tracker_db_name;
        bool exists = fs::exists(database_location);
        if (!exists) {
            throw std::runtime_error(
                    fmt::format("Custom tracker database path [{}] does not contain \"{}\" file",
                            custom_path.string(), tracker_db_name));
        }
    }
    else {
        database_location = custom_path;
    }

    tracker_db_ptr = std::make_unique<tracker_database>(database_location);
    return tracker_db_ptr.get();
}

tracker_database::const_iterator tracker_database::begin() const noexcept
{
    return trackers_.begin();
}

tracker_database::const_iterator tracker_database::end() const noexcept
{
    return trackers_.end();
}

tracker_database::const_iterator tracker_database::cbegin() const noexcept
{
    return trackers_.cbegin();
}

tracker_database::const_iterator tracker_database::cend() const noexcept
{
    return trackers_.cend();
}

tracker_database::const_reverse_iterator tracker_database::rbegin() const noexcept
{
    return trackers_.rbegin();
}

tracker_database::const_reverse_iterator tracker_database::rend() const noexcept
{
    return trackers_.rend();
}

tracker_database::const_reverse_iterator tracker_database::crbegin() const noexcept
{
    return trackers_.crbegin();
}

tracker_database::const_reverse_iterator tracker_database::crend() const noexcept
{
    return trackers_.crend();
}


}
