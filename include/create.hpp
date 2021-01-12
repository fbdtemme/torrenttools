#pragma once
#include <string>
#include <vector>
#include <optional>
#include <cstdint>
#include <filesystem>
#include <dottorrent/metafile.hpp>

#include "config.hpp"

#include "dottorrent/storage_hasher.hpp"
#include "dottorrent/dht_node.hpp"
#include "info.hpp"

namespace CLI { class App; }

namespace  {
namespace fs = std::filesystem;
}

// forward declare
namespace torrenttools {
class file_matcher;
}

struct create_app_options
{
    std::filesystem::path target;
    std::optional<std::filesystem::path> destination;
    std::vector<std::string> include_patterns;
    std::vector<std::string> exclude_patterns;
    std::vector<std::vector<std::string>> announce_list;
    std::optional<std::string> comment;
    std::optional<std::string> source;
    std::optional<std::string> name;
    std::vector<std::string> web_seeds;
    std::vector<dottorrent::dht_node> dht_nodes;
    std::optional<std::size_t> piece_size;
    std::uint8_t threads = 4;
    // determine private flag based on tracker or override with --private
    std::optional<bool> is_private = std::nullopt;
    bool set_created_by = true;
    bool set_creation_date = true;
    bool include_hidden_files = false;
    std::unordered_set<dottorrent::hash_function> checksums = {};
    dottorrent::protocol protocol_version = dottorrent::protocol::v1;
};

void configure_create_app(CLI::App* app, create_app_options& options);

void configure_matcher(torrenttools::file_matcher& matcher, const create_app_options& options);

void run_create_app(const create_app_options& options);

void run_with_progress(dottorrent::storage_hasher& hasher, const dottorrent::metafile& m);

void print_creation_statistics(const dottorrent::metafile& m, std::chrono::system_clock::duration duration);

void set_files(dottorrent::metafile& m, const create_app_options& options);

void set_trackers(dottorrent::metafile& m, const create_app_options& options);

fs::path get_destination_path(dottorrent::metafile& m, const create_app_options& options);
