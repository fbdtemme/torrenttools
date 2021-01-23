#pragma once
#include <vector>
#include <memory>
#include <atomic>
#include <thread>

#include <gsl-lite/gsl-lite.hpp>

#include "dottorrent/hash_function.hpp"
#include "dottorrent/hash.hpp"
#include "dottorrent/checksum.hpp"

#include "dottorrent/file_storage.hpp"
#include "dottorrent/data_chunk.hpp"
#include "dottorrent/hash_function_traits.hpp"
#include "dottorrent/chunk_processor.hpp"
#include "dottorrent/hasher/factory.hpp"

namespace dottorrent {

class v1_checksum_hasher : public chunk_hasher {
public:

    explicit v1_checksum_hasher(file_storage& storage, hash_function f, std::size_t capacity);

protected:
    void hash_chunk(std::vector<std::unique_ptr<hasher>>& hashers, const data_chunk& chunk) override;

    void hash_chunk(hasher& hasher, const data_chunk& item);

private:
    std::atomic<std::size_t> current_file_index_ = 0;
    std::atomic<std::size_t> current_file_size_ = 0;
    std::atomic<std::size_t> current_file_data_hashed_ = 0;
};

} // namespace dottorrent