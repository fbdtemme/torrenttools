#include "dottorrent/v2_checksum_hasher.hpp"
#include <gsl-lite/gsl-lite.hpp>

namespace dottorrent {

v2_checksum_hasher::v2_checksum_hasher(file_storage& storage, hash_function f, std::size_t capacity)
        : base_type(storage, {f}, capacity, 1)
{}

void v2_checksum_hasher::hash_chunk(std::vector<std::unique_ptr<hasher>>& hashers, const data_chunk& chunk) {
    hash_chunk(*hashers.front(), chunk);
}

void v2_checksum_hasher::hash_chunk(hasher& hasher, const data_chunk& item)
{
    Expects(item.data != nullptr);

    file_storage& storage = storage_;

    // offset from the start of data where we are currently processing
    std::size_t chunk_offset = 0;
    std::size_t chunk_size = item.data->size();

    Expects(current_file_index_ <= item.file_index);

    if (item.file_index > current_file_index_) {
        // Set the hash for the previous file.
        Expects(hash_functions_.size() == 1);
        auto hash_function = hash_functions_.front();
        auto checksum = make_checksum(hash_function);
        hasher.finalize_to(checksum->value());
        storage[current_file_index_].add_checksum(std::move(checksum));

        // update current file index
        ++current_file_index_;
    }

    Expects(current_file_index_ == item.file_index);
    hasher.update(std::span(*item.data));
    bytes_hashed_.fetch_add(chunk_size, std::memory_order_relaxed);
    bytes_done_.fetch_add(chunk_size, std::memory_order_relaxed);
}

}