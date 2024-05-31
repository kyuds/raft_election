#include "storage.h"

#include <iostream>

namespace raft {

Storage::Storage(const std::string& _root) {
    std::cout << "Built storage on: " << _root << std::endl;
}

Storage::~Storage() {
    
}

} // namespace raft
