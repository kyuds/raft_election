#include <plog/Log.h>
#include "plog/Initializers/RollingFileInitializer.h"

#include "raft.h"
#include "utils.h"

// (TODO)
// figure out how to put define flags into CMake
// so ideally, we will compile with -DRAFT_DEBUG
#define RAFT_DEBUG

namespace raft {

Raft::Raft(std::string _name, 
           std::string _address, 
           const std::string& conf_file,
           const std::string& member_file)
    : name(_name),
      address(_address) {
    // need to change name.
    #ifdef RAFT_DEBUG
    plog::init(plog::debug, "Hello.txt");
    #else
    plog::init(plog::info, "Hello.txt");
    #endif
    // members = file_line_to_vec(member_file);
    // status = Status::Follower;

    // // parse from configuration file
    // auto * conf = new ConfigParser(name, conf_file);
    // durable = std::make_unique<Durable>(conf->get_storage_dir());
    // // rpc = new Rpc(address,
    // //               conf->get_rpc_timeout(),
    // //               conf->get_max_retries());
    // delete conf;

    // // create callback variables.
}

// For each module, there is a separate constructor and a separate
// initializer. This is to ensure that each module is initialized
// and started properly before the Raft service is started. 
// A return value of "false" from this function indicates that 
// any of the modules that were supposed to start didn't start
// properly. This is a design choice made because returning success
// indicators from consturctors is rather complicated.
bool Raft::start() {
    return true;
}

void Raft::stop() {

}

} // namespace raft
