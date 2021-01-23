
#include "cliprogressbar/widget.hpp"
#include "cliprogressbar/layout/layout_manager.hpp"
#include <cliprogressbar/events/event.hpp>
namespace cliprogress
{

widget::widget()
{
    id_ = guid_counter_.fetch_add(1);
}

std::size_t widget::id() const noexcept
{
    return id_;
}

std::string_view widget::name() const noexcept
{
    return name_;
}

void widget::set_name(std::string_view name)
{
    name_ = std::string(name);
}

void cliprogress::widget::queue_render() {
    auto e = std::make_shared<render_event>(weak_from_this());
    // Pass the event to the event queue do not pass a target
    // since we do not support partial renders
    application::instance()->queue_event(std::move(e));
}

void widget::queue_layout() {
    auto e = std::make_shared<layout_event>(weak_from_this());
    application::instance()->queue_event(std::move(e), get_parent());
}

void widget::close()
{
    auto e = std::make_shared<close_event>(weak_from_this(), weak_from_this());
    application::instance()->queue_event(std::move(e), weak_from_this());
    closed();

    std::unique_lock lck(mutex_);
    wait_closed_.wait(lck, [&](){ return closed_.load(std::memory_order_relaxed); });
}

void widget::render(terminal_writer& out)
{
    namespace tc = termcontrol;
    if (children_.empty()) return;

    out.write(tc::format<tc::def::character_position_absolute>(position()+1));

    auto spacing = layout_manager_->get_spacing();
    auto curr_size = 0;

    for (int idx = 0; idx < int(layout_manager_->size()) - 1 ; ++idx) {
        const auto& item = layout_manager_->at(idx);

        if (item->is_widget()) {
            auto* w = item->get_widget();
            if (w->is_visible()) {
                curr_size += w->allocated_size();
                w->render(out);
            } else {
                auto i = 3;
            }
            out.write(spacing, ' ');
            curr_size += spacing;
        }
    }

    const auto& item = layout_manager_->at(layout_manager_->size()-1);

    if (item->is_widget()) {
        auto* w = item->get_widget();

        if (w->is_visible()) {
            curr_size += w->allocated_size();
            w->render(out);
        }
    }
    out.write(tc::format(tc::ecma48::erase_in_line, tc::erase_line_mode::after));
}

bool widget::is_visible() const
{
    return visible_.load(std::memory_order_relaxed);
}

void widget::set_visible(bool flag)
{
    visible_.store(flag, std::memory_order_relaxed);

    if (flag) {
        auto e = std::make_shared<show_event>();
        application::instance()->queue_event(std::move(e), weak_from_this());
    } else {
        auto e = std::make_shared<hide_event>();
        application::instance()->queue_event(std::move(e), weak_from_this());
    }
    visibility_changed();
}


void widget::show()
{
    set_visible(true);
}

void widget::hide()
{
    set_visible(false);
}


std::size_t widget::minimum_size() const {
    return 0;
}

std::size_t widget::natural_size() const {
    return 0;
}

std::size_t widget::maximum_size() const {
    return std::numeric_limits<std::size_t>::max();
}


std::size_t widget::allocated_size() const {
    return allocated_size_.load(std::memory_order_acquire);
}

void widget::allocate_size(std::size_t size) {
    Ensures(size >= 0);
    allocated_size_.store(size, std::memory_order_release);
}

std::size_t widget::position() const {
    return position_.load(std::memory_order_acquire);
}

void widget::set_position(std::size_t pos) {
    position_.store(pos, std::memory_order_release);
}

size_policy widget::get_size_policy() const {
    std::unique_lock lck(mutex_);
    return size_policy_;
}

enum alignment widget::get_alignment() const {
    return alignment_.load(std::memory_order_acquire);
}

void widget::set_size_policy(const size_policy& policy) {
    std::unique_lock lck(mutex_);
    size_policy_ = policy;
}

void widget::set_alignment(cliprogress::alignment alm) {
    alignment_.store(alm, std::memory_order_release);
}

void widget::set_parent(std::weak_ptr<widget> parent) {
    std::unique_lock lck(ptr_mutex_);
    parent_ = std::move(parent);
}

std::shared_ptr<widget> widget::get_parent() const {
    std::unique_lock lck(ptr_mutex_);
    return parent_.lock();
}

std::shared_ptr<widget> widget::get_root_parent() const {
    std::unique_lock lck(ptr_mutex_);
    auto prev = std::const_pointer_cast<widget>(shared_from_this());
    for (auto p = parent_.lock(); p ; prev = std::move(p)) {};
    return prev;
}

void widget::add_child(std::shared_ptr<widget> w)
{
    std::unique_lock lck(ptr_mutex_);
    children_.push_back(std::move(w));
    auto e = std::make_shared<child_event>(
            weak_from_this(),
            children_.back(),
            child_action::added);

    application::instance()->queue_event(std::move(e));
    child_added(w.get());
}

void widget::remove_child(widget* w)
{
    std::unique_lock lck(ptr_mutex_);

    auto it = std::find_if(children_.begin(), children_.end(),
            [&](const auto& ptr) { return ptr.get() == w; });

    auto e = std::make_shared<child_event>(
            weak_from_this(),
            *it,
            child_action::removed);

    application::instance()->queue_event(std::move(e));
    children_.erase(it);

    // remove child from layout
    if (layout_manager_) {
        if (auto l = layout_manager_->find(w); l) {
            layout_manager_->erase(l);
        }
    }

    child_removed(w);
}

void widget::remove_child(const std::shared_ptr<widget>& w)
{
    return remove_child(w.get());
}

std::shared_ptr<layout_manager> widget::get_layout_manager()
{
    return layout_manager_;
}


void widget::set_layout_manager(std::shared_ptr<layout_manager> layout)
{
    layout_manager_ = std::move(layout);
    layout_manager_->reparent_widgets(shared_from_this());
    layout_manager_->set_parent_widget(weak_from_this());
}


void widget::on_event(std::shared_ptr<event> e) {
    switch (e->type()) {
    case event_type::close: {
        on_close_event(std::dynamic_pointer_cast<close_event>(e));
        break;
    }
    case event_type::hide: {
        on_hide_event(std::dynamic_pointer_cast<hide_event>(e));
        break;
    }
    case event_type::show: {
        on_show_event(std::dynamic_pointer_cast<show_event>(e));
        break;
    }
    case event_type::resize: {
        on_resize_event(std::dynamic_pointer_cast<resize_event>(e));
        break;
    }
    case event_type::timer: {
        on_timer_event(std::dynamic_pointer_cast<timer_event>(e));
        break;
    }
    case event_type::layout: {
        on_layout_event(std::dynamic_pointer_cast<layout_event>(e));
        break;
    }
    case event_type::render: {
        on_render_event(std::dynamic_pointer_cast<render_event>(e));
        break;
    }
    case event_type::termination: {
        on_termination_event(std::dynamic_pointer_cast<termination_event>(e));
        break;
    }
    case event_type::child: {
        on_child_event(std::dynamic_pointer_cast<child_event>(e));
        break;
    }
    default:
        break;
    }

    if (!e->is_accepted()) {
        std::unique_lock lck{ptr_mutex_};
        rng::for_each(children_, [e = std::move(e)](auto& c) { c->on_event(e); });
    }
}


void widget::on_layout_event(std::shared_ptr<layout_event> e) {
    if (layout_manager_)
        layout_manager_->update();
    e->accept();
}

void widget::on_resize_event(std::shared_ptr<resize_event> e) {
    allocate_size(e->size().cols);
    if (layout_manager_)
        layout_manager_->update();
    resized();
    e->accept();
}

void widget::on_close_event(std::shared_ptr<close_event> e)
{
    auto p = get_parent();
    if (p)
        p->remove_child(this);
    else
        application::instance()->remove_widget(this);

    closed_.store(true, std::memory_order_relaxed);
    wait_closed_.notify_all();
    e->accept();
}

void widget::on_hide_event(std::shared_ptr<hide_event> e)
{
    e->accept();
}

void widget::on_show_event(std::shared_ptr<show_event> e)
{
    e->accept();
}


void widget::on_render_event(std::shared_ptr<render_event> e)
{
    render(application::instance()->writer());
    e->accept();
}


void widget::on_termination_event(std::shared_ptr<termination_event> e)
{
}


void widget::on_child_event(std::shared_ptr<child_event> e)
{
    if (e->action() == child_action::added) {
        auto child_ptr = e->child().lock();
        if (!child_ptr)
            return;
        application::instance()->register_widget_id(child_ptr->id(), get_root_parent());
    }
    if (e->action() == child_action::removed) {
        auto child_ptr = e->child().lock();
        if (!child_ptr)
            return;
        application::instance()->unregister_widget_id(child_ptr->id());
    }
    e->accept();
}
widget::~widget()
{
    destroyed();
};



}
