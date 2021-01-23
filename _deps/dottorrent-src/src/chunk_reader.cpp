#include "dottorrent/chunk_reader.hpp"

#include <bit>

namespace dottorrent {

chunk_reader::chunk_reader(file_storage& storage, std::size_t block_size, std::size_t max_memory)
        : storage_(storage)
        , chunk_size_(block_size)
        , pool_(max_memory / chunk_size_)
{
    Expects(storage.piece_size() >= 16_KiB);
    Expects(std::has_single_bit(storage.piece_size()));
    Expects(chunk_size_ % storage.piece_size() == 0);
    Expects(max_memory > chunk_size_);
}

void chunk_reader::register_hash_queue(std::shared_ptr<hash_queue> q)
{
    hash_queues_.push_back(std::move(q));
}

void chunk_reader::register_checksum_queue(std::shared_ptr<checksum_queue> q)
{
    checksum_queues_.push_back(std::move(q));
}

void chunk_reader::start() {
    if (started()) return;

    if (cancelled())
        throw std::runtime_error("cancelled");

    thread_ = std::jthread(&chunk_reader::run, this);
    started_.store(true, std::memory_order_release);
}

void chunk_reader::request_cancellation() noexcept { cancelled_.store(true, std::memory_order_relaxed); }

void chunk_reader::wait() {
    if (!started())
        throw std::logic_error("not started");

    if (thread_.joinable()) {
        thread_.join();
    }
}

bool chunk_reader::started() const noexcept
{
    return started_.load(std::memory_order_acquire);
}

bool chunk_reader::done() const noexcept
{
    return cancelled() || (started() && !thread_.joinable());
}

bool chunk_reader::cancelled() const noexcept
{
    return cancelled_.load(std::memory_order_acquire);
}

std::size_t chunk_reader::bytes_read() const noexcept
{
    return bytes_read_.load(std::memory_order_relaxed);
}

chunk_reader::~chunk_reader() {
    if (started() && !done()) {
        std::terminate();
    }
}

}