#ifndef TIMEDCYCLE
#define TIMEDCYCLE

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>

// adapted to use closures.
// https://github.com/kyuds/timedcycle

namespace raft{
    
class TimedCycle {
    public:
        TimedCycle(std::function<std::chrono::milliseconds()> interval, 
                   std::function<void()> task);
        ~TimedCycle();

        void reset();

        void pause();
        void resume();
    
    private:
        static void thread_func(TimedCycle * t);
        bool should_die();
        bool should_pause();

        std::function<std::chrono::milliseconds()> interval;
        std::thread worker;
        std::mutex cv_m;
        std::condition_variable cv_c;
        std::mutex p_m;
        std::condition_variable p_c;
        std::atomic<bool> alive;
        std::atomic<bool> halt;
        std::function<void()> task;
};

} // namespace raft

#endif // TIMEDCYCLE
