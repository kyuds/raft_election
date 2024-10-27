#ifndef RAFT_NODE
#define RAFT_NODE

#include <mutex>
#include <random>
#include <string>
#include <vector>
#include <unordered_map>

#include "callback.h"
#include "config.h"
#include "rpc.h"
#include "storage.h"
#include "utils.h"
#include "tasks/electiontask.h"
#include "tasks/synctask.h"

#define TMP_DIR "tmp"
#define PEER_FILE "peer.txt"

namespace raft {

enum class Status {
    Follower,
    Candidate,
    Leader
};

typedef std::unordered_map<std::string, 
    std::chrono::time_point<std::chrono::steady_clock>> peer_respond_t;

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

// helper functions for Raft
private:
//     // processors for RPC requests
    rep_t process_vote_request(uint64_t term, const std::string& candidate);
    rep_t process_append_entries(uint64_t term, const std::string& leader);

    // tasks
    void handle_election_task();
    void handle_heartbeat_task();
    void start_tasks();

    // helpers
    void update_term(uint64_t term);
    void become_leader();

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
    const std::vector<std::string> peers;

    //////////////////////
    //     services     //
    //////////////////////

    std::unique_ptr<Storage> storage;
    std::unique_ptr<Rpc> rpc;

    //////////////////////
    //       task       //
    //////////////////////

    ElectionTask * election_task;
    SyncTask * heartbeat_task;

// election-related
private:
    std::mt19937 random_election_timer;
    int votes;
    int majority_quorum() { return (peers.size() + 1) / 2; }

// heartbeat-related
private:
    peer_respond_t peer_last_respond;
    void update_peer_time(const std::string& p) { 
        peer_last_respond[p] = std::chrono::steady_clock::now();
    }

    long long peer_elapsed_time(const std::string& p) {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - peer_last_respond[p]
        ).count();
    }
};

} // namespace raft

#endif // RAFT_NODE
