#pragma once

#include <string>
#include <span>
#include <array>
#include <termcontrol/termcontrol.hpp>

namespace cliprogress {

namespace tc = termcontrol;
namespace rng = std::ranges;

/// Define symbols for drawing a progress bar.
/// The complete_frames member supports fractional symbols.
struct bar_symbols
{
    std::string left_seperator = "|";
    std::string right_seperator = "|";
    std::string incomplete = " ";
    std::string complete = "=";
    std::string complete_lead = "=";
    std::span<std::string> complete_frames = {};
};

/// Type holding color and emphasis information for the bar of a progress_bar.
struct bar_style
{
    termcontrol::text_style left_separator  = {};
    termcontrol::text_style right_separator  = {};
    termcontrol::text_style complete   = {};
    termcontrol::text_style incomplete = {};
};


///// Progress frames for progress indicators with high precision.
namespace bar_frames {

inline auto horizontal_blocks = std::array<std::string, 10> {
        "▏",
        "▎",
        "▍",
        "▌",
        "▋",
        "▊",
        "▉",
        "▉",
        "▉",
        "█",
};

inline auto vertical_blocks = std::array<std::string, 8> {
        "▁",                ///< Lower one eighth block
        "▂",                ///< Lower one quarter block
        "▃",                ///< Lower three eighths block
        "▄",                ///< Lower half block
        "▅",                ///< Lower five eighths block
        "▆",                ///< Lower three quarters block
        "▇",                ///< Lower seven eighths block
        "█",                ///< Full block
};

}
inline std::string draw_progress_bar(
        double percentage,
        const bar_symbols& symbols,
        const bar_style& style,
        std::size_t size = 40)
{
    std::string buffer {};
    auto seperators_size = symbols.left_seperator.size() + symbols.right_seperator.size();

    std::uint32_t state_complete = 0;
    std::uint32_t state_incomplete = 0;
    double state_fractional = 0;

    if (size <= seperators_size) {
        state_complete = 0;
        state_incomplete = 0;
        state_fractional = 0;
        return buffer;
    }

    auto bar_size = size - seperators_size;

    Ensures(bar_size <= std::numeric_limits<std::uint16_t>::max());

    double target_done_full = percentage / 100 * bar_size;
    auto target_complete = static_cast<std::uint32_t>(target_done_full);
    auto target_fractional = target_done_full - double(target_complete);
    auto target_incomplete = bar_size - target_complete;

    auto frame_count = symbols.complete_frames.size();
    auto has_fractional = target_fractional >= (1.0 / frame_count);
    if (has_fractional && target_incomplete > 0) { --target_incomplete; }

    Ensures(target_complete + has_fractional + target_incomplete == bar_size);

    state_complete = target_complete;
    state_incomplete = target_incomplete;
    state_fractional = target_fractional;

    using sgr = termcontrol::definitions::select_graphics_rendition;
    auto it = std::back_inserter(buffer);
    buffer.clear();

    std::size_t frames_count = symbols.complete_frames.size();
    std::size_t fractional_numerator = state_fractional * frames_count;

    tc::format_to<sgr>(it, style.left_separator);
    buffer.append(symbols.left_seperator);

    tc::format_to<sgr>(it, style.complete);

    if (frames_count > 1) {
        for (std::size_t i = 0; i < state_complete; ++i) {
            buffer.append(symbols.complete_frames[frames_count-1]);
        }
        if (fractional_numerator != 0) {
            buffer.append(symbols.complete_frames[fractional_numerator-1]);
        }
        tc::format_to<sgr>(it, style.incomplete);
    }
    else {
        if (state_complete > 0) {
            for (std::size_t i = 0; i < state_complete-1; ++i) {
                buffer.append(symbols.complete);
            }
            buffer.append(symbols.complete_lead);
        }
        // render fractional propertion as incomplete as long as there are incomplete values
        tc::format_to<sgr>(it, style.incomplete);

        // we count unrenderable fractional values as incomplete.
        if (state_fractional > 0) {
            buffer.append(symbols.incomplete);
        }
    }

    for (std::size_t i = 0; i < state_incomplete; ++i) {
        buffer.append(symbols.incomplete);
    }
    tc::format_to<sgr>(it, style.right_separator);
    buffer.append(symbols.right_seperator);
    tc::format_to<sgr>(it, tc::text_style::reset());

    return buffer;
}

}