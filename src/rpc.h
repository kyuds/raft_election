#ifndef RAFT_RPC_TRANSPORT
#define RAFT_RPC_TRANSPORT

#include <atomic>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <grpcpp/grpcpp.h>

#include "raft.grpc.pb.h"
#include "common.h"

using grpc::ClientContext;
using grpc::Status;

using grpc::CallbackServerContext;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerUnaryReactor;

namespace raft {

class Rpc {
    public:
        Rpc(const std::string& node_address, int rpc_timeout)
            : address(node_address), timeout(rpc_timeout) {}
        ~Rpc();

        // call gRPC server
        bool start(rv_service_clbk_t rv_cb, ae_service_clbk_t ae_cb);
        // deletes all stubs (when node converts to follower).
        void clear(); 

        // (TODO) add more params to complete Raft.
        // rpc frontend api
        void request_vote(const std::string& peer,
                          const uint64_t term,
                          const std::string& address,
                          rv_clbk_t cb);
        
        void append_entries(const std::string& peer,
                            const uint64_t term,
                            const std::string& address,
                            ae_clbk_t cb);

    private:
        // might have not made a client connection yet.
        // gets the connection, or creates/stores into the unordered_map.
        std::unique_ptr<RaftRpc::Stub> get_stub(const std::string& address);
    
    private:
        // Impl Wrappers for Client and Server Implementations.
        // ClientImpl allows for state to be contained.
        // ServerImpl allows for the Rpc class to store service information.
        class ClientImpl {
            public:
                ClientImpl(std::unique_ptr<RaftRpc::Stub> stub_, const int timeout_)
                    : stub(std::move(stub_)), timeout(timeout_) {}

                void RequestVote(VoteRequest r, rv_clbk_t cb);
                void AppendEntries(AppendRequest r, ae_clbk_t cb);

            private:
                const int timeout;
                std::unique_ptr<RaftRpc::Stub> stub;
                ClientContext ctx;
                VoteReply vr;
                AppendReply ar;
        };

        class ServerImpl : public RaftRpc::CallbackService {
            public:
                ServerImpl(rv_service_clbk_t rv_clbk_,
                           ae_service_clbk_t ae_clbk_)
                    : rv_clbk(std::move(rv_clbk_)), ae_clbk(std::move(ae_clbk_)) {}

                ServerUnaryReactor* RequestVote(CallbackServerContext* context,
                                                const VoteRequest* request,
                                                VoteReply* reply) override;

                ServerUnaryReactor* AppendEntries(CallbackServerContext* context,
                                                const AppendRequest* request,
                                                AppendReply* reply) override;
            
            private:
                rv_service_clbk_t rv_clbk;
                ae_service_clbk_t ae_clbk;
        };
    
    private:
        // configs
        const std::string address;
        const int timeout;

        // channel map --> one stub per client connection.
        std::mutex channel_m;
        std::unordered_map<std::string, std::shared_ptr<grpc::Channel>> channels;

        // server
        ServerImpl * service;
        std::unique_ptr<Server> server;
};

} // namespace raft

#endif // RAFT_RPC
