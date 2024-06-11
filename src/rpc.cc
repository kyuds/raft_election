#include <iostream>

#include "rpc.h"

namespace raft {

Rpc::Rpc(const std::string& node_address,
         int rpc_timeout,
         int rpc_max_retries,
         request_vote_callback_t _rpc_rv_clbk,
         ping_callback_t _rpc_ping_clbk)
    : address(node_address),
      timeout(rpc_timeout),
      max_retries(rpc_max_retries),
      rpc_rv_clbk(_rpc_rv_clbk),
      rpc_ping_clbk(_rpc_ping_clbk) {

    std::cout << "Built rpc on: " << address << std::endl;
}

Rpc::~Rpc() {

}

} // namespace raft
