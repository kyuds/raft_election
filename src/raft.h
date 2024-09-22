#ifndef RAFT_NODE
#define RAFT_NODE

#include <mutex>
#include <string>

#include "config.h"
#include "utils.h"

namespace raft {

enum class Status {
    Follower,
    Candidate,
    Leader
};

class Raft{

// API
public:
    Raft(Config * config);
    Raft(const std::string& name, const std::string& address)
        : Raft(Config::default_config(
            name, address, combine_paths("tmp", name), "peer.txt"
        )) {}
    ~Raft() {}

// member variables
private:
    //////////////////////
    //     protocol     //
    //////////////////////

    // global node lock
    std::mutex raft_mutex;

    // configurations
    const std::string name;
    const std::string address;
    const int min_election_timeout;
    const int max_election_timeout;
    const int heartbeat;

    // state
    Status status;
    std::string leader;
    // peer vector
    // 

    //////////////////////
    //     services     //
    //////////////////////

    // storage
    // rpc

    //////////////////////
    //       task       //
    //////////////////////

    // election
    // heartbeat
};

} // namespace raft

#endif // RAFT_NODE
