#pragma once

#include <memory>
#include <cliprogressbar/events/event.hpp>
#include <cliprogressbar/widget.hpp>

namespace cliprogress {

/// Sigint signal
class termination_event : public event
{
public:
    termination_event()
            : event(event_type::termination)
    {};
};

}