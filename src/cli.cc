#include <iostream>

#include "raft.h"

using Raft = raft::Raft;

int main(void) {
    std::string name("node1");
    std::string address("localhost:10001");
    std::string conf_file("config.json");
    std::string member_file("members.txt");
    
    Raft * r = new Raft(name, address, conf_file, member_file);

    return 0;
}
