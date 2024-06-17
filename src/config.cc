#include <fstream>
#include "nlohmann/json.hpp"

#include "config.h"
#include "utils.h"

using json = nlohmann::json;

namespace raft {

ConfigParser::ConfigParser(const std::string& name, const std::string& config_file) {
    std::ifstream f(config_file);
    if (!f.is_open()) {
        // (TODO: kyuds) error here.
    } 

    json data = json::parse(f);

    min_timeout = data["election"]["min_timeout"];
    max_timeout = data["election"]["max_timeout"];

    storage_dir = std::string(data["durable"]["directory"]);
    if (data["durable"]["append_node_name"]) {
        storage_dir = combine_paths(storage_dir, name);
    }
}

} // namespace raft
