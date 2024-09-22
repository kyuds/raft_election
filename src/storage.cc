#include <iostream>

#include <rocksdb/options.h>

#include "storage.h"
#include "utils.h"

namespace raft {

Storage::Storage(const std::string& dir) {
    rocksdb::Options options;
    options.create_if_missing = true;
    rocksdb::Status status = rocksdb::DB::Open(options, combine_paths(dir, "raft.db"), &db);
    
    if (status.ok()) {
        std::cout << "RocksDB instance created successfully." << std::endl;
    } else {
        std::cerr << "Error opening RocksDB: " << status.ToString() << std::endl;
    }
}

Storage::~Storage() {
    if (db) {
        delete db;
    }
}

} // namespace raft
