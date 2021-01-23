#pragma once
#include <ostream>

#include "definitions.hpp"
#include "color.hpp"
#include "formatted_size.hpp"

namespace termcontrol {

class text_style
{
public:
    /// Set graphics rendition to the system defined defaults.
    static constexpr text_style reset()
    {
        return text_style(emphasis::reset, terminal_color::reset, terminal_color::reset);
    }
    /// Do not change the current graphics rendition.
    constexpr text_style() noexcept
            : emphasis_()
            , foreground_color_()
            , background_color_() {};

    constexpr text_style(emphasis em) noexcept   // NOLINT
            : emphasis_(em)
            , foreground_color_()
            , background_color_() { }

    constexpr text_style(color_type color, bool is_foreground = true) noexcept    // NOLINT
            : emphasis_()
            , foreground_color_(is_foreground ? decltype(foreground_color_){color} : std::nullopt)
            , background_color_(!is_foreground ? decltype(background_color_){color} : std::nullopt)
    {}

    constexpr text_style(emphasis em, color_type color, bool is_foreground = true) noexcept   // NOLINT
            : text_style(color, is_foreground)
    {
        emphasis_ = em;
    }

    constexpr text_style(emphasis em, color_type fg, color_type bg) noexcept    // NOLINT
            : emphasis_(em)
            , foreground_color_(fg)
            , background_color_(bg) { }

    constexpr bool has_foreground_color() const noexcept
    { return foreground_color_.has_value(); }

    constexpr bool has_background_color() const noexcept
    { return background_color_.has_value(); }

    constexpr bool has_emphasis() const noexcept
    { return std::underlying_type_t<emphasis>(emphasis_) != 0; }

    constexpr color_type get_foreground_color() const noexcept
    {
        Ensures(foreground_color_.has_value());
        return *foreground_color_;
    }

    constexpr color_type get_background_color() const noexcept
    {
        Ensures(background_color_.has_value());
        return *background_color_;
    }

    constexpr emphasis get_emphasis() const noexcept
    { return emphasis_; }

    bool empty() const noexcept
    {
        return !has_emphasis() && !has_background_color() && ! has_background_color();
    }

    constexpr text_style& operator|=(const text_style& rhs)
    {
        if (!foreground_color_.has_value()) {
            foreground_color_ = rhs.foreground_color_;
        }
            // OR of colors for RGB colors
        else if (foreground_color_.has_value() && rhs.foreground_color_.has_value()) {
            throw std::invalid_argument("cannot OR colors");
        }

        if (!background_color_.has_value()) {
            background_color_ = rhs.background_color_;
        }
        else if (background_color_.has_value() && rhs.background_color_.has_value()) {
            throw std::invalid_argument("cannot OR colors");
        }

        emphasis_ |= rhs.emphasis_;
        return *this;
    }

private:
    emphasis emphasis_;
    std::optional<color_type> foreground_color_;
    std::optional<color_type> background_color_;
};

constexpr auto operator|(const text_style& lhs, const text_style& rhs) -> text_style
{
    auto r = lhs;
    return r |= rhs;
}

constexpr text_style fg(color_type c) noexcept
{ return text_style(c, /*is_foreground=*/true); }

constexpr text_style bg(color_type c) noexcept
{ return text_style(c, /*is_foreground=*/false); }

constexpr text_style em(emphasis e) noexcept
{ return text_style(e); }

TERMCONTROL_DEFINE_MAX_FORMATTED_SIZE(text_style,
        detail::max_formatted_size_v<emphasis>
        + 2 * detail::max_formatted_size_v<rgb_color>);

}