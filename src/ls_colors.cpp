//
// Created by fbdtemme on 7/31/21.
#include "ls_colors.hpp"

namespace detail {

tc::emphasis parse_emphasis(std::uint8_t graphics_attribute)
{
    switch (graphics_attribute) {
    case 0  :  return tc::emphasis::reset;
    case 1  :  return tc::emphasis::bold;
    case 2  :  return tc::emphasis::faint;
    case 3  :  return tc::emphasis::italic;
    case 4  :  return tc::emphasis::underline;
    case 5  :  return tc::emphasis::blinking;
    case 7  :  return tc::emphasis::reverse;
    case 9  :  return tc::emphasis::striketrough;
    case 21 :  return tc::emphasis::double_underline;
    case 53 :  return tc::emphasis::overline;
    default :
        throw std::invalid_argument(
                fmt::format("no mapping from graphics attribute: {} to emphasis", graphics_attribute));
    }
}



std::pair<tc::text_style, std::size_t>
parse_color(std::string_view arguments)
{
    std::uint8_t int_value;
    std::size_t offset = 0;

    auto sep_it = rng::find(arguments, ';');
    auto value_substr = std::string_view(arguments.begin() + offset, sep_it);

    auto [p, ec] = std::from_chars(value_substr.begin(), value_substr.end(), int_value);
    if (ec != std::errc{}) { throw std::invalid_argument("Error parsing color: not an integer value"); }

    offset += value_substr.size();

    bool is_background_color;
    tc::color_type color;

    if (30 <= int_value && int_value <= 37) {
        is_background_color = false;
        color = tc::terminal_color(int_value);
    }
    else if (40 <= int_value && int_value <= 47) {
        is_background_color = true;
        color = tc::terminal_color(int_value-10);
    }
    else {
        if (int_value == 38) {
            is_background_color = false;
        }
        if (int_value == 48) {
            is_background_color = true;
        }

        if (arguments[offset] != ';') {
            throw std::invalid_argument("invalid parameters seperator");
        } else {
            ++offset;
        }

        sep_it = rng::find(arguments.substr(offset), ';');
        if (sep_it == arguments.end()) {
            throw std::invalid_argument("expected value after alternate color mode selector");
        }
        value_substr = std::string_view(arguments.begin() + offset, sep_it);
        auto [p, ec] = std::from_chars(value_substr.begin(), value_substr.end(), int_value);
        if (ec != std::errc{}) { throw std::invalid_argument("Error parsing alternate color mode"); }
        offset += value_substr.size() + 1;

        // 256-bit colors
        if (int_value == 5) {
            sep_it = rng::find(arguments.substr(offset), ';');
            value_substr = std::string_view(arguments.begin() + offset, sep_it);
            auto [p, ec] = std::from_chars(value_substr.begin(), value_substr.end(), int_value);
            if (ec != std::errc{}) { throw std::invalid_argument("Error parsing 256-bit color: not an integer value"); }
            offset += value_substr.size();
            color = tc::terminal_color_256(int_value);
        }
        else if (int_value == 2) {
            auto [rgb_color, rgb_offset] = parse_rgb_color_component(arguments.substr(offset));
            offset += rgb_offset;
            color = rgb_color;
        } else {
            throw std::invalid_argument("unrecognised alternate color mode");
        }
    }

    if (is_background_color) {
        return std::make_pair(tc::bg(color), offset);
    } else {
        return std::make_pair(tc::fg(color), offset);
    }
}

std::pair<tc::rgb_color, std::size_t>
parse_rgb_color_component(std::string_view rgb_component)
{
    std::uint8_t r, g, b;
    std::size_t offset = 0;

    auto sep_it = rng::find(rgb_component.substr(offset), ';');
    if (sep_it == rgb_component.end()) {
        throw std::invalid_argument("expected 3 color mode values, only found 1");
    }
    auto value_substr = std::string_view(rgb_component.begin() + offset, sep_it);
    auto [pr, ecr] = std::from_chars(value_substr.begin(), value_substr.end(), r);
    if (ecr != std::errc{}) {
        throw std::invalid_argument("Error parsing RGB color: R component is not an integer value");
    }
    offset += value_substr.size()+1;

    sep_it = rng::find(rgb_component.substr(offset), ';');
    if (sep_it == rgb_component.end()) {
        throw std::invalid_argument("expected 3 color mode values, only found 2");
    }
    value_substr = std::string_view(rgb_component.begin() + offset, sep_it);
    auto [pg, ecg] = std::from_chars(value_substr.begin(), value_substr.end(), g);
    if (ecg != std::errc{}) {
        throw std::invalid_argument("Error parsing RGB color: G component is not an integer value");
    }
    offset += value_substr.size()+1;

    sep_it = rng::find(rgb_component.substr(offset), ';');
    value_substr = std::string_view(rgb_component.begin() + offset, sep_it);
    auto [pb, ecb] = std::from_chars(value_substr.begin(), value_substr.end(), b);
    if (ecb != std::errc{}) {
        throw std::invalid_argument("Error parsing RGB color: B component is not an integer value");
    }
    offset += value_substr.size();

    return std::make_pair(tc::rgb_color(r, g, b), offset);
}


/// Parse the LS_COLORS string into a map of text_styles.
std::map<std::string, termcontrol::text_style>
parse_ls_colors(std::string_view ls_colors)
{
    using namespace std::string_literals;
    std::map<std::string, termcontrol::text_style> ls_style_map {};

    std::size_t ls_offset = 0, pos = 0, idx = 0, sep_prev = 0, sep = 0;
    std::uint8_t emphasis;
    std::uint8_t foreground;
    std::uint8_t background;

    while ((pos = ls_colors.find(':', ls_offset)) != std::string_view::npos) {
        auto entry = ls_colors.substr(ls_offset, pos-ls_offset);
        std::size_t equal_sign_pos = entry.find('=');
        auto key = entry.substr(0, equal_sign_pos);
        auto value = entry.substr(equal_sign_pos+1);

        termcontrol::text_style style;
        std::size_t offset = 0;
        auto sep = value.substr(offset).find(';');

        while (offset < value.size())
        {
            std::uint8_t int_value;
            auto part_str = value.substr(offset, sep);

            auto [p, ec] = std::from_chars(part_str.begin(), part_str.end(), int_value);

            // attribute is color or start of a color
            if (30 <= int_value && int_value <= 49
            || 90 <= int_value && int_value <= 97
            || 100 < int_value && int_value <= 107 )
            {
                auto res = parse_color(value.substr(offset));
                style |= res.first;
                offset += res.second;
            }
            else {
                auto res = parse_emphasis(int_value);
                style |= tc::em(res);
                offset += part_str.size();
            }

            if (offset < value.size()) {
                if (value[offset] == ';') { ++offset; }
                else { throw std::invalid_argument("invalid seperator"); }
            }
        }

        ls_style_map.insert_or_assign(std::string(key), style);
        ls_offset = pos;

        if (ls_offset < ls_colors.size()) {
            if (ls_offset == pos) { ++ls_offset; }
            else {
                throw std::invalid_argument("invalid entry seperator");
            }
            pos = ls_colors.find(':', ls_offset);
        } else {
            pos = std::string_view::npos;
        }
    }

    return ls_style_map;
}


} // namespace detail

