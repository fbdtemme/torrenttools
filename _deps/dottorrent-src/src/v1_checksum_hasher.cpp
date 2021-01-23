#include "dottorrent/chunk_hasher.hpp"
#include "dottorrent/v1_checksum_hasher.hpp"

namespace dottorrent {

v1_checksum_hasher::v1_checksum_hasher(file_storage& storage, hash_function f, std::size_t capacity)
        : chunk_hasher(storage, {f}, capacity, 1)
{}

void v1_checksum_hasher::hash_chunk(std::vector<std::unique_ptr<hasher>>& hashers, const data_chunk& chunk)
{
    hash_chunk(*hashers.front(), chunk);
}

void v1_checksum_hasher::hash_chunk(hasher& hasher, const data_chunk& item) {
    Expects(item.data != nullptr);

    file_storage& storage = storage_;

    // offset from the start of data where we are currently processing
    std::size_t chunk_offset = 0;
    std::size_t chunk_size = item.data->size();
    std::size_t current_file_size = storage[current_file_index_].file_size();
    auto data = std::span(*item.data);

    Ensures(current_file_index_ < storage.file_count());

    // Verify that we are receive chunks sequentially

    while (chunk_offset != chunk_size) {
        auto current_file_data_remaining = current_file_size - current_file_data_hashed_;
        auto current_chunk_data_remaining = chunk_size-chunk_offset;
        auto current_file_subchunk_size = std::min(current_file_data_remaining, current_chunk_data_remaining);

        hasher.update(data.subspan(chunk_offset, current_file_subchunk_size));
        chunk_offset += current_file_subchunk_size;

        bytes_hashed_.fetch_add(current_file_subchunk_size, std::memory_order_relaxed);
        bytes_done_.fetch_add(current_file_subchunk_size, std::memory_order_relaxed);
        current_file_data_hashed_.fetch_add(current_file_subchunk_size, std::memory_order_relaxed);

        // check if we completed a file checksum
        if (current_file_data_hashed_.load(std::memory_order_relaxed) == current_file_size) {
            Expects(hash_functions_.size() == 1);
            auto hash_function = hash_functions_.front();
            auto checksum = make_checksum(hash_function);
            hasher.finalize_to(checksum->value());
            storage[current_file_index_].add_checksum(std::move(checksum));

            current_file_index_.fetch_add(1, std::memory_order_relaxed);

            if (current_file_index_ < storage.file_count()) {
                current_file_size = storage[current_file_index_].file_size();
                current_file_data_hashed_.store(0, std::memory_order_relaxed);
            }
        }
    }
}
} // namespace dottorrent
