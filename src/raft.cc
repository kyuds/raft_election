#include <plog/Log.h>
#include "plog/Initializers/RollingFileInitializer.h"

#include "raft.h"
#include "utils.h"

#include <iostream> // delete later (testing for cli)

// (TODO)
// figure out how to put define flags into CMake
// so ideally, we will compile with -DRAFT_DEBUG
// #define RAFT_DEBUG

namespace raft {

Raft::Raft(Config * conf) 
    : name(conf->get_name())
    , address(conf->get_address())
    , min_election_timeout(conf->get_min_timeout())
    , max_election_timeout(conf->get_max_timeout())
    , heartbeat(conf->get_heartbeat())
    , batchsize(conf->get_batchsize())
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
void Raft::start() {
    status = Status::Follower;
    state->initialize();
    rpc->start(
        [this] (uint64_t term, const std::string& address) {
            return prcs_vote_request(term, address);
        },
        [this] (uint64_t term, const std::string& address) {
            return prcs_append_entries(term, address);
        }
    );
    start_election_task();
    PLOGI << "Started raft service on " << address << " as " << name << ".";
}

void Raft::stop() {
    stop_election_task();
    stop_heartbeat_task();
}

rpc_rep_t Raft::prcs_vote_request(uint64_t term, const std::string& address) {
    std::lock_guard<std::mutex> lock(node_m);
    std::cout << "Received vote request from " << address << " for term " << term << std::endl;
    std::cout << "Current state: " << "<" << state->voted_for() << ">, " << state->term() << std::endl;
    update_term(term);

    bool grant = state->term() == term; // term is updated if smaller
    grant &= (state->voted_for().empty() || state->voted_for() == address);
    // grant &= <condition for log at least up to date> (TODO)

    if (grant) {
        state->set_voted_for(address);
        state->save_pstate();
        std::cout << "Granted vote to " << address << " for term " << term << std::endl;
        PLOGI << "Granted vote to " << address << " for term " << term << ".";
    }
    return rpc_rep_t {
        .success = grant,
        .term = state->term()
    };
}

rpc_rep_t Raft::prcs_append_entries(uint64_t term, const std::string& address) {
    std::lock_guard<std::mutex> lock(node_m);
    std::cout << "Received heartbeat from " << address << " for term " << term << std::endl;
    update_term(term);
    if (leader != address) {
        PLOGI << "Found new leader " << address << ".";
        leader = address;
    }
    // do other logic for later.
    return rpc_rep_t {
        .success = true, // ignored
        .term = state->term()
    };
}

// assume that Raft node is locked.
void Raft::update_term(uint64_t term) {
    if (status != Status::Leader) election_task->reset();
    if (state->term() < term) {
        PLOGI << "Found higher term " << term 
              << " than current term " << state->term() 
              << " . Switching to follower.";
        state->set_term(term);
        state->set_voted_for("");
        state->save_pstate();
        if (status == Status::Leader) {
            stop_heartbeat_task();
            start_election_task();
        }
        status = Status::Follower;
    }
}

// assume that Raft node is locked.
void Raft::become_leader() {
    status = Status::Leader;
    stop_election_task();
    start_heartbeat_task();
    std::cout << "Became leader for term " << state->term() << std::endl;
    PLOGI << "Became leader for term " << state->term() << ".";
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
                status = Status::Candidate;
                votes = 1;
                state->increment_term();
                state->set_voted_for(address);
                state->save_pstate();
                PLOGI << "Timed out. Starting election for term: " << state->term() << ".";
                std::cout << "starting election" << std::endl;
                
                for (const auto& peer : peers) {
                    if (peer == address)
                        continue;
                    rpc->request_vote(peer, state->term(), address, 
                        [this, &peer] (uint64_t term, bool granted) {
                            std::lock_guard<std::mutex> lock(node_m);
                            update_term(term);
                            if (status == Status::Candidate && granted) {
                                std::cout << "Received vote" << std::endl;
                                if (++votes >= majority_quorum()) {
                                    become_leader();
                                }
                                PLOGI << "Received vote from " << peer << ".";
                            } else {
                                PLOGI << "Didn't receive vote from " << peer << ".";
                            }
                        }
                    );
                    PLOGI << "Sent VoteRequest rpc to " << peer << ".";
                }
            }
        );
        PLOGI << "Started election task.";
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
                    if (peer == address)
                        continue;
                    rpc->append_entries(peer, state->term(), address, 
                        [this] (uint64_t term, bool success) {
                            std::lock_guard<std::mutex> lock(node_m);
                            update_term(term);
                        }
                    );
                }
            }
        );
        PLOGI << "Started heartbeat task.";
    } else {
        PLOGF << "Trying to start heartbeat task while a task pre-exists.";
    }
}

void Raft::stop_election_task() {
    if (election_task != nullptr) {
        delete election_task;
        election_task = nullptr;
        PLOGI << "Stopped election task.";
    }
}

void Raft::stop_heartbeat_task() {
    if (heartbeat_task != nullptr) {
        delete heartbeat_task;
        heartbeat_task = nullptr;
        PLOGI << "Stopped heartbeat task.";
    }
}

} // namespace raft
