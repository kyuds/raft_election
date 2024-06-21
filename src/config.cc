#include <fstream>
#include <plog/Log.h>
#include <cstdlib>
#include <nlohmann/json.hpp>

#include "config.h"
#include "utils.h"

using json = nlohmann::json;

namespace raft {

Config::Config(const std::string& name,
               const std::string& address,
               const std::string& config_file)
    : name(name), address(address) {
    try {
        std::ifstream f(config_file);
        json data = json::parse(f);

        storage_dir = std::string(data.value("/storage/directory"_json_pointer, "tmp"));
        append_node_name = data.value("/storage/append_node_name"_json_pointer, false);
        min_timeout = data.value("/election/min_timeout"_json_pointer, 150);
        max_timeout = data.value("/election/max_timeout"_json_pointer, 300);
        rpc_timeout = data.value("/rpc/timeout"_json_pointer, 100);
        peer_file = std::string(data.value("/peer/config"_json_pointer, "peer.txt"));
        heartbeat = data.value("/peer/heartbeat"_json_pointer, 20);
        PLOGI << "Successfully parsed file: " << config_file;
        PLOGI << name << " " << address;
    } catch (const std::exception& e) {
        PLOGE << "Error occurred in parsing config file " << config_file << " " << e.what();
        exit(0);
    }
}

Config * Config::default_config(const std::string& name,
                                const std::string& address) {
    return (new Config())
            ->set_name(name)
            ->set_address(address)
            ->set_peer_file("peer.txt")
            ->set_heartbeat(20)
            ->set_min_timeout(150)
            ->set_max_timeout(300)
            ->set_rpc_timeout(100)
            ->set_storage_dir("tmp", true);
}

Config * Config::set_name(const std::string& n) {
    name = n;
    return this;
}

Config * Config::set_address(const std::string& a) {
    address = a;
    return this;
}

Config * Config::set_peer_file(const std::string& p) {
    peer_file = p;
    return this;
}

Config * Config::set_heartbeat(const int t) {
    heartbeat = t;
    return this;
}

Config * Config::set_min_timeout(const int t) {
    min_timeout = t;
    return this;
}

Config * Config::set_max_timeout(const int t) {
    max_timeout = t;
    return this;
}

Config * Config::set_rpc_timeout(const int t) {
    rpc_timeout = t;
    return this;
}

Config * Config::set_storage_dir(const std::string& d, bool append_name) {
    append_node_name = append_name;
    storage_dir = d;
    return this;
}

const std::string& Config::get_storage_dir() {
    if (append_node_name) {
        if (!name.empty()) 
            storage_dir = combine_paths(storage_dir, name);
        else 
            PLOGW << "Configured to add node name to storage path but node name not set. Skipping";
        append_node_name = false; // prevent further combination
    }
    return storage_dir;
}

} // namespace raft
