#include <iostream>
#include <thread>

#include "config.h"
#include "raft.h"

using Config = raft::Config;
using Raft = raft::Raft;

void test_config(int argc, char ** argv);
void test_raft_node(int argc, char ** argv);
void run_election_cli(int argc, char ** argv);

int main(int argc, char ** argv) {
    run_election_cli(argc, argv);
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
