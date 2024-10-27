#ifndef RAFT_ELECTION_TASK
#define RAFT_ELECTION_TASK

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>

// adapted to use closures.
// https://github.com/kyuds/timedcycle

namespace raft {
    
class ElectionTask {
    public:
        ElectionTask(
            std::function<std::chrono::milliseconds()> interval_func, 
            std::function<void()> task);
        ~ElectionTask();

        void reset();
    
    private:
        static void thread_func(ElectionTask * t);

        std::function<std::chrono::milliseconds()> interval_func;
        std::thread worker;
        std::mutex cv_m;
        std::condition_variable cv_c;
        std::atomic<bool> alive;
        std::function<void()> task;
};

} // namespace raft

#endif // RAFT_ELECTION_TASK
