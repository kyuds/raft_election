#include "electiontask.h"

namespace raft {

ElectionTask::ElectionTask(
    std::function<std::chrono::milliseconds()> interval_func, 
    std::function<void()> task)
    : interval_func(interval_func)
    , task(task)
    , alive(true)
{
    // start cycle on external thread.
    worker = std::thread(ElectionTask::thread_func, this);
}

ElectionTask::~ElectionTask() {
    alive.store(false);
    reset();
    worker.join();
}

void ElectionTask::thread_func(ElectionTask * t) {
    while (true) {
        std::unique_lock lk(t->cv_m);
        auto status = t->cv_c.wait_for(lk, t->interval_func());
        if (!t->alive.load()) break;
        if (status == std::cv_status::timeout)
            t->task();
    }
}

void ElectionTask::reset() {
    cv_c.notify_one();
}

} // namespace raft