ls_colors::ls_colors()
        : data_()
        , extensions_()
{
    load_system_colors();
}

termcontrol::text_style ls_colors::file_style(const dottorrent::file_entry& entry) const {
    if (entry.is_symlink()) {
        return data_.link;
    }
    else if (entry.is_executable()) {
        return data_.exec;
    }
    else {
        if (entry.path().has_extension()) {
            const auto& ext = entry.path().extension().string();

            // Prefer an extension
            if (auto it = extensions_.find(ext); it != extensions_.end()) {
                return it->second;
            }
        }

        return data_.normal;
    }
}

termcontrol::text_style ls_colors::directory_style() const {
    return data_.dir;
}


bool ls_colors::load_system_colors() noexcept {
    const char* lc_colors_p = std::getenv("LS_COLORS");
    if (!lc_colors_p)
        return false;

    try {
        auto map = detail::parse_ls_colors(lc_colors_p);

        data_.normal                = map["no"];
        data_.file                  = map["fi"];
        data_.dir                   = map["di"];
        data_.link                  = map["ln"];
        data_.exec                  = map["ex"];

        for (const auto& [k, v] : map) {
            if (k.starts_with('*')) {
                // strip * from key.
                // We keep the dot since fs::path::extension() includes the dot.
                extensions_.emplace(k.substr(1), v);
            }
        }
        return true;
    }
    catch (const std::exception& e) {
        return false;
    }
}

