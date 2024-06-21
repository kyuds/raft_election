#ifndef RAFT_NODE
#define RAFT_NODE

#include <memory>
#include <string>
#include <vector>

#include "config.h"
#include "state.h"
#include "rpc.h"

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
        Raft(std::string _name, 
             std::string _address, 
             const std::string& conf_file,
             const std::string& member_file);
        // TODO: implement
        Raft(Config * conf) {}
        Raft(const std::string& name, const std::string address)
            : Raft(Config::default_config(name, address)) {}
        ~Raft() {}

        bool start();
        void stop();

    private:
        // functions for Raft operations

    private:
        //////////////////////
        //     protocol     //
        //////////////////////

        const std::string name;
        const std::string address;

        Status status;
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

        // const int min_election_timeout;
        // const int max_election_timeout;
        // const int heartbeat;

};

} // namespace raft

#endif // RAFT_NODE
