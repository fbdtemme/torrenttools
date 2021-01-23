#pragma once
/*
 * Enum class defining the bittorrent protocol version.
 */
#include <dottorrent/bitmask_operators.hpp>
#include "dottorrent/literals.hpp"

#include <bencode/bencode.hpp>


namespace dottorrent {

using namespace dottorrent::literals;

inline constexpr std::size_t v2_block_size = 16_KiB;

enum class protocol {
    none = 0,      ///< incomplete metadata: for file_storage with not yet hashed files.
    v1 = 1,        ///< contains v1 metadata
    v2 = 2,        ///< contains v2 metadata
    hybrid = 3,    ///< contains v1 and v2 metadata
};

enum class file_mode {
    empty,    ///< no files: state after default construction of a file_storage object.
    single,   ///< single-file torrent: single file in "name" string
    multi     ///< multi-file torrent: files in "files" list
};

enum class file_options {
    none = 0,
    /// BEP-47 : Add attributes.
    add_attributes = 1,
    /// BEP-47 : Add symlinks as symlinks an not the file/directory they point to.
    /// This option enables the add_attributes option.
    copy_symlinks = 3,
};

constexpr file_options default_file_options = file_options::add_attributes;

enum class metafile_options {
    none = 0,
    // Add the "announce" key for clients that do not support multi-tracker torrents.
    single_tracker_compatibility = 1
};

enum class file_attributes {
    none = 0,
    symlink = 1,
    executable = 2,
    hidden = 4,
    padding_file = 8,
};

} // namespace dottorrent

DOTTORRENT_ENABLE_BITMASK_OPERATORS(dottorrent::protocol);
DOTTORRENT_ENABLE_BITMASK_OPERATORS(dottorrent::file_attributes);
DOTTORRENT_ENABLE_BITMASK_OPERATORS(dottorrent::file_options);

namespace dottorrent {

// terminal type representing symbol / character of any type
template <auto v>
struct symbol {
    static constexpr auto value = v;
};


inline std::string to_string(dottorrent::file_attributes v)
{
    std::string out = "";
    if (v == file_attributes::none) return out;
    if ((v & file_attributes::executable) == file_attributes::executable){
        out.push_back('x');
    }
    if ((v & file_attributes::symlink) == file_attributes::symlink) {
        out.push_back('l');
    }
    if ((v & file_attributes::hidden) == file_attributes::hidden) {
        out.push_back('h');
    }
    if ((v & file_attributes::padding_file) == file_attributes::padding_file) {
        out.push_back('p');
    }
    return out;
}

constexpr file_attributes make_file_attributes(std::string_view value)
{
    auto attributes = file_attributes::none;

    for (char c: value) {
        switch (c) {
        case 'l': attributes |= file_attributes::symlink;      break;
        case 'x': attributes |= file_attributes::executable;   break;
        case 'h': attributes |= file_attributes::hidden;       break;
        case 'p': attributes |= file_attributes::padding_file; break;
        default:
            throw std::invalid_argument(
                    fmt::format("unrecognised file attribute: {}", c));
        }
    }
    return attributes;
}

} // namespace dottorrent
