#ifndef RAFT_NODE
#define RAFT_NODE

#include <memory>
#include <random>
#include <string>
#include <vector>

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
        Raft(Config * conf);
        Raft(const std::string& name, const std::string address)
            : Raft(Config::default_config(name, address)) {}
        ~Raft() { stop(); }

        bool start();
        void stop();

    private:
        // functions for Raft operations
        rpc_rep_t prcs_vote_request(uint64_t term, const std::string& address);
        rpc_rep_t prcs_append_entries(uint64_t term, const std::string& address);

        void update_term(uint64_t term);
        void become_leader();

        void start_election_task();
        void start_heartbeat_task();
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
        const int max_election_timeout;
        int votes;
        const int heartbeat;
        std::mt19937 generator;
};

} // namespace raft

#endif // RAFT_NODE
