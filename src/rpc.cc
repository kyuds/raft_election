#include <iostream>

#include "rpc.h"

namespace raft {

Rpc::~Rpc() {
    // stop server
    server->Shutdown();
    delete service;
}

void Rpc::start(rv_service_clbk_t rv_cb, ae_service_clbk_t ae_cb) {
    ServerBuilder builder;
    builder.AddListeningPort(address, grpc::InsecureServerCredentials());
    service = new ServerImpl(std::move(rv_cb), std::move(ae_cb));
    builder.RegisterService(service);
    server = builder.BuildAndStart();
}

void Rpc::clear() {
    std::lock_guard<std::mutex> lock(channel_m);
    channels.clear();
}

void Rpc::request_vote(const std::string& peer,
                        const uint64_t term,
                        const std::string& address,
                        rv_clbk_t cb) {
    VoteRequest req;
    req.set_term(term);
    req.set_candidateid(address);
    (new ClientImpl(get_stub(peer), timeout))->RequestVote(std::move(req), std::move(cb));
}
        
void Rpc::append_entries(const std::string& peer,
                        const uint64_t term,
                        const std::string& address,
                        ae_clbk_t cb) {
    AppendRequest req;
    req.set_term(term);
    req.set_leaderid(address);
    (new ClientImpl(get_stub(peer), timeout))->AppendEntries(std::move(req), std::move(cb));
}

ServerUnaryReactor* Rpc::ServerImpl::RequestVote(CallbackServerContext* context,
                                                const VoteRequest* request,
                                                VoteReply* reply) {

    rpc_rep_t rep = rv_clbk(request->term(), request->candidateid());
    reply->set_term(rep.term);
    reply->set_votegranted(rep.success);
    ServerUnaryReactor* reactor = context->DefaultReactor();
    reactor->Finish(Status::OK);
    return reactor;
}

ServerUnaryReactor* Rpc::ServerImpl::AppendEntries(CallbackServerContext* context,
                                                    const AppendRequest* request,
                                                    AppendReply* reply) {
    
    rpc_rep_t rep = ae_clbk(request->term(), request->leaderid());
    reply->set_term(rep.term);
    reply->set_success(rep.success);
    ServerUnaryReactor* reactor = context->DefaultReactor();
    reactor->Finish(Status::OK);
    return reactor;
}

std::unique_ptr<RaftRpc::Stub> Rpc::get_stub(const std::string& address) {
    std::lock_guard<std::mutex> lock(channel_m);
    if (!channels.contains(address)) {
        grpc::ChannelArguments args;
        args.SetInt(GRPC_ARG_MAX_RECONNECT_BACKOFF_MS, 10000);
        args.SetInt(GRPC_ARG_INITIAL_RECONNECT_BACKOFF_MS, 500);
        args.SetInt(GRPC_ARG_MIN_RECONNECT_BACKOFF_MS , 500);
        channels[std::string(address)] = grpc::CreateCustomChannel(
            address, grpc::InsecureChannelCredentials(), args);
    }
    auto channel = channels.find(address)->second;
    return RaftRpc::RaftRpc::NewStub(channel);
}

// helper function for rpc timeouts
std::chrono::system_clock::time_point create_deadline(int timeout) {
    return std::chrono::system_clock::now() + std::chrono::milliseconds(timeout);
}

// ClientImpl
void Rpc::ClientImpl::RequestVote(VoteRequest r, rv_clbk_t cb) {
    ctx.set_deadline(create_deadline(timeout));
    stub->async()->RequestVote(&ctx, &r, &vr,
        [this, cb = std::move(cb)](Status status) {
            if (status.ok()) 
                cb(vr.term(), vr.votegranted());
            delete this;
        });
}

void Rpc::ClientImpl::AppendEntries(AppendRequest r, ae_clbk_t cb) {
    ctx.set_deadline(create_deadline(timeout));
    stub->async()->AppendEntries(&ctx, &r, &ar,
        [this, cb = std::move(cb)](Status status) {
            if (status.ok()) 
                cb(ar.term(), ar.success());
            delete this;
        });
}

} // namespace raft
