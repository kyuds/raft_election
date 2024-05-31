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

    private:
        //////////////////////
        //     protocol     //
        //////////////////////

        // persistent state
        int64_t current_term;
        std::string voted_for;
        
        // volatile state
        std::vector<std::string> members;

        //////////////////////
        // node information //
        //////////////////////

        std::string name;
        std::string address;

        //////////////////////
        //      module      //
        //////////////////////

        Storage * storage;
        Rpc * rpc;
};

} // namespace raft

#endif // RAFT_NODE
