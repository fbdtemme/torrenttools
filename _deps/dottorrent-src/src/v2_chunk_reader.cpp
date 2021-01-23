#include "dottorrent/v2_chunk_reader.hpp"

#include <fstream>

namespace dottorrent {

void v2_chunk_reader::run()
{
    const auto file_paths = absolute_file_paths(storage_);
    // index of the first piece in a chunk

    auto& storage = storage_.get();
    const auto piece_size = storage.piece_size();
    const auto pieces_per_chunk = chunk_size_ / piece_size;
    auto chunk = pool_.get();
    chunk->resize(chunk_size_);

    for (const fs::path& file_path: file_paths) {
        const file_entry& file_entry = storage.at(file_index_);

        if (file_entry.is_padding_file()) {
            bytes_read_.fetch_add(file_entry.file_size(), std::memory_order_relaxed);
            ++file_index_;
            continue;
        }

        // handle pieces if the file does not exists. Used when verifying torrents.
        if (!fs::exists(file_path)) {
            auto file_size = file_entry.file_size();
            push({static_cast<std::uint32_t>(0),
                  static_cast<std::uint32_t>(file_index_),
                  nullptr});
            bytes_read_.fetch_add(file_size, std::memory_order_relaxed);
            ++file_index_;
            continue;
        }

        // set last modified date in the file entry of the storage
        storage.set_last_modified_time(file_index_, fs::last_write_time(file_path));
        f_.open(file_path, std::ios::binary);
        // reset piece index. piece index is per file for v2!
        piece_index_ = 0;

        while (!cancelled_.load(std::memory_order_relaxed)) {
            f_.read(reinterpret_cast<char*>(chunk->data()), chunk_size_);
            current_chunk_size_ = f_.gcount();
            Expects(current_chunk_size_ > 0);

            chunk->resize(current_chunk_size_);
            bytes_read_.fetch_add(current_chunk_size_, std::memory_order_relaxed);

            // push chunk to consumers
            push({static_cast<std::uint32_t>(piece_index_),
                  static_cast<std::uint32_t>(file_index_),
                  std::move(chunk)});
            // recycle a new chunk from the pool
            chunk = pool_.get();
            chunk->resize(chunk_size_);
            // clear chunk and file offsets
            // increment chunk index
            piece_index_ += pieces_per_chunk;

            if (f_.eof()) break;
        }
        f_.close();
        f_.clear();
        ++file_index_;
    }
}


void v2_chunk_reader::push(const data_chunk& chunk) {
    Expects(0 <= chunk.piece_index && chunk.piece_index < storage_.get().piece_count());

    // hasher queues
    for (auto& queue : hash_queues_) {
        queue->push(chunk);
    }
    for (auto& queue: checksum_queues_) {
        queue->push(chunk);
    }
}

}