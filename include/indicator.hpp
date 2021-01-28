#pragma once

#include <memory>
#include <ranges>

#include <cliprogressbar/progress_indicator.hpp>
#include <dottorrent/file_storage.hpp>
#include <dottorrent/file_entry.hpp>

namespace rng = std::ranges;
namespace clp = cliprogress;
namespace dt = dottorrent;

std::unique_ptr<cliprogress::progress_indicator>
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
std::unique_ptr<cliprogress::progress_indicator>
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