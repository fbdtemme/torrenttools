#pragma once

#include <memory>
#include <cliprogressbar/events/event.hpp>
#include <cliprogressbar/widget.hpp>

namespace cliprogress {

class render_event : public event
{
public:
    render_event()
            : event(event_type::render)
            , source_()
    {}

    explicit render_event(std::weak_ptr<widget> source)
            : event(event_type::render)
            , source_(source)
    {}

    [[nodiscard]]
    std::weak_ptr<widget> source() const
    {
        return source_;
    }

private:
    std::weak_ptr<widget> source_;
};
}