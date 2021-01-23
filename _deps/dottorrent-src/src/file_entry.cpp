//
// Created by fbdtemme on 9/11/20.
//
#include <fmt/format.h>
#include "dottorrent/file_entry.hpp"

namespace dottorrent
{

file_entry::file_entry(fs::path path, std::size_t file_size, std::optional<file_attributes> attributes,
        std::optional<fs::path> symlink_path)
        : path_(std::move(path))
        , file_size_(file_size)
        , checksums_()
        , attributes_(attributes)
        , symlink_path_(std::move(symlink_path))
{
    // Check invariants
    if (is_symlink()) {
        Ensures(symlink_path_.has_value());
        Ensures(file_size_ == 0);
    }
}

file_entry::file_entry(const file_entry& other)
        : path_(other.path_)
        , file_size_(other.file_size_)
        , checksums_()
        , pieces_root_(other.pieces_root_)
        , piece_layer_(other.piece_layer_)
        , attributes_(other.attributes_)
        , symlink_path_(other.symlink_path_)
        , last_modified_time_(other.last_modified_time_)
        , has_v2_data_(other.has_v2_data_)
{
    // Check invariants
    if (is_symlink()) {
        Ensures(symlink_path_.has_value());
        Ensures(file_size_ == 0);
    }

    for (const auto& [k, v] : other.checksums_) {
        checksums_.emplace(k, make_checksum(v->name(), v->value()));
    }
}

file_entry::file_entry(file_entry&& other) noexcept
    : path_(std::move(other.path_))
    , file_size_(other.file_size_)
    , checksums_(std::move(other.checksums_))
    , pieces_root_(other.pieces_root_)
    , piece_layer_(std::move(other.piece_layer_))
    , attributes_(other.attributes_)
    , symlink_path_(std::move(other.symlink_path_))
    , last_modified_time_(other.last_modified_time_)
    , has_v2_data_(other.has_v2_data_)
{
    // Check invariants
    if (is_symlink()) {
        Ensures(symlink_path_.has_value());
        Ensures(file_size_ == 0);
    }
}

file_entry& file_entry::operator=(const file_entry& other) {
    if (&other == this) return *this;

    path_ = other.path_;
    file_size_ = other.file_size_;
    pieces_root_ = other.pieces_root_;
    piece_layer_ = other.piece_layer_;
    attributes_ = other.attributes_;
    symlink_path_ = other.symlink_path_;
    last_modified_time_ = other.last_modified_time_;
    has_v2_data_ = other.has_v2_data_;

    checksums_.clear();

    for (const auto& [k, v] : other.checksums_) {
        checksums_.emplace(k, make_checksum(v->name(), v->value()));
    }
    return *this;
}


file_entry& file_entry::operator=(file_entry&& other) noexcept
{
    if (&other == this) return *this;

    path_ = std::move(other.path_);
    file_size_ = other.file_size_;
    pieces_root_ = other.pieces_root_;
    piece_layer_ = std::move(other.piece_layer_);
    attributes_ = other.attributes_;
    symlink_path_ = std::move(other.symlink_path_);
    last_modified_time_ = other.last_modified_time_;
    checksums_ = std::move(checksums_);
    has_v2_data_ = other.has_v2_data_;

    return *this;
}

const fs::path& file_entry::path() const noexcept
{
    return path_;
}

std::size_t file_entry::file_size() const noexcept
{
    return file_size_;
}

bool file_entry::has_v2_data() const noexcept
{
    return has_v2_data_;
}


const sha256_hash& file_entry::pieces_root() const
{
    if (!has_v2_data_)
        throw std::invalid_argument("file entry does not contain v2 data");

    return pieces_root_;
}

void file_entry::set_pieces_root(const sha256_hash& hash) noexcept
{
    pieces_root_ = hash;
    has_v2_data_ = true;
}

std::span<const sha256_hash> file_entry::piece_layer() const
{
    if (!has_v2_data_) {
        throw std::invalid_argument("file entry does not contain v2 data");
    }
    return std::span(piece_layer_.data(), piece_layer_.size());
}

void file_entry::set_piece_layer(std::span<const sha256_hash> layer)
{
    piece_layer_.assign(layer.begin(), layer.end());
    has_v2_data_ = true;
}

std::optional<fs::path> file_entry::symlink_path() const noexcept
{
    return symlink_path_;
}

std::optional<fs::file_time_type> file_entry::last_modified_time() const noexcept
{
    return last_modified_time_;
}

std::optional<file_attributes> file_entry::attributes() const noexcept
{
    return attributes_;
}

bool file_entry::is_symlink() const noexcept {
    return attributes_ ?
           (*attributes_ & file_attributes::symlink) == file_attributes::symlink :
           false;
}

bool file_entry::is_executable() const noexcept {
    return attributes_ ?
           (*attributes_ & file_attributes::executable) == file_attributes::executable :
           false;
}

bool file_entry::is_hidden() const noexcept {
    return attributes_ ?
           (*attributes_ & file_attributes::hidden) == file_attributes::hidden :
           false;
}

bool file_entry::is_padding_file() const noexcept {
    return attributes_ ?
           (*attributes_ & file_attributes::padding_file) == file_attributes::padding_file :
           false;
}

const checksum* file_entry::get_checksum(std::string_view algorithm) const
{
    std::unique_lock lck{checksum_mutex_};
    if (auto it = checksums_.find(algorithm); it != checksums_.end()) {
        const auto& ptr = *it;
        return it->second.get();
    }
    return nullptr;
}

const checksum* file_entry::get_checksum(hash_function algorithm) const {
    std::unique_lock lck{checksum_mutex_};
    if (auto it = checksums_.find(to_string(algorithm)); it != checksums_.end()) {
        const auto& ptr = *it;
        return it->second.get();
    }
    return nullptr;
}

void file_entry::add_checksum(std::unique_ptr<checksum> value)
{
    std::unique_lock lck{checksum_mutex_};
    checksums_.emplace(std::string(value->name()), std::move(value));
}

const file_entry::checksum_map& file_entry::checksums() const
{
    return checksums_;
}


void file_entry::set_last_modified_time(fs::file_time_type time)
{
    last_modified_time_ = time;
}

bool file_entry::operator==(const file_entry& rhs) const noexcept
{
    return (path_ == rhs.path_);
}

std::strong_ordering file_entry::operator<=>(const file_entry& rhs) const noexcept
{
    return this->path_ <=> rhs.path_;
}



//const std::optional<std::vector<sha256_hash>>& file_entry::piece_layers() const noexcept
//{
//    return piece_layers_;
//}
//
//void file_entry::set_piece_layers(std::span<sha256_hash> range)
//{
//    piece_layers_ = std::vector(rng::begin(range), rng::end(range));
//}

file_entry make_file_entry(fs::path file, const fs::path& root_directory, file_options options)
{
    if (!file.is_absolute()) {
        file = fs::absolute(file);
    }

    if (!fs::exists(file))
        throw std::invalid_argument("file does not exist");

    if (!(fs::is_regular_file(file) || fs::is_symlink(file)))
        throw std::invalid_argument("not a valid file");

    fs::path relative_file = file.lexically_relative(root_directory);

    if (relative_file.generic_string().starts_with(".."))
        throw std::invalid_argument("file or symlink not located inside root_directory");

    fs::directory_entry de {file};
    std::size_t file_size = de.file_size();
    std::optional<file_attributes> attributes = std::nullopt;
    std::optional<fs::path> symlink_path = std::nullopt;

    // Extended file attributes
    if ((options & file_options::add_attributes) != file_options::none)
    {
        attributes = file_attributes::none;

        // executable flag
        if (de.is_regular_file() && ((de.status().permissions() & (
                fs::perms::owner_exec |
                fs::perms::group_exec |
                fs::perms::others_exec)) != fs::perms::none))
        {
            *attributes |= file_attributes::executable;
        }

        // windows only
        if (detail::is_hidden_file(file)) {
            *attributes |= file_attributes::hidden;
        }
    }

    // Copy symlinks as symlinks
    if ((options & file_options::copy_symlinks) != file_options::none) {
        if (de.is_symlink()) {
            *attributes |= file_attributes::symlink;
            symlink_path = fs::read_symlink(file);
            // file size of symlinks should be 0 for backward compatibility
            file_size = 0;
        }
    }

    return file_entry(std::move(relative_file), file_size,
                      attributes, std::move(symlink_path));
}


file_entry make_padding_file_entry(std::size_t padding_size)
{
    using namespace fmt::literals;
    auto path = fs::path(".pad/{}"_format(padding_size));
    return file_entry(path, padding_size, file_attributes::padding_file);
}


}