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
    , min_election_timeout(conf->get_min_timeout())
    , max_election_timeout(conf->get_max_timeout())
    , heartbeat(conf->get_heartbeat()) 
    , generator(std::mt19937(std::random_device()())) {
    
    #ifdef RAFT_DEBUG
    plog::init(plog::debug, combine_paths(conf->get_storage_dir(), "log.txt").c_str());
    #else
    plog::init(plog::info, combine_paths(conf->get_storage_dir(), "log.txt").c_str());
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
    status = Status::Follower;
    success &= state->initialize();
    rpc->start(
        [this] (uint64_t term, const std::string& address) {
            return prcs_vote_request(term, address);
        },
        [this] (uint64_t term, const std::string& address) {
            return prcs_append_entries(term, address);
        }
    );
    start_election_task();
    return success;
}

void Raft::stop() {
    stop_election_task();
    stop_heartbeat_task();
}

rpc_rep_t Raft::prcs_vote_request(uint64_t term, const std::string& address) {
    
}

rpc_rep_t Raft::prcs_append_entries(uint64_t term, const std::string& address) {
    
}

void Raft::update_term(uint64_t term) {

}

void Raft::become_leader() {

}

void Raft::start_election_task() {
    if (election_task == nullptr) {
        election_task = new TimedCycle(
            [this]() {
                std::uniform_int_distribution<> d(min_election_timeout, 
                                                  max_election_timeout);
                return std::chrono::milliseconds(d(generator));
            },
            [this]() {
                std::lock_guard<std::mutex> lock(node_m);
                int votes = 0;
                for (const auto& peer : peers) {
                    rpc->request_vote(peer, state->term(), address, 
                        [this] (uint64_t term, bool granted) {
                            std::lock_guard<std::mutex> lock(node_m);
                            update_term(term);
                            if (granted) {
                                if (++votes >= 3) { // (TODO:) Calculate vote requirements
                                    become_leader();
                                }
                            }
                        }
                    );
                }
            }
        );
    } else {
        PLOGF << "Trying to start election task while a task pre-exists.";
    }
}

void Raft::start_heartbeat_task() {
    if (heartbeat_task == nullptr) {
        heartbeat_task = new TimedCycle(
            [this]() {
                return std::chrono::milliseconds(heartbeat);
            },
            [this]() {
                std::lock_guard<std::mutex> lock(node_m);
                for (const auto& peer : peers) {
                    rpc->append_entries(peer, state->term(), address, 
                        [this] (uint64_t term, bool success) {
                            std::lock_guard<std::mutex> lock(node_m);
                            update_term(term);
                        }
                    );
                }
            }
        );
    } else {
        PLOGF << "Trying to start heartbeat task while a task pre-exists.";
    }
}

void Raft::stop_election_task() {
    if (election_task != nullptr) {
        delete election_task;
        election_task = nullptr;
    }
}

void Raft::stop_heartbeat_task() {
    if (heartbeat_task != nullptr) {
        delete heartbeat_task;
        heartbeat_task = nullptr;
    }
}

} // namespace raft
