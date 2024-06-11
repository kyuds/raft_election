#ifndef RAFT_NODE
#define RAFT_NODE

#include <string>
#include <vector>

#include "common.h"
#include "durable.h"
#include "rpc.h"

namespace raft {

class Raft {
    public:
        Raft(std::string _name, 
             std::string _address, 
             const std::string& conf_file,
             const std::string& member_file);
        ~Raft();

        bool start();
        void stop();

        // const std::string& get_cluster_leader();
    private:
        // functions for repeated tasks
        // void hold_leader_election();
        // void heartbeat();

        // functions for RPC callbacks
        rpc_rv_rep_t process_request_vote(const rpc_rv_req_t& req);
        request_vote_callback_t process_request_vote_callback;
        void process_ping();
        ping_callback_t process_ping_callback;
    
    private:
        // helper functions
        bool save_pstate();
        bool load_pstate();
        uint64_t current_term();
        const std::string& voted_for();
        const pstate_t& get_pstate();

    private:
        // member variables

        //////////////////////
        //     protocol     //
        //////////////////////

        // node information
        const std::string name;
        const std::string address;

        // persistent state
        pstate_t pstate;
        std::mutex pstate_m;

        // locks
        std::mutex node_m;
        
        // volatile state
        State state;
        std::vector<std::string> members;

        //////////////////////
        //      module      //
        //////////////////////

        Durable * durable;
        Rpc * rpc;

        //////////////////////
        //       task       //
        //////////////////////
};

} // namespace raft

#endif // RAFT_NODE
