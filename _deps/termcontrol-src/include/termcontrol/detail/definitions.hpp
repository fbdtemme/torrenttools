////
//// created by fbdtemme on 06/01/2020.
////
#pragma once
#include <cstdint>
#include <array>
#include <limits>

#include "ascii.hpp"
#include "control_sequence_definition.hpp"
#include "parameters.hpp"

namespace termcontrol {

struct text_style;

namespace control_functions {

inline static constexpr std::array control_sequence_introducer  {ascii::escape, '['};
inline static constexpr std::array escape                       {ascii::escape};
inline static constexpr std::array single_character_introducer  {ascii::escape, 'Z'};
inline static constexpr std::array dec_save_cursor              {ascii::escape, '7'};
inline static constexpr std::array dec_restore_cursor           {ascii::escape, '8'};

}

inline namespace acronyms {
inline namespace control_function_acronyms {

inline constexpr auto& csi = termcontrol::control_functions::control_sequence_introducer;
inline constexpr auto& esc = termcontrol::control_functions::escape;
inline constexpr auto& sci = termcontrol::control_functions::single_character_introducer;

}
}

namespace definitions {

// Format effectors
using character_position_absolute = detail::basic_control_sequence_definition<'`', std::uint16_t>;
using character_position_backward = detail::basic_control_sequence_definition<'j', std::uint16_t>;
using character_position_forward  = detail::basic_control_sequence_definition<'a', std::uint16_t>;
using character_and_line_position = detail::basic_control_sequence_definition<'f', std::uint16_t, std::uint16_t>;
using page_position_absolute      = detail::extended_control_sequence_definition<'P', ' ', std::uint16_t>;
using page_position_backward      = detail::extended_control_sequence_definition<'R', ' ', std::uint16_t>;
using page_position_forward       = detail::extended_control_sequence_definition<'Q', ' ', std::uint16_t>;
using tabulation_clear            = detail::basic_control_sequence_definition<'g', tabulation_clear_mode>;
using tabulation_stop_remove      = detail::extended_control_sequence_definition<'d', ' ', std::uint16_t>;
using line_position_absolute      = detail::basic_control_sequence_definition<'d', std::uint16_t>;
using line_position_backward      = detail::basic_control_sequence_definition<'k', std::uint16_t>;
using line_position_forward       = detail::basic_control_sequence_definition<'e', std::uint16_t>;

// Presentation control functions
using select_graphics_rendition   = detail::basic_control_sequence_definition<'m',
                                        detail::overload_set<
                                                detail::argument_pack<graphics_rendition_attribute>,
                                                detail::argument_pack<text_style>,
                                                detail::argument_pack<detail::variadic<std::uint8_t>>>>;

// Editor functions
using delete_character            = detail::basic_control_sequence_definition<'P', std::uint16_t>;
using delete_line                 = detail::basic_control_sequence_definition<'M', std::uint16_t>;
using erase_in_area               = detail::basic_control_sequence_definition<'O', erase_in_area_mode>;
using erase_character             = detail::basic_control_sequence_definition<'X', std::uint16_t>;
using erase_in_page               = detail::basic_control_sequence_definition<'J', erase_page_mode>;
using erase_in_field              = detail::basic_control_sequence_definition<'N', erase_in_field_mode>;
using erase_in_line               = detail::basic_control_sequence_definition<'K', erase_line_mode>;
using insert_character            = detail::basic_control_sequence_definition<'@', std::uint16_t>;
using insert_line                 = detail::basic_control_sequence_definition<'L', std::uint16_t>;

// Cursor control functions
using cursor_backward_tabulation  = detail::basic_control_sequence_definition<'Z', std::uint16_t>;
using cursor_character_absolute   = detail::basic_control_sequence_definition<'G', std::uint16_t>;
using cursor_forward_tabulation   = detail::basic_control_sequence_definition<'I', std::uint16_t>;
using cursor_next_line            = detail::basic_control_sequence_definition<'E', std::uint16_t>;
using cursor_preceding_line       = detail::basic_control_sequence_definition<'F', std::uint16_t>;
using cursor_tabulation_control   = detail::basic_control_sequence_definition<'W', cursor_tabulation_control_mode>;
using cursor_left                 = detail::basic_control_sequence_definition<'D', std::uint16_t>;
using cursor_down                 = detail::basic_control_sequence_definition<'B', std::uint16_t>;
using cursor_right                = detail::basic_control_sequence_definition<'C', std::uint16_t>;
using cursor_position             = detail::basic_control_sequence_definition<'H', std::uint16_t, std::uint16_t>;
using cursor_up                   = detail::basic_control_sequence_definition<'A', std::uint16_t>;
using cursor_line_tabulation      = detail::basic_control_sequence_definition<'Y', std::uint16_t>;

// Display control functions
using next_page                   = detail::basic_control_sequence_definition<'U', std::uint8_t>;
using preceding_page              = detail::basic_control_sequence_definition<'V', std::uint8_t>;
using scroll_down                 = detail::basic_control_sequence_definition<'T', std::uint16_t>;
using scroll_left                 = detail::extended_control_sequence_definition<'@', ' ', std::uint16_t>;
using scroll_up                   = detail::basic_control_sequence_definition<'S', std::uint16_t>;
using scroll_right                = detail::extended_control_sequence_definition<'A', ' ', std::uint16_t>;

// Area definition
using define_area_qualification   = detail::basic_control_sequence_definition<'o', area_qualification>;

// Mode setting
using reset_mode                  = detail::basic_control_sequence_definition<'l',
                                        detail::overload_set<
                                                detail::argument_pack<mode>,
                                                detail::argument_pack<dec_mode>>>;

using set_mode                    = detail::basic_control_sequence_definition<'h',
                                        detail::overload_set<
                                                detail::argument_pack<mode>,
                                                detail::argument_pack<dec_mode>>>;

using restore_cursor              = detail::basic_control_sequence_definition<'u'>;
using save_cursor                 = detail::basic_control_sequence_definition<'s'>;

using set_top_and_bottom_margins  = detail::basic_control_sequence_definition<'r', std::uint16_t, std::uint16_t>;

} // namespace definitions

TERMCONTROL_DEFINE_DEFAULT_ARGUMENTS(definitions::character_position_absolute,    1);
TERMCONTROL_DEFINE_DEFAULT_ARGUMENTS(definitions::character_position_backward,    1);
TERMCONTROL_DEFINE_DEFAULT_ARGUMENTS(definitions::character_position_forward,     1);
TERMCONTROL_DEFINE_DEFAULT_ARGUMENTS(definitions::character_and_line_position,    1, 1);
TERMCONTROL_DEFINE_DEFAULT_ARGUMENTS(definitions::page_position_absolute,         1);
TERMCONTROL_DEFINE_DEFAULT_ARGUMENTS(definitions::page_position_backward,         1);
TERMCONTROL_DEFINE_DEFAULT_ARGUMENTS(definitions::page_position_forward,          1);
TERMCONTROL_DEFINE_DEFAULT_ARGUMENTS(definitions::tabulation_clear,               tabulation_clear_mode::character);
TERMCONTROL_DEFINE_DEFAULT_ARGUMENTS(definitions::line_position_absolute,         1);
TERMCONTROL_DEFINE_DEFAULT_ARGUMENTS(definitions::line_position_backward,         1);
TERMCONTROL_DEFINE_DEFAULT_ARGUMENTS(definitions::line_position_forward,          1);

TERMCONTROL_DEFINE_DEFAULT_ARGUMENTS(definitions::delete_character,                1);
TERMCONTROL_DEFINE_DEFAULT_ARGUMENTS(definitions::delete_line,                     1);
TERMCONTROL_DEFINE_DEFAULT_ARGUMENTS(definitions::erase_in_area,                   erase_in_area_mode::after);
TERMCONTROL_DEFINE_DEFAULT_ARGUMENTS(definitions::erase_character,                 1);
TERMCONTROL_DEFINE_DEFAULT_ARGUMENTS(definitions::erase_in_page,                   erase_page_mode::below);
TERMCONTROL_DEFINE_DEFAULT_ARGUMENTS(definitions::erase_in_field,                  erase_in_field_mode::after);
TERMCONTROL_DEFINE_DEFAULT_ARGUMENTS(definitions::erase_in_line,                   erase_line_mode::after);
TERMCONTROL_DEFINE_DEFAULT_ARGUMENTS(definitions::insert_character,                1);
TERMCONTROL_DEFINE_DEFAULT_ARGUMENTS(definitions::insert_line,                     1);

TERMCONTROL_DEFINE_DEFAULT_ARGUMENTS(definitions::cursor_backward_tabulation,      1);
TERMCONTROL_DEFINE_DEFAULT_ARGUMENTS(definitions::cursor_character_absolute,       1);
TERMCONTROL_DEFINE_DEFAULT_ARGUMENTS(definitions::cursor_forward_tabulation,       1);
TERMCONTROL_DEFINE_DEFAULT_ARGUMENTS(definitions::cursor_next_line,                1);
TERMCONTROL_DEFINE_DEFAULT_ARGUMENTS(definitions::cursor_preceding_line,           1);
TERMCONTROL_DEFINE_DEFAULT_ARGUMENTS(definitions::cursor_tabulation_control,       cursor_tabulation_control_mode::set_character_tab);
TERMCONTROL_DEFINE_DEFAULT_ARGUMENTS(definitions::cursor_left,                     1);
TERMCONTROL_DEFINE_DEFAULT_ARGUMENTS(definitions::cursor_down,                     1);
TERMCONTROL_DEFINE_DEFAULT_ARGUMENTS(definitions::cursor_right,                    1);
TERMCONTROL_DEFINE_DEFAULT_ARGUMENTS(definitions::cursor_position,                 1, 1);
TERMCONTROL_DEFINE_DEFAULT_ARGUMENTS(definitions::cursor_up,                       1);
TERMCONTROL_DEFINE_DEFAULT_ARGUMENTS(definitions::cursor_line_tabulation,          1);

TERMCONTROL_DEFINE_DEFAULT_ARGUMENTS(definitions::next_page,                       1);
TERMCONTROL_DEFINE_DEFAULT_ARGUMENTS(definitions::preceding_page,                  1);
TERMCONTROL_DEFINE_DEFAULT_ARGUMENTS(definitions::scroll_down,                     1);
TERMCONTROL_DEFINE_DEFAULT_ARGUMENTS(definitions::scroll_left,                     1);
TERMCONTROL_DEFINE_DEFAULT_ARGUMENTS(definitions::scroll_up,                       1);
TERMCONTROL_DEFINE_DEFAULT_ARGUMENTS(definitions::scroll_right,                    1);

TERMCONTROL_DEFINE_DEFAULT_ARGUMENTS(definitions::define_area_qualification, area_qualification::unprotected_unguarded);


inline namespace acronyms {
inline namespace definition_acronyms {

// Format effectors
using hpa = definitions::character_position_absolute;
using hpb = definitions::character_position_backward;
using hpr = definitions::character_position_forward;
using hvp = definitions::character_and_line_position;
using ppa = definitions::page_position_absolute;
using ppb = definitions::page_position_backward;
using ppr = definitions::page_position_forward;
using tbc = definitions::tabulation_clear;
using tsr = definitions::tabulation_stop_remove;
using vpa = definitions::line_position_absolute;
using vpb = definitions::line_position_backward;
using vpr = definitions::line_position_forward;

// Presentation control functions
using sgr = definitions::select_graphics_rendition;

// Editor functions
using dch = definitions::delete_character;
using dl  = definitions::delete_line;
using ea  = definitions::erase_in_area;
using ech = definitions::erase_character;
using ed  = definitions::erase_in_page;
using ef  = definitions::erase_in_field;
using el  = definitions::erase_in_line;
using ich = definitions::insert_character;
using il  = definitions::insert_line;

// Cursor control functons
using cbt = definitions::cursor_backward_tabulation;
using cha = definitions::cursor_character_absolute;
using cht = definitions::cursor_forward_tabulation;
using cnl = definitions::cursor_next_line;
using cpl = definitions::cursor_preceding_line;
using ctc = definitions::cursor_tabulation_control;
using cub = definitions::cursor_left;
using cud = definitions::cursor_down;
using cuf = definitions::cursor_right;
using cup = definitions::cursor_position;
using cuu = definitions::cursor_up;
using cvt = definitions::cursor_line_tabulation;

// Display control functions
using np = definitions::next_page;
using pp = definitions::preceding_page;
using sd = definitions::scroll_down;
using sl = definitions::scroll_left;
using su = definitions::scroll_up;
using sr = definitions::scroll_right;

// Area definition
using daq = definitions::define_area_qualification;

// Mode setting
using rm = definitions::reset_mode;
using sm = definitions::set_mode;
//// TODO: finish Presentation control functions

} // namespace defintion_acronyms
} // namespace acronyms

namespace cf = termcontrol::control_functions;
namespace def = termcontrol::definitions;


namespace ecma48
{

inline constexpr auto character_position_absolute = detail::control_sequence_definition_tag<def::character_position_absolute>{};
inline constexpr auto character_position_backward = detail::control_sequence_definition_tag<def::character_position_backward>{};
inline constexpr auto character_position_forward  = detail::control_sequence_definition_tag<def::character_position_forward>{};
inline constexpr auto character_and_line_position = detail::control_sequence_definition_tag<def::character_and_line_position>{};
inline constexpr auto page_position_absolute      = detail::control_sequence_definition_tag<def::page_position_absolute>{};
inline constexpr auto page_position_backward      = detail::control_sequence_definition_tag<def::page_position_backward>{};
inline constexpr auto page_position_forward       = detail::control_sequence_definition_tag<def::page_position_forward>{};
inline constexpr auto tabulation_clear            = detail::control_sequence_definition_tag<def::tabulation_clear>{};
inline constexpr auto tabulation_stop_remove      = detail::control_sequence_definition_tag<def::tabulation_stop_remove>{};
inline constexpr auto line_position_absolute      = detail::control_sequence_definition_tag<def::line_position_absolute>{};
inline constexpr auto line_position_backward      = detail::control_sequence_definition_tag<def::line_position_backward>{};
inline constexpr auto line_position_forward       = detail::control_sequence_definition_tag<def::line_position_forward>{};

inline constexpr auto select_graphics_rendition   = detail::control_sequence_definition_tag<def::select_graphics_rendition>{};

inline constexpr auto delete_character            = detail::control_sequence_definition_tag<def::delete_character>{};
inline constexpr auto delete_line                 = detail::control_sequence_definition_tag<def::delete_line>{};
inline constexpr auto erase_in_area               = detail::control_sequence_definition_tag<def::erase_in_area>{};
inline constexpr auto erase_character             = detail::control_sequence_definition_tag<def::erase_character>{};
inline constexpr auto erase_in_page               = detail::control_sequence_definition_tag<def::erase_in_page>{};
inline constexpr auto erase_in_field              = detail::control_sequence_definition_tag<def::erase_in_field>{};
inline constexpr auto erase_in_line               = detail::control_sequence_definition_tag<def::erase_in_line>{};
inline constexpr auto insert_character            = detail::control_sequence_definition_tag<def::insert_character>{};
inline constexpr auto insert_line                 = detail::control_sequence_definition_tag<def::insert_line>{};

inline constexpr auto cursor_backward_tabulation  = detail::control_sequence_definition_tag<def::cursor_backward_tabulation>{};
inline constexpr auto cursor_character_absolute   = detail::control_sequence_definition_tag<def::cursor_character_absolute>{};
inline constexpr auto cursor_forward_tabulation   = detail::control_sequence_definition_tag<def::cursor_forward_tabulation>{};
inline constexpr auto cursor_next_line            = detail::control_sequence_definition_tag<def::cursor_next_line>{};
inline constexpr auto cursor_preceding_line       = detail::control_sequence_definition_tag<def::cursor_preceding_line>{};
inline constexpr auto cursor_tabulation_control   = detail::control_sequence_definition_tag<def::cursor_tabulation_control>{};
inline constexpr auto cursor_left                 = detail::control_sequence_definition_tag<def::cursor_left>{};
inline constexpr auto cursor_down                 = detail::control_sequence_definition_tag<def::cursor_down>{};
inline constexpr auto cursor_right                = detail::control_sequence_definition_tag<def::cursor_right>{};
inline constexpr auto cursor_position             = detail::control_sequence_definition_tag<def::cursor_position>{};
inline constexpr auto cursor_up                   = detail::control_sequence_definition_tag<def::cursor_up>{};
inline constexpr auto cursor_line_tabulation      = detail::control_sequence_definition_tag<def::cursor_line_tabulation>{};

inline constexpr auto next_page                   = detail::control_sequence_definition_tag<def::next_page>{};
inline constexpr auto preceding_page              = detail::control_sequence_definition_tag<def::preceding_page>{};
inline constexpr auto scroll_down                 = detail::control_sequence_definition_tag<def::scroll_down>{};
inline constexpr auto scroll_left                 = detail::control_sequence_definition_tag<def::scroll_left>{};
inline constexpr auto scroll_up                   = detail::control_sequence_definition_tag<def::scroll_up>{};
inline constexpr auto scroll_right                = detail::control_sequence_definition_tag<def::scroll_right>{};
inline constexpr auto define_area_qualification   = detail::control_sequence_definition_tag<def::define_area_qualification>{};
inline constexpr auto reset_mode                  = detail::control_sequence_definition_tag<def::reset_mode>{};
inline constexpr auto set_mode                    = detail::control_sequence_definition_tag<def::set_mode>{};
inline constexpr auto restore_cursor              = detail::control_sequence_definition_tag<def::restore_cursor>{};
inline constexpr auto save_cursor                 = detail::control_sequence_definition_tag<def::save_cursor>{};
inline constexpr auto set_top_and_bottom_margins  = detail::control_sequence_definition_tag<def::set_top_and_bottom_margins>{};

} // namespace ecma48
} // namespace termcontrol