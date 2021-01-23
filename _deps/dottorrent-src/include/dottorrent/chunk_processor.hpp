#pragma once
#include <vector>
#include <memory>
#include <atomic>
#include <thread>
#include <ranges>

#include <gsl-lite/gsl-lite.hpp>
#include "dottorrent/mpmcqueue.hpp"
#include "dottorrent/hash.hpp"
#include "dottorrent/file_storage.hpp"
#include "dottorrent/data_chunk.hpp"

namespace dottorrent {

namespace rng = std::ranges;

class chunk_processor
{
public:
    using chunk_type = data_chunk;
    using queue_type = rigtorp::mpmc::Queue<chunk_type>;

    /// Start the worker threads
    virtual void start() = 0;

    /// Signal the workers to shut down after finishing all pending work.
    /// No items can be added to the work queue after this call.
    virtual void request_stop() = 0;

    /// Signal the workers to shut down and discard pending work.
    virtual void request_cancellation() = 0;

    /// Block until all workers finish their execution.
    /// Note that workers will only finish after a call to stop or cancel.
    virtual void wait() = 0;

    virtual bool running() const noexcept = 0;

    /// Return true if the thread has been started.
    virtual bool started() const noexcept = 0;

    virtual bool cancelled() const noexcept = 0;

    /// Check if the hasher is completed or cancelled.
    virtual bool done() const noexcept = 0;

    virtual std::shared_ptr<queue_type> get_queue() = 0;

    virtual std::shared_ptr<const queue_type> get_queue() const = 0;

    /// Number of total bytes hashes.
    virtual std::size_t bytes_hashed() const noexcept = 0;

    /// Number of bytes processed.
    virtual std::size_t bytes_done() const noexcept = 0;

    virtual ~chunk_processor() = default;
};

} // namespace dottorrent