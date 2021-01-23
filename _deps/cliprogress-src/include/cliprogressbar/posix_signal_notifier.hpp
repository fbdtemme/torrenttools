#if defined(_WIN32) || defined(__MINGW64__)
#else

#pragma once
#include <functional>
#include <thread>

#include <iostream>
#include <unistd.h>

#include <sys/epoll.h>
#include <sys/signalfd.h>
#include <sys/eventfd.h>
#include <csignal>

namespace cliprogress {

/// Register callbacks for POSIX signals.
/// Signals that have registered callbacks are delivered to a file descriptor that is polled
/// using epoll.
class posix_signal_notifier
{
private:
    posix_signal_notifier() noexcept
    {
        // Initialize the signal set
        sigemptyset(&signal_set_);
    }

public:
    void connect(int signum, std::function<void()> callback)
    {
        sigaddset(&signal_set_, signum);
        callbacks_[signum].push_back(std::move(callback));
    }

    void disconnect(int signum)
    {
        sigdelset(&signal_set_, signum);
        callbacks_.erase(signum);
    }

    /// Block all signals in the current mask from being delivered to the calling thread.
    sigset_t set_sigmask()
    {
        sigset_t old;
        if (pthread_sigmask(SIG_SETMASK, &signal_set_, &old) < 0) {
            throw std::system_error(errno, std::generic_category());
        }
        return old;
    }

    /// MUST BE CALLED FROM THE MAIN THREAD!
    /// If not, the signals processed by the signal handler must be manually blocked from
    /// each thread created before the signal notifier was started!
    void start()
    {
        // Block the signals thet we handle using signalfd(), so they don't
        // cause signal handlers or default signal actions to execute.
        old_signal_set_ = set_sigmask();

        notify_fd_ = eventfd(0, 0);
        if (notify_fd_ < 0)
            throw std::system_error(errno, std::generic_category());

        thread_ = std::jthread([this](std::stop_token token) { this->run(token); });
    }

    void request_stop()
    {
        const uint64_t count = 1;

        if (thread_.get_stop_source().stop_possible()) {
            thread_.request_stop();
            auto r = write(notify_fd_, &count, sizeof(count));
            if (r < 0) {
                throw std::system_error(errno, std::generic_category());
            }
        }
    }

    /// Wait for the
    void wait()
    {
        if (thread_.joinable())
            thread_.join();
    }

    ~posix_signal_notifier() {
        request_stop();
        wait();
    }

private:
    void run(const std::stop_token& token)
    {
        int sfd = 0;
        int epfd = 0;

        try {
            // Create a file descriptor from which we will read the signals.
            sfd = signalfd(-1, &signal_set_, 0);
            // Create a file descriptor from which we will signal to stop waiting.
            if (sfd < 0) throw std::system_error(errno, std::generic_category());

            // Setup polling for events from signalfd.
            epfd = epoll_create1(0);

            epoll_event signal_event {};
            signal_event.data.fd = sfd;
            signal_event.events = EPOLLIN;
            epoll_ctl(epfd, EPOLL_CTL_ADD, sfd, &signal_event);

            // Setup polling for events from eventfd.

            epoll_event notify_event {};
            notify_event.data.fd = notify_fd_;
            notify_event.events = EPOLLIN;

            epoll_ctl(epfd, EPOLL_CTL_ADD, notify_fd_, &notify_event);

            constexpr int max_events = 4;
            signalfd_siginfo signal_info{};
            epoll_event incoming_events[max_events];

            while (!token.stop_requested()) {
                // wait for a signal or event to become available
                auto num_ready = epoll_wait(epfd, incoming_events, max_events, -1);
                if (num_ready == -1) {
                    if (errno == EINTR) continue;
                    throw std::system_error(errno, std::generic_category());
                }

                // Read signal info from the signal fd.

                for (auto idx = 0; num_ready > 0; ++idx, --num_ready) {
                    // do not read when receiving notify events
                    if (incoming_events[idx].data.fd == notify_fd_) {
                        continue;
                    }

                    auto r = read(sfd, &signal_info, sizeof(signalfd_siginfo));
                    if (r == -1) {
                        if (errno == EINTR) continue;
                        throw std::system_error(errno, std::generic_category());
                    }

                    // dispatch events to their handlers
                    auto& cb = callbacks_[signal_info.ssi_signo];
                    std::for_each(cb.begin(), cb.end(), [] (const auto& c){ std::invoke(c); });
                }
            }
        }
        catch (...) {
            close(epfd);
            close(sfd);
            sigprocmask(SIG_SETMASK, &old_signal_set_, nullptr);
            throw;
        }
        close(epfd);
        close(sfd);
        sigprocmask(SIG_SETMASK, &old_signal_set_, nullptr);
    }

    std::jthread thread_;
    sigset_t signal_set_ {};
    sigset_t old_signal_set_ {};
    int notify_fd_ = 0;
    std::unordered_map<int, std::vector<std::function<void()>>> callbacks_ {};
    inline static std::shared_ptr<posix_signal_notifier> instance_;

    friend std::shared_ptr<posix_signal_notifier> get_posix_signal_notifier();
};


/// Singleton factory function
inline std::shared_ptr<posix_signal_notifier> get_posix_signal_notifier()
{
    auto& instance = posix_signal_notifier::instance_;
    if (!instance) {
        instance = std::shared_ptr<posix_signal_notifier>(new posix_signal_notifier());
    }
    return instance;
}

}

#endif