#ifndef RAFT_NODE
#define RAFT_NODE

#include <mutex>
#include <string>

#include "callback.h"
#include "config.h"
#include "rpc.h"
#include "storage.h"
#include "utils.h"

#define TMP_DIR "tmp"
#define PEER_FILE "peer.txt"

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
            name, address, combine_paths(TMP_DIR, name), PEER_FILE)) {}
    ~Raft() { stop(); }

    void start();
    void stop();

// // helper functions for Raft
// private:
//     // processors for RPC requests
//     rep_t process_vote_request(uint64_t term, const std::string& address);

//     // helpers
//     void update_term(uint64_t term);
//     void become_leader();

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

    // peers
    // 

    //////////////////////
    //     services     //
    //////////////////////

    std::unique_ptr<Storage> storage;
    std::unique_ptr<Rpc> rpc;

    //////////////////////
    //       task       //
    //////////////////////

    // election
    // heartbeat
};

} // namespace raft

#endif // RAFT_NODE
