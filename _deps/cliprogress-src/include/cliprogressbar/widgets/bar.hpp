#pragma once

#include <csignal>
#include <iostream>
#include <string>
#include <string_view>
#include <iostream>
#include <span>
#include <mutex>

#include <termcontrol/detail/control_sequence.hpp>
#include <termcontrol/detail/definitions.hpp>
#include <termcontrol/detail/format.hpp>

#include "cliprogressbar/widget.hpp"
#include "cliprogressbar/functional/progress_bar.hpp"

namespace cliprogress {

using namespace std::string_view_literals;

/// simple progress bar widget.
class bar : public widget
{
private:
    struct bar_state
    {
        std::uint32_t complete = 0;
        std::uint32_t incomplete = 0;
        double fractional_numerator = 0;
    };

public:
    bar(bar_symbols symbols, bar_style style, std::size_t size = 40)
        : symbols_(std::move(symbols))
        , style_(std::move(style))
        , size_(size)
        , state_incomplete_(size)
    {
        // Expand to available width by default
        bar::set_size_policy({size_policy_flag::expand | size_policy_flag::shrink, 1});
    }

    // Set the progress value
    void set_percentage(double value)
    {
        Expects(value >= 0);
        Expects(value <= 100);
        percentage_ = value;
        queue_render();

        percentage_changed(value);
    }

    double percentage()
    { return percentage_; }

    std::size_t minimum_size() const override
    {
        std::size_t complete_size = 0;
        if (!symbols_.complete_frames.empty()) {
            complete_size = symbols_.complete_frames.size();
        } else {
            complete_size = std::size(symbols_.complete);
        }
        auto min_size = (
                std::size(symbols_.left_seperator)
                + std::size(symbols_.right_seperator)
                + complete_size);

        return min_size;
    }

    std::size_t natural_size() const override
    {
        return size_;
    }

    void set_size(std::size_t n)
    {
        size_ = n;
        queue_layout();
    }

    void render(terminal_writer& out) override
    {
        std::unique_lock lck{mutex_};
        render_to_buffer();
        out.write(buffer_);
    }

    void render_to_buffer()
    {
        using sgr = termcontrol::definitions::select_graphics_rendition;
        auto it = std::back_inserter(buffer_);
        buffer_.clear();

        update_state();

        std::size_t frames_count = symbols_.complete_frames.size();
        std::size_t fractional_numerator = state_fractional_ * frames_count;

        tc::format_to<sgr>(it, style_.left_separator);
        buffer_.append(symbols_.left_seperator);

        tc::format_to<sgr>(it, style_.complete);

        if (frames_count > 1) {
            for (std::size_t i = 0; i < state_complete_; ++i) {
                buffer_.append(symbols_.complete_frames[frames_count-1]);
            }
            if (fractional_numerator != 0) {
                buffer_.append(symbols_.complete_frames[fractional_numerator-1]);
            }
            tc::format_to<sgr>(it, style_.incomplete);
        }
        else {
            if (state_complete_ > 0) {
                for (std::size_t i = 0; i < state_complete_-1; ++i) {
                    buffer_.append(symbols_.complete);
                }
                buffer_.append(symbols_.complete_lead);
            }
            // render fractional propertion as incomplete as long as there are incomplete values
            tc::format_to<sgr>(it, style_.incomplete);

            // we count unrenderable fractional values as incomplete.
            if (state_fractional_ > 0) {
                buffer_.append(symbols_.incomplete);
            }
        }

        for (std::size_t i = 0; i < state_incomplete_; ++i) {
            buffer_.append(symbols_.incomplete);
        }
        tc::format_to<sgr>(it, style_.right_separator);
        buffer_.append(symbols_.right_seperator);
        tc::format_to<sgr>(it, tc::text_style::reset());
    }

private:

    void update_state()
    {
        auto seperators_size = symbols_.left_seperator.size() + symbols_.right_seperator.size();

        if (allocated_size() <= seperators_size) {
            state_complete_ = 0;
            state_incomplete_ = 0;
            state_fractional_ = 0;
            return;
        }

        auto bar_size = allocated_size() - seperators_size;

        Ensures(bar_size <= std::numeric_limits<std::uint16_t>::max());

        double target_done_full = percentage_ / 100 * bar_size;
        auto target_complete = static_cast<std::uint32_t>(target_done_full);
        auto target_fractional = target_done_full - double(target_complete);
        auto target_incomplete = bar_size - target_complete;

        auto frame_count = symbols_.complete_frames.size();
        auto has_fractional = target_fractional >= (1.0 / frame_count);
        if (has_fractional && target_incomplete > 0) { --target_incomplete; }

        Ensures(target_complete + has_fractional + target_incomplete == bar_size);

        state_complete_ = target_complete;
        state_incomplete_ = target_incomplete;
        state_fractional_ = target_fractional;
    }

    sigslot::signal<double> percentage_changed;

    std::atomic<double> percentage_ = 0;
    std::atomic<std::size_t> size_ = 40;
    bar_style style_ {};
    bar_symbols symbols_ {};
    std::uint32_t state_complete_ = 0;
    std::uint32_t state_incomplete_ = 0;
    double state_fractional_ = 0;
    std::string buffer_ {};

    // signals
};

} // namespace cliprogress


