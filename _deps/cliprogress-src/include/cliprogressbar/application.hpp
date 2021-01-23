#pragma once
#include <atomic>
#include <concepts>
#include <map>
#include <memory>

#include "cliprogressbar/progress_data.hpp"
#include "cliprogressbar/events/event.hpp"
#include "cliprogressbar/event_queue.hpp"
#include "cliprogressbar/widget.hpp"
#include "cliprogressbar/periodic_timer.hpp"
#include "cliprogressbar/terminal_writer.hpp"

#if defined(_WIN32) || defined(__MINGW64__)
#else
#include "cliprogressbar/posix_signal_notifier.hpp"
#endif

namespace cliprogress {


/// The root progressbar
class application
{
public:
    application();

    application(std::ostream& os);

    void queue_event(std::shared_ptr<event> e, std::weak_ptr<widget> receiver = {});

    /// Enter main event loop.s
    void start();

    /// Block until the application quits after a call to request_stop().
    void wait();

    /// Stops the event loop.
    void request_stop();

    std::weak_ptr<widget> add_widget(std::shared_ptr<widget> w);

    void remove_widget(const widget* w);

    void remove_widget(const std::shared_ptr<widget>& w);

    /// Add a widget id to the register
    void register_widget_id(std::size_t id, std::weak_ptr<widget> root);

    void unregister_widget_id(std::size_t id);

    terminal_writer& writer();

    /// Return the singe application instance.
    static application* instance() noexcept;

    ~application() noexcept;

private:
    void register_signals();

    void resize_handler();

    void event_loop();

    void process_event(event_item& item);

    template <typename Fn>
    void for_each_widget(Fn&& fn)
    {
        std::unique_lock lck(widgets_mutex_);
        std::for_each(widgets_.begin(), widgets_.end(), fn);
    }

    std::jthread event_loop_;
    std::atomic<termcontrol::terminal_size> term_size_;
    terminal_writer writer_ {};
    event_queue event_queue_;

#if defined(_WIN32) || defined(__MINGW64__)
#else
    std::shared_ptr<posix_signal_notifier> signal_notifier_ {};
#endif

    std::mutex widgets_mutex_;
    std::list<std::shared_ptr<widget>> widgets_ {};

    detail::periodic_timer frame_clock_;
    detail::periodic_timer terminal_size_clock_;

    std::mutex widget_register_mutex_;
    std::map<std::size_t, std::weak_ptr<widget>> widget_register_;

    inline static std::atomic<application*> instance_;

};
}