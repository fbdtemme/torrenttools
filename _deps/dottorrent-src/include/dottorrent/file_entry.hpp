#pragma once

#include <set>
#include <optional>
#include <string>
#include <string_view>
#include <filesystem>
#include <memory>
#include <mutex>
#include <compare>

#include <gsl-lite/gsl-lite.hpp>

#include "dottorrent/general.hpp"
#include "dottorrent/utils.hpp"
#include "dottorrent/checksum.hpp"
#include "dottorrent/hash.hpp"

namespace dottorrent {

namespace fs = std::filesystem;

class file_storage;

class file_entry {
public:
    using checksum_map = std::map<std::string, std::unique_ptr<checksum>, std::less<>>;

    file_entry(fs::path path,
               std::size_t file_size,
               std::optional<file_attributes> attributes = std::nullopt,
               std::optional<fs::path> symlink_path = std::nullopt);

    file_entry(const file_entry& other);

    file_entry(file_entry&&) noexcept;

    file_entry& operator=(const file_entry& other);

    file_entry& operator=(file_entry&&) noexcept;

    const fs::path& path() const noexcept;

    std::size_t file_size() const noexcept;

    /// V2 only
    bool has_v2_data() const noexcept;

    const sha256_hash& pieces_root() const;

    void set_pieces_root(const sha256_hash& hash) noexcept;

    std::span<const sha256_hash> piece_layer() const;

    void set_piece_layer(std::span<const sha256_hash> range);

    std::optional<fs::path> symlink_path() const noexcept;

    std::optional<fs::file_time_type> last_modified_time() const noexcept;

    void set_last_modified_time(fs::file_time_type time);

    /// Returns the optional attributes.
    std::optional<file_attributes> attributes() const noexcept;

    bool is_symlink() const noexcept;

    bool is_executable() const noexcept;

    bool is_hidden() const noexcept;

    bool is_padding_file() const noexcept;

    /// Return the checksum for given key if present or nullptr.
    const checksum* get_checksum(std::string_view algorithm) const;

    const checksum* get_checksum(hash_function algorithm) const;

    void add_checksum(std::unique_ptr<checksum> value);

    template <typename KeyType>
    void remove_checksum(const KeyType& algorithm)
    {
        std::unique_lock lck{checksum_mutex_};
        if (auto it = checksums_.find(algorithm); it != checksums_.end()) {
            checksums_.erase(*it);
        }
    }

    const checksum_map& checksums() const;

    bool operator==(const file_entry& rhs) const noexcept;

    std::strong_ordering operator<=>(const file_entry& rhs) const noexcept;

private:
    /// Path of the file relative to to the torrent root directory.
    fs::path path_;
    /// Size of the file in bytes.
    std::size_t file_size_;
    /// Optional checksums for the file.
    checksum_map checksums_;
    mutable std::mutex checksum_mutex_ {};
    /// Root hash of binary merkle tree constructed from 16 KiB blocks of the file.
    bool has_v2_data_ = false;
    sha256_hash pieces_root_ {};
    /// Piece layers covering piece of piece size bytes.
    std::vector<sha256_hash> piece_layer_ {};
    /// BEP-47 : Extended attributes.
    std::optional<file_attributes> attributes_;
    /// BEP-47 : Path of the symlink target relative to the torrent root directory.
    std::optional<fs::path> symlink_path_;
    /// Last modified time of the file.
    std::optional<fs::file_time_type> last_modified_time_;

    friend class file_storage;
};


file_entry make_file_entry(fs::path file,
                            const fs::path& root_directory,
                            file_options options = default_file_options);

/// Create a padding file using the BEP47 canonical path.
/// @param padding_size The size of the padding file.
/// @returns A file entry of the padding file.
file_entry make_padding_file_entry(std::size_t padding_size);

} // namespace dottorrent