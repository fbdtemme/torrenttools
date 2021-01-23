#pragma once
#include <cstdint>
#include <memory>
#include <termcontrol/termcontrol.hpp>
#include <iostream>
#include "cliprogressbar/terminal.hpp"

namespace cliprogress {

class widget;

enum class event_type
{
    unknown,
    close,
    hide,
    show,
    resize,
    timer,
    layout,
    render,
    termination,
    progress,
    child,
    change,
};

inline std::ostream& operator<<(std::ostream& os, event_type t)
{
    switch (t) {
    case event_type::resize :      os << "resize";      return os;
    case event_type::timer  :      os << "timer";       return os;
    case event_type::layout :      os << "layout";      return os;
    case event_type::render :      os << "render";      return os;
    case event_type::termination : os << "termination"; return os;
    case event_type::progress :    os << "progress";    return os;
    case event_type::unknown:      os << "unknown";     return os;
    case event_type::close:        os << "close";       return os;
    case event_type::hide:         os << "hide";        return os;
    case event_type::show:         os << "show";        return os;
    case event_type::child:        os << "child";       return os;
    case event_type::change:       os << "change";       return os;
    }
    return os;
}


class event
{
public:
    event() noexcept
        : type_(event_type::unknown)
    {}

    explicit event(event_type type) noexcept
        : type_(type)
    {}

    event_type type() const noexcept
    {
        return type_;
    }

    bool is_accepted() const noexcept
    {
        return is_accepted_;
    }

    void accept() noexcept
    {
        is_accepted_ = true;
    }

    void ignore() noexcept
    {
        is_accepted_ = false;
    }

    virtual ~event() = default;

private:
    event_type type_;
    bool is_accepted_ = false;
};



inline std::ostream& operator<<(std::ostream& os, const event& e)
{
    return os << "Event: " << e.type();
}

} // namespace cliprogres