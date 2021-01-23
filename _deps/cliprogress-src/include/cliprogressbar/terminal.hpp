#pragma once
#include <termcontrol/detail/control_sequence.hpp>
#include <termcontrol/detail/control_sequence_definition.hpp>

#include <iostream>

namespace cliprogress {


using cursor_position                    = termcontrol::control_sequence<termcontrol::def::cursor_position>;
using set_top_and_bottom_margins         = termcontrol::control_sequence<termcontrol::def::set_top_and_bottom_margins>;
using cursor_up                          = termcontrol::control_sequence<termcontrol::def::cursor_up>;
using cursor_right                       = termcontrol::control_sequence<termcontrol::def::cursor_right>;
inline constexpr auto move_to_line_start = termcontrol::control_sequence<termcontrol::def::cursor_character_absolute>();
inline constexpr auto save_cursor        = termcontrol::control_sequence<termcontrol::def::save_cursor>();
inline constexpr auto restore_cursor     = termcontrol::control_sequence<termcontrol::def::restore_cursor>();
inline constexpr auto erase_in_page      = termcontrol::control_sequence<termcontrol::def::erase_in_page>();
inline constexpr auto erase_line         = termcontrol::control_sequence<termcontrol::def::erase_in_line>();
inline constexpr auto erase_full_line    = termcontrol::control_sequence<termcontrol::def::erase_in_line>(termcontrol::erase_line_mode::full);
inline constexpr auto move_up            = termcontrol::control_sequence<termcontrol::def::cursor_up>();
inline constexpr auto scroll_down        = termcontrol::control_sequence<termcontrol::def::scroll_down>();
inline constexpr auto scroll_up          = termcontrol::control_sequence<termcontrol::def::scroll_up>();

}