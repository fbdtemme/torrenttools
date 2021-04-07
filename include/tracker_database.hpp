#pragma once
#include <filesystem>

#include <vector>
#include <string>
#include <fstream>
#include <ranges>

#include <fmt/format.h>
#include <re2/set.h>
#include <nlohmann/json.hpp>

#include "app_data.hpp"
#include "config.hpp"
#include "config_parser.hpp"

namespace torrenttools
{
namespace fs = std::filesystem;
namespace rng = std::ranges;

struct tracker
{
    std::string name;
    std::string abbreviation;
    std::string announce_url;
    bool is_private;

    /// Return announce url with paramters inserted from config file.
    std::string substitute_parameters(const config& config) const;
};


class tracker_database
{
public:
    using tracker_storage      = std::vector<tracker>;
    using abbreviation_storage = std::unordered_map<std::string_view, const tracker*>;
    using name_storage         = std::unordered_map<std::string_view, const tracker*>;

    using value_type = tracker;
    using const_reference = const tracker&;
    using const_iterator  = typename tracker_storage::const_iterator;
    using const_reverse_iterator = typename tracker_storage::const_reverse_iterator;

public:
    tracker_database();

    explicit tracker_database(const std::filesystem::path& path);

    const_iterator find_by_name(std::string_view name) const;
    const_iterator find_by_abbreviation(std::string_view abbreviation) const;
    const_iterator find_by_url(std::string_view url) const;

    /// Lookup a tracker by name or abbreviation.
    /// @throws std::out_of_range when no matching tracker is found.
    const_reference at(std::string_view key) const;
    bool contains(std::string_view key) const noexcept;

    // iterators

    const_iterator begin() const noexcept;
    const_iterator end() const noexcept;
    const_iterator cbegin() const noexcept;
    const_iterator cend() const noexcept;
    const_reverse_iterator rbegin() const noexcept;
    const_reverse_iterator rend() const noexcept;
    const_reverse_iterator crbegin() const noexcept;
    const_reverse_iterator crend() const noexcept;

private:
    static std::string to_lower(std::string_view);
    static std::string build_announce_regex(std::string_view);

    friend tracker_database* load_tracker_database();

    std::vector<tracker> trackers_;
    std::unordered_map<std::string, const tracker*> name_index_;
    std::unordered_map<std::string, const tracker*> abbreviation_index_;

    RE2::Set announces_regex_ ;
};

inline std::unique_ptr<tracker_database> tracker_db_ptr = nullptr;
inline std::string_view tracker_db_name = "trackers.json";

tracker_database* load_tracker_database();

tracker_database* load_tracker_database(const fs::path& custom_path);

}