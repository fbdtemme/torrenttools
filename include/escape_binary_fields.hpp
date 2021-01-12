#pragma once

#include <bencode/bencode.hpp>
#include <dottorrent/metafile.hpp>

namespace bc = bencode;
namespace dt = dottorrent;
namespace rng = std::ranges;

bc::bvalue escape_binary_metafile_fields(const bc::bvalue& value);

bc::bvalue escape_binary_metafile_fields_hex(const bencode::bvalue& value);
