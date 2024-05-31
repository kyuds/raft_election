#include <iostream>

#include "raft.h"
#include "utils.h"

namespace raft {

Raft::Raft(std::string _name, 
           std::string _address, 
           const std::string& conf_file,
           const std::string& member_file) {
    name = _name;
    address = _address;
    members = file_line_to_vec(member_file);

    // parse from configuration file
    auto * conf = new ConfigParser(name, conf_file);
    storage = new Storage(conf->get_storage_dir());
    rpc = new Rpc(address,
                  conf->get_rpc_timeout(),
                  conf->get_max_retries());
    delete conf;
}

Raft::~Raft() {
    // free other variables too
    delete storage;
    delete rpc;
}

} // namespace raft
