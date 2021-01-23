#pragma once
#include <string>
#include <string_view>
#include <ranges>


#include <termcontrol/detail/parse.hpp>
#include <termcontrol/detail/display_width.hpp>

#include "cliprogressbar/widget.hpp"
#include "cliprogressbar/alignment.hpp"

namespace cliprogress {

namespace rng = std::ranges;

enum class ellipsize_mode
{
    start,
    middle,
    end
};


class label : public widget
{
public:
    label()
        : text_()
        , text_display_width_(0)
        , border_padding_(0)
        , ellipsize_mode_(ellipsize_mode::end)
        , alignment_(alignment::left)
        , size_(std::nullopt)
    {
        widget::set_size_policy({size_policy_flag::grow | size_policy_flag::shrink, 1});
    }

    explicit label(std::string text, std::optional<std::size_t> size = std::nullopt)
        : text_(std::move(text))
        , text_display_width_(termcontrol::display_width(text_))
        , border_padding_(0)
        , ellipsize_mode_(ellipsize_mode::end)
        , alignment_(alignment::left)
        , size_(std::nullopt)
    {
        widget::set_size_policy({size_policy_flag::grow | size_policy_flag::shrink, 1});
    }

    std::size_t natural_size() const override
    {
        // check if user has provided a size override.
        if (size_) return *size_;
        auto text_size = termcontrol::display_width(text_);
        return text_size + (2 * border_padding_);
    }

    /// Whitespace left and right from the borders of the label
    void set_padding(std::size_t n)
    {
        std::unique_lock lck{mutex_};
        border_padding_ = n;
    }

    /// Sets the with in unicode glyphs.
    /// By default the with is determined on the contained text in natural_size().
    void set_size(std::size_t n)
    {
        {
            std::unique_lock lck{mutex_};
            size_ = n;
        }
        queue_layout();
    }

    std::size_t get_size() const
    {
        if (size_) return *size_;
        return natural_size();
    }

    std::size_t get_padding() const
    {
        return border_padding_;
    }

    const std::string& get_text() const
    {
        return text_;
    }

    void set_text(std::string_view text)
    {
        std::size_t old_size;
        std::size_t new_size;
        {
            std::unique_lock lck{mutex_};
            old_size = text_display_width_;
            text_ = std::string(text);
            new_size = termcontrol::display_width(text_);
            text_display_width_ = new_size;
        }


        if (old_size != new_size) {
            queue_layout();
        }
        queue_render();
    }

    void set_ellipsize_mode(ellipsize_mode mode)
    {
        std::unique_lock lck{mutex_};
        ellipsize_mode_ = mode;
    }

    /// Render the widget to the assigned space.
    void render(terminal_writer& out) override
    {
        std::unique_lock lck{mutex_};

        if (allocated_size() < 3) {
            out.write("...");
            return;
        }

        auto available_text_size = allocated_size() - 2 * border_padding_;
        auto text_size = termcontrol::display_width(text_);
        auto text_padding = (available_text_size > text_size) ? available_text_size - text_size : 0;

        bool ellipsized = false;
        constexpr std::string_view ellipsis = "...";
        constexpr std::size_t ellipses_size = std::size(ellipsis);

        std::string ellipsized_text {};

        /// We need to ellipsize to fit in text space
        if (text_size > available_text_size) {
            ellipsized = true;

            if (ellipsize_mode_ == ellipsize_mode::start) {
                ellipsized_text = ellipsis;
                ellipsized_text.append(text_, text_.size() + ellipses_size - available_text_size);
            }
            if (ellipsize_mode_ == ellipsize_mode::end) {
                ellipsized_text.append(text_, 0, available_text_size-ellipses_size);
                ellipsized_text.append(ellipsis);
            }
            else if (ellipsize_mode_ == ellipsize_mode::middle) {
                auto start_text_size = (available_text_size - ellipses_size) / 2;
                auto end_text_size = start_text_size
                                     + std::remainder(available_text_size - ellipses_size, 2) > 0;
                ellipsized_text.append(text_, 0, start_text_size);
                ellipsized_text.append(ellipsis);
                ellipsized_text.append(text_, text_.size() - end_text_size);
            }
        }

        std::size_t start_text_padding = text_padding / 2;
        std::size_t end_text_padding = text_padding / 2 + (std::remainder(text_padding, 2) > 0);

        if (alignment_ == alignment::left) {
            start_text_padding = 0;
            end_text_padding = text_padding;
        }
        else if (alignment_ == alignment::center) {
            start_text_padding = text_padding / 2;
            end_text_padding = text_padding / 2 + (std::remainder(text_padding, 2) > 0);
        }
        else if (alignment_ == alignment::right) {
            start_text_padding = 0;
            end_text_padding = text_padding;
        }

        out.write(border_padding_, ' ');
        out.write(start_text_padding, ' ');
        out.write((ellipsized ? ellipsized_text : text_));
        out.write(end_text_padding, ' ');
        out.write(border_padding_, ' ');
    }

private:
    /// The formatted text to display
    std::string text_ ;
    std::size_t text_display_width_;
    std::size_t border_padding_;
    ellipsize_mode ellipsize_mode_;
    enum alignment alignment_;
    std::optional<std::size_t> size_;
};



} // namespace cliprogress