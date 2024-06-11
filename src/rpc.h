#ifndef RAFT_RPC_TRANSPORT
#define RAFT_RPC_TRANSPORT

#include <string>
#include <grpcpp/grpcpp.h>

#include "raft.grpc.pb.h"
#include "common.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

namespace raft {

class Rpc {
    public:
        Rpc(const std::string& node_address,
            int rpc_timeout,
            int rpc_max_retries,
            request_vote_callback_t _rpc_rv_clbk,
            ping_callback_t _rpc_ping_clbk);
        ~Rpc();

        // bool initialize(); // starts gRPC server.

    private:

        // configs
        std::string address;
        int timeout;
        int max_retries;

        // callbacks
        request_vote_callback_t rpc_rv_clbk;
        ping_callback_t rpc_ping_clbk;
};

} // namespace raft

#endif // RAFT_RPC
