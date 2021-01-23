#pragma once

#include <functional>
#include <span>
#include <string>
#include <any>
#include <atomic>
#include <ranges>

#include "cliprogressbar/widget.hpp"
#include "cliprogressbar/size_policy.hpp"
#include "cliprogressbar/periodic_timer.hpp"
#include "cliprogressbar/animation_styles.hpp"
#include "cliprogressbar/application.hpp"

#include <termcontrol/detail/display_width.hpp>

namespace cliprogress {

namespace rng = std::ranges;

/// Throbber which calls a callback function every time the animation is updated.
class animation : public widget
{
public:
    explicit animation(std::span<const std::string_view> frames, std::uint16_t interval)
            : frames_(frames)
            , timer_id_(reinterpret_cast<std::size_t>(this))
    {
        widget::set_size_policy(size_policy_flag::fixed);
        timer_.set_interval(std::chrono::milliseconds(interval));
        timer_.set_function([this]() {
            auto* app = application::instance();
            app->queue_event(
                    std::make_shared<timer_event>(timer_id_),
                    weak_from_this());
        });
        timer_.start();
    }

    explicit animation(const animation_style& style)
            : frames_(style.frames)
    {
        widget::set_size_policy(size_policy_flag::fixed);
        timer_.set_interval(std::chrono::milliseconds(style.interval));
        timer_.set_function([this]() {
            auto* app = application::instance();
            app->queue_event(
                    std::make_shared<timer_event>(timer_id_),
                    weak_from_this());
        });
        timer_.start();
    }

    void set_frames(std::span<const std::string_view> frames)
    {
        std::unique_lock lck{mutex_};
        frames_ = frames;
        frame_index_ = 0;
    }

    void set_interval(std::uint16_t interval)
    { timer_.set_interval(std::chrono::milliseconds(interval)); }

    void render(terminal_writer& out) override
    {
        out.write(current_frame());
    }

    std::size_t natural_size() const override
    {
        current_frame_buffer_.clear();
        rng::copy(current_frame(), std::back_inserter(current_frame_buffer_));
        auto r = termcontrol::display_width(current_frame_buffer_);
        return r;
    }

    void pause()
    { timer_.pause(); }

    void resume()
    { timer_.resume(); }

    auto is_running() const noexcept -> bool
    { return timer_.is_running(); }

    void update_frame() noexcept
    {
        std::unique_lock lck{mutex_};
        frame_index_ = (frame_index_ + 1) % frames_.size();
        queue_render();
    }

    std::string_view current_frame() const noexcept
    {
        std::unique_lock lck{mutex_};
        return frames_[frame_index_ % frames_.size()];
    }

    void on_timer_event(std::shared_ptr<timer_event> e) override
    {
        if (e->timer_id() == timer_id_) {
            update_frame();
            queue_render();
        }
    }

    ~animation() override
    {
        if (timer_.is_running())
            timer_.stop();
    }


private:
    std::span<const std::string_view> frames_;
    mutable std::string current_frame_buffer_ {};
    std::size_t frame_index_ = 0;
    detail::periodic_timer timer_;
    mutable std::mutex mutex_;
    std::function<void()> callback_;
    int timer_id_ = 0;
};

} // namespace cliprogress