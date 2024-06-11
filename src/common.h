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

// custom structs to encapsulate RPC formats. 
typedef struct {
    uint64_t term;
    std::string candidate_id;
    uint64_t last_log_index;
    uint64_t last_log_term;
} rpc_rv_req_t;

typedef struct {
    uint64_t term;
    bool vote_granted;
} rpc_rv_rep_t;

// typedefs for closure callbacks
typedef std::function<rpc_rv_rep_t(const rpc_rv_req_t&)> request_vote_callback_t;
typedef std::function<void()> ping_callback_t;

} // namespace raft

#endif // RAFT_COMMON_DEFINES
