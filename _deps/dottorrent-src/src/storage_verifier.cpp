//
// Created by fbdtemme on 9/11/20.
//
#include <bit>
#include "dottorrent/v1_chunk_verifier.hpp"
#include "dottorrent/storage_verifier.hpp"


namespace dottorrent {

storage_verifier::storage_verifier(file_storage& storage, const storage_verifier_options& options)
        : storage_(storage)
        , protocol_(options.protocol_version)
        , checksums_(options.checksums)
        , memory_({.min_chunk_size=options.min_chunk_size, .max_memory=options.max_memory})
        , threads_(options.threads)
{
    if (storage.piece_size() == 0)
        storage.set_piece_size(choose_piece_size(storage));

    Expects(protocol_ != dottorrent::protocol::none);
    Expects(std::has_single_bit(storage.piece_size()));    // is a power of 2
    Expects(storage.has_root_directory());

    if (protocol_ == protocol::v1) {
        cumulative_file_size_ = inclusive_file_size_scan_v1(storage);
    }
    else {
        cumulative_file_size_ = inclusive_file_size_scan_v2(storage);
    }
}

file_storage& storage_verifier::storage()
{
    return storage_.get();
}

const file_storage& storage_verifier::storage() const
{
    return storage_;
}

enum protocol storage_verifier::protocol() const noexcept
{
    return protocol_;
}


void storage_verifier::start() {
    if (done())
        throw std::runtime_error("cannot start finished or cancelled hasher");

    auto& storage = storage_.get();
    auto chunk_size = std::max(memory_.min_chunk_size, storage.piece_size());

    if (protocol_ == protocol::v1) {
        reader_ = std::make_unique<v1_chunk_reader>(storage_, chunk_size, memory_.max_memory);
    }
    else {
        reader_ = std::make_unique<v2_chunk_reader>(storage_, chunk_size, memory_.max_memory);
    }

    // add piece hashers and register them with the reader

    if (protocol_ == protocol::v1) {
        verifier_ = std::make_unique<v1_chunk_verifier>(storage_, threads_);
    }
    else {
        verifier_ = std::make_unique<v2_chunk_verifier>(storage_, threads_);
    }
    reader_->register_hash_queue(verifier_->get_queue());

    // start all parts
    verifier_->start();
    reader_->start();
    started_ = true;
}

void storage_verifier::cancel() {
    if (done()) {
        throw std::runtime_error(
                "cannot cancel finished or cancelled hasher");
    }
    if (!started_) {
        cancelled_ = true;
        return;
    }

    // cancel all tasks
    reader_->request_cancellation();
    verifier_->request_cancellation();

    // wait for all tasks to complete
    reader_->wait();
    verifier_->wait();

    cancelled_ = true;
    stopped_ = true;
}

void storage_verifier::wait()
{
    if (!started())
        throw std::runtime_error("hasher not running");
    if (done())
        throw std::runtime_error("hasher already done");

    reader_->wait();
    // no pieces will be added after the reader finishes.
    // So we can signal hashers that they can shutdown after
    // finishing all remaining work
    verifier_->request_stop();
    verifier_->wait();

    stopped_ = true;
}

bool storage_verifier::running() const noexcept
{
    return started_ && !cancelled_ && !stopped_;
}

bool storage_verifier::started() const noexcept
{
    return started_;
}

bool storage_verifier::cancelled() const noexcept
{
    return cancelled_;
}

bool storage_verifier::done() const noexcept
{
    return cancelled_ || (started_ && stopped_);
}

std::size_t storage_verifier::bytes_read() const noexcept
{
    return reader_->bytes_read();
}

std::size_t storage_verifier::bytes_hashed() const noexcept
{
    return verifier_->bytes_hashed();
}

std::size_t storage_verifier::bytes_done() const noexcept
{
    return verifier_->bytes_done();
}


storage_verifier::file_progress_data storage_verifier::current_file_progress() const noexcept
{
    const auto& storage = storage_.get();
    const auto bytes = bytes_done();

    auto it = std::lower_bound(
            std::next(cumulative_file_size_.begin(), current_file_index_),
            cumulative_file_size_.end(),
            bytes);
    auto index = std::distance(cumulative_file_size_.begin(), it);
    current_file_index_ = static_cast<std::size_t>(index);

    if (index == 0) {
        return {current_file_index_, bytes};
    }
    else {
        return {current_file_index_, bytes-*std::next(it, - 1)};
    }
}

const std::vector<std::uint8_t>& storage_verifier::result() const noexcept
{
    return verifier_->result();
}

const double storage_verifier::percentage(std::size_t file_index) const noexcept
{
    return verifier_->percentage(file_index);
}

const double storage_verifier::percentage(const file_entry& entry) const
{
    file_storage storage = storage_;

    std::size_t file_index = std::distance(storage.begin(), rng::find(storage, entry));
    if (file_index == storage.file_count())
        throw std::invalid_argument("entry does not exist in storage");

    return verifier_->percentage(file_index);
}


} //namespace dottorrent