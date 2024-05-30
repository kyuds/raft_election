#ifndef RAFT_NODE
#define RAFT_NODE

#include <string>
#include <vector>

#include "config.h"

namespace raft {

class Raft {
    public:
        Raft(std::string _name, 
             std::string _address, 
             const std::string& conf_file,
             const std::string& member_file);
        ~Raft() {}

    private:
        ///////////////////
        // configuration //
        ///////////////////

        Config * conf;
        std::string name;
        std::string address;

        ///////////////////
        // raft protocol //
        ///////////////////

        // persistent state
        int64_t current_term;
        std::string voted_for;

        // volatile state

        ///////////////////
        // grpc protocol //
        ///////////////////

        std::vector<std::string> members;
};

} // namespace raft

#endif // RAFT_NODE
