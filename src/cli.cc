#include <iostream>
#include <functional>
#include <memory>
#include <thread>
#include <chrono>

#include "config.h"
#include "raft.h"
#include "rpc.h"

#include <plog/Log.h>
#include "plog/Initializers/RollingFileInitializer.h"

using Raft = raft::Raft;
using Rpc = raft::Rpc;
using rpc_rep_t = raft::rpc_rep_t;
using Config = raft::Config;
using State = raft::State;

void config_parse();
void state_test();
void raft_cli();
void rpc_cli_mutual(int argc, char ** argv);

int main(int argc, char ** argv) {
    // rpc_cli_mutual(argc, argv);
    plog::init(plog::debug, "Hello.txt");
    state_test();
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void raft_cli() {
    std::string name("node1");
    std::string address("localhost:10001");
    std::string conf_file("config.json");
    std::string member_file("members.txt");
    
    Raft * r = new Raft(name, address, conf_file, member_file);
    r->start();
}

class RpcWrapper {
    public:
        RpcWrapper(const std::string& address_) 
        : address(address_) {
            rpc = std::make_unique<Rpc>(address, 100);
            rpc->start(
                [this](uint64_t _term, const std::string& address) {
                    term2 = _term;
                    return rpc_rep_t { .term = _term, .success = true };
                },
                [this](uint64_t _term, const std::string& address) {
                    return rpc_rep_t { .term = _term, .success = true };
                }
            );
            term1 = 0;
            term2 = 0;
        }

        void call_rv(const std::string& peer) {
            std::cout << "Sent " << ++term1 << std::endl;
            rpc->request_vote(peer, term1, address, 
                [this](uint64_t term, bool success) {
                    std::cout << "Request Reply: Got term " << term << " with " << success << std::endl;
                }
            );
        }

    private:
        const std::string address;
        std::unique_ptr<Rpc> rpc;
        uint64_t term1;
        uint64_t term2;

};

/*
./raft localhost:50051 localhost:50052
./raft localhost:50052 localhost:50051
*/
void rpc_cli_mutual(int argc, char ** argv) {
    std::string addr1(argv[1]);
    std::string addr2(argv[2]);

    RpcWrapper * r = new RpcWrapper(addr1);

    while (true) {
        r->call_rv(addr2);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void config_parse() {
    // auto c = new Config("node", "ip", "config.json");
    auto c = Config::default_config("node", "ip");
    std::cout << c->get_storage_dir() << std::endl;
    std::cout << c->get_peer_file() << std::endl;
    std::cout << c->get_heartbeat() << std::endl;
}

void state_test() {
    auto s = new State("tmp");
    std::cout << s->initialize() << std::endl;
    std::cout << s->term() << " <" << s->voted_for() << ">" << std::endl;
}
