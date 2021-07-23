#pragma once

#include <filesystem>
#include <string_view>
#include <string>
#include <memory>
#include <map>
#include <variant>

#include <CLI/ConfigFwd.hpp>
#include "config.hpp"
#include "profile.hpp"

namespace YAML { class Node; }

namespace torrenttools {

namespace fs = std::filesystem;

class config
{
public:
    using tracker_parameter_map = std::map<std::string, std::string, std::less<>>;

    explicit config(const fs::path& path);

    explicit config(const std::string& body);

    /// Return the value in the config for a tracker.
    /// @throws std::out_of_range when no key for tracker and or parameter is found.
    std::string_view get_announce_parameter(
            std::string_view tracker,
            std::string_view parameter) const;

    /// Return the trackers from a tracker group.
    /// @throws std::out_of_range when no group by given name is found.
    const std::vector<std::string>& get_announce_group(std::string_view group_name) const;

    const profile& get_profile(std::string_view profile_name) const;

private:
    friend config* load_config();

    void parse_tracker_parameters(const YAML::Node& data);
    void parse_tracker_groups(const YAML::Node& data);
    void parse_profiles(const YAML::Node& data);

    std::map<std::string, tracker_parameter_map, std::less<>> announce_parameters_;
    std::map<std::string, std::vector<std::string>, std::less<>> announce_groups_;
    std::map<std::string, profile> profiles_;
};


inline std::unique_ptr<config> config_ptr = nullptr;
inline std::string_view config_name = "config.yml";

/// Load the config file from the system directories
config* load_config();

config* load_config(const fs::path& custom_path);


} // namespace torrenttools