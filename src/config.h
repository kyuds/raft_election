#ifndef RAFT_CONFIG
#define RAFT_CONFIG

/*
Class for storing cluster/node configurations for the raft
protocol. We currently assume that ALL CONFIGURATIONS are
the same for all nodes in single membership. 

TODO:
- figure out how to import gRPC settings.
*/

#include <string>
#include <vector>

namespace raft {

class Config {
public:
    // empty config object
    Config() {}
    
    // default configuration for common use cases.
    static Config * default_config(
        const std::string& name, 
        const std::string& address,
        const std::string& storage_dir,
        const std::string& peer_file);

    // getters
    const std::string& get_name() { return name; }
    const std::string& get_address() { return address; }
    const std::string& get_storage_dir() { return storage_dir; }
    std::vector<std::string>& get_peers() { return peers; }

    int get_heartbeat() { return heartbeat; }
    int get_min_election_timeout() { return min_election_timeout; }
    int get_max_election_timeout() { return max_election_timeout; }
    int get_rpc_timeout() { return rpc_timeout; }

    // setters. Allows chaining
    Config * set_name(const std::string& name);
    Config * set_address(const std::string& address);
    Config * set_storage_dir(const std::string& storage_dir);
    Config * set_heartbeat(const int heartbeat);
    Config * set_min_election_timeout(const int timeout);
    Config * set_max_election_timeout(const int timeout);
    Config * set_rpc_timeout(const int timeout);
    Config * set_peers_from_file(const std::string& file);
    Config * add_peer(const std::string& address);

    // info (for debug purposes)
    std::string info();

private:
    // node
    std::string name;
    std::string address;
    std::string storage_dir;
    std::vector<std::string> peers;

    // leader
    int heartbeat; // milliseconds

    // election
    int min_election_timeout; // milliseconds
    int max_election_timeout; // milliseconds

    // rpc
    int rpc_timeout;
};

} // namespace raft

#endif // RAFT_CONFIG
