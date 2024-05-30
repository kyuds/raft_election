#include <fstream>
#include "nlohmann/json.hpp"

#include "config.h"
#include "utils.h"

using json = nlohmann::json;

namespace raft {

Config::Config(const std::string& name, const std::string& config_file) {
    std::ifstream f(config_file);
    if (!f.is_open()) {
        // (TODO: kyuds) error here.
    } 

    json data = json::parse(f);

    rpc_timeout = data["rpc"]["rpc_timeout"];
    max_retries = data["rpc"]["max_retries"];

    storage_dir = std::string(data["durable"]["directory"]);
    if (data["durable"]["append_node_name"]) {
        storage_dir = combine_paths(storage_dir, name);
    }
}

} // namespace raft
