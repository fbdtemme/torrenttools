//
// Created by fbdtemme on 8/30/20.
//
#include <memory>

#include "cliprogressbar/widget.hpp"
#include "event.hpp"

namespace cliprogress
{

class close_event : public event
{
public:
    close_event(std::weak_ptr<widget> target, std::weak_ptr<widget> source)
            : target_(std::move(target))
            , source_(std::move(source))
            , event(event_type::close)
    {}

    std::weak_ptr<widget> target() const
    {
        return target_;
    }

    std::weak_ptr<widget> source() const
    {
        return source_;
    }

private:
    std::weak_ptr<widget> target_;
    std::weak_ptr<widget> source_;

};
}