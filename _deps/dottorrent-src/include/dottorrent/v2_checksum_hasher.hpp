#pragma once
#include <vector>
#include <memory>
#include <atomic>
#include <thread>

#include "dottorrent/hash_function.hpp"
#include "dottorrent/hash.hpp"
#include "dottorrent/checksum.hpp"

#include <dottorrent/object_pool.hpp>
#include "dottorrent/file_storage.hpp"
#include "dottorrent/data_chunk.hpp"
#include "dottorrent/hash_function_traits.hpp"
#include "dottorrent/chunk_hasher.hpp"

#include "dottorrent/hasher/factory.hpp"

namespace dottorrent {

/// Only useful for v1 torrents.
/// Per file merkle root checksums are in the base v2 spec.


class v2_checksum_hasher : public chunk_hasher
{
    using base_type = chunk_hasher;
public:

    explicit v2_checksum_hasher(file_storage& storage, hash_function f, std::size_t capacity);

protected:
    void hash_chunk(std::vector<std::unique_ptr<hasher>>& hashers, const data_chunk& chunk);

    void hash_chunk(hasher& hasher, const data_chunk& item);

private:
    std::atomic<std::size_t> current_file_index_ = 0;
    std::atomic<std::size_t> current_file_size_ = 0;
    std::atomic<std::size_t> current_file_data_hashed_ = 0;
};



}