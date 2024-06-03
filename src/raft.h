#ifndef RAFT_NODE
#define RAFT_NODE

#include <string>
#include <vector>

#include "config.h"
#include "storage.h"
#include "rpc.h"

namespace raft {

enum class State {
    Leader,
    Follower,
    Candidate
};

class Raft {
    public:
        Raft(std::string _name, 
             std::string _address, 
             const std::string& conf_file,
             const std::string& member_file);
        ~Raft();

        void start();
        void stop();

    private:
        //////////////////////
        //     protocol     //
        //////////////////////

        // node information
        std::string name;
        std::string address;

        // persistent state
        uint64_t current_term;
        std::string voted_for;
        
        // volatile state
        State state;
        std::vector<std::string> members;

        //////////////////////
        //      module      //
        //////////////////////

        Storage * storage;
        Rpc * rpc;

        //////////////////////
        //       task       //
        //////////////////////
};

} // namespace raft

#endif // RAFT_NODE
