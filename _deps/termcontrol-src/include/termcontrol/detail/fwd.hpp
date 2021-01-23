#pragma once
#include <cstdint>

namespace termcontrol {

// parameters.hpp
enum class erase_line_mode : std::uint8_t;
enum class erase_page_mode : std::uint8_t;
enum class erase_field_mode : std::uint8_t;
enum class erase_area_mode : std::uint8_t;
enum class tab_control_mode : std::uint8_t;
enum class tab_clear_mode : std::uint8_t;
enum class qualified_area_mode : std::uint16_t;
enum class mode : std::uint8_t;
enum class dec_mode : std::uint16_t;

// colors.hpp
enum class terminal_color : std::uint8_t;
enum class css_color : std::uint32_t;
enum class x11_color : std::uint32_t;
enum class pwc_color: std::uint32_t;
enum class color : std::uint32_t;

struct rgb_color;
struct color_type;
struct text_style;

} // namespace termcontrol
