#include <memory>
#include <ranges>

#include <fmt/format.h>

#include <cliprogressbar/progress_indicator.hpp>
#include <cliprogressbar/widgets/label.hpp>
#include <cliprogressbar/widgets/bar.hpp>
#include <cliprogressbar/formatters.hpp>
#include <cliprogressbar/progress_plugins/ewma_rate.hpp>
#include <cliprogressbar/progress_plugins/smoothed_eta.hpp>

#include "indicator.hpp"

namespace rng = std::ranges;
namespace clp = cliprogress;
namespace dt = dottorrent;


progress_indicator::progress_indicator(clp::application* app, const dottorrent::file_storage& storage,
        bool skip_padding_files)
        : app_(app)
        , storage_(storage)
        , root_widget_(std::make_shared<clp::widget>())
        , root_layout_(std::make_shared<clp::box_layout_manager>())
        , per_file_data_(std::make_shared<clp::progress_data>())
        , total_data_(std::make_shared<clp::progress_data>())
        , skip_padding_files_(skip_padding_files)
{
    root_widget_->set_layout_manager(
            std::dynamic_pointer_cast<clp::layout_manager>(root_layout_));

    total_data_->set_rate_stategy(std::make_unique<clp::ewma_rate>());
    total_data_->set_eta_stategy(std::make_unique<clp::smoothed_eta>());

    fileindex_label_ = std::make_shared<clp::label>();
    filename_label_ = std::make_shared<clp::label>();
    filesize_label_ = std::make_shared<clp::label>();

    progressbar_ = std::make_shared<clp::bar>(
            clp::bar_symbols { .complete_frames = std::span(clp::bar_frames::horizontal_blocks) },
            clp::bar_style {},
            10);
    progressbar_->set_name("bar");

    total_value_changed.connect(
            [&](const clp::progress_data* data) {
                filesize_label_->set_text(clp::format_binary_unit(data->value(), "B | "));
            });

    total_value_changed.connect(
            [&](const clp::progress_data* data) {
                progressbar_->set_percentage(data->percentage());
            });

    percentage_ = std::make_shared<clp::label>();
    total_value_changed.connect(
            [&](const clp::progress_data* data) {
                percentage_->set_text(fmt::format("{:>3.0f}%", data->percentage()));
            });
    percentage_->set_name("percent");

    rate_label_ = std::make_shared<clp::label>();
    total_value_changed.connect(
            [&](const clp::progress_data* data) {
                rate_label_->set_text(clp::format_binary_unit(data->rate(), "B/s"));
            });
    rate_label_->set_name("rate");

    eta_label_ = std::make_shared<clp::label>();
    total_value_changed.connect(
            [&](const cliprogress::progress_data* data) {
                auto duration = std::chrono::duration_cast<std::chrono::seconds>(data->eta());
                eta_label_->set_text(fmt::format("    {:%M:%S}  ", duration));
            });

    eta_label_->set_name("eta");

    if (storage.file_count() > 0) [[likely]] {
        set_current_file(0);
        total_data_->set_range(0, storage.total_file_size());
    }

    fileindex_label_->set_size_policy(clp::size_policy_flag::fixed);
    filename_label_->set_size_policy(clp::size_policy_flag::grow | clp::size_policy_flag::shrink);
    percentage_->set_size_policy(clp::size_policy_flag::fixed);
    progressbar_->set_size_policy(clp::size_policy_flag::fixed);
    filesize_label_->set_size_policy(clp::size_policy_flag::fixed);
    rate_label_->set_size_policy(clp::size_policy_flag::fixed);
    eta_label_->set_size_policy(clp::size_policy_flag::fixed);

    filename_label_->set_ellipsize_mode(clp::ellipsize_mode::end);

    root_layout_->push_back(fileindex_label_);
    root_layout_->push_back(filename_label_);
    root_layout_->push_back(percentage_);
    root_layout_->push_back(progressbar_);
    root_layout_->push_back(rate_label_);
    root_layout_->push_back(eta_label_);
    root_layout_->set_spacing(1);
}

