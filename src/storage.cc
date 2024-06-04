#include "storage.h"

#include <iostream>

namespace raft {

Storage::Storage(const std::string& _root) : root(_root) {
    std::cout << "Built storage on: " << root << std::endl;
}

bool Storage::commit_metadata(uint64_t term, const std::string& voted_for) {

    return true;
}

} // namespace raft
