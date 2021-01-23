//
// Created by fbdtemme on 8/30/20.
//
#pragma once
#include <memory>

#include "cliprogressbar/events/event.hpp"
namespace cliprogress
{
struct widget;

enum class child_action
{
    added,
    removed
};

class child_event : public event
{
public:
    child_event(std::weak_ptr<widget> source, std::weak_ptr<widget> child, child_action act)
            : source_(std::move(source))
            , child_(std::move(child))
            , action_(act)
            , event(event_type::child)
    {}

    child_action action() const
    {
        return action_;
    }

    std::weak_ptr<widget> source() const
    {
        return source_;
    }

    std::weak_ptr<widget> child() const
    {
        return child_;
    }

private:
    std::weak_ptr<widget> source_;
    std::weak_ptr<widget> child_;
    child_action action_;
};

}