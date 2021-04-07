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

//std::unique_ptr<progress_indicator>
//make_indicator(const dottorrent::file_storage& s, const dottorrent::file_entry& e)
//{
//    auto i = std::make_unique<progress_indicator>(cliprogress::application::instance());
//    i->set_per_file_progress_range(0, e.file_size());
//    i->set_total_rate_strategy(std::make_unique<clp::ewma_rate>());
//    i->set_total_eta_strategy(std::make_unique<clp::smoothed_eta>());
//
//    auto it = rng::find(s, e);
//    auto file_idx = std::distance(s.begin(), it);
//
//    // file index
//    auto index_label = std::make_shared<clp::label>(fmt::format("({}/{})", file_idx+1, s.file_count()));
//    index_label->set_name("index");
//
//    // file name
//    auto name_label = std::make_shared<clp::label>(e.path().filename().string());
//    name_label->set_name("filename");
//
//    // file size
//    auto size_label = std::make_shared<clp::label>();
//    size_label->set_name("size");
//
//    i->total_value_changed.connect(
//            [=](const clp::progress_data* data) {
//                size_label->set_text(clp::format_binary_unit(data->value(), "B | "));
//            });
//
//    // bar
//    auto bar = std::make_shared<clp::bar>(
//            clp::bar_symbols { .complete_frames = std::span(clp::bar_frames::horizontal_blocks) },
//            clp::bar_style {},
//            10);
//    bar->set_name("bar");
//
//    i->total_value_changed.connect(
//            [=](const clp::progress_data* data) {
//                bar->set_percentage(data->percentage());
//            });
//
//    auto percent = std::make_shared<clp::label>();
//    i->total_value_changed.connect(
//            [=](const clp::progress_data* data) {
//                percent->set_text(fmt::format("{:>3.0f}%", data->percentage()));
//            });
//    percent->set_name("percent");
//
//    auto rate = std::make_shared<clp::label>();
//    i->total_value_changed.connect(
//            [=](const clp::progress_data* data) {
//                rate->set_text(clp::format_binary_unit(data->rate(), "B/s"));
//            });
//    rate->set_name("rate");
//
//    index_label->set_size_policy(clp::size_policy_flag::fixed);
//    name_label->set_size_policy(clp::size_policy_flag::grow | clp::size_policy_flag::shrink);
//    percent->set_size_policy(clp::size_policy_flag::fixed);
//    bar->set_size_policy(clp::size_policy_flag::fixed);
//    size_label->set_size_policy(clp::size_policy_flag::fixed);
//    rate->set_size_policy(clp::size_policy_flag::fixed);
//
//    name_label->set_ellipsize_mode(clp::ellipsize_mode::end);
//
//    i->push_back(index_label);
//    i->push_back(name_label);
//    i->push_back(percent);
//    i->push_back(bar);
//    i->push_back(rate);
//    i->set_spacing(1);
//
//    return i;
//}
//
//std::unique_ptr<cliprogress::progress_indicator>
//make_indicator_v2(const dottorrent::file_storage& s, const dottorrent::file_entry& e)
//{
//    if (e.is_padding_file())
//        return nullptr;
//
//    auto i = std::make_unique<cliprogress::progress_indicator>(cliprogress::application::instance());
//    i->set_progress_range(0, e.file_size());
//    i->set_rate_strategy(std::make_unique<clp::ewma_rate>());
//    i->set_eta_strategy(std::make_unique<clp::smoothed_eta>());
//
//    auto it = rng::find(s, e);
//    auto file_idx = std::distance(s.begin(), it);
//
//    std::size_t regular_file_count = s.regular_file_count();
//    const auto is_padding_file = [](const dt::file_entry& e) { return e.is_padding_file(); };
//    std::size_t regular_file_idx = file_idx - std::count_if(s.begin(), s.begin()+file_idx, is_padding_file);
//
//    // file index
//    auto index_label = std::make_shared<clp::label>(fmt::format("({}/{})", regular_file_idx+1, regular_file_count));
//    index_label->set_name("index");
//
//    // file name
//    auto name_label = std::make_shared<clp::label>(e.path().filename().string());
//    name_label->set_name("filename");
//
//    // file size
//    auto size_label = std::make_shared<clp::label>();
//    size_label->set_name("size");
//
//    i->value_changed.connect(
//            [=](const clp::progress_data* data) {
//                size_label->set_text(clp::format_binary_unit(data->value(), "B | "));
//            });
//
//    // bar
//    auto bar = std::make_shared<clp::bar>(
//            clp::bar_symbols { .complete_frames = std::span(clp::bar_frames::horizontal_blocks) },
//            clp::bar_style {},
//            10);
//    bar->set_name("bar");
//
//    i->value_changed.connect(
//            [=](const clp::progress_data* data) {
//                bar->set_percentage(data->percentage());
//            });
//
//    auto percent = std::make_shared<clp::label>();
//    i->value_changed.connect(
//            [=](const clp::progress_data* data) {
//                percent->set_text(fmt::format("{:>3.0f}%", data->percentage()));
//            });
//    percent->set_name("percent");
//
//    auto rate = std::make_shared<clp::label>();
//    i->value_changed.connect(
//            [=](const clp::progress_data* data) {
//                rate->set_text(clp::format_binary_unit(data->rate(), "B/s"));
//            });
//    rate->set_name("rate");
//
//    index_label->set_size_policy(clp::size_policy_flag::fixed);
//    name_label->set_size_policy(clp::size_policy_flag::grow | clp::size_policy_flag::shrink);
//    percent->set_size_policy(clp::size_policy_flag::fixed);
//    bar->set_size_policy(clp::size_policy_flag::fixed);
//    size_label->set_size_policy(clp::size_policy_flag::fixed);
//    rate->set_size_policy(clp::size_policy_flag::fixed);
//
//    name_label->set_ellipsize_mode(clp::ellipsize_mode::end);
//
//    i->push_back(index_label);
//    i->push_back(name_label);
//    i->push_back(percent);
//    i->push_back(bar);
//    i->push_back(rate);
//    i->set_spacing(1);
//
//    return i;
//}

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

    fmt::print(os, "({}/{}) {} ...\n", file_idx+1, s.file_count(), filename);
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

    fmt::print(os, "({}/{}) {} ...\n", regular_file_idx+1, regular_file_count, filename);
}
