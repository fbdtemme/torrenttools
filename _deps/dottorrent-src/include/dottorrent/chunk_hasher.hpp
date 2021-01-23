#pragma once

#include "dottorrent/chunk_processor.hpp"

namespace dottorrent {

class chunk_hasher : public chunk_processor
{
public:
    explicit chunk_hasher(file_storage& storage,
                         std::vector<hash_function> hf,
                         std::size_t capacity,
                         std::size_t thread_count = 1)
            : storage_(storage)
            , threads_(thread_count)
            , queue_(std::make_shared<queue_type>(capacity))
            , hash_functions_(std::move(hf))
    { }

    chunk_hasher(const chunk_hasher& other) = delete;
    chunk_hasher(chunk_hasher&& other) = delete;

    chunk_hasher& operator=(const chunk_hasher& other) = delete;
    chunk_hasher& operator=(chunk_hasher&& other) = delete;

    /// Start the worker threads
    void start() override
    {
        Ensures(!started());
        Ensures(!cancelled());

        for (std::size_t i = 0; i < threads_.size(); ++i) {
            threads_[i] = std::jthread(&chunk_hasher::run, this, i);
        }
        started_.store(true, std::memory_order_release);
    }

    /// Signal the workers to shut down after finishing all pending work.
    /// No items can be added to the work queue after this call.
    void request_stop() override
    {
        rng::for_each(threads_, [](std::jthread& t) { t.request_stop(); });
    }

    /// Signal the workers to shut down and discard pending work.
    void request_cancellation() override
    {
        cancelled_.store(true, std::memory_order_relaxed);
        request_stop();
    }

    /// Block until all workers finish their execution.
    /// Note that workers will only finish after a call to stop or cancel.
    void wait() override
    {
        if (!started()) return;

        // Wake up all threads with a poison pill if they are blocked on pop()
        for (auto i = 0; i < 2 * threads_.size()+1; ++i) {
            queue_->push({
                    std::numeric_limits<std::uint32_t>::max(),
                    std::numeric_limits<std::uint32_t>::max(),
                    nullptr
            });
        }

        // should all be finished by now
        for (auto& t : threads_) {
            if (t.joinable()) {
                t.join();
            }
        }
    }

    bool running() const noexcept override
    {
        return started_.load(std::memory_order_relaxed) &&
                !cancelled_.load(std::memory_order_relaxed) &&
                !stopped_.load(std::memory_order_relaxed);
    }

    /// Return true if the thread has been started.
    auto started() const noexcept -> bool
    { return started_.load(std::memory_order_relaxed); }

    auto cancelled() const noexcept -> bool
    { return cancelled_.load(std::memory_order_relaxed); }

    /// Check if the hasher is completed or cancelled.
    auto done() const noexcept -> bool
    {
        return cancelled() || (started() && std::none_of(
                threads_.begin(), threads_.end(),
                [](const auto& t) { return t.joinable(); }));
    }

    std::shared_ptr<queue_type> get_queue() override
    { return queue_; }

    std::shared_ptr<const queue_type> get_queue() const override
    { return queue_; }

    /// Number of total bytes hashes.
    auto bytes_hashed() const noexcept -> std::size_t
    { return bytes_hashed_.load(std::memory_order_relaxed); }

    /// Number of bytes processed.
    auto bytes_done() const noexcept -> std::size_t
    { return bytes_done_.load(std::memory_order_relaxed); }

    virtual ~chunk_hasher() override = default;

protected:
    virtual void run(int thread_idx)
    {
        // copy the global hasher object to a per-thread hasher
        std::vector<std::unique_ptr<hasher>> hashers {};
        for (const auto f : hash_functions_) {
            hashers.push_back(std::move(make_hasher(f)));
        }

        data_chunk item {};
        auto stop_token = threads_[thread_idx].get_stop_token();

        // Process tasks until stopped is set
        while (!stop_token.stop_requested() && stop_token.stop_possible()) {
            queue_->pop(item);
            // check if the data_chunk is valid or a stop wake-up signal
            if (item.data == nullptr &&
                item.piece_index == std::numeric_limits<std::uint32_t>::max() &&
                item.file_index == std::numeric_limits<std::uint32_t>::max())
            {
                Ensures(stop_token.stop_requested());
                break;
            }

            hash_chunk(hashers, item);
            item.data.reset();
        }

        // finish pending tasks if the hasher is not cancelled,
        // otherwise discard all remaining work
        if (!cancelled_.load(std::memory_order_relaxed)) {
            while (queue_->try_pop(item)) {
                if (item.data == nullptr && item.piece_index == -1 && item.file_index == -1) {
                    break;
                }
                hash_chunk(hashers, item);
            }
        }
    }

    virtual void hash_chunk(std::vector<std::unique_ptr<hasher>>& hashers, const data_chunk& chunk) = 0;

protected:
    std::reference_wrapper<file_storage> storage_;
    std::vector<std::jthread> threads_;
    std::shared_ptr<queue_type> queue_;
    std::vector<hash_function> hash_functions_;

    std::atomic<bool> started_ = false;
    std::atomic<bool> cancelled_ = false;
    std::atomic<bool> stopped_ = false;

    // The amount of bytes that were actually hashed.
    std::atomic<std::size_t> bytes_hashed_ = 0;
    // How many pieces were processed (some pieces can be processed without hashing).
    std::atomic<std::size_t> bytes_done_ = 0;
};

} // namespace dottorrent