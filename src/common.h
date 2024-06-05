#ifndef RAFT_COMMON_DEFINES
#define RAFT_COMMON_DEFINES

#include <string>

#define RAFT_PSTATE_FILE        "pstate.txt"
// #define RAFT_LOG_DIR        "logs"
// #define RAFT_SNAPSHOT_DIR   "snapshots"

#define PSTATE_TM_INIT          0
#define PSTATE_VF_INIT          ""

namespace raft {

enum class State {
    Leader,
    Follower,
    Candidate
};

typedef struct {
    uint64_t term;
    std::string voted_for;
} pstate_t;

} // namespace raft

#endif // RAFT_COMMON_DEFINES
