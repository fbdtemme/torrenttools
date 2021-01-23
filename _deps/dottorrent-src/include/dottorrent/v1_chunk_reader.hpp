#pragma once

#include <algorithm>
#include <fstream>

#include "dottorrent/chunk_reader.hpp"

namespace dottorrent {


class v1_chunk_reader : public chunk_reader
{
public:
    using chunk_reader::chunk_reader;

    void run() final;

private:
    void handle_missing_file();

    /// @param file_idx: index of the file the data is read from,
    ///     when a chunk consists of data from multiple files,
    ///     the index of the first part as given by file_offset.
    void push(const data_chunk& data_chunk);

    // index of the first piece in a chunk
    std::size_t piece_index_ = 0;
    // position of the first free byte in the current chunk
    std::size_t chunk_offset_ = 0;
    // position of the first byte from a different file than the
    // previous byte in the current chunk. A file offset of zero
    // indicates all data is from a single file.
    std::size_t offset_index_ = 0;
    // vector of all offsets in the current chunk.
    std::vector<std::size_t> file_offsets_ {};
    // index of the current file being read in the storage object
    std::size_t file_index_ = 0;
    // the current file being read
    std::ifstream f_;
    // the current chunk being filled
    std::shared_ptr<data_type> chunk_;

};
}