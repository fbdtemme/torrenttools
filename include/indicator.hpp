#pragma once

#include <memory>
#include <ranges>
#include <string>
#include <string_view>

#include <cliprogressbar/event_queue.hpp>
#include <cliprogressbar/events/event.hpp>
#include <cliprogressbar/widgets/label.hpp>
#include <cliprogressbar/widgets/bar.hpp>
#include <cliprogressbar/progress_plugins/ewma_rate.hpp>
#include <cliprogressbar/progress_plugins/smoothed_eta.hpp>

#include <cliprogressbar/layout/box_layout_manager.hpp>
#include <cliprogressbar/posix_signal_notifier.hpp>
#include <cliprogressbar/progress_indicator.hpp>
#include <cliprogressbar/widget.hpp>
#include <cliprogressbar/formatters.hpp>

#include <dottorrent/file_storage.hpp>
#include <dottorrent/file_entry.hpp>

namespace rng = std::ranges;
namespace clp = cliprogress;
namespace dt = dottorrent;
namespace tc = termcontrol;


class progress_indicator
{
public:
    explicit progress_indicator(clp::application* app,
                                const dottorrent::file_storage& storage,
                                bool skip_padding_files = false);

    void set_current_file(std::size_t file_index);

    void push_back(std::shared_ptr<clp::widget> w);

    void push_front(std::shared_ptr<clp::widget> w);

    void set_spacing(std::size_t n);

    double per_file_value() const noexcept;

    double total_value() const noexcept;

    // Set the progress value
    void set_per_file_value(double value);

    void set_total_value(double value);

    /// Return the layout managing all child widgets
    std::shared_ptr<clp::box_layout_manager> get_layout();

    void start();

    void stop();

    sigslot::signal<const clp::progress_data*> per_file_value_changed;
    sigslot::signal<const clp::progress_data*> total_value_changed;

private:
    clp::application* app_;
    std::reference_wrapper<const dt::file_storage> storage_;

    std::shared_ptr<clp::label> fileindex_label_;
    std::shared_ptr<clp::label> filename_label_;
    std::shared_ptr<clp::label> filesize_label_;
    std::shared_ptr<clp::bar> progressbar_;
    std::shared_ptr<clp::label> percentage_;
    std::shared_ptr<clp::label> rate_label_;
    std::shared_ptr<clp::label> eta_label_;
    std::shared_ptr<clp::widget> root_widget_;

    std::shared_ptr<clp::box_layout_manager> root_layout_;
    std::shared_ptr<clp::progress_data> per_file_data_ {};
    std::shared_ptr<clp::progress_data> total_data_ {};
    bool skip_padding_files_;
    bool current_file_is_padding_ = false;
};



std::unique_ptr<progress_indicator>
make_indicator(
        const dottorrent::file_storage& s,
        const dottorrent::file_entry& e);


inline void on_indicator_completion(std::unique_ptr<cliprogress::progress_indicator>& indicator)
{
    auto layout = indicator->get_layout();
    auto size = layout->size();

    /// set the three last children to invisible
    for (auto i = size-3; i < size; ++i) {
        layout->at(i)->get_widget()->set_visible(false);
    }
}


/// Indicator for v2 and hybrid torrents which does not show padding files.
std::unique_ptr<progress_indicator>
make_indicator_v2(
        const dottorrent::file_storage& s,
        const dottorrent::file_entry& e);



void print_simple_indicator(
            std::ostream& os, const dottorrent::file_storage& s,
            std::size_t file_idx, dt::protocol hasher_protocol);

void print_simple_indicator_v1(
        std::ostream& os, const dottorrent::file_storage& s,
        std::size_t file_idx);

void print_simple_indicator_v2(
        std::ostream& os, const dottorrent::file_storage& s,
        std::size_t file_idx);