#ifndef RAFT_NODE
#define RAFT_NODE

#include <memory>
#include <string>
#include <vector>

#include "common.h"
#include "durable.h"
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

class Raft {
    public:
        Raft(std::string _name, 
             std::string _address, 
             const std::string& conf_file,
             const std::string& member_file);
        ~Raft() {}

        bool start();
        void stop();

    private:
        // functions for Raft operations

    private:
        //////////////////////
        //     protocol     //
        //////////////////////

        // node information
        const std::string name;
        const std::string address;

        // persistent state
        pstate_t pstate;

        // volatile state
        State state;
        std::vector<std::string> members;

        // locks
        std::mutex node_m;
        
        //////////////////////
        //      module      //
        //////////////////////

        std::unique_ptr<Durable> durable;
        std::unique_ptr<Rpc> rpc;

        //////////////////////
        //       task       //
        //////////////////////
    
    private:
        // trivial helper functions
        bool save_pstate() { return durable->save_pstate(pstate); }
        bool load_pstate() { return durable->load_pstate(pstate); }
        uint64_t current_term() { return pstate.term; }
        const std::string& voted_for() { return pstate.voted_for; }
        const pstate_t& get_pstate() { return pstate; }
};

} // namespace raft

#endif // RAFT_NODE
