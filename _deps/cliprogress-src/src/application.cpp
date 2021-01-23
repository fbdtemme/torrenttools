#include <cliprogressbar/application.hpp>
#include <chrono>
#include <termcontrol/detail/definitions.hpp>
#include <termcontrol/detail/format.hpp>


namespace cliprogress {

using namespace std::chrono_literals;
namespace tc = termcontrol;

application::application()
    : application(std::cout)
{}

application::application(std::ostream& os)
        : event_queue_(128)
#if defined(_WIN32) || defined(__MINGW64__)
#else
        , signal_notifier_(get_posix_signal_notifier())
#endif
        , term_size_(tc::get_terminal_size())
        , writer_(os)
{
    instance_ = this;
    // disable line wrapping
    writer().write(tc::format<tc::def::reset_mode>(tc::dec_mode::autowrap));
}



void application::queue_event(std::shared_ptr<event> e, std::weak_ptr<widget> destination)
{
    event_item p {.value=std::move(e), .destination=std::move(destination)};
    event_queue_.push(std::move(p));
}

void application::start()
{
    if (event_loop_.joinable()) return;

    // register event queue to the signal notifier.
    // start dispatching signals to the event queue.

    register_signals();
#if defined(_WIN32) || defined(__MINGW64__)
#else
    signal_notifier_->start();
#endif
    auto term_size = termcontrol::get_terminal_size();
    for_each_widget([cols=term_size.cols](const auto& w) {
        w->allocate_size(cols > 0 ? cols : 120);
    });
    for_each_widget([](const auto& w) {w->queue_layout(); });
    for_each_widget([](const auto& w) {w->queue_render(); });

    frame_clock_.set_interval(100ms);
    frame_clock_.set_function([this]() {
        queue_event(std::make_shared<render_event>());
    });
    terminal_size_clock_.set_interval(1000ms);
    terminal_size_clock_.set_function([this]() { resize_handler(); });
    event_loop_ = std::jthread(&application::event_loop, this);
}

// Blok until the event loop stops running
void application::wait()
{
#if defined(_WIN32) || defined(__MINGW64__)
#else
    signal_notifier_->wait();
#endif
    if (event_loop_.joinable())
        event_loop_.join();
}

void application::request_stop()
{
#if defined(_WIN32) || defined(__MINGW64__)
#else
    signal_notifier_->request_stop();
#endif
    // wake the queue if blocked on waiting for next event
    event_queue_.push(event_item{
        .value=std::make_shared<termination_event>(),
        .destination={}
    });
    event_loop_.request_stop();
}

std::weak_ptr<widget> application::add_widget(std::shared_ptr<widget> w)
{
    // all root widgets get full terminal width
    w->allocate_size(term_size_.load(std::memory_order_relaxed).cols);

    std::shared_ptr<widget> wc = w;
    {
        std::unique_lock lck(widgets_mutex_);
         widgets_.emplace_front(w);
         widget_register_.emplace(w->id(), std::weak_ptr<widget>{});
    }

    w->queue_layout();
    w->queue_render();

    return wc->weak_from_this();
}

void application::remove_widget(const std::shared_ptr<widget>& w)
{
    {
        std::unique_lock lck(widgets_mutex_);
        auto it = std::find_if(widgets_.begin(), widgets_.end(),
                [&](const auto& ptr) { return ptr.get() == w.get(); });
        if (it != std::end(widgets_))
            widgets_.erase(it);

        widget_register_.erase(widget_register_.find(w->id()));
    }
}

void application::remove_widget(const widget* w)
{
    {
        std::unique_lock lck(widgets_mutex_);
        auto it = std::find_if(widgets_.begin(), widgets_.end(),
                [&](const auto& ptr) { return ptr.get()==w; });
        widgets_.erase(it);
        widget_register_.erase(widget_register_.find(w->id()));
    }
}


terminal_writer& application::writer() {
    return writer_;
}

cliprogress::application* cliprogress::application::instance() noexcept {
    Expects(instance_);
    return instance_;
}

application::~application() noexcept {
    request_stop();
    wait();

    // disconnect signal notifier to make so signals handlers do not get called after the application is destroyed,
    // if another application is started that reused the same signal notifier.
#if defined(_WIN32) || defined(__MINGW64__)
#else
    signal_notifier_->disconnect(SIGWINCH);
#endif

    // restore line wrapping
    writer().write(tc::format<tc::def::set_mode>(tc::dec_mode::autowrap));
}

void application::register_signals()
{
#if defined(_WIN32) || defined(__MINGW64__)
#else
    signal_notifier_->connect(SIGWINCH, [this](){ this->resize_handler(); });
#endif
}

void application::resize_handler()
{
    auto old_size = term_size_.load(std::memory_order_relaxed);
    auto new_size = termcontrol::get_terminal_size();
    term_size_.store(new_size, std::memory_order_relaxed);

//    for_each_widget([&](auto& w) { w->allocate_size(new_size.cols); } );

    auto e = event_item { .value = std::make_shared<resize_event>(new_size, old_size) };
    event_queue_.push(std::move(e));
}

void application::process_event(event_item& item)
{
        if (item.value && item.value->type() == event_type::render) {
            for_each_widget([&](const auto& w) { w->on_event(item.value); });
            writer().flush();
            return;
        }

        if (auto dest = item.destination.lock(); dest) {
            // check if dest is a registered widget.
            // this is to avoid passing event to widgets that are already removed
            // but are still alive.
            if (widget_register_.contains(dest->id())) {
                dest->on_event(std::move(item.value));
            }
        }
        // broadcast event to all widgets
        else {
            for_each_widget([&](const auto& w) { w->on_event(item.value); });
        }
}


void application::event_loop()
{
    ///
    event_item item;
    event_item resize_item;

    frame_clock_.start();
    terminal_size_clock_.start();
    auto stop_token = event_loop_.get_stop_token();

    while (!stop_token.stop_requested() && stop_token.stop_possible()) {
        event_queue_.pop(item);

        if (item.value) {
           process_event(item);
        }
    }
    frame_clock_.stop();
    terminal_size_clock_.stop();
    // pump queue empty

    while(event_queue_.try_pop(item)) {
        process_event(item);
    }
}



void application::register_widget_id(std::size_t id, std::weak_ptr<widget> root)
{
    std::unique_lock lck(widget_register_mutex_);
    widget_register_.emplace(id, std::move(root));
}

void application::unregister_widget_id(std::size_t id)
{
    std::unique_lock lck(widget_register_mutex_);
    widget_register_.erase(id);
}

}