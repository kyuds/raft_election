#ifndef RAFT_CONFIG
#define RAFT_CONFIG

#include <string>

/*
Class for storing cluster/node configurations for the raft
protocol. We currently assume that ALL CONFIGURATIONS are
the same for all nodes in single membership. 

The Config class is used in "raft.h" to store information
about timing and other settings to tune raft. It does not
store configurations about cluster membership as we will
advance the protocol to allow for membership changes. 

See "config.json" for more details on how the json file
should be organized.

TODO:
- figure out how to import gRPC settings.
*/

namespace raft {

class Config {
    public:
        Config(const std::string& name, const std::string& config_file);
        ~Config() {}

        int get_rpc_timeout() { return rpc_timeout; }
        int get_max_retries() { return max_retries; }
        const std::string& get_storage_dir() { return storage_dir; }
    
    private:
        // raft protocol

        // durable
        std::string storage_dir;

        // rpc
        int rpc_timeout; // milliseconds
        int max_retries;
};
    
} // namespace raft

#endif // RAFT_CONFIG
