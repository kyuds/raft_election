#include <iostream>
#include <thread>

#include <plog/Log.h>
#include "plog/Initializers/RollingFileInitializer.h"
#include "config.h"
#include "storage.h"
#include "raft.h"
#include "utils.h"
#include "log.pb.h"

using Config = raft::Config;
using Raft = raft::Raft;
using Storage = raft::Storage;
using LogEntry = raft::LogEntry;

void test_config(int argc, char ** argv);
void test_raft_node(int argc, char ** argv);
void run_election_cli(int argc, char ** argv);
void test_storage(int argc, char ** argv);

int main(int argc, char ** argv) {
    test_storage(argc, argv);
    return 0;
}

///

// build/bin/raft node1 localhost:10001 tmp/node1 peer.txt
void test_config(int argc, char ** argv) {
    Config * config = Config::default_config(
        argv[1], argv[2], argv[3], argv[4]);
    std::cout << config->info();
}

// build/bin/raft node1 localhost:10001
void test_raft_node(int argc, char ** argv) {
    Raft * raft = new Raft(argv[1], argv[2]);
}

/*
build/bin/raft node1 localhost:10001
build/bin/raft node2 localhost:10002
build/bin/raft node3 localhost:10003
build/bin/raft node4 localhost:10004
build/bin/raft node5 localhost:10005
*/
void run_election_cli(int argc, char ** argv) {
    std::cout << "Creating raft node with name <" << argv[1] << "> and address " << argv[2] << std::endl;
    Raft * r = new Raft(argv[1], argv[2]);
    r->start();

    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10000));
    }
}

/*
build/bin/raft tmp
*/
void test_storage(int argc, char ** argv) {
    plog::init(plog::info, 
               raft::combine_paths(argv[1], "log.txt").c_str(), 
               1000000, 5);
    Storage * storage = new Storage(argv[1]);
    storage->load_state();

    std::cout << "term: " << storage->term() << " voted_for: (" << storage->voted_for() << ")" << std::endl;
    std::cout << "start: " << storage->get_start_index() << " next: " << storage->get_next_index() << std::endl;

    storage->inc_term();
    storage->save_state();

    assert(storage->get_log(1000) == nullptr);
    std::cout << "asserted non-existent log returns null." << std::endl;

    LogEntry entry;
    entry.set_term(storage->get_next_index());
    entry.set_value("hello");
    std::cout << storage->append_log(entry) << std::endl;
}
