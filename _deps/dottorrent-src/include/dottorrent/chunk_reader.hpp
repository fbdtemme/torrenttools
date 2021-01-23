#pragma once
#include <atomic>
#include <cstdint>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include <gsl-lite/gsl-lite.hpp>

#include "dottorrent/mpmcqueue.hpp"
#include "dottorrent/data_chunk.hpp"
#include "dottorrent/utils.hpp"
#include "dottorrent/file_storage.hpp"
#include "dottorrent/object_pool.hpp"

namespace dottorrent {

class chunk_reader
{
public:
    using chunk_type = data_chunk;
    using data_type = typename data_chunk::data_type ;
    using hash_queue = rigtorp::mpmc::Queue<chunk_type>;
    using checksum_queue = rigtorp::mpmc::Queue<chunk_type>;

    using hash_queue_vector = std::vector<std::shared_ptr<hash_queue>>;
    using checksum_queue_vector = std::vector<std::shared_ptr<checksum_queue>>;

    chunk_reader(file_storage& storage, std::size_t block_size, std::size_t max_memory);

    void register_hash_queue(std::shared_ptr<hash_queue> q);

    void register_checksum_queue(std::shared_ptr<checksum_queue> q);

    /// Start the worker threads
    void start();

    /// Signal the reader to stop reading new pieces.
    /// @note This does not join the thread and wait must be called after.
    void request_cancellation() noexcept;;

    /// Block until the reader completes all work or terminates because of a cancellation request.
    void wait();

    bool started() const noexcept;

    bool done() const noexcept;

    bool cancelled() const noexcept;

    std::size_t bytes_read() const noexcept;

    virtual ~chunk_reader();

protected:
    virtual void run() = 0;

    std::reference_wrapper<file_storage> storage_;
    std::size_t chunk_size_;
    pool::object_pool<data_type> pool_;
    hash_queue_vector hash_queues_ {};
    checksum_queue_vector checksum_queues_ {};
    std::jthread thread_;
    std::atomic<std::size_t> bytes_read_ = 0;
    std::atomic<bool> started_ = false;
    std::atomic<bool> cancelled_ = false;
};

} // namespace dottorrent