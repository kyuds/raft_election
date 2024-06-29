#ifndef RAFT_NODE
#define RAFT_NODE

#include <memory>
#include <random>
#include <string>
#include <vector>
#include <unordered_map>

#include "config.h"
#include "state.h"
#include "rpc.h"
#include "timedcycle.h"

/*
Operating Assumptions:
- grpc server is not going to crash
- background threads are going to operate
- fail stop model

**  actually don't know how to develop a fail stop
    model, but this is what the actual raft
    protocol relies on as well. 
*/

namespace raft {

// raft node status.
enum class Status {
    Follower,
    Candidate,
    Leader
};

class Raft {
    public:
        // API
        Raft(Config * conf);
        Raft(const std::string& name, const std::string address)
            : Raft(Config::default_config(name, address)) {}
        ~Raft() { stop(); }

        void start();
        void stop();

    private:
        // functions for Raft operations
        rpc_rep_t prcs_vote_request(uint64_t term, const std::string& address);
        rpc_rep_t prcs_append_entries(uint64_t term, const std::string& address);

        void update_term(uint64_t term); // true when term updated.
        void become_leader();

        void start_election_task();
        void start_heartbeat_task();
        void reset_election_task();
        void pause_heartbeat_task();
        void resume_heartbeat_task();
        // should not be called when node_m mutex is held
        // may deadlock. These are only for destruction.
        void stop_election_task();
        void stop_heartbeat_task();

    private:
        //////////////////////
        //     protocol     //
        //////////////////////

        const std::string name;
        const std::string address;

        Status status;
        std::string leader;
        std::unique_ptr<State> state;
        std::vector<std::string> peers;
        std::unordered_map<std::string, 
            std::chrono::time_point<std::chrono::steady_clock>> peer_last_respond;

        std::mutex node_m;
        
        //////////////////////
        //       comm       //
        //////////////////////

        std::unique_ptr<Rpc> rpc;

        //////////////////////
        //       task       //
        //////////////////////

        TimedCycle * election_task;
        TimedCycle * heartbeat_task;

        const int min_election_timeout;
        const int max_election_timeout; // also serves as leader timeout
        const int heartbeat;
        const int batchsize;
        std::mt19937 generator;
    
    private:
        // election helpers
        int votes;
        int majority_quorum() { return (peers.size() + 1) / 2; }

        // heartbeat helpers
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
