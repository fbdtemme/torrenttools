#pragma once
#include <string>
#include <vector>
#include <optional>
#include <chrono>
#include <unordered_set>

#include <yaml-cpp/yaml.h>

#include "dottorrent/general.hpp"
#include "dottorrent/dht_node.hpp"
#include "dottorrent/hash_function.hpp"
#include "dottorrent/info_hash.hpp"
#include "list_edit_mode.hpp"

dottorrent::protocol protocol_transformer(const std::vector<std::string>& v, bool allow_hybrid = true);

std::optional<std::size_t> piece_size_transformer(const std::vector<std::string>& v);

std::optional<std::size_t> io_block_size_transformer(const std::vector<std::string>& v);

std::vector<std::vector<std::string>> announce_transformer(const std::vector<std::string>& s);

std::vector<std::vector<std::string>> announce_transformer(const YAML::Node& s);

std::vector<dottorrent::dht_node> dht_node_transformer(const std::vector<std::string>& s);

std::unordered_set<dottorrent::hash_function> checksum_transformer(const std::vector<std::string>& s);

std::filesystem::path path_transformer(const std::vector<std::string>& v,
                                         bool check_exists = true,
                                         bool keep_trailing = true);

std::filesystem::path config_path_transformer(const std::vector<std::string>& v, bool check_exists);

std::filesystem::path metafile_target_transformer(const std::vector<std::string>& v, bool allow_read_from_stdin = false);

std::optional<std::size_t> parse_commandline_size(std::string_view option, const std::string& v);

std::vector<std::optional<bool>> parse_commandline_booleans(std::string_view option, const std::vector<std::string>& v);

std::optional<bool> parse_commandline_bool(std::string_view option, const std::vector<std::string>& v);

std::chrono::system_clock::time_point
creation_date_transformer(std::string_view option, const std::vector<std::string>& v);

torrenttools::list_edit_mode
parse_list_edit_mode(std::string_view options, const std::vector<std::string>& v);

bool parse_explicit_flag(std::string_view option, const std::vector<std::string>& v);

std::vector<dottorrent::info_hash>
similar_transformer(std::string_view option, const std::vector<std::string>& v);

std::vector<std::string>
seed_transformer(std::string_view option, const std::vector<std::string>& v, bool allow_ftp = false);

std::string
profile_transformer(std::string_view option, const std::vector<std::string>& v);