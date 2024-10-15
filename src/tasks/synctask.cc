#include "synctask.h"

namespace raft {

SyncTask::SyncTask(const int interval, std::function<void()> task)
    : interval(interval)
    , task(task)
    , alive(true)
{
    // start cycle on external thread
    worker = std::thread(SyncTask::thread_func, this);
}

SyncTask::~SyncTask() {
    alive.store(false);
    worker.join();
}

void SyncTask::thread_func(SyncTask * t) {
    while (true) {
        std::this_thread::sleep_for(t->interval);
        if (!t->alive.load()) break;
        if (t->halt.load()) {
            std::unique_lock lk(t->cv_m);
            t->cv_c.wait(lk);
        }
        t->task();
    }
}

void SyncTask::pause() {
    halt.store(true);
}

void SyncTask::resume() {
    halt.store(false);
    cv_c.notify_one();
}

} // namespace raft
