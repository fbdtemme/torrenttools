#pragma once

#include <fmt/format.h>

#include "color.hpp"
#include "control_sequence_definition.hpp"
#include "parameters.hpp"
#include "definitions.hpp"
#include "format.hpp"
#include "text_style.hpp"



namespace termcontrol {

/// Return a string formatted by std::format(`fmt_string`, `args...`) with given `style`.
template <typename... Ts>
auto format(text_style style, std::string_view fmt_string, Ts&& ... args) -> std::string
{
    using namespace termcontrol::definition_acronyms;
    std::string s{};
    auto out = std::back_inserter(s);
    out = termcontrol::format_to<sgr>(out, style);
    out = fmt::format_to(out, fmt_string, std::forward<Ts>(args)...);
    out = termcontrol::format_to<sgr>(out, text_style::reset());
    return s;
}

/// Write text formatted by std::format(`fmt_string`, `args...`) with given `style` to `out`.
template <typename OIter, typename... Ts>
auto format_to(OIter out, text_style style, std::string_view fmt_string, Ts&& ... args) -> OIter
{
    using namespace termcontrol::definition_acronyms;
    out = termcontrol::format_to<sgr>(out, style);
    out = fmt::format_to(out, fmt_string, std::forward<Ts>(args)...);
    out = termcontrol::format_to<sgr>(out, text_style::reset());
    return out;
}

} // namespace termcontrol