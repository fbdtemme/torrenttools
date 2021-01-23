#include <cliprogressbar/widget.hpp>
#include <cliprogressbar/layout/layout_item.hpp>
#include <cliprogressbar/alignment.hpp>

#include <cstdint>
#include <memory>

namespace cliprogress {

class widget_layout_item : public layout_item
{
public:
    explicit widget_layout_item(widget* w)
        : widget_(w)
    {}

    std::size_t minimum_size() const override
    { return widget_->minimum_size(); };

    std::size_t natural_size() const override
    { return widget_->natural_size(); };

    std::size_t maximum_size() const override
    { return widget_->maximum_size(); };

    std::size_t allocated_size() const override
    { return widget_->allocated_size(); };

    void allocate_size(std::size_t size) override
    { widget_->allocate_size(size); }

    size_policy get_size_policy() const override
    {
        return widget_->get_size_policy();
    }

    bool is_widget() const override
    { return true; }

    widget* get_widget() override
    { return widget_; }

private:
    widget* widget_;
};

}