#pragma once

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <mutex>
#include <numeric>
#include <optional>
#include <string>
#include <thread>
#include <vector>
#include <unordered_set>

#include <gsl-lite/gsl-lite.hpp>

#include "dottorrent/literals.hpp"
#include "dottorrent/file_entry.hpp"
#include "dottorrent/file_storage.hpp"

#include "dottorrent/data_chunk.hpp"
#include "dottorrent/hash.hpp"
#include "dottorrent/checksum.hpp"
#include "dottorrent/chunk_reader.hpp"
#include "dottorrent/chunk_hasher.hpp"



namespace dottorrent {

namespace fs = std::filesystem;
using namespace dottorrent::literals;
using namespace std::chrono_literals;


/// Options to control the memory usage of a storage_hasher.
struct storage_hasher_options
{
    /// The bittorrent procol version to create a metafile for.
    protocol protocol_version = protocol::v1;
    /// The per file checksums to include in the file list.
    std::unordered_set<hash_function> checksums = {};
    /// The minimum size of a block to read from disk.
    /// For piece sizes smaller than the min_chunk_size multiple pieces
    /// will be read in a single block for faster disk I/O.
    std::size_t min_chunk_size = 1_MiB;
    /// Max size of all file chunks in memory
    std::size_t max_memory = 128_MiB;

    /// Number of threads to hash pieces.
    /// Total number of threads will be equal to:
    /// 1 main thread + 1 reader + <thread> piece hashers + <#checksums types> checksum hashers
    std::size_t threads = 2;
};



class storage_hasher
{
public:
    /// Options to control the memory usage of a storage_hasher.
    struct memory_options
    {
        std::size_t min_chunk_size;
        std::size_t max_memory;
    };

    explicit storage_hasher(
                file_storage& storage,
                const storage_hasher_options& options = {});

    /// Return a reference to the file_storage object.
    file_storage& storage();

    const file_storage& storage() const;

    enum protocol protocol() const noexcept;

    void start();

    void cancel();

    /// Wait for all threads to complete
    void wait();

    bool running() const noexcept;

    bool started() const noexcept;

    bool cancelled() const noexcept;

    /// Check if the hasher is still processing tasks or is just waiting.
    bool done() const noexcept;

    /// The number of bytes read from disk by the reader thread.
    std::size_t bytes_read() const noexcept;

    /// Return the number of bytes hashed by the v1 or v2 hasher,
    /// or the average between the two for hybrid torrents.
    std::size_t bytes_hashed() const noexcept;

    /// Return the number of bytes hashed by the v1 or v2 hasher,
    /// or the average between the two for hybrid torrents.
    std::size_t bytes_done() const noexcept;

    /// Get information about the file currently being hashed.
    /// @returns A pair with a pointer to the file_entry and
    ///     the number of bytes hashed for this file
    struct file_progress_data
    {
        std::size_t file_index;
        std::size_t bytes_hashed;
    };

    file_progress_data current_file_progress() const noexcept;

private:
    std::reference_wrapper<file_storage> storage_;
    enum protocol protocol_;
    std::unordered_set<hash_function> checksums_;
    memory_options memory_;
    std::size_t threads_;

    std::unique_ptr<chunk_reader> reader_;
    std::unique_ptr<chunk_processor> hasher_;
    std::vector<std::unique_ptr<chunk_processor>> checksum_hashers_;

    bool started_ = false;
    bool stopped_ = false;
    bool cancelled_ = false;

    // progress info
    mutable std::size_t current_file_index_ = 0;
    std::vector<std::size_t> cumulative_file_size_;
};

} // namespace dottorrent