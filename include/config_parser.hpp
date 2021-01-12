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
    std::string_view get_tracker_parameter(
            std::string_view tracker,
            std::string_view parameter) const;

private:
    friend config* load_config();

    std::map<std::string, tracker_parameter_map, std::less<>> announce_parameters_;
};

config* load_config();


} // namespace torrenttools