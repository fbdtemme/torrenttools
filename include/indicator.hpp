#pragma once

#include <memory>
#include <ranges>

#include <cliprogressbar/progress_indicator.hpp>
#include <cliprogressbar/widgets/label.hpp>
#include <cliprogressbar/widgets/bar.hpp>
#include <cliprogressbar/formatters.hpp>
#include <cliprogressbar/progress_plugins/ewma_rate.hpp>
#include <cliprogressbar/progress_plugins/smoothed_eta.hpp>

#include "dottorrent/file_entry.hpp"

namespace rng = std::ranges;
namespace clp = cliprogress;

inline std::unique_ptr<cliprogress::progress_indicator>
make_indicator(
        const dottorrent::file_storage& s,
        const dottorrent::file_entry& e)
{
    auto i = std::make_unique<cliprogress::progress_indicator>(cliprogress::application::instance());
    i->set_progress_range(0, e.file_size());
    i->set_rate_strategy(std::make_unique<clp::ewma_rate>());
    i->set_eta_strategy(std::make_unique<clp::smoothed_eta>());

    auto it = rng::find(s, e);
    auto file_idx = std::distance(s.begin(), it);

    // file index
    auto index_label = std::make_shared<clp::label>(fmt::format("({}/{})", file_idx+1, s.file_count()));
    index_label->set_name("index");

    // file name
    auto name_label = std::make_shared<clp::label>(e.path().filename().string());
    name_label->set_name("filename");

    // file size
    auto size_label = std::make_shared<clp::label>();
    size_label->set_name("size");

    i->value_changed.connect(
            [=](const clp::progress_data* data) {
                size_label->set_text(clp::format_binary_unit(data->value(), "B | "));
            });

    // bar
    auto bar = std::make_shared<clp::bar>(
            clp::bar_symbols { .complete_frames = std::span(clp::bar_frames::horizontal_blocks) },
            clp::bar_style {},
            10);
    bar->set_name("bar");

    i->value_changed.connect(
            [=](const clp::progress_data* data) {
                bar->set_percentage(data->percentage());
            });

    auto percent = std::make_shared<clp::label>();
    i->value_changed.connect(
            [=](const clp::progress_data* data) {
                percent->set_text(fmt::format("{:>3.0f}%", data->percentage()));
            });
    percent->set_name("percent");

    auto rate = std::make_shared<clp::label>();
    i->value_changed.connect(
            [=](const clp::progress_data* data) {
                rate->set_text(clp::format_binary_unit(data->rate(), "B/s"));
            });
    rate->set_name("rate");

    index_label->set_size_policy(clp::size_policy_flag::fixed);
    name_label->set_size_policy(clp::size_policy_flag::grow | clp::size_policy_flag::shrink);
    percent->set_size_policy(clp::size_policy_flag::fixed);
    bar->set_size_policy(clp::size_policy_flag::fixed);
    size_label->set_size_policy(clp::size_policy_flag::fixed);
    rate->set_size_policy(clp::size_policy_flag::fixed);

    name_label->set_ellipsize_mode(clp::ellipsize_mode::end);

    i->push_back(index_label);
    i->push_back(name_label);
    i->push_back(percent);
    i->push_back(bar);
    i->push_back(rate);
    i->set_spacing(1);

    return i;
}

inline void on_indicator_completion(std::unique_ptr<cliprogress::progress_indicator>& indicator)
{
    auto layout = indicator->get_layout();
    auto size = layout->size();

    /// set the three last children to invisible
    for (auto i = size-3; i < size; ++i) {
        layout->at(i)->get_widget()->set_visible(false);
    }
}