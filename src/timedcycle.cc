#include "timedcycle.h"

namespace raft {

TimedCycle::TimedCycle(int _interval, std::function<void()> _task)
    : interval(std::chrono::milliseconds(_interval)),
      task(_task) {
    alive.store(true, std::memory_order_relaxed);
    worker = std::thread(TimedCycle::thread_func, this);
}

TimedCycle::~TimedCycle() {
    alive.store(false, std::memory_order_release);
    reset();
    worker.join();
}

void TimedCycle::thread_func(TimedCycle * t) {
    while (true) {
        std::unique_lock lk(t->cv_m);
        auto status = t->cv_c.wait_for(lk, t->interval);
        if (t->should_die())
            break;
        if (status == std::cv_status::timeout) {
            t->task();
        }
        if (t->should_die())
            break;
    }
}

void TimedCycle::reset() {
    cv_c.notify_one();
}

bool TimedCycle::should_die() {
    return !alive.load(std::memory_order_relaxed);
}

} // namespace raft
