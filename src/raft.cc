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
    durable = new Durable(conf->get_storage_dir());
    rpc = new Rpc(address,
                  conf->get_rpc_timeout(),
                  conf->get_max_retries());
    delete conf;

    // create callback variables.
}

Raft::~Raft() {
    // free other variables too
    delete durable;
    delete rpc;
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
    std::cout << pstate.term << " <" << pstate.voted_for << ">" << std::endl;

    save_pstate();
    load_pstate();
    std::cout << current_term() << std::endl;

    return s;
}

void Raft::stop() {

}

// rpc callbacks
rpc_rv_rep_t Raft::process_request_vote(const rpc_rv_req_t& req) {
    std::lock_guard<std::mutex> lock(node_m);
    // (TODO: kyuds) reset election timer.
    const pstate_t& p = get_pstate();
    rpc_rv_rep_t r;
    r.term = p.term;
    if (req.term < p.term && p.voted_for.compare("") != 0) {
        r.vote_granted = true;
    } else {
        r.vote_granted = false;
    }
    return r;
}

void Raft::process_ping() {

}

// trivial helpers
bool Raft::save_pstate() {
    std::lock_guard<std::mutex> lock(pstate_m);
    return durable->save_pstate(pstate);
}

bool Raft::load_pstate() {
    std::lock_guard<std::mutex> lock(pstate_m);
    return durable->load_pstate(pstate);
}

uint64_t Raft::current_term() {
    std::lock_guard<std::mutex> lock(pstate_m);
    return pstate.term;
}

const std::string& Raft::voted_for() {
    std::lock_guard<std::mutex> lock(pstate_m);
    return pstate.voted_for;
}

const pstate_t& Raft::get_pstate() {
    std::lock_guard<std::mutex> lock(pstate_m);
    return pstate;
}

} // namespace raft
