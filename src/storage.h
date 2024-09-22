#ifndef RAFT_STORAGE
#define RAFT_STORAGE

#include <memory>
#include <string>

#include <rocksdb/db.h>

namespace raft {

class Storage {
public:
    Storage(const std::string& dir);
    ~Storage();

private:
    rocksdb::DB * db;
};

} // namespace raft

#endif // RAFT_STORAGE
