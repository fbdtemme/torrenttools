//
// Created by fbdtemme on 8/31/20.
//
#include <cstdint>
#include <memory>
#include <ranges>

#include <cliprogressbar/widget.hpp>
#include <cliprogressbar/layout/layout_item.hpp>

namespace cliprogress {

namespace rng = std::ranges;

/// Object responsible for sizing child widgets
class layout_manager : public layout_item
{
public:
    layout_manager() = default;

    /// Do layout
    virtual void update() = 0;

    virtual std::size_t get_spacing() const
    {
        return spacing_;
    }

    virtual void set_spacing(std::size_t size)
    {
        spacing_ = size ;
    }

    /// Return the parent of the widget or nullptr if layout is not assigned to any widget.
    virtual std::shared_ptr<widget> parent_widget()
    {
        return parent_widget_.lock();
    }

    size_t allocated_size() const override
    {
        if (auto pw = parent_widget_.lock(); pw) {
            return pw->allocated_size();
        }
        return 0;
    }

    void allocate_size(std::size_t size) override
    {
        if (auto pw = parent_widget_.lock(); pw) {
            pw->allocate_size(size);
        }
    }

    virtual std::size_t size() const noexcept = 0;

    virtual std::shared_ptr<const layout_item> at(std::size_t pos) const = 0;
    virtual std::shared_ptr<layout_item> at(std::size_t pos) = 0;
    virtual std::shared_ptr<const layout_item> find(const widget* w) const = 0;
    virtual std::shared_ptr<layout_item> find(widget* w) = 0;

    virtual void erase(std::size_t pos) = 0;
    virtual void erase(const std::shared_ptr<layout_item>& w) = 0;

    virtual ~layout_manager() = default;

protected:
    void set_parent_widget(std::weak_ptr<widget> parent)
    {
        parent_widget_ = std::move(parent);
    }

    void reparent_widgets(std::shared_ptr<widget> parent)
    {
        rng::for_each(widget_buffer_, [&](std::shared_ptr<widget>& w) {
            w->set_parent(parent);
            parent->add_child(std::move(w));
        });
        widget_buffer_.clear();
    }

    friend widget;

    // widgets that need to be reparented when the layout is assigned to a widget.
    std::vector<std::shared_ptr<widget>> widget_buffer_;
    std::weak_ptr<widget> parent_widget_;
    std::size_t spacing_ = 0;

};

}