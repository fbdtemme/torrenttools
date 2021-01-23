#pragma once
#include <cstdint>

#include <cliprogressbar/alignment.hpp>

namespace cliprogress {

class size_policy;
class widget;

class layout_item {
public:
    virtual std::size_t minimum_size() const = 0;

    virtual std::size_t natural_size() const = 0;

    virtual std::size_t maximum_size() const = 0;

    virtual std::size_t allocated_size() const = 0;

    virtual size_policy get_size_policy() const = 0;

    virtual void allocate_size(std::size_t) = 0;

    virtual bool is_widget() const { return false; }

    virtual bool is_spacer() const { return false; }

    virtual bool is_layout() const { return false; }

    virtual widget* get_widget() { return nullptr; }
};

}