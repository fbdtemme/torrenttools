#include <thread>
#include <mutex>

#include "cliprogressbar/event_queue.hpp"

namespace cliprogress
{

class event_loop
{
public:
    event_loop() = default;

    void start();

    void stop();

    void is_running();

    /// Enqueue an event
    void queue_event(std::shared_ptr<event> e);

    /// Process an event immediatly
    void send_event(std::shared_ptr<event> e);

private:
    void main_loop();

    event_queue event_queue_ {};
    std::jthread event_loop_;
};


}