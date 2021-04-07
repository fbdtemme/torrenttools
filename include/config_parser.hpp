#pragma once

#include <filesystem>
#include <string_view>
#include <string>
#include <memory>
#include <map>

#include "config.hpp"

namespace torrenttools {

namespace fs = std::filesystem;


class config
{
public:
    using tracker_parameter_map = std::map<std::string, std::string, std::less<>>;

    explicit config(const fs::path& path);

    /// Return the value in the config for a tracker.
    /// @throws std::out_of_range when no key for tracker and or parameter is found.
    std::string_view get_announce_parameter(
            std::string_view tracker,
            std::string_view parameter) const;

    /// Return the trackers from a tracker group.
    /// @throws std::out_of_range when no group by given name is found.
    const std::vector<std::string>& get_announce_group(std::string_view group_name) const;

private:
    friend config* load_config();

    std::map<std::string, tracker_parameter_map, std::less<>> announce_parameters_;
    std::map<std::string, std::vector<std::string>, std::less<>> announce_groups_;
};


inline std::unique_ptr<config> config_ptr = nullptr;
inline std::string_view config_name = "config.yml";

/// Load the config file from the system directories
config* load_config();

config* load_config(const fs::path& custom_path);


} // namespace torrenttools