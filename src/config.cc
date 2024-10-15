#include <sstream>

#include "config.h"
#include "utils.h"

namespace raft {

Config * Config::default_config(
    const std::string& name, 
    const std::string& address,
    const std::string& storage_dir,
    const std::string& peer_file) 
{
    return (new Config())
        ->set_name(name)
        ->set_address(address)
        ->set_storage_dir(storage_dir)
        ->set_peers_from_file(peer_file)
        ->set_heartbeat(20)
        ->set_min_election_timeout(300)
        ->set_max_election_timeout(500)
        ->set_rpc_timeout(100);
}

std::string Config::info() {
    std::ostringstream oss;
    oss << "Configurations for Raft Node: "
        << "\n- Name: " << name
        << "\n- Address: " << address
        << "\n- Peers: " << vector_to_string(peers)
        << "\n- Storage Directory: " << storage_dir
        << "\n- Heartbeat: " << heartbeat
        << "\n- Election Timeout: " << min_election_timeout 
        << ", " << max_election_timeout
        << "\n- RPC Timeout: " << rpc_timeout << "\n";
    return oss.str();
}

// misc: setters

Config * Config::set_name(const std::string& name) {
    this->name = name;
    return this;
}

Config * Config::set_address(const std::string& address) {
    this->address = address;
    return this;
}

Config * Config::set_peers_from_file(const std::string& peer_file) {
    this->peers = get_lines(peer_file);
    return this;
}

Config * Config::add_peer(const std::string& address) {
    this->peers.emplace_back(address);
    return this;
}

Config * Config::set_storage_dir(const std::string& storage_dir) {
    this->storage_dir = storage_dir;
    return this;
}

Config * Config::set_heartbeat(const int heartbeat) {
    this->heartbeat = heartbeat;
    return this;
}

Config * Config::set_min_election_timeout(const int timeout) {
    min_election_timeout = timeout;
    return this;
}

Config * Config::set_max_election_timeout(const int timeout) {
    max_election_timeout = timeout;
    return this;
}

Config * Config::set_rpc_timeout(const int timeout) {
    rpc_timeout = timeout;
    return this;
}

} // namespace raft
