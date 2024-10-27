#include <plog/Log.h>
#include "plog/Initializers/RollingFileInitializer.h"

#include "raft.h"
#include "storage.h"

namespace raft {

Raft::Raft(Config * config)
    : name(config->get_name())
    , address(config->get_address())
    , min_election_timeout(config->get_min_election_timeout())
    , max_election_timeout(config->get_max_election_timeout())
    , heartbeat(config->get_heartbeat())
    , peers(config->get_peers())
{
    create_directories_if_needed(config->get_storage_dir());
    plog::init(plog::info, 
               combine_paths(config->get_storage_dir(), "log.txt").c_str(), 
               1000000, 5);
    storage = std::make_unique<Storage>(config->get_storage_dir());
    rpc = std::make_unique<Rpc>(address, config->get_rpc_timeout());
    election_task = nullptr;
    heartbeat_task = nullptr;
    PLOGI << "Initialized raft node with config:\n" << config->info();
}

void Raft::start() {
    status = Status::Follower;
    storage->load_state();
    rpc->start(
        [this] (uint64_t term, const std::string& address) {
            return process_vote_request(term, address);
        },
        [this] (uint64_t term, const std::string& address) {
            return process_append_entries(term, address);
        }
    );
    start_tasks();
    PLOGI << "Started raft service on " << address << " as " << name << ".";
}

void Raft::stop() {
    if (election_task != nullptr) {
        delete election_task;
        election_task = nullptr;
        PLOGI << "Stopped election task";
    }
    if (heartbeat_task != nullptr) {
        delete heartbeat_task;
        heartbeat_task = nullptr;
        PLOGI << "Stopped heartbeat task.";
    }
}

// RPC request processors
rep_t Raft::process_vote_request(uint64_t term, const std::string& candidate) {
    std::lock_guard<std::mutex> lock(raft_mutex);
    update_term(term);

    bool grant = true;
    grant &= storage->term() == term;
    grant &= (storage->voted_for().empty() || storage->voted_for() == candidate);
    // more conditions here.

    if (grant) {
        storage->set_voted_for(candidate);
        storage->save_state();
        PLOGI << "Granted vote to " << candidate << " for term " << term << ".";
    }
    return rep_t {
        .success = grant,
        .term = storage->term()
    };
}

rep_t Raft::process_append_entries(uint64_t term, const std::string& leader) {
    std::lock_guard<std::mutex> lock(raft_mutex);
    update_term(term);
    if (this->leader != leader) {
        PLOGI << "Found new leader " << leader << ".";
        this->leader = leader;
    }
    // do other logic later
    return rep_t {
        .success = true, // ignored for now
        .term = storage->term()
    };
}

// handlers
void Raft::handle_election_task() {
    std::lock_guard<std::mutex> lock(raft_mutex);
    status = Status::Candidate;
    votes = 1;
    storage->inc_term();
    storage->set_voted_for(address);
    storage->save_state(); // Need to check for db failure.
    election_task->reset();
    PLOGI << "Timeout: Starting election for term: " << storage->term() << ".";

    for (const auto& peer: peers) {
        if (peer == address) continue;
        rpc->request_vote(peer, storage->term(),
            [this, &peer] (uint64_t term, bool granted) {
                std::lock_guard<std::mutex> lock(raft_mutex);
                update_term(term);
                if (status != Status::Candidate) return;

                if (granted) {
                    PLOGI << "Received vote from " << peer << ".";
                    if (++votes >= majority_quorum()) 
                        become_leader();
                } else {
                    PLOGI << "Didn't receive vote from " << peer << ".";
                }
            });
        PLOGI << "Sent VoteRequest RPC to " << peer << ".";
    }
}

void Raft::handle_heartbeat_task() {
    std::lock_guard<std::mutex> lock(raft_mutex);
    if (status != Status::Leader) {
        PLOGW << "Attempted to send heartbeats while not leader.";
        return;
    }
    int seen = 1;
    for (const auto& peer : peers) {
        if (peer == address) continue;
        rpc->append_entries(peer, storage->term(),
            [this, &peer] (uint64_t term, bool success) {
                std::lock_guard<std::mutex> lock(raft_mutex);
                update_term(term);
                if (status != Status::Leader) return;

                update_peer_time(peer);
            });
        if (peer_elapsed_time(peer) < (long long) max_election_timeout) seen++;
    }
    // check "seen" nodes and demote leader if necessary
    if (seen < majority_quorum()) {
        status = Status::Follower;
        heartbeat_task->pause();
        rpc->clear();
        PLOGI << "Demoted from leader. Unable to contact majority cluster.";
    }
    election_task->reset();
}

// task starter
void Raft::start_tasks() {
    if (election_task == nullptr) {
        election_task = new ElectionTask(
            [this]() {
                std::uniform_int_distribution<> d(
                    min_election_timeout, max_election_timeout);
                return std::chrono::milliseconds(d(random_election_timer));
            },
            [this]() { handle_election_task(); }
        );
        PLOGI << "Started election task.";
    } else {
        PLOGF << "Trying to start election task when task exists.";
    }
    if (heartbeat_task == nullptr) {
        heartbeat_task = new SyncTask(
            heartbeat,
            [this]() { handle_heartbeat_task(); }
        );
        // Pause heartbeat task as when node starts node is a follower.
        heartbeat_task->pause();
        rpc->clear();
        PLOGI << "Started heartbeat task.";
    } else {
        PLOGF << "Trying to start heartbeat task when task exists.";
    }
}

// helpers
void Raft::update_term(uint64_t term) {
    if (storage->term() < term) {
        PLOGI << "Found higher term " << term 
              << " than current term " << storage->term() << ". "
              << "Switching to follower.";
        storage->set_term(term);
        storage->set_voted_for("");
        storage->save_state();

        if (status == Status::Leader) {
            heartbeat_task->pause();
            rpc->clear();
        }
        status = Status::Follower;
    }
    election_task->reset();
}

void Raft::become_leader() {
    status = Status::Leader;
    for (auto peer : peers)
        update_peer_time(peer);
    heartbeat_task->resume();
    PLOGI << "Became leader for term " << storage->term() << ".";
}

} // namespace raft
