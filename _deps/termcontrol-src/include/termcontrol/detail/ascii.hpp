#pragma once

/// The ascii character set.
/*
legend:
    char     the printed representation of the character, if any
    dec      the decimal code for the character
    row/col  the decimal row/column representation for the character
    oct      the octal (base 8) code for the character
    hex      the hexadecimal (base 16) code for the character
    name and description
*/
// sources: http://ascii-table.com/ascii.php
//         http://www.kermitproject.org/ascii.html

namespace termcontrol::ascii {

// control code block - short name
static constexpr char const nul = 0;   // null
static constexpr char const soh = 1;   // start of header
static constexpr char const stx = 2;   // start of text
static constexpr char const etx = 3;   // end of text
static constexpr char const eot = 4;   // end of transmission
static constexpr char const enq = 5;   // enquiry
static constexpr char const ack = 6;   // acknowledge
static constexpr char const bel = 7;   // bell
static constexpr char const bs  = 8;   // backspace
static constexpr char const ht  = 9;   // horizontal tabulation
static constexpr char const lf  = 10;  // line feed
static constexpr char const vt  = 11;  // vertical tabulation
static constexpr char const ff  = 12;  // form feed
static constexpr char const cr  = 13;  // carriage return
static constexpr char const so  = 14;  // shift out
static constexpr char const si  = 15;  // shift in
static constexpr char const dle = 16;  // data link escape
static constexpr char const dc1 = 17;  // device control 1 (xon)
static constexpr char const dc2 = 18;  // device control 2
static constexpr char const dc3 = 19;  // device control 3 (xoff)
static constexpr char const dc4 = 20;  // device control 4
static constexpr char const nak = 21;  // negative acknowledge
static constexpr char const syn = 22;  // synchronous idle
static constexpr char const etb = 23;  // end of transmission block
static constexpr char const can = 24;  // cancel
static constexpr char const em  = 25;  // end of medium
static constexpr char const sub = 26;  // substitute
static constexpr char const esc = 27;  // escape
static constexpr char const fs  = 28;  // file separator
static constexpr char const gs  = 29;  // group separator
static constexpr char const rs  = 30;  // record separator
static constexpr char const us  = 31;  // unit separator

// control code block - long name
static constexpr char const null                      = 0;
static constexpr char const start_of_heading          = 1;
static constexpr char const start_of_text             = 2;
static constexpr char const end_of_text               = 3;
static constexpr char const end_of_transmission       = 4;
static constexpr char const enquiry                   = 5;
static constexpr char const acknowledge               = 6;
static constexpr char const bell                      = 7;
static constexpr char const backspace                 = 8;
static constexpr char const horizontal_tab            = 9;
static constexpr char const line_feed                 = 10;
static constexpr char const vertical_tab              = 11;
static constexpr char const form_feed                 = 12;
static constexpr char const carriage_return           = 13;
static constexpr char const shift_out                 = 14;
static constexpr char const shift_in                  = 15;
static constexpr char const data_link_escape          = 16;
static constexpr char const device_control_1          = 17;
static constexpr char const device_control_2          = 18;
static constexpr char const device_control_3          = 19;
static constexpr char const device_control_4          = 20;
static constexpr char const negative_acknowledge      = 21;
static constexpr char const synchronous_idle          = 22;
static constexpr char const end_of_transmission_block = 23;
static constexpr char const cancel                    = 24;
static constexpr char const end_of_medium             = 25;
static constexpr char const substitute                = 26;
static constexpr char const escape                    = 27;
static constexpr char const file_separator            = 28;
static constexpr char const group_separator           = 29;
static constexpr char const record_separator          = 30;
static constexpr char const unit_separator            = 31;

// printable character block
static constexpr char const space                 = 32;  // [space]
static constexpr char const exclamation_mark      = 33;  // !
static constexpr char const quotes                = 34;  // "
static constexpr char const hash                  = 35;  // #
static constexpr char const dollar                = 36;  // $
static constexpr char const percent               = 37;  // %
static constexpr char const ampersand             = 38;  // %
static constexpr char const apostrophe            = 39;  // '
static constexpr char const open_parenthesis      = 40;  // (
static constexpr char const close_parenthesis     = 41;  // )
static constexpr char const asterisk              = 42;  // *
static constexpr char const plus                  = 43;  // +
static constexpr char const comma                 = 44;  // ,
static constexpr char const dash                  = 45;  // -
static constexpr char const full_stop             = 46;  // .
static constexpr char const slash                 = 47;  // /
static constexpr char const digit_zero            = 48;  // 0
static constexpr char const digit_one             = 49;  // 1
static constexpr char const digit_two             = 50;  // 2
static constexpr char const digit_three           = 51;  // 3
static constexpr char const digit_four            = 52;  // 4
static constexpr char const digit_five            = 53;  // 5
static constexpr char const digit_six             = 54;  // 6
static constexpr char const digit_seven           = 55;  // 7
static constexpr char const digit_eight           = 56;  // 8
static constexpr char const digit_nine            = 57;  // 9
static constexpr char const colon                 = 58;  // :
static constexpr char const semi_colon            = 59;  // ;
static constexpr char const less_than             = 60;  // <
static constexpr char const equals                = 61;  // =
static constexpr char const greater_than          = 62;  // >
static constexpr char const question_mark         = 63;  // ?
static constexpr char const at                    = 64;  // @
static constexpr char const capital_letter_a      = 65;  // a
static constexpr char const capital_letter_b      = 66;  // b
static constexpr char const capital_letter_c      = 67;  // c
static constexpr char const capital_letter_d      = 68;  // d
static constexpr char const capital_letter_e      = 69;  // e
static constexpr char const capital_letter_f      = 70;  // f
static constexpr char const capital_letter_g      = 71;  // g
static constexpr char const capital_letter_h      = 72;  // h
static constexpr char const capital_letter_i      = 73;  // i
static constexpr char const capital_letter_j      = 74;  // j
static constexpr char const capital_letter_k      = 75;  // k
static constexpr char const capital_letter_l      = 76;  // l
static constexpr char const capital_letter_m      = 77;  // m
static constexpr char const capital_letter_n      = 78;  // n
static constexpr char const capital_letter_o      = 79;  // o
static constexpr char const capital_letter_p      = 80;  // p
static constexpr char const capital_letter_q      = 81;  // q
static constexpr char const capital_letter_r      = 82;  // r
static constexpr char const capital_letter_s      = 83;  // s
static constexpr char const capital_letter_t      = 84;  // t
static constexpr char const capital_letter_u      = 85;  // u
static constexpr char const capital_letter_v      = 86;  // v
static constexpr char const capital_letter_w      = 87;  // w
static constexpr char const capital_letter_x      = 88;  // x
static constexpr char const capital_letter_y      = 89;  // y
static constexpr char const capital_letter_z      = 90;  // z
static constexpr char const open_bracket          = 91;  // [
static constexpr char const backslash             = 92;  // [\]
static constexpr char const close_bracket         = 93;  // ]
static constexpr char const caret                 = 94;  // ^
static constexpr char const underscore            = 95;  // _
static constexpr char const grave                 = 96;  // `
static constexpr char const small_letter_a        = 97;  // a
static constexpr char const small_letter_b        = 98;  // b
static constexpr char const small_letter_c        = 99;  // c
static constexpr char const small_letter_d        = 100; // d
static constexpr char const small_letter_e        = 101; // e
static constexpr char const small_letter_f        = 102; // f
static constexpr char const small_letter_g        = 103; // g
static constexpr char const small_letter_h        = 104; // h
static constexpr char const small_letter_i        = 105; // i
static constexpr char const small_letter_j        = 106; // j
static constexpr char const small_letter_k        = 107; // k
static constexpr char const small_letter_l        = 108; // l
static constexpr char const small_letter_m        = 109; // m
static constexpr char const small_letter_n        = 110; // n
static constexpr char const small_letter_o        = 111; // o
static constexpr char const small_letter_p        = 112; // p
static constexpr char const small_letter_q        = 113; // q
static constexpr char const small_letter_r        = 114; // r
static constexpr char const small_letter_s        = 115; // s
static constexpr char const small_letter_t        = 116; // t
static constexpr char const small_letter_u        = 117; // u
static constexpr char const small_letter_v        = 118; // v
static constexpr char const small_letter_w        = 119; // w
static constexpr char const small_letter_x        = 120; // x
static constexpr char const small_letter_y        = 121; // y
static constexpr char const small_letter_z        = 122; // z
static constexpr char const open_brace            = 123; // {
static constexpr char const pipe                  = 124; // |
static constexpr char const close_brace           = 125; // }
static constexpr char const tilde                 = 126; // ~
static constexpr char const delete_               = 127; // [delete]


/// Return the ascii character with given Row/Column representation.
template <typename CharT = char, typename CharTraits = std::char_traits<CharT>>
constexpr char from_column_row(std::uint8_t col, std::uint8_t row)
{ return CharTraits::to_char_type((16 * col) + row); }

}