#include "timedcycle.h"

#include <iostream>

namespace raft {

TimedCycle::TimedCycle(std::function<std::chrono::milliseconds()> interval, 
                       std::function<void()> task)
    : interval(interval)
    , task(task) {
    alive.store(true, std::memory_order_relaxed);
    worker = std::thread(TimedCycle::thread_func, this);
}

TimedCycle::~TimedCycle() {
    alive.store(false, std::memory_order_release);
    reset();
    worker.join();
    std::cout << "DELETING TASK" << std::endl; 
}

void TimedCycle::thread_func(TimedCycle * t) {
    while (true) {
        std::unique_lock lk(t->cv_m);
        auto status = t->cv_c.wait_for(lk, t->interval());
        if (t->should_die()) break;
        if (t->should_pause()) {
            std::cout << "PAUSING TASK" << std::endl;
            std::unique_lock plk(t->p_m);
            t->p_c.wait(plk);
            std::cout << "WOKE UP" << std::endl;
        }
        if (status == std::cv_status::timeout) {
            t->task();
        }
        if (t->should_die()) break;
        if (t->should_pause()) {
            std::cout << "PAUSING TASK" << std::endl;
            std::unique_lock plk(t->p_m);
            t->p_c.wait(plk);
            std::cout << "WOKE UP" << std::endl;
        }
    }
}

void TimedCycle::reset() {
    cv_c.notify_one();
}

void TimedCycle::pause() {
    halt.store(true, std::memory_order_relaxed);
}

void TimedCycle::resume() {
    halt.store(false, std::memory_order_relaxed);
    p_c.notify_one();
}

bool TimedCycle::should_die() {
    return !alive.load(std::memory_order_relaxed);
}

bool TimedCycle::should_pause() {
    return halt.load(std::memory_order_relaxed);
}

} // namespace raft
