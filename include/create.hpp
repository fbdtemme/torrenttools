#pragma once
#include <string>
#include <vector>
#include <optional>
#include <cstdint>
#include <filesystem>
#include <chrono>

#include <dottorrent/metafile.hpp>
#include <dottorrent/storage_hasher.hpp>
#include <dottorrent/dht_node.hpp>
#include <dottorrent/hash.hpp>
#include <dottorrent/info_hash.hpp>

#include "config.hpp"
#include "tracker_database.hpp"
#include "info.hpp"

namespace {
namespace fs = std::filesystem;
namespace dt = dottorrent;
}

// forward declarations
namespace CLI { class App; }
namespace torrenttools { class file_matcher; }

struct create_app_options
{
    std::filesystem::path target;
    std::optional<std::filesystem::path> destination;
    bool write_to_stdout;
    bool read_from_stdin;
    dottorrent::protocol protocol_version = dt::protocol::v1;
    std::optional<std::size_t> piece_size;
    std::unordered_set<dottorrent::hash_function> checksums;
    std::vector<std::vector<std::string>> announce_list;
    std::vector<std::string> announce_group_list;
    std::vector<std::string> http_seeds;
    std::vector<std::string> web_seeds;
    std::vector<dottorrent::dht_node> dht_nodes;
    std::vector<std::string> include_patterns;
    std::vector<std::string> exclude_patterns;
    bool include_hidden_files;
    std::optional<std::string> comment;
    std::optional<std::string> source;
    std::optional<std::string> name;
    std::vector<std::string> collections;
    std::vector<dottorrent::info_hash> similar_torrents;
    std::optional<bool> is_private;
    bool set_created_by = true;
    std::optional<std::string> created_by;
    bool set_creation_date = true;
    std::optional<std::chrono::system_clock::time_point> creation_date;
    std::uint8_t threads = 1;
    std::optional<std::size_t> io_block_size;
    bool simple_progress;
    std::optional<std::string> profile;
    bool enable_cross_seeding = true;
};

void configure_create_app(CLI::App* app, create_app_options& options);

void configure_matcher(torrenttools::file_matcher& matcher, const create_app_options& options);

void merge_create_profile(const tt::config& cfg, std::string_view profile_name,
                          const CLI::App* app, create_app_options& options);

void postprocess_create_app(const CLI::App* app, const main_app_options& main_options, create_app_options& options);

void run_create_app(const main_app_options& main_options, create_app_options& options);

void set_files_with_progress(dottorrent::metafile& m, const create_app_options& options, std::ostream& os);