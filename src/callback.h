#ifndef RAFT_CALLBACK_DEFINITIONS
#define RAFT_CALLBACK_DEFINITIONS

#include <functional>

namespace raft {

// Common
typedef struct {
    uint64_t term;
    bool success;
} rep_t;

// Request Vote
typedef std::function<void(uint64_t, bool)> client_rv_t;
typedef std::function<rep_t(uint64_t, const std::string&)> service_rv_t;

} // namespace raft

#endif // RAFT_CALLBACK_DEFINITIONS
