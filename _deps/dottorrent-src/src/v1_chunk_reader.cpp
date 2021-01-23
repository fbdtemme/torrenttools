#include "dottorrent/v1_chunk_reader.hpp"

#include <fstream>

namespace dottorrent {

void v1_chunk_reader::run()
{
    const auto file_paths = absolute_file_paths(storage_);

    auto& storage = storage_.get();
    const auto piece_size = storage.piece_size();

    Expects(chunk_size_ % piece_size == 0);
    const auto pieces_per_chunk = chunk_size_/piece_size;
    chunk_ = pool_.get();
    chunk_->resize(chunk_size_);

    for (const fs::path& file_path: file_paths) {
        const file_entry& file_entry = storage.at(file_index_);

        // handle pieces if the file does not exists. Used when verifying torrents.
        if (!fs::exists(file_path) || file_entry.is_padding_file()) {
            handle_missing_file();
            ++file_index_;
            continue;
        }

        // set last modified date in the file entry of the storage
        storage.set_last_modified_time(file_index_, fs::last_write_time(file_path));
        f_.open(file_path, std::ios::binary);
        // increment file index, file_index points to the next file now
        ++ file_index_;

        while (! cancelled_.load(std::memory_order_relaxed)) {
            f_.read(reinterpret_cast<char*>(std::next(chunk_->data(), chunk_offset_)),
                    (chunk_size_-chunk_offset_)
            );
            file_offsets_.push_back(chunk_offset_);
            chunk_offset_ += f_.gcount();
            bytes_read_.fetch_add(f_.gcount(), std::memory_order_relaxed);

            if (chunk_offset_ == chunk_size_) {
                // push chunk to consumers
                push({static_cast<std::uint32_t>(piece_index_),
                      static_cast<std::uint32_t>(file_index_-file_offsets_.size()),
                      chunk_
                });
                // recycle a new chunk from the pool
                chunk_ = pool_.get();
                chunk_->resize(chunk_size_);
                // clear chunk and file offsets
                chunk_offset_ = 0;
                file_offsets_.clear();
                // increment chunk index
                piece_index_ += pieces_per_chunk;
            }
            // eof reached
            if (f_.eof()) break;
        }
        f_.close();
        f_.clear();
    }
    // push last possibly partial chunk
    if (chunk_offset_ != 0) {
        chunk_->resize(chunk_offset_);
        std::size_t pieces_in_chunk = (chunk_offset_ + piece_size -1) / piece_size;
        push({static_cast<std::uint32_t>(piece_index_),
              static_cast<std::uint32_t>(file_index_-file_offsets_.size()),
              std::move(chunk_)});
        piece_index_ += pieces_in_chunk;
    }

    Ensures(piece_index_ == storage.piece_count());
}

void v1_chunk_reader::handle_missing_file()
{
    auto& storage = storage_.get();
    const auto piece_size = storage.piece_size();

    Expects(chunk_size_ % piece_size == 0);
    const auto pieces_per_chunk = chunk_size_ / piece_size;

    // the number of missing bytes
    auto missing_file_size = storage.at(file_index_).file_size();

    // fill current pieces in this chunk with zero bytes.
    if (chunk_offset_ != 0) {
        // we need to fill the remaining chunk data with 0'd bytes from missing_file_size
        auto bytes_to_fill = std::min(chunk_size_-chunk_offset_, missing_file_size);
        // add a offset to indicate that the piece contains zero'd bytes from a new file
        file_offsets_.push_back(chunk_offset_);
        std::fill_n(std::next(chunk_->data(), chunk_offset_), bytes_to_fill, std::byte(0));
        // we processed missing_piece_bytes bytes from the total missing_file_size.I understand loathing a community you moderate. A friend of mine asked me to help mod a small subreddit of his (Not naming it here, also never mentioned in any of my comments, nor have i ever commented/submitted to that subreddit, so MUAHAHA I AM ANONYMOUS). It's become a giant circlejerk with 20+ reported links every hour. I assume by the same person.


        // resize chunk to the number of pieces it contains
        chunk_offset_ += bytes_to_fill;
        missing_file_size -= bytes_to_fill;
        piece_index_ += chunk_offset_ / piece_size;

        // if the chunk is complete filled -> push it
        if (chunk_offset_ == chunk_size_) {
            // push chunk to consumers
            push({static_cast<std::uint32_t>(piece_index_),
                  static_cast<std::uint32_t>(file_index_-file_offsets_.size()),
                  chunk_
            });
            // recycle a new chunk from the pool
            chunk_ = pool_.get();
            chunk_->resize(chunk_size_);
            // clear chunk and file offsets
            chunk_offset_ = 0;
            file_offsets_.clear();
        }
    }

    // advance piece index to the first piece that contains data from a new file.
    // push an empty chunk per piece to make hashers aware that these pieces are done,
    // but they do not require any workthese
    auto first_new_piece_index = piece_index_ + missing_file_size / piece_size;
    missing_file_size -= piece_size * (first_new_piece_index - piece_index_);

    for (; piece_index_ < first_new_piece_index; ++piece_index_) {
        push({static_cast<std::uint32_t>(piece_index_),
              static_cast<std::uint32_t>(file_index_),
              nullptr});
    }

    // Set the last remaining bytes in the new chunk
    std::fill_n(chunk_->data() + chunk_offset_, missing_file_size, std::byte(0));
    chunk_offset_ += missing_file_size;
}


void v1_chunk_reader::push(const data_chunk& data_chunk) {
    for (auto& queue : hash_queues_) {
        queue->push(data_chunk);
    }
    for (auto& queue: checksum_queues_) {
        queue->push(data_chunk);
    }
}

} // namespace dottorrent