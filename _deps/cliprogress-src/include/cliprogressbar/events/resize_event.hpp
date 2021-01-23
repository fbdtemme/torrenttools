#pragma once

#include <memory>
#include <cliprogressbar/events/event.hpp>
#include <cliprogressbar/widget.hpp>

namespace cliprogress {

class resize_event : public event
{
public:
    using terminal_size = termcontrol::terminal_size;

    resize_event(const terminal_size& new_size , const terminal_size& old_size)
            : event(event_type::resize)
            , new_size_(new_size)
            , old_size_(old_size)
    {}

    const terminal_size& size() const
    {
        return new_size_;
    }

    const terminal_size& previous_size() const
    {
        return old_size_;
    }

private:
    terminal_size new_size_;
    terminal_size old_size_;
};

}