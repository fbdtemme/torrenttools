#pragma once

#include <vector>
#include <string>
#include <filesystem>

#include <dottorrent/metafile.hpp>
#include "tracker_database.hpp"

namespace fs = std::filesystem;
namespace tt = torrenttools;

namespace torrenttools {
class config;
class tracker_database;
}

struct main_app_options
{
    fs::path config;
    fs::path tracker_db;
};

std::pair<const tt::config*, const tt::tracker_database*>
load_config_and_tracker_db(const main_app_options& main_options);

void set_trackers(dottorrent::metafile& m, const std::vector<std::vector<std::string>>& options);

void set_trackers(dottorrent::metafile& m, const std::vector<std::vector<std::string>>& options,
        const torrenttools::tracker_database* tracker_db,
        const torrenttools::config* config);

void set_tracker_group(dottorrent::metafile& m, const std::vector<std::string>& announce_group_list,
        const torrenttools::tracker_database* tracker_db,
        const torrenttools::config* config);

std::filesystem::path get_destination_path(dottorrent::metafile& m, std::optional<fs::path> destination_path);