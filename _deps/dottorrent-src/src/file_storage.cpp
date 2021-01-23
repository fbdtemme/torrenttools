#include "dottorrent/file_storage.hpp"

namespace dottorrent {

file_storage::file_storage(const file_storage& other)
    : total_file_size_(other.total_file_size_)
    , piece_size_(other.piece_size_)
    , root_directory_(other.root_directory_)
    , files_(other.files_)
    , pieces_(other.pieces_)
    {}

file_storage& file_storage::operator=(const file_storage& other)
{
    if (&other == this) return *this;
    total_file_size_ = other.total_file_size_;
    piece_size_ = other.piece_size_;
    root_directory_ = other.root_directory_;
    files_ = other.files_;
    pieces_ = other.pieces_;

    return *this;
}

bool file_storage::has_root_directory() const noexcept
{
    return !root_directory_.empty();
}

const fs::path&  file_storage::root_directory() const noexcept
{
    return root_directory_;
}

void file_storage::set_root_directory(const fs::path& root)
{
    Ensures(fs::exists(root));
    root_directory_ = fs::absolute(root).lexically_normal();
}

void file_storage::add_file(const file_entry& file)
{
    auto s = file.file_size();
    total_file_size_ += s;
    if (!file.is_padding_file()) { total_regular_file_size_ += s; }
    files_.push_back(file);
}

void file_storage::add_file(file_entry&& file)
{
    auto s = file.file_size();
    total_file_size_ += s;
    if (!file.is_padding_file()) { total_regular_file_size_ += s; }
    files_.push_back(std::move(file));
}

void file_storage::add_file(const fs::path& path, file_options options)
{
    file_entry f = make_file_entry(path, root_directory_, options);
    add_file(std::move(f));
}

void file_storage::remove_file(const file_entry& entry)
{
    auto it = std::find_if(files_.begin(), files_.end(),
            [&](const file_entry& x) { return x == entry; });

    if (it != std::end(files_)) {
        auto s = it->file_size();
        total_file_size_ -= s;
        if (!it->is_padding_file()) { total_regular_file_size_ -= s; }
        files_.erase(it);
    }
}

file_mode file_storage::file_mode() const
{
    // empty torrent
    auto fc = file_count();
    if (fc == 0) return file_mode::empty;
    // one file
    if (fc == 1) {
        auto& f = files_[0].path();
        std::size_t components_count = std::distance(std::begin(f), std::end(f));
        // file contains no directories relative to root path.
        if (components_count == 1) return file_mode::single;
    }
    // more then one file
    return file_mode::multi;
}

enum protocol file_storage::protocol() const noexcept
{
    bool v1 = !pieces_.empty();
    bool v2 = rng::all_of(files_, [](const file_entry& entry) {
        // all regular files must have v2 data, skip pad files and symlinks
        if (entry.is_padding_file() || entry.is_symlink()) return true;
        return entry.has_v2_data();
    });

