#pragma once

#include <memory>
#include <string>
#include <string_view>

#include "cliprogressbar/posix_signal_notifier.hpp"
#include "cliprogressbar/events/event.hpp"
#include "cliprogressbar/event_queue.hpp"
#include "cliprogressbar/widget.hpp"
#include "cliprogressbar/layout/box_layout_manager.hpp"

/// The main application class
/// event_queue
/// progress_bar spawn thread with

/// Class bar
namespace cliprogress {

namespace tc = termcontrol;

// TODO: progress plugins for eta rate and timers

class progress_indicator
{
public:
    explicit progress_indicator(application* app)
        : app_(app)
        , root_widget_(std::make_shared<widget>())
        , root_layout_(std::make_shared<box_layout_manager>())
        , data_(std::make_shared<progress_data>())
    {
        root_widget_->set_layout_manager(
                std::dynamic_pointer_cast<layout_manager>(root_layout_));
    }

    void set_progress_range(double min_value = 0, double max_value = 100)
    {
        data_->set_range(min_value, max_value);
        value_changed(data_.get());
    }

    void set_rate_strategy(std::unique_ptr<rate_strategy> strategy)
    {
        data_->set_rate_stategy(std::move(strategy));
    }

    void set_eta_strategy(std::unique_ptr<eta_strategy> strategy)
    {
        data_->set_eta_stategy(std::move(strategy));
    }

    void push_back(std::shared_ptr<widget> w)
    {
        root_layout_->push_back(std::move(w));
    }

    void push_front(std::shared_ptr<widget> w)
    {
        root_layout_->push_front(std::move(w));
    }

    void set_spacing(std::size_t n)
    {
        root_layout_->set_spacing(n);
    }

    double value() const noexcept
    {
        return data_->value();
    }

    // Set the progress value
    void set_value(double value)
    {
        data_->update(value);
        // notify c off the progress
        value_changed(data_.get());
    }

    /// Return the layout managing all child widgets
    std::shared_ptr<box_layout_manager> get_layout()
    {
        return root_layout_;
    }

    void start()
    {
        namespace tc = termcontrol;
        static constexpr auto disable_autowrap =
                tc::control_sequence<tc::def::reset_mode>(tc::dec_mode::autowrap);

//        app_->writer().write(disable_autowrap);
//         layout before adding
        // move ownerschip to the application !
        app_->add_widget(root_widget_);
    }


    /// Wait for the
    void stop()
    {
//        static constexpr auto enable_autowrap =
//                tc::control_sequence<tc::def::set_mode>(tc::dec_mode::autowrap);
        // render last time to enable for example setting bars to 100%
        root_widget_->queue_render();
        root_widget_->close();

//        app_->writer().write(enable_autowrap);
    }

    sigslot::signal<const progress_data*> value_changed;

private:
    application* app_;

    std::shared_ptr<widget> root_widget_;
    std::shared_ptr<box_layout_manager> root_layout_;
    std::shared_ptr<progress_data> data_ {};
};


}