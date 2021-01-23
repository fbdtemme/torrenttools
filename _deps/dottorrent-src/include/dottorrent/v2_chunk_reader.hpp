#pragma once
#include <fstream>

#include "dottorrent/chunk_reader.hpp"

namespace dottorrent {


/// Chunk reader suitable for v2 and hybrid torrents.
class v2_chunk_reader : public chunk_reader
{
public:
    using chunk_reader::chunk_reader;

    void run() final;

private:
    /// @param file_idx: index of the file the data is read from,
    ///     when a chunk consists of data from multiple files,
    ///     the index of the first part as given by file_offset.
    void push(const data_chunk& chunk);

    std::size_t piece_index_ = 0;
    // size of the current chunk, less then chunk_size_ for files smaller then `chunk_size_`
    std::size_t current_chunk_size_ = 0;
    // index of the current file being read in the storage object
    std::size_t file_index_ = 0;
    // the current file being read, disable read buffer
    std::ifstream f_;
};

}