#pragma once

#include "dottorrent/chunk_verifier.hpp"
#include "dottorrent/v2_chunk_hasher.hpp"

namespace dottorrent
{

class v2_chunk_verifier : public chunk_verifier
{
    using base_type = chunk_verifier;
public:
    v2_chunk_verifier(file_storage& storage, std::size_t thread_count);

    const std::vector<std::uint8_t>& result() const noexcept override;

    const double percentage(std::size_t file_index) const noexcept override;

protected:
    void hash_chunk(std::vector<std::unique_ptr<hasher>>& hashers, const data_chunk& chunk) override;

    void hash_chunk(hasher& hasher, const data_chunk& chunk);

    void verify_piece_layers_and_root(hasher& hasher, std::size_t file_index);

private:
    std::vector<merkle_tree<hash_function::sha256>> merkle_trees_;
    /// Vector with the count of hashed bytes per file.per
    std::vector<std::atomic<std::size_t>> file_bytes_hashed_ {};
    std::vector<std::uint8_t> piece_map_;
    // index in piece_map where the pieces for each file start
    std::vector<std::size_t> file_offsets_;
};

}