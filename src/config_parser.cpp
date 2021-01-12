
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
            throw std::invalid_argument("not a dict");
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
}

std::string_view config::get_tracker_parameter(
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




config* load_config()
{
    static std::unique_ptr<config> config_ptr;
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
        auto exists = fs::exists(path/"config.yaml", ec);
        if (ec) continue;
        if (exists) {
            config_location = path/"config.yaml";
            break;
        }
    }
    if (config_location.empty()) {
        throw std::invalid_argument("could not find config.yaml file");
    }

    config_ptr = std::make_unique<config>(config_location);
    return config_ptr.get();
}
} // namespace torrenttools