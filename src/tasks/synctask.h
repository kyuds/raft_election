#ifndef RAFT_SYNC_TASK
#define RAFT_SYNC_TASK

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>

// adapted to use closures.
// https://github.com/kyuds/timedcycle

namespace raft{
    
class SyncTask {
    public:
        SyncTask(const int interval, std::function<void()> task);
        ~SyncTask();

        void pause();
        void resume();
    
    private:
        static void thread_func(SyncTask * t);

        const std::chrono::milliseconds interval;
        std::thread worker;
        std::mutex cv_m;
        std::condition_variable cv_c;
        std::atomic<bool> alive;
        std::atomic<bool> halt;
        std::function<void()> task;
};

} // namespace raft

#endif // RAFT_SYNC_TASK
