#pragma once

#include "cliprogressbar/events/event.hpp"

namespace cliprogress {

class hide_event : public event
{
public:
    hide_event()
        : event(event_type::hide)
    {};
};

}