#include <plog/Log.h>
#include "plog/Initializers/RollingFileInitializer.h"

#include "raft.h"
#include "utils.h"

// (TODO)
// figure out how to put define flags into CMake
// so ideally, we will compile with -DRAFT_DEBUG
#define RAFT_DEBUG

namespace raft {

Raft::Raft(Config * conf) 
    : name(conf->get_name())
    , address(conf->get_address())
    , status(Status::Follower)
    , min_election_timeout(conf->get_min_timeout())
    , max_election_timeout(conf->get_max_timeout())
    , heartbeat(conf->get_heartbeat()) {
    
    #ifdef RAFT_DEBUG
    plog::init(plog::debug, combine_paths(conf->get_storage_dir(), "log.txt"));
    #else
    plog::init(plog::info, combine_paths(conf->get_storage_dir(), "log.txt"));
    #endif

    state = std::make_unique<State>(conf->get_storage_dir());
    rpc = std::make_unique<Rpc>(address, conf->get_rpc_timeout());
    peers = file_line_to_vec(conf->get_peer_file());
}

// For each module, there is a separate constructor and a separate
// initializer. This is to ensure that each module is initialized
// and started properly before the Raft service is started. 
// A return value of "false" from this function indicates that 
// any of the modules that were supposed to start didn't start
// properly. This is a design choice made because returning success
// indicators from consturctors is rather complicated.
bool Raft::start() {
    bool success = true;
    success &= state->initialize();
    // success &= rpc->start();
    // logic to start election timer;

    return success;
}

void Raft::stop() {

}

} // namespace raft
