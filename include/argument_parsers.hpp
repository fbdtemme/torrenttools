#include <string>
#include <vector>
#include <optional>

#include "dottorrent/general.hpp"
#include "dottorrent/dht_node.hpp"
#include "dottorrent/hash_function.hpp"


dottorrent::protocol protocol_transformer(const std::vector<std::string>& v);

std::optional<std::size_t> piece_size_transformer(const std::vector<std::string>& v);

std::vector<std::vector<std::string>> announce_transformer(const std::vector<std::string>& s);

std::vector<dottorrent::dht_node> dht_node_transformer(const std::vector<std::string>& s);

std::unordered_set<dottorrent::hash_function> checksum_transformer(const std::vector<std::string>& s);

std::filesystem::path target_transformer(const std::vector<std::string>& v);
