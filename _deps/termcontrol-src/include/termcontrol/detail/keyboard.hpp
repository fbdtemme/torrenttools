#pragma once

#include <cstdint>

namespace termcontrol {

// keypad buttons / function keys
namespace keypad {

inline static constexpr char function                     = ascii::tilde;
inline static constexpr std::int8_t home                  = 1;
inline static constexpr std::int8_t insert                = 2;
inline static constexpr std::int8_t del                   = 3;
inline static constexpr std::int8_t end                   = 4;
inline static constexpr std::int8_t pgup                  = 5;
inline static constexpr std::int8_t pgdn                  = 6;
inline static constexpr std::int8_t f1                    = 11;
inline static constexpr std::int8_t f2                    = 12;
inline static constexpr std::int8_t f3                    = 13;
inline static constexpr std::int8_t f4                    = 14;
inline static constexpr std::int8_t f5                    = 15;
inline static constexpr std::int8_t f6                    = 17; // skip 16.
inline static constexpr std::int8_t f7                    = 18;
inline static constexpr std::int8_t f8                    = 19;
inline static constexpr std::int8_t f9                    = 20;
inline static constexpr std::int8_t f10                   = 21;
inline static constexpr std::int8_t f11                   = 23; // skip 22.
inline static constexpr std::int8_t f12                   = 24;

}

// the following modifiers can apply to all keypad/function key controls.

namespace modifier {

inline static constexpr std::int8_t shift               = 2;
inline static constexpr std::int8_t alt                 = 3;
inline static constexpr std::int8_t shift_alt           = 4;
inline static constexpr std::int8_t ctrl                = 5;
inline static constexpr std::int8_t shift_ctrl          = 6;
inline static constexpr std::int8_t alt_ctrl            = 7;
inline static constexpr std::int8_t shift_alt_ctrl      = 8;
inline static constexpr std::int8_t meta                = 9;
inline static constexpr std::int8_t meta_shift          = 10;
inline static constexpr std::int8_t meta_alt            = 11;
inline static constexpr std::int8_t meta_shift_alt      = 12;
inline static constexpr std::int8_t meta_ctrl           = 13;
inline static constexpr std::int8_t meta_shift_ctrl     = 14;
inline static constexpr std::int8_t meta_alt_ctrl       = 15;
inline static constexpr std::int8_t meta_shift_alt_ctrl = 16;

}
} // namespace termcontrol
