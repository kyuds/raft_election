#ifndef RAFT_DURABLE
#define RAFT_DURABLE

#include <string>

#define RAFT_META_FILE      "meta.txt"
// #define RAFT_LOG_DIR        "logs"
// #define RAFT_SNAPSHOT_DIR   "snapshots"

namespace raft {

class Storage {
    public:
        Storage(const std::string& _root);
        ~Storage() {}

        bool commit_metadata(uint64_t term, const std::string& voted_for);
        // bool commit_log(); // (TODO: kyuds) implement
    
    private:
        const std::string root;
};

} // namespace raft

#endif // RAFT_DURABLE