    if (v1 && v2) return protocol::v1 | protocol::v2;
    else if (v1)  return protocol::v1;
    else if (v2)  return protocol::v2;
    else          return protocol::none;
}

std::size_t file_storage::piece_size() const noexcept
{
    return piece_size_;
}

std::size_t file_storage::piece_count() const noexcept
{
    if (piece_size_ == 0) return 0;
    return ((total_file_size_ + piece_size_-1) / piece_size_);
}

void file_storage::set_piece_size(std::size_t size) {
    // These conditions are only required in the v2-spec.
    // They are assumed to hold for v1 as well when creating new torrents.
    Expects(size >= 16_KiB);                    // minimum size is 16 KiB
    Expects(std::has_single_bit(size));       // size must be a power of two

    piece_size_ = size;
}

void file_storage::allocate_pieces()
{
    pieces_.clear();
    pieces_.resize(piece_count());
}

const sha1_hash& file_storage::get_piece_hash(std::size_t index) const noexcept
{
    Expects(index < piece_count());
    return pieces_[index];
}

void file_storage::set_last_modified_time(std::size_t index, fs::file_time_type time) {
    Expects(index < file_count());
    files_[index].set_last_modified_time(time);
}

file_storage::iterator file_storage::begin() noexcept
{ return files_.begin(); }

file_storage::iterator file_storage::end() noexcept
{ return files_.end(); }

file_storage::reverse_iterator file_storage::rbegin() noexcept
{ return files_.rbegin(); }

file_storage::reverse_iterator file_storage::rend() noexcept
{ return files_.rend(); }

file_storage::const_iterator file_storage::begin() const noexcept
{ return files_.begin(); }

file_storage::const_iterator file_storage::end() const noexcept
{ return files_.end(); }

file_storage::const_reverse_iterator file_storage::rbegin() const noexcept
{ return files_.rbegin(); }

file_storage::const_reverse_iterator file_storage::rend() const noexcept
{ return files_.rend(); }

file_storage::const_iterator file_storage::cbegin() const noexcept
{ return files_.cbegin(); }

file_storage::const_iterator file_storage::cend() const noexcept
{ return files_.cend(); }

file_storage::const_reverse_iterator file_storage::crbegin() const noexcept
{ return files_.crbegin(); }

file_storage::const_reverse_iterator file_storage::crend() const noexcept
{ return files_.crend(); }

bool file_storage::operator==(const file_storage& other) const {
    return (
            (root_directory_ == other.root_directory_) &&
                    (files_ == other.files_) &&
                    (piece_size_ == other.piece_size_) );
}

std::span<const sha1_hash> file_storage::get_pieces_span(std::size_t file_index) const
{
    Expects(file_index < file_count());
    const auto [first, last] = get_pieces_offsets(file_index);
    return std::span(pieces_).subspan(first, last-first);
}


std::pair<std::size_t, std::size_t> file_storage::get_pieces_offsets(std::size_t file_index) const
{
    Expects(file_index < file_count());

    const auto& entry = at(file_index);

    std::size_t cumulative_size = std::transform_reduce(
            files_.begin(), std::next(files_.begin(), file_index), 0ul,
            std::plus<>{},
            [&](const file_entry& e) { return e.file_size(); });

    auto offset = cumulative_size / piece_size_;
    auto count = (entry.file_size() + piece_size_ - 1) / piece_size_;
    return {offset, offset+count};
}

void file_storage::set_piece_hash(std::size_t index, const sha1_hash& hash)
{
    Expects(index < pieces_.size());
    Expects(pieces_.size() == piece_count());
    std::unique_lock lck{pieces_mutex_};
    pieces_[index] = hash;
}

std::span<const sha1_hash> file_storage::pieces() const noexcept
{
    return std::span(pieces_.data(), pieces_.size());
}

std::size_t file_storage::size() const noexcept
{
    return files_.size();
}

auto choose_piece_size(file_storage& storage) -> std::size_t
{
    double exp = std::log2(storage.total_file_size()) - 9;
    exp = std::min(std::max(15, static_cast<int>(exp)), 24);
    auto piece_size = static_cast<std::size_t>(pow(2, exp));
    storage.set_piece_size(piece_size);
    return piece_size;
}

/// Check if a directory contains only padding files.
bool is_padding_directory(const file_storage& storage, const fs::path& directory)
{
    std::vector<const file_entry*> files {};

    for (const auto& f: storage) {
        if (f.path().string().starts_with(directory.string())) {
            files.push_back(&f);
        }
    }
    return rng::all_of(files, [](const file_entry* e) { return e->is_padding_file(); });
}


std::size_t directory_count(const file_storage& storage, const fs::path& root, bool include_padding_directories)
{
    std::set<std::string> directories {};
    for (const auto& f : storage) {
        if (!root.empty() && !f.path().string().starts_with(root.string())) {
            continue;
        }

        fs::path current_dir = "";
        auto it = f.path().begin();
        auto last = std::next(f.path().end(), -1);

        while (it != last) {
            directories.emplace((current_dir / *it).string());
            it++;
        }
    }
    if (include_padding_directories)
        return directories.size();

    std::size_t directory_count = 0;
    for (const auto& dir: directories) {
        if (!is_padding_directory(storage, dir)) {
            ++directory_count;
        }
    }
    return directory_count;
}


auto exclusive_file_size_scan(const file_storage& storage) -> std::vector<std::size_t>
{
    std::vector<std::size_t> res{};
    res.reserve(storage.file_count());

    std::transform_exclusive_scan(
            storage.begin(), storage.end(), std::back_inserter(res), 0UL, std::plus<>{},
            [&](const file_entry& e) { return e.file_size(); }
    );
    return res;
}

std::vector<std::size_t> inclusive_file_size_scan_v1(const file_storage& storage)
{
    std::vector<std::size_t> res{};
    res.reserve(storage.file_count());

    std::transform_inclusive_scan(
            storage.begin(), storage.end(), std::back_inserter(res), std::plus<>{},
            [&](const file_entry& e) { return e.file_size(); }, 0ul
    );
    return res;
}

std::vector<std::size_t> inclusive_file_size_scan_v2(const file_storage& storage)
{
    std::vector<std::size_t> res{};
    res.reserve(storage.file_count());

    std::transform_inclusive_scan(
            storage.begin(), storage.end(), std::back_inserter(res), std::plus<>{},
            [&](const file_entry& e) -> std::size_t {
                // v2 padding for hybrid torrents is implicit, padding files are not counted in progress
                if (e.is_padding_file()) { return 0ul; }
                return e.file_size();
            },
            0ul
    );
    return res;
}

auto absolute_file_paths(const file_storage& storage) -> std::vector<fs::path>
{
    Ensures(storage.has_root_directory());

    const auto& root_directory = storage.root_directory();
    if (!fs::exists(root_directory)) {
        throw std::invalid_argument("storage is not associated with a physical location");
    }

    std::vector<fs::path> files {};
    files.reserve(storage.file_count());

    std::transform(storage.begin(), storage.end(), std::back_inserter(files),
            [&](const file_entry& f) {
                return fs::weakly_canonical(root_directory / f.path());
            }
    );
    return files;
}

std::string make_v1_pieces_string(const file_storage& storage)
{
    std::string pieces_string{};
    pieces_string.reserve(storage.piece_size() * sha1_hash::size());
    for (const auto& p: storage.pieces()) {
        pieces_string.append(std::string_view(p));
    }
    return pieces_string;
}

std::string make_v2_piece_layers_string(const file_entry& entry)
{
    std::string piece_layer_string{};
    auto layers = entry.piece_layer();
    piece_layer_string.reserve(sha256_hash::size() * layers.size());

    for (const auto& p: layers) {
        piece_layer_string.append(std::string_view(p));
    }
    return piece_layer_string;
}


bool is_piece_aligned(const file_storage& storage) noexcept
{
    Expects(storage.piece_size() != 0);

    const std::size_t piece_size = storage.piece_size();
    std::size_t bytes_offset = 0;

    bool is_aligned = true;
    for (const auto& entry : storage) {
        if (!entry.is_padding_file()) {
            is_aligned &= (bytes_offset % piece_size == 0);
        }
        bytes_offset += entry.file_size();
    }
    return is_aligned;
}


void optimize_alignment(file_storage& storage)
{
    // piece size needs to be initialized
    Expects(storage.piece_size() != 0);

    const std::size_t piece_size = storage.piece_size();

    std::size_t total_padding_bytes = 0;
    std::vector<file_entry> aligned_entries {};

    for (std::size_t i = 0; i < storage.file_count()-1; ++i) {
        auto& entry = storage[i];
        auto size = entry.file_size();
        aligned_entries.push_back(std::move(entry));

        auto bytes_till_next_piece = piece_size - (size % piece_size);

        Expects(bytes_till_next_piece != 0);
        if (bytes_till_next_piece != piece_size) [[likely]] {
            aligned_entries.push_back(make_padding_file_entry(bytes_till_next_piece));
            total_padding_bytes += bytes_till_next_piece;
        }
    }
    // add the last file
    aligned_entries.push_back(std::move(storage.back()));

    // clear old storage and set new
    storage.clear();
    storage.add_files(aligned_entries.begin(), aligned_entries.end());

    Ensures(storage.total_file_size() == storage.total_regular_file_size() + total_padding_bytes);
}

}