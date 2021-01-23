#pragma once
#include <atomic>
#include <chrono>
#include <functional>
#include <thread>
#include <condition_variable>
#include <mutex>

//std::experimental::net::steady_timer

namespace cliprogress::detail {

using namespace std::chrono_literals;

/// A thread that cycles through an index range [0...frame_count].
/// The index is updated every `1/interval` seconds.
/// Note that this does not scale since every timer uses a single thread.
class periodic_timer
{
public:
    using duration_type = std::chrono::steady_clock::duration;

    periodic_timer() = default;

    periodic_timer(const periodic_timer&) = delete;

    template <typename Rep, typename Period, typename F>
    periodic_timer(std::chrono::duration<Rep, Period> interval, F&& function)
            : interval_(std::chrono::duration_cast<duration_type>(interval))
            , function_(std::forward<F>(function))
    {}

    template <typename Rep, typename Period>
    void set_interval(std::chrono::duration<Rep, Period> interval)
    {
        interval_.store(
                std::chrono::duration_cast<std::chrono::milliseconds>(interval),
                std::memory_order_relaxed);
    }

    /// Stops the periodic timer af a specific dueation
    template <typename Rep, typename Period>
    void set_timout(std::chrono::duration<Rep, Period> duration)
    {
        interval_.store(
                std::chrono::duration_cast<duration_type>(duration),
                std::memory_order_relaxed);
    }

    template <typename F>
    void set_function(F&& function)
    {
        pause();
        function_ = function;
        resume();
    }

    bool is_running() const noexcept
    { return thread_.joinable(); }

    /// Start the spinner. Does nothing if the spinner is already running.
    void start()
    {
        thread_ = std::thread(&periodic_timer::run, this);
    }

    /// Stop the timer.
    void stop()
    {
        if (!thread_.joinable()) return;
        stop_flag_.store(true, std::memory_order_relaxed);

        // Wake the thread if we are currently paused by clearing interrupt_flag
        // Or break from timer loop by setting interrupt flag
        if (interrupt_flag_.load(std::memory_order_relaxed)) {
            interrupt_flag_.store(false, std::memory_order_relaxed);
        } else {
            interrupt_flag_.store(true, std::memory_order_relaxed);
        }
        resume_cv_.notify_one();
        thread_.detach();
    };

    /// Pause the timer.
    void pause() noexcept
    {
        if (!thread_.joinable()) return;
        interrupt_flag_.store(true, std::memory_order_relaxed);
        resume_cv_.notify_one();
    }

    /// Resume the timer.
    void resume() noexcept
    {
        if (!thread_.joinable()) return;
        interrupt_flag_.store(false, std::memory_order_relaxed);
        resume_cv_.notify_one();
    };

private:
    void run()
    {
        bool has_timout = timeout_.load().count() != 0;
        bool exit = false;
        std::unique_lock lck {resume_mutex_, std::defer_lock};
        auto starttime_ = std::chrono::system_clock::now();

        while (!exit) {
            while (!interrupt_flag_.load(std::memory_order_relaxed)) {
                // calculate next wakeup before executing the function
                // to take callback runtime into account
                auto next_wakeup = std::chrono::system_clock::now()
                        + interval_.load(std::memory_order_relaxed);

                if (function_)
                    std::invoke(function_);

                resume_cv_.wait_until(lck, next_wakeup,
                        [&]() noexcept {
                            return interrupt_flag_.load(std::memory_order_relaxed);
                        } );

                // check if we exceeded timeout
                if (has_timout && (std::chrono::system_clock::now() - starttime_ >=
                                   timeout_.load(std::memory_order_relaxed)))
                {
                    return;
                }
            }

            // Check if stop was requested
            if (stop_flag_.load(std::memory_order_relaxed)) {
                return;
            }
            // wait until interrupt flag is cleared
            else if (interrupt_flag_.load(std::memory_order_relaxed)) {
                resume_cv_.wait(lck,
                        [&]() noexcept {
                            return !interrupt_flag_.load(std::memory_order_relaxed);
                        } );
                // Check if stop was requested before continuing to main loop
                if (stop_flag_.load(std::memory_order_relaxed)) {
                    return;
                }
            }
        }
    }

private:
    std::thread thread_ {};
    std::atomic_bool interrupt_flag_ = false;
    std::atomic_bool stop_flag_ = false;
    std::condition_variable resume_cv_ {};
    std::mutex resume_mutex_ {};
    std::atomic<duration_type> interval_ {1s};
    std::atomic<duration_type> timeout_ {0ns};
    std::function<void()> function_ {};
};

} // namespace cliprogress::detail