#ifndef RAFT_NODE
#define RAFT_NODE

#include <string>
#include <vector>

#include "common.h"
#include "config.h"
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

    private:
        // functions for callbacks

    private:
        // helper functions
        bool save_pstate() { return durable->save_pstate(pstate); }
        bool load_pstate() { return durable->load_pstate(pstate); }

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
