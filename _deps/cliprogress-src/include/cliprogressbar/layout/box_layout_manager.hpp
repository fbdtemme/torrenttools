#pragma once

#include <memory>
#include <deque>

#include <cliprogressbar/layout/layout_manager.hpp>
#include <cliprogressbar/layout/layout_item.hpp>
#include <cliprogressbar/layout/widget_layout_item.hpp>
#include <numeric>

namespace cliprogress {


class box_layout_manager : public layout_manager
{
public:
    box_layout_manager() = default;

    void push_front(std::shared_ptr<widget> w)
    {
        widget* ptr = w.get();
        auto item = std::make_shared<widget_layout_item>(ptr);
        layout_items_.push_back(std::move(item));

        if (auto p = parent_widget_.lock()) {
            p->add_child(w);
            w->set_parent(p);
        } else {
            widget_buffer_.push_back(std::move(w));
        }
    }

    void push_back(std::shared_ptr<widget> w)
    {
        widget* ptr = w.get();
        auto item = std::make_shared<widget_layout_item>(ptr);
        layout_items_.push_back(std::move(item));

        if (auto p = parent_widget_.lock()) {
            p->add_child(w);
            w->set_parent(p);
        } else {
            widget_buffer_.push_back(std::move(w));
        }

        if (auto pw = parent_widget(); pw) {
            pw->queue_layout();
        }
    }

    /// Insert at postition into the box
    void insert(std::shared_ptr<widget> w, std::size_t pos)
    {
        widget* ptr = w.get();
        auto item = std::make_shared<widget_layout_item>(ptr);
        layout_items_.insert(std::next(layout_items_.begin(), pos), std::move(item));

        if (auto p = parent_widget_.lock()) {
            p->add_child(w);
        } else {
            widget_buffer_.push_back(std::move(w));
        }

        if (auto pw = parent_widget(); pw) {
            pw->queue_layout();
        }
    }

    std::size_t minimum_size() const override
    {
        return std::accumulate(layout_items_.begin(), layout_items_.end(), 0UL,
                [&](std::size_t init, auto& item) {
                    return init + get_spacing() + item->minimum_size();
                });
    }

    std::size_t natural_size() const override
    {
        return std::accumulate(layout_items_.begin(), layout_items_.end(), 0UL,
            [&](std::size_t init, auto& item) {
                    return init + get_spacing() + item->natural_size();
                });
    }

    std::size_t maximum_size() const override
    {
        return std::accumulate(layout_items_.begin(), layout_items_.end(), 0UL,
                [&](std::size_t init, auto& item) {
                    return init + get_spacing() + item->natural_size();
                });
    }

    size_policy get_size_policy() const override
    {
        if (auto pw = parent_widget_.lock(); pw) {
            pw->get_size_policy();
        }
        return {};
    }

    std::size_t size() const noexcept override
    { return layout_items_.size(); }

    std::shared_ptr<const layout_item> at(std::size_t pos) const override
    {
        return layout_items_.at(pos);
    }

    std::shared_ptr<layout_item> at(std::size_t pos) override
    {
        return layout_items_.at(pos);
    }


    std::shared_ptr<const layout_item> find(const widget* w) const override
    {
        auto it = rng::find_if(layout_items_, [w](const auto& l) {
            if (l->is_widget()) {
                return l->get_widget() == w;
            }
            return false;
        });
        return *it;
    };

    std::shared_ptr<layout_item> find(widget* w) override
    {
        return std::const_pointer_cast<layout_item>(find(const_cast<const widget*>(w)));
    };

    void erase(const std::shared_ptr<layout_item>& l) override
    {
        auto it = rng::find(layout_items_, l);
        if (it == layout_items_.end()) {
            throw std::invalid_argument("no layout item for widget");
        }
        layout_items_.erase(it);

        if (auto pw = parent_widget(); pw) {
            pw->queue_layout();
        }
    };

    void erase(std::size_t pos) override
    {
        Expects(pos < layout_items_.size());
        layout_items_.erase(std::next(layout_items_.begin(), pos));

        if (auto pw = parent_widget(); pw) {
            pw->queue_layout();
        }
    }

    void update() override
    {
        // available size to distribute among children
        int spacing_size = layout_items_.size() > 1 ? spacing_ * (layout_items_.size() - 1) : 0;
        int available = int(allocated_size()) - spacing_size;
        auto total_natural_size = natural_size();

        // allocate to each child its prefered size.
        for (auto& c : layout_items_) {
            auto s = c->natural_size();
            c->allocate_size(s);
            available -= s;
        }

        if (available > 0) {
            // allocate extra space to widgets that are expanding proportional to growth factors.
            auto [expanding_ch, total_expand_factor] = expanding_children();

            auto expanding_size = available;
            for (auto& c : expanding_ch) {
                auto sp = c->get_size_policy();
                std::size_t size_to_expand = sp.resize_factor() / total_expand_factor * expanding_size;
                c->allocate_size(c->allocated_size() + size_to_expand);
                available -= size_to_expand;
            }

            if (available > 0) {
                // if there are no expanding widgets assign to other widgets with growth_policy set.
                auto [growing_ch, total_growth_factor] = growing_children();

                auto grow_size = available;
                for (auto& c : growing_ch) {
                    auto sp = c->get_size_policy();
                    std::size_t extra_size = sp.resize_factor() / total_growth_factor * grow_size;
                    c->allocate_size(c->allocated_size() + extra_size);
                    available -= extra_size;
                }
            }
        }
        else {
            // remove  excess space from widgets that can shrink proportional to growth factors.

            auto [shrinking_ch, total_shrink_factor] = shrinking_children();

            auto shrink_size = -available;
            Ensures(shrink_size >= 0);
            for (auto& c : shrinking_ch) {
                auto sp = c->get_size_policy();
                // make sure to not shrink more than the size of the actual widget
                auto size_to_shrink = int(sp.resize_factor() / total_shrink_factor * shrink_size);
                auto current_size = int(c->allocated_size());
                auto new_size = (current_size - size_to_shrink);

                if (new_size >= int(c->minimum_size())) {
                    c->allocate_size(new_size);
                } else {
                    c->allocate_size(c->minimum_size());
                }
                available += size_to_shrink;
            }
        }
    }

protected:

    std::pair<std::vector<layout_item*>, double> growing_children()
    {
        std::vector<layout_item*> growing_children {};
        double total_growth_factor = 0;

        for (auto& c : layout_items_) {
            if (c->get_size_policy().can_grow()) {
                growing_children.push_back(c.get());
                total_growth_factor += c->get_size_policy().resize_factor();
            }
        }
        return {growing_children, total_growth_factor};
    }

    std::pair<std::vector<layout_item*>, std::size_t> shrinking_children()
    {
        std::vector<layout_item*> shrinking_children {};
        double total_shrink_factor = 0;

        for (auto& c : layout_items_) {
            if (c->get_size_policy().can_shrink()) {
                shrinking_children.push_back(c.get());
                total_shrink_factor += c->get_size_policy().resize_factor();
            }
        }
        return {shrinking_children, total_shrink_factor};
    }

    std::pair<std::vector<layout_item*>, std::size_t> expanding_children()
    {
        std::vector<layout_item*> expanding_children {};
        double total_expand_factor = 0;

        for (auto& c : layout_items_) {
            if (c->get_size_policy().is_expanding()) {
                expanding_children.push_back(c.get());
                total_expand_factor += c->get_size_policy().resize_factor();
            }
        }
        return {expanding_children, total_expand_factor};
    }


    std::deque<std::shared_ptr<layout_item>> layout_items_;
};

}