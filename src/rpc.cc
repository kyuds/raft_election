#include <iostream>

#include "rpc.h"

namespace raft {

Rpc::Rpc(const std::string& node_address,
         int rpc_timeout,
         int rpc_max_retries)
    : address(node_address),
      timeout(rpc_timeout),
      max_retries(rpc_max_retries) {

    std::cout << "Built rpc on: " << address << std::endl;
}

Rpc::~Rpc() {

}

} // namespace raft
