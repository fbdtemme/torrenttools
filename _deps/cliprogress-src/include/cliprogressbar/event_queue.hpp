#pragma once
#include <memory>


#include "cliprogressbar/mpmcqueue.hpp"
#include "cliprogressbar/events/event.hpp"

namespace cliprogress {

class event_item {
public:
    std::shared_ptr<event> value;
    std::weak_ptr<widget> destination;
};

using event_queue = rigtorp::mpmc::Queue<event_item>;

}