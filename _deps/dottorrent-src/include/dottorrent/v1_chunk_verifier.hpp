#pragma once

#include "dottorrent/chunk_verifier.hpp"
#include "dottorrent/v1_chunk_hasher.hpp"

namespace dottorrent
{

class v1_chunk_verifier : public chunk_verifier
{
public:
    using base = v1_chunk_hasher;

    v1_chunk_verifier(file_storage& storage, std::size_t thread_count);

    const std::vector<std::uint8_t>& result() const noexcept override;

    const double percentage(std::size_t file_index) const noexcept override;

protected:
    void hash_chunk(std::vector<std::unique_ptr<hasher>>& hashers, const data_chunk& chunk) override;

    void hash_chunk(hasher& hasher, const data_chunk& chunk);

    void process_piece_hash(std::size_t piece_idx, std::size_t file_idx, const sha1_hash& piece_hash);

private:
    std::vector<std::uint8_t> piece_map_;
};

}