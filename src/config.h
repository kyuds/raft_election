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
should be organized if using the file configuration version.

TODO:
- figure out how to import gRPC settings.
*/

namespace raft {

class Config {
    public:
        // empty Config object
        Config() {}
        // Config object constructed from a config_file
        Config(const std::string& name,
               const std::string& address,
               const std::string& config_file);

        // default configuration. Most common use case.
        static Config * default_config(const std::string& name,
                                       const std::string& address);

        const std::string& get_name() { return name; }
        const std::string& get_address() { return address; }
        const std::string& get_peer_file() { return peer_file; }
        int get_heartbeat() { return heartbeat; }
        int get_min_timeout() { return min_timeout; }
        int get_max_timeout() { return max_timeout; }
        int get_rpc_timeout() { return rpc_timeout; }
        const std::string& get_storage_dir();

        // setters. Returns 'this' to chain functions.
        Config * set_name(const std::string& n);
        Config * set_address(const std::string& a);
        Config * set_peer_file(const std::string& p);
        Config * set_heartbeat(const int t);
        Config * set_min_timeout(const int t);
        Config * set_max_timeout(const int t);
        Config * set_rpc_timeout(const int t);
        Config * set_storage_dir(const std::string& d, bool append_name = false);
    
    private:
        // protocol
        std::string name;
        std::string address;
        std::string peer_file;
        int heartbeat; // milliseconds
        
        // election
        int min_timeout; // milliseconds
        int max_timeout;

        // rpc
        int rpc_timeout; // milliseconds

        // storage
        std::string storage_dir;
        bool append_node_name;
};
    
} // namespace raft

#endif // RAFT_CONFIG
