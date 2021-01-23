#pragma once
#include <span>
#include <vector>
#include <mutex>
#include <atomic>

#include <gsl-lite/gsl-lite.hpp>

#include "dottorrent/merkle_tree.hpp"
#include "dottorrent/chunk_hasher.hpp"

namespace dottorrent {

class v2_chunk_hasher : public chunk_hasher
{
public:
    using base_type = chunk_hasher;

    explicit v2_chunk_hasher(file_storage& storage, std::size_t capacity, bool v1_compatible = false, std::size_t thread_count = 1);

protected:
    void hash_chunk(std::vector<std::unique_ptr<hasher>>& hashers, const data_chunk& chunk) override;

    void hash_chunk(hasher& sha256_hasher, hasher& sha1_hasher, const data_chunk& chunk);

    void set_piece_layers_and_root(hasher& hasher, std::size_t file_index);

    void process_piece_hash(std::size_t piece_idx, std::size_t file_index, const sha1_hash& piece_hash);

private:
    std::vector<merkle_tree<hash_function::sha256>> merkle_trees_;
    /// Vector with the count of hashed bytes per file
    std::vector<std::atomic<std::size_t>> file_bytes_hashed_ {};
    std::size_t piece_size_;

    bool add_v1_compatibility_;
    // for each file the v1 piece index the file starts at.
    std::vector<std::size_t> v1_piece_offsets_;
    std::vector<std::byte> padding_;
};

} // namespace dottorrent
