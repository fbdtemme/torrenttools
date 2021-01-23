#pragma once
#include <ctre.hpp>
#include <ctll.hpp>

inline constexpr auto escape_seq_regex = ctll::fixed_string {
        "(?:" "\x1B" "[@-Z\\-_]|[" "\x80" "-" "\x9A" "\x9C" "-" "\x9F" "]"
        "|(?:" "\x1B" "\\[|" "\x9B" ")[0-?]*[ -/]*[@-~])"};

namespace termcontrol {

/// Return the cumulative length of escape sequences contained in `text`.
constexpr auto total_control_sequences_length(std::string_view text) -> std::size_t
{
    std::size_t size = 0;
    auto match = ctre::search<escape_seq_regex>(text);
    while (match) {
        auto m = match.get<0>();
        size += m.size();
        match = ctre::search<escape_seq_regex>(m.end(), text.end());
    }
    return size;
}

///// Control
//template <typename ForwardIt>
//constexpr void search_control_sequences(ForwardIt first, ForwardIt last, OutputIt out) -> std::size_t
//{
//    string_view
//    std::size_t pos = 0;
//    auto match = ctre::search<escape_seq_regex>(first, last);
//
//    while (match) {
//        auto m = match.get<0>();
//        pos += std::distance(first, m.begin());
//        *out++ = std::pair(pos, pos + m.size());
//        match = ctre::search<escape_seq_regex>(m.end(), last);
//    }
//    return;
//}
//
//


}