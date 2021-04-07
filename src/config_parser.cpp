
#include <fstream>
#include <string>
#include <string_view>
#include <filesystem>
#include <stdexcept>

#include <fmt/format.h>
#include <yaml-cpp/yaml.h>

#include "app_data.hpp"
#include "config_parser.hpp"

namespace torrenttools {


config::config(const fs::path& path)
{
    if (!fs::exists(path)) {
        throw std::invalid_argument("path does not exist");
    }
    auto ifs = std::ifstream(path);

    YAML::Node config = YAML::Load(ifs);

    // parse tracker-parameters

    auto tracker_params_node = config["tracker-parameters"];
    for (const auto& tracker_node : tracker_params_node) {
        if (!tracker_node.second.IsMap()) {
            throw std::invalid_argument("tracker parameters must be a dict");
        }
        tracker_parameter_map map {};
        auto tracker_name = tracker_node.first.as<std::string>();

        for (const auto& param_node : tracker_node.second) {
            map.insert_or_assign(
                    param_node.first.as<std::string>(),
                    param_node.second.as<std::string>());
        }
        announce_parameters_.try_emplace(tracker_name, std::move(map));
    }

    auto tracker_group_node = config["tracker-groups"];

    for (const auto& group_node : tracker_group_node) {
        if (!group_node.second.IsSequence()) {
            throw std::invalid_argument("Tracker group must be a list");
        }
        auto group_name = group_node.first.as<std::string>();
        std::vector<std::string> group_announces = {};

        for (const auto& tracker : group_node.second) {
            group_announces.push_back(tracker.as<std::string>());
        }

        announce_groups_.try_emplace(std::move(group_name), std::move(group_announces));
    }

    // Verify uniqueness of tracker group ids
    std::vector<std::string> group_names {};
    std::transform(announce_groups_.begin(), announce_groups_.end(), std::back_inserter(group_names),
                  [](const auto& pair) { return pair.first; });
    auto it = std::adjacent_find(group_names.begin(), group_names.end());
    if (it != group_names.end()) {
        std::invalid_argument(fmt::format("Duplicate tracker group key found: {}", *it));
    }
}

std::string_view config::get_announce_parameter(
        std::string_view tracker,
        std::string_view parameter) const
{
    auto it = announce_parameters_.find(tracker);
    if (it == announce_parameters_.end()) {
        throw std::out_of_range(fmt::format(R"(No parameters found for tracker: "{}")", tracker));
    }
    const auto& tracker_params = it->second;
    auto it2 = tracker_params.find(parameter);
    if (it2 == tracker_params.end()) {
        throw std::out_of_range(
                fmt::format(R"(No parameter "{}" for tracker: "{}")", parameter, tracker));
    }

    return it2->second;
}


/// Return the trackers from a tracker group.
/// @throws std::out_of_range when no group by given name is found.
const std::vector<std::string>& config::get_announce_group(std::string_view group_name) const
{
    auto it = announce_groups_.find(group_name);
    if (it == announce_groups_.end()) {
        throw std::out_of_range(fmt::format(R"(No tracker group found with name: "{}")", group_name));
    }
    return it->second;
}

config* load_config()
{
    std::vector<fs::path> data_dirs {
        get_user_data_dir(),
        GLOBAL_DATA_DIR,
        BUILD_DATA_DIR,
    };

    if (config_ptr) {
        return config_ptr.get();
    }

    fs::path config_location;
    for (const auto& path : data_dirs) {
        std::error_code ec;
        auto exists = fs::exists(path / config_name, ec);
        if (ec) continue;
        if (exists) {
            config_location = path/config_name;
            break;
        }
    }
    if (config_location.empty()) {
        return nullptr;
    }

    config_ptr = std::make_unique<config>(config_location);
    return config_ptr.get();
}

config* load_config(const fs::path& custom_path)
{
    fs::path config_location;

    if (!fs::exists(custom_path)) {
        throw std::runtime_error(
                fmt::format("Custom config path [{}] does not exists", custom_path.string()));
    }

    if (fs::is_directory(custom_path)) {
        config_location = custom_path / config_name;
        bool exists = fs::exists(config_location);
        if (!exists) {
            throw std::runtime_error(
                    fmt::format("Custom config path [{}] does not contain \"{}\" file",
                                custom_path.string(), config_name));
        }
    }
    else {
        config_location = custom_path;
    }

    config_ptr = std::make_unique<config>(config_location);
    return config_ptr.get();
}


} // namespace torrenttools