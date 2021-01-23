#pragma once

#include <string_view>
#include <optional>

#include <bencode/bvalue.hpp>
#include <bencode/bview.hpp>
#include <bencode/traits/string_view.hpp>
#include <bencode/traits/unordered_set.hpp>

#include <dottorrent/metafile.hpp>

#include <fmt/format.h>
#include <dottorrent/error.hpp>


namespace dottorrent {

namespace detail {

namespace bc = bencode;

template <bc::bvalue_or_bview T>
void parse_announce(const T& data, metafile& m);

extern template void parse_announce<>(const bc::bvalue& data, metafile& m);
extern template void parse_announce<>(const bc::bview& data, metafile& m);

template <bc::bvalue_or_bview T>
void parse_private(const T& data, metafile& m);

extern template void parse_private<>(const bc::bvalue& data, metafile& m);
extern template void parse_private<>(const bc::bview& data, metafile& m);

template <bc::bvalue_or_bview T>
void parse_comment(const T& data, metafile& m);

extern template void parse_comment<>(const bc::bvalue& data, metafile& m);
extern template void parse_comment<>(const bc::bview& data, metafile& m);

template <bc::bvalue_or_bview T>
void parse_created_by(const T& data, metafile& m);

extern template void parse_created_by<>(const bc::bvalue& data, metafile& m);
extern template void parse_created_by<>(const bc::bview& data, metafile& m);

template <bc::bvalue_or_bview T>
void parse_collections(const T& data, metafile& m);

extern template void parse_collections<>(const bc::bvalue& data, metafile& m);
extern template void parse_collections<>(const bc::bview& data, metafile& m);

template <bc::bvalue_or_bview T>
void parse_http_seeds(const T& data, metafile& m);

extern template void parse_http_seeds<>(const bc::bvalue& data, metafile& m);
extern template void parse_http_seeds<>(const bc::bview& data, metafile& m);

template <bc::bvalue_or_bview T>
void parse_web_seeds(const T& data, metafile& m);

extern template void parse_web_seeds<>(const bc::bvalue& data, metafile& m);
extern template void parse_web_seeds<>(const bc::bview& data, metafile& m);

template <bc::bvalue_or_bview T>
void parse_dht_nodes(const T& data, metafile& m);

extern template void parse_dht_nodes<>(const bc::bvalue& data, metafile& m);
extern template void parse_dht_nodes<>(const bc::bview& data, metafile& m);

template <bc::bvalue_or_bview T>
void parse_creation_date(const T& data, metafile& m);

extern template void parse_creation_date<>(const bc::bvalue& data, metafile& m);
extern template void parse_creation_date<>(const bc::bview& data, metafile& m);

template <bc::bvalue_or_bview T>
void parse_similar_torrents(const T& data, metafile& m);

extern template void parse_similar_torrents<>(const bc::bvalue& data, metafile& m);
extern template void parse_similar_torrents<>(const bc::bview& data, metafile& m);

template <bc::bvalue_or_bview T>
void parse_name(const T& data, metafile& m);

extern template void parse_name<>(const bc::bvalue& data, metafile& m);
extern template void parse_name<>(const bc::bview& data, metafile& m);

template <bc::bvalue_or_bview T>
void parse_source(const T& data, metafile& m);

extern template void parse_source<>(const bc::bvalue& data, metafile& m);
extern template void parse_source<>(const bc::bview& data, metafile& m);

template <bc::bvalue_or_bview T>
fs::path parse_path(const T& data);

extern template fs::path parse_path<>(const bc::bvalue& data);
extern template fs::path parse_path<>(const bc::bview& data);

template <bc::bvalue_or_bview T>
std::optional<file_attributes> parse_file_attributes(const T& data);

extern template std::optional<file_attributes> parse_file_attributes<>(const bc::bvalue& data);
extern template std::optional<file_attributes> parse_file_attributes<>(const bc::bview& data);

template <bc::bvalue_or_bview T>
file_entry parse_file_entry_v1(const T& data);

extern template file_entry parse_file_entry_v1<>(const bc::bvalue& data);
extern template file_entry parse_file_entry_v1<>(const bc::bview& data);

template <bc::bvalue_or_bview T>
void parse_file_list_v1(const T& data, metafile& m);

extern template void parse_file_list_v1<>(const bc::bvalue& data, metafile& m);
extern template void parse_file_list_v1<>(const bc::bview& data, metafile& m);

template <bc::bvalue_or_bview T>
void parse_file_tree_v2(const T& data, metafile& m);

extern template void parse_file_tree_v2<>(const bc::bvalue& data, metafile& m);
extern template void parse_file_tree_v2<>(const bc::bview& data, metafile& m);

template <bc::bvalue_or_bview T>
void parse_piece_size(const T& data, metafile& m);

extern template void parse_piece_size<>(const bc::bvalue& data, metafile& m);
extern template void parse_piece_size<>(const bc::bview& data, metafile& m);


template <bc::bvalue_or_bview T>
void parse_pieces_v1(const T& data, metafile& m);

extern template void parse_pieces_v1<>(const bc::bvalue& data, metafile& m);
extern template void parse_pieces_v1<>(const bc::bview& data, metafile& m);


template <bc::bvalue_or_bview T>
void parse_piece_layers_v2(const T& data, metafile& m);

extern template void parse_piece_layers_v2<>(const bc::bvalue& data, metafile& m);
extern template void parse_piece_layers_v2<>(const bc::bview& data, metafile& m);


template <bc::bvalue_or_bview T>
protocol parse_protocol(const T& data);

extern template protocol parse_protocol<>(const bc::bvalue& data);
extern template protocol parse_protocol<>(const bc::bview& data);

template <bc::bvalue_or_bview T>
bool check_if_hybrid(const T& data);

extern template bool check_if_hybrid<>(const bc::bvalue& data);
extern template bool check_if_hybrid<>(const bc::bview& data);

}

namespace bc = bencode;

template<bc::bvalue_or_bview T>
metafile parse_metafile(const T& data);

extern template metafile parse_metafile<>(const bc::bvalue& data);
extern template metafile parse_metafile<>(const bc::bview& data);


} // namespace dottorrent