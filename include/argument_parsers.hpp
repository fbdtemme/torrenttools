#include <string>
#include <vector>
#include <optional>
#include <chrono>

#include "dottorrent/general.hpp"
#include "dottorrent/dht_node.hpp"
#include "dottorrent/hash_function.hpp"


dottorrent::protocol protocol_transformer(const std::vector<std::string>& v);

std::optional<std::size_t> piece_size_transformer(const std::vector<std::string>& v);

std::optional<std::size_t> io_block_size_transformer(const std::vector<std::string>& v);

std::vector<std::vector<std::string>> announce_transformer(const std::vector<std::string>& s);

std::vector<dottorrent::dht_node> dht_node_transformer(const std::vector<std::string>& s);

std::unordered_set<dottorrent::hash_function> checksum_transformer(const std::vector<std::string>& s);

std::filesystem::path target_transformer(const std::vector<std::string>& v);

std::optional<std::size_t> parse_commandline_size(std::string_view option, const std::string& v);

std::vector<std::optional<bool>> parse_commandline_booleans(std::string_view option, const std::vector<std::string>& v);

std::optional<bool> parse_commandline_bool(std::string_view option, const std::vector<std::string>& v);

std::chrono::system_clock::time_point
creation_date_transformer(std::string_view option, const std::vector<std::string>& v);