void progress_indicator::set_current_file(std::size_t file_index) {
    const auto& storage = storage_.get();
    static const auto is_padding_file = [](const dt::file_entry& e) { return e.is_padding_file(); };

    const dt::file_entry& entry = storage.at(file_index);
    current_file_is_padding_ = entry.is_padding_file();

    if (current_file_is_padding_)
        return;

    std::size_t file_count;

    if (skip_padding_files_) {
        file_count = storage.regular_file_count();
        std::size_t n_padding_files = rng::count_if(storage.begin(), storage.begin()+file_index, is_padding_file);
        file_index = file_index - n_padding_files;
    }
    else {
        file_count = storage.file_count();
    }

    per_file_data_->set_range(0, entry.file_size());
    per_file_data_->update(0);
    per_file_value_changed(per_file_data_.get());

    // update per file labels
    fileindex_label_->set_text(fmt::format("({}/{})", file_index+1, file_count));
    fileindex_label_->set_name("index");

    filename_label_->set_text(entry.path().filename().string());
    filename_label_->set_name("filename");

    filesize_label_->set_text("");
    filesize_label_->set_name("size");
}

void progress_indicator::push_back(std::shared_ptr<clp::widget> w) {
    root_layout_->push_back(std::move(w));
}

void progress_indicator::push_front(std::shared_ptr<clp::widget> w) {
    root_layout_->push_front(std::move(w));
}

void progress_indicator::set_spacing(std::size_t n) {
    root_layout_->set_spacing(n);
}

double progress_indicator::per_file_value() const noexcept {
    return per_file_data_->value();
}

double progress_indicator::total_value() const noexcept {
    return per_file_data_->value();
}

void progress_indicator::set_per_file_value(double value) {
    if (skip_padding_files_ && current_file_is_padding_)
        return;

    per_file_data_->update(value);
    // notify c off the progress
    per_file_value_changed(per_file_data_.get());
}

void progress_indicator::set_total_value(double value) {
    if (skip_padding_files_ && current_file_is_padding_)
        return;

    total_data_->update(value);
    // notify c off the progress
    total_value_changed(total_data_.get());
}

std::shared_ptr<clp::box_layout_manager> progress_indicator::get_layout() {
    return root_layout_;
}

void progress_indicator::start() {
    namespace tc = termcontrol;
    static constexpr auto disable_autowrap =
            tc::control_sequence<tc::def::reset_mode>(tc::dec_mode::autowrap);

    app_->start();
    app_->add_widget(root_widget_);
}

void progress_indicator::stop() {
    root_widget_->queue_render();
    root_widget_->close();
    app_->request_stop();
    app_->wait();
}

void print_simple_indicator(std::ostream& os, const dottorrent::file_storage& s, std::size_t file_idx, dt::protocol hasher_protocol)
{
    if (hasher_protocol == dt::protocol::v1) {
        print_simple_indicator_v1(os, s, file_idx);
    }
    else {
        print_simple_indicator_v2(os, s, file_idx);
    }
}



void print_simple_indicator_v1(std::ostream& os, const dottorrent::file_storage& s, std::size_t file_idx)
{

    const auto& entry = s.at(file_idx);
    // file index
    auto filename = entry.path().filename().string();
    double progress;

    auto out = std::ostreambuf_iterator(os);
    fmt::format_to(out, "({}/{}) {} ...\n", file_idx+1, s.file_count(), filename);
}


void print_simple_indicator_v2(std::ostream& os, const dottorrent::file_storage& s, std::size_t file_idx)
{

    std::size_t regular_file_count = s.regular_file_count();
    const auto is_padding_file = [](const dt::file_entry& e) { return e.is_padding_file(); };
    std::size_t regular_file_idx = file_idx - std::count_if(s.begin(), s.begin()+file_idx, is_padding_file);

    const auto& entry = s.at(file_idx);
    if (entry.is_padding_file()) {
        return;
    }
    // file index
    auto filename = entry.path().filename().string();

    auto out = std::ostreambuf_iterator(os);
    fmt::format_to(out, "({}/{}) {} ...\n", regular_file_idx+1, regular_file_count, filename);
}

