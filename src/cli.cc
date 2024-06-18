#include <iostream>

#include "raft.h"

using Raft = raft::Raft;

void raft_cli(void) {
    std::string name("node1");
    std::string address("localhost:10001");
    std::string conf_file("config.json");
    std::string member_file("members.txt");
    
    Raft * r = new Raft(name, address, conf_file, member_file);
    r->start();
}

void rpc_cli_mutual(void) {

}

int main(void) {
    raft_cli();
    return 0;
}
