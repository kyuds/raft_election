#include <iostream>

#include "config.h"
#include "raft.h"

using Config = raft::Config;
using Raft = raft::Raft;

void test_config(int argc, char ** argv);
void test_raft_node(int argc, char ** argv);

int main(int argc, char ** argv) {
    test_raft_node(argc, argv);
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
