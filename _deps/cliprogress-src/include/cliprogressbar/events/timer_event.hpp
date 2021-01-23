#pragma once

#include <memory>
#include <cliprogressbar/events/event.hpp>
#include <cliprogressbar/widget.hpp>

namespace cliprogress {

class timer_event : public event
{
public:
    explicit timer_event(std::size_t timer_id)
        : event(event_type::timer)
        , timer_id_(timer_id)
    {}

    std::size_t timer_id() const
    {
        return timer_id_;
    }

private:
    int timer_id_;
};

}