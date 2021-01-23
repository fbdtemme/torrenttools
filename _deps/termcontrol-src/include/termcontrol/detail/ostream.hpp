#include "definitions.hpp"
#include "format.hpp"

namespace termcontrol {

#define TERMCONTROL_OSTREAM_PARAM_TEMPLATE(PARAM, DEFINITION)             \
inline std::ostream& operator<<(std::ostream& os, PARAM mode)             \
{                                                                         \
    format_to<DEFINITION>(std::ostreambuf_iterator<char>(os), mode);      \
    return os;                                                            \
}                                                                         \

TERMCONTROL_OSTREAM_PARAM_TEMPLATE(erase_line_mode,                   def::erase_in_line)
TERMCONTROL_OSTREAM_PARAM_TEMPLATE(erase_page_mode,                   def::erase_in_page)
TERMCONTROL_OSTREAM_PARAM_TEMPLATE(erase_in_field_mode,               def::erase_in_field)
TERMCONTROL_OSTREAM_PARAM_TEMPLATE(erase_in_area_mode,                def::erase_in_area)
TERMCONTROL_OSTREAM_PARAM_TEMPLATE(cursor_tabulation_control_mode,    def::cursor_tabulation_control)
TERMCONTROL_OSTREAM_PARAM_TEMPLATE(tabulation_clear_mode,             def::tabulation_clear)
TERMCONTROL_OSTREAM_PARAM_TEMPLATE(area_qualification,                def::define_area_qualification)

#undef TERMCONTROL_OSTREAM_PARAM_TEMPLATE

inline auto operator<<(std::ostream& os, const text_style& style) -> std::ostream&
{
    if (!style.empty())
        format_to<def::select_graphics_rendition>(std::ostreambuf_iterator<char>(os), style);
    return os;
}

}