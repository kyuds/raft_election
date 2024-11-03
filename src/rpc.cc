#include "rpc.h"

#include <chrono>
#include <stdexcept>
#include <plog/Log.h>

namespace raft {

// API
void Rpc::start(service_rv_t rv_cb, service_ae_t ae_cb) {
    grpc::ServerBuilder builder;
    builder.AddListeningPort(address, grpc::InsecureServerCredentials());
    service = new ServerImpl(std::move(rv_cb), std::move(ae_cb));
    builder.RegisterService(service);
    server = builder.BuildAndStart();
    if (!server) {
        PLOGE << "Failed to start gRPC server!";
        throw std::runtime_error("Unable to start gRPC server.");
    }
}

void Rpc::clear() {
    std::lock_guard<std::mutex> lock(channel_mutex);
    channels.clear();
}

void Rpc::request_vote(const std::string& peer, 
                        const int64_t term, 
                        client_rv_t callback)
{
    VoteRequest request;
    request.set_term(term);
    request.set_addr(address);
    auto client = new ClientImpl(get_stub(peer), timeout);
    client->RequestVote(std::move(request), std::move(callback));
}

void Rpc::append_entries(const std::string& peer,
                        const int64_t term,
                        client_ae_t callback)
{
    AppendRequest request;
    request.set_term(term);
    request.set_leader(address);
    auto client = new ClientImpl(get_stub(peer), timeout);
    client->AppendEntries(std::move(request), std::move(callback));
}

// helper for request callback timeouts
std::chrono::system_clock::time_point deadline(const int t) {
    return std::chrono::system_clock::now() + std::chrono::milliseconds(t);
}

// ClientImpl
void Rpc::ClientImpl::RequestVote(VoteRequest request, client_rv_t callback) {
    context.set_deadline(deadline(timeout));
    stub->async()->RequestVote(&context, &request, &vr,
        [this, callback = std::move(callback)](grpc::Status status) {
            if (status.ok()) 
                callback(vr.term(), vr.granted());
            delete this;
        });
}

void Rpc::ClientImpl::AppendEntries(AppendRequest request, client_ae_t callback) {
    context.set_deadline(deadline(timeout));
    stub->async()->AppendEntries(&context, &request, &ar,
        [this, callback = std::move(callback)](grpc::Status status) {
            if (status.ok())
                callback(ar.term(), ar.success());
            delete this;
        });
}

// ServerImpl
ServerUnaryReactor* Rpc::ServerImpl::RequestVote(
    CallbackServerContext* context,
    const VoteRequest* request,
    VoteResponse* response)
{
    rep_t r = rv_clbk(request->term(), request->addr());
    response->set_term(r.term);
    response->set_granted(r.success);
    ServerUnaryReactor* reactor = context->DefaultReactor();
    reactor->Finish(grpc::Status::OK);
    return reactor;
}

ServerUnaryReactor* Rpc::ServerImpl::AppendEntries(
    CallbackServerContext* context,
    const AppendRequest* request,
    AppendResponse* response)
{
    rep_t r = ae_clbk(request->term(), request->leader());
    response->set_term(r.term);
    response->set_success(r.success);
    ServerUnaryReactor* reactor = context->DefaultReactor();
    reactor->Finish(grpc::Status::OK);
    return reactor;
}

// Channel Caching
std::unique_ptr<RaftService::Stub> Rpc::get_stub(const std::string& address) {
    std::lock_guard<std::mutex> lock(channel_mutex);
    if (!channels.contains(address)) {
        grpc::ChannelArguments args;
        args.SetInt(GRPC_ARG_MAX_RECONNECT_BACKOFF_MS, 500);
        args.SetInt(GRPC_ARG_INITIAL_RECONNECT_BACKOFF_MS, 100);
        args.SetInt(GRPC_ARG_MIN_RECONNECT_BACKOFF_MS , 100);
        channels[std::string(address)] = grpc::CreateCustomChannel(
            address, grpc::InsecureChannelCredentials(), args);
    }
    auto channel = channels.find(address)->second;
    return RaftService::NewStub(channel);
}

} // namespace raft
