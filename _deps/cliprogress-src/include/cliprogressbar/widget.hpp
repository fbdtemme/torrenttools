#pragma once

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <list>
#include <memory>
#include <vector>

#include <sigslot/signal.hpp>

#include "cliprogressbar/alignment.hpp"
#include "cliprogressbar/application.hpp"
#include "cliprogressbar/event_queue.hpp"
#include "cliprogressbar/events/all.hpp"
#include "cliprogressbar/size_policy.hpp"
#include "cliprogressbar/terminal_writer.hpp"
#include <cliprogressbar/events/child_event.hpp>
#include <cliprogressbar/events/close_event.hpp>

#include <termcontrol/detail/definitions.hpp>

namespace cliprogress {

class application;
class layout_manager;

class widget : public std::enable_shared_from_this<widget>
{
public:
    using child_iterator = std::list<std::shared_ptr<widget>>::iterator;

    widget();

    virtual std::size_t id() const noexcept;

    virtual std::string_view name() const noexcept;

    virtual void set_name(std::string_view name);

    /// Widget content must be redrawn.
    virtual void queue_render();

    /// Widget dimensions must be renegotiated.
    virtual void queue_layout();

    /// Remove the widget on the next event loop iteration.
    virtual void close();

    /// Render the widget to the output buffer.
    virtual void render(terminal_writer& out);

    virtual bool is_visible() const;

    virtual void set_visible(bool flag);

    virtual void show();

    virtual void hide();

    virtual std::size_t minimum_size() const;

    virtual std::size_t natural_size() const;

    virtual std::size_t maximum_size() const;

    virtual std::size_t allocated_size() const;

    virtual void allocate_size(std::size_t size);

    std::size_t position() const;

    void set_position(std::size_t pos);

    virtual size_policy get_size_policy() const;

    virtual void set_size_policy(const size_policy& policy);

    virtual enum alignment get_alignment() const;

    virtual void set_alignment(cliprogress::alignment alm);

    void set_parent(std::weak_ptr<widget> parent);

    std::shared_ptr<widget> get_parent() const;

    std::shared_ptr<widget> get_root_parent() const;

    void add_child(std::shared_ptr<widget> w);

    void remove_child(widget* w);

    void remove_child(const std::shared_ptr<widget>& w);

    std::shared_ptr<layout_manager> get_layout_manager();

    void set_layout_manager(std::shared_ptr<layout_manager> layout);

    virtual void on_event(std::shared_ptr<event> e);

    virtual void on_layout_event(std::shared_ptr<layout_event> e);

    virtual void on_render_event(std::shared_ptr<render_event> e);

    virtual void on_resize_event(std::shared_ptr<resize_event> e);

    virtual void on_close_event(std::shared_ptr<close_event> e);

    virtual void on_hide_event(std::shared_ptr<hide_event> e);

    virtual void on_show_event(std::shared_ptr<show_event> e);

    virtual void on_termination_event(std::shared_ptr<termination_event> e);

    virtual void on_child_event(std::shared_ptr<child_event> e);

    virtual void on_timer_event(std::shared_ptr<timer_event> e) {};

    virtual ~widget();

    // signals
    sigslot::signal<widget*> child_added;
    sigslot::signal<widget*> child_removed;
    sigslot::signal<> resized;
    sigslot::signal<> closed;
    sigslot::signal<> destroyed;
    sigslot::signal<> visibility_changed;

protected:
    std::size_t id_;
    std::string name_;
    mutable std::recursive_mutex mutex_;

    std::atomic_size_t position_ = 0;
    std::atomic_size_t allocated_size_ = 0;
    std::atomic<enum alignment> alignment_;
    size_policy size_policy_ {};

    std::atomic_bool closed_ = false;
    std::atomic_bool visible_ = true;
    std::condition_variable_any wait_closed_;

    mutable std::mutex ptr_mutex_;
    std::weak_ptr<widget> parent_;
    std::list<std::shared_ptr<widget>> children_;

    std::shared_ptr<layout_manager> layout_manager_;

    friend application;
    friend layout_manager;

private:
    inline static std::atomic_size_t guid_counter_ = 0;
};

inline std::ostream& operator<<(std::ostream& os, widget& w)
{
    terminal_writer writer{};
    w.render(writer);
    return os;
}


} // namespace cliprogres