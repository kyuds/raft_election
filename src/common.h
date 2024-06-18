#ifndef RAFT_COMMON_DEFINES
#define RAFT_COMMON_DEFINES

#include <functional>
#include <string>

#define RAFT_PSTATE_FILE        "pstate.txt"
// #define RAFT_LOG_DIR        "logs"
// #define RAFT_SNAPSHOT_DIR   "snapshots"

#define PSTATE_INIT             pstate_t {0, ""}

namespace raft {

// raft node state.
enum class State {
    Leader,
    Follower,
    Candidate
};

typedef struct {
    uint64_t term;
    std::string voted_for;
} pstate_t;

typedef std::function<void(uint64_t, bool)> rv_clbk_t;
typedef std::function<void(uint64_t, bool)> ae_clbk_t;

typedef std::function<void(uint64_t, const std::string&)> rv_service_clbk_t;
typedef std::function<void(uint64_t, const std::string&)> ae_service_clbk_t;

} // namespace raft

#endif // RAFT_COMMON_DEFINES
