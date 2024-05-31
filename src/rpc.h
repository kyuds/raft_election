#ifndef RAFT_RPC_TRANSPORT
#define RAFT_RPC_TRANSPORT

#include <string>

namespace raft {

class Rpc {
    public:
        Rpc(const std::string& node_address,
            int rpc_timeout,
            int rpc_max_retries);
        ~Rpc();

    private:

        // configs
        std::string address;
        int timeout;
        int max_retries;
};

} // namespace raft

#endif // RAFT_RPC
