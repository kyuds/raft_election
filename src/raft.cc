#include <iostream>

#include "raft.h"
#include "config.h"
#include "utils.h"

namespace raft {

Raft::Raft(std::string _name, 
           std::string _address, 
           const std::string& conf_file,
           const std::string& member_file)
    : name(_name),
      address(_address) {
    members = file_line_to_vec(member_file);
    state = State::Follower;

    // parse from configuration file
    auto * conf = new ConfigParser(name, conf_file);
    durable = std::make_unique<Durable>(conf->get_storage_dir());
    // rpc = new Rpc(address,
    //               conf->get_rpc_timeout(),
    //               conf->get_max_retries());
    delete conf;

    // create callback variables.
}

// For each module, there is a separate constructor and a separate
// initializer. This is to ensure that each module is initialized
// and started properly before the Raft service is started. 
// A return value of "false" from this function indicates that 
// any of the modules that were supposed to start didn't start
// properly. This is a design choice made because returning success
// indicators from consturctors is rather complicated.
bool Raft::start() {
    bool s = true;
    s &= durable->initialize();
    s &= load_pstate();

    std::cout << "Success: " << s << std::endl;
    std::cout << current_term() << " <" << voted_for() << ">" << std::endl;

    return s;
}

void Raft::stop() {

}

} // namespace raft
