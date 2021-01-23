#pragma once
#include <vector>
#include <utility>
#include <cstdint>
#include <memory>
#include <cstddef>

namespace dottorrent {

/// Chunk of file data.
struct data_chunk
{
    /// A vector with bytes of data.
    using data_type = std::vector<std::byte>;
    /// Index of the first `piece size` bytes in data. [v1]
    /// The position in the file of the first byte in `data` divided by the piece_size. [v2]
    std::uint32_t piece_index{};
    /// Index of the file in the file_storage object.
    std::uint32_t file_index{};
    /// Variable length vector of bytes.
    std::shared_ptr<data_type> data{};
};

